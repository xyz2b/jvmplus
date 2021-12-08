//
// Created by xyzjiao on 10/11/21.
//

#include "instanceKlass.hpp"
#include "../../../share/vm/prims/JavaNativeInterface.hpp"
#include "../../../share/vm/classfile/systemDictionary.h"

instanceOop InstanceKlass::allocate_instance(KlassHandle k) {
    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(k, sizeof(instanceOopDesc));

    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* fields_map = new Hashmap<Symbol*, jobject, HashCode<const Symbol*>>(non_static_filed_count(k));
    i->set_fields(fields_map);

    InstanceKlass* klass = (InstanceKlass*) i->klass();
    InstanceKlassHandle this_klass(klass);
    put_non_static_field_to_oop(i, this_klass);
    return i;
}

void InstanceKlass::put_non_static_field_to_oop(instanceOop child_oop, InstanceKlassHandle this_klass) {
    // 将非静态变量存储到oop对象中
    ConstantPool* constant_pool = this_klass->get_constant_pool();
    Array<FiledInfo*>* fields = this_klass->get_fields();
    Symbol* class_name = constant_pool->get_class_name_by_class_ref(this_klass->get_this_class());


    if (class_name->start_with("java")) {
        INFO_PRINT("不link java包的类");
        return;
    }

    for(int index = 0; index < fields->size(); index++) {
        FiledInfo* f = fields->get_at(index);
        Symbol* f_name = constant_pool->symbol_at(f->name_index());
        Symbol* f_descriptor = constant_pool->symbol_at(f->signature_index());


        if (!f->access_flags().is_static()) {
            switch (*(f_descriptor->as_C_string())) {
                case JVM_SIGNATURE_ARRAY: {
                    INFO_PRINT("解析array类型");

                    jobject field_val = nullptr;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_CLASS: {
                    INFO_PRINT("解析reference类型");

                    jobject field_val = nullptr;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_BOOLEAN: {
                    INFO_PRINT("解析boolean类型");

                    jobject field_val = nullptr;
                    *((jboolean *)(&field_val)) = false;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_BYTE: {
                    INFO_PRINT("解析byte类型");

                    jobject field_val = nullptr;
                    *((jbyte *)(&field_val)) = 0;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_CHAR: {
                    INFO_PRINT("解析char类型");

                    jobject field_val = nullptr;
                    *((jchar *)(&field_val)) = 0;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_SHORT: {
                    INFO_PRINT("解析short类型");

                    jobject field_val = nullptr;
                    *((jshort *)(&field_val)) = 0;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_INT: {
                    INFO_PRINT("解析int类型");

                    jobject field_val = nullptr;
                    *((jint*)(&field_val)) = 0;
                    INFO_PRINT("%d", *((jlong *)(&field_val)));
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_FLOAT: {
                    INFO_PRINT("解析float类型");

                    jobject field_val = nullptr;
                    *((jfloat *)(&field_val)) = 0.0;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_LONG: {
                    INFO_PRINT("解析long类型");

                    jobject field_val = nullptr;
                    *((jlong*)(&field_val)) = 0;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_DOUBLE: {
                    INFO_PRINT("解析double类型");

                    jobject field_val = nullptr;
                    *((jdouble*)(&field_val)) = 0.0;
                    child_oop->put_field(class_name, f_name, field_val);
                    break;
                }
                default: {
                    ERROR_PRINT("无法识别的元素类型: %c", *(f_descriptor->as_C_string()));
                    exit(-1);

                }
            }
        }
    }

    Klass* super_klass = this_klass->get_super_klass();
    if (super_klass == nullptr) return;
    InstanceKlassHandle super_oop(super_klass);

    return put_non_static_field_to_oop(child_oop, super_oop);
}

// 这里的链接阶段，包含了验证、准备、解析
/**
 * 何时解析？
    答: 并不是加载之后就立刻解析，用的时候才会去解析。
    JVM没有触发解析的机制，只有触发初始化的机制，通过触发初始化阶段从而触发解析阶段。
    在初始化之前会去检测这个类有没有被解析（借助缓存判断），没有就会去解析。
 * */
void InstanceKlass::link_class() {
    if (!this->is_linked()) {
        link_class_impl(this);
    }
}

bool InstanceKlass::link_class_impl(InstanceKlass* this_oop) {
    if (this_oop->is_in_error_state()) {
        ERROR_PRINT("link class出错");
        exit(-1);
    }

    if (this_oop->is_linked()) {
        INFO_PRINT("已完成link");
        return true;
    }

    // 触发父类加载
    if (this_oop->get_super_klass() == nullptr)  {
        Symbol* super_class_name = this_oop->get_constant_pool()->get_class_name_by_class_ref(this_oop->get_super_class());

        if (super_class_name->start_with("java")) {
            WARNING_PRINT("不link java包的类: %s", super_class_name->as_C_string());

            INFO_PRINT("link: %s", this_oop->get_constant_pool()->get_class_name_by_class_ref(this_oop->get_this_class())->as_C_string());
            initialize_static_filed(this_oop);

            this_oop->load_interface(this_oop);

            this_oop->initialize_vtable();

            this_oop->initialize_itable();

            this_oop->set_init_state(ClassState::linked);

            return true;
        }

        INFO_PRINT("触发父类的加载: %s", super_class_name->as_C_string());

        InstanceKlass* instanceKlass = (InstanceKlass*) SystemDictionary::resolve_or_null(super_class_name);

        this_oop->set_super_klass(instanceKlass);

        link_class_impl(instanceKlass);
    }

    initialize_static_filed(this_oop);

    // 触发接口加载
    this_oop->load_interface(this_oop);

    // vtable
    this_oop->initialize_vtable();

    // itable
    this_oop->initialize_itable();


    // other thread is already linked this klass
    if (this_oop->is_linked()) {
        return true;
    }

    // TODO: verification and rewrite
    // rewrite: rewrite constant_pool such as class index -> klass
    // rewrite class
    // rewrite method
    // constant_pool_cache

    // 设置状态
    this_oop->set_init_state(ClassState::linked);
    return true;
}

void InstanceKlass::load_interface(InstanceKlass* this_oop) {
    if (this_oop->get_interfaces_count() == 0) {
        INFO_PRINT("没有实现接口，不需要load");
        return;
    }

    ConstantPool* constant_pool = this_oop->get_constant_pool();
    Array<u2>* interfaces = this_oop->get_interfaces();

    Array<InstanceKlass*>* interfaces_klass= new Array<InstanceKlass*>(this_oop->get_interfaces_count());

    for (int i = 0; i < interfaces->size(); i++) {
        Symbol* class_name = constant_pool->get_class_name_by_class_ref(interfaces->get_at(i));

        InstanceKlass* instanceKlass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);

        link_class_impl(instanceKlass);

        interfaces_klass->add(instanceKlass);
    }

    this_oop->set_interfaces_klass(interfaces_klass);
}

void InstanceKlass::initialize_static_filed(InstanceKlass* this_oop) {
    // 为静态变量分配内存、赋初值（零值）
    // 将静态变量存储到InstanceMirrorKlass对象中
    ConstantPool* constant_pool = this_oop->get_constant_pool();
    Array<FiledInfo*>* fields = this_oop->get_fields();
    Symbol* class_name = constant_pool->get_class_name_by_class_ref(this_oop->get_this_class());
    oop mirror = this_oop->java_mirror();

    INFO_PRINT("link class: %s", class_name->as_C_string());

    // link this class
    for(int index = 0; index < fields->size(); index++) {
        FiledInfo* f = fields->get_at(index);
        Symbol* f_name = constant_pool->symbol_at(f->name_index());
        Symbol* f_descriptor = constant_pool->symbol_at(f->signature_index());


        if (f->access_flags().is_static()) {
            switch (*(f_descriptor->as_C_string())) {
                case JVM_SIGNATURE_ARRAY: {
                    INFO_PRINT("解析array类型");

                    jobject field_val = nullptr;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_CLASS: {
                    INFO_PRINT("解析reference类型");

                    jobject field_val = nullptr;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_BOOLEAN: {
                    INFO_PRINT("解析boolean类型");

                    jobject field_val = nullptr;
                    *((jboolean *)(&field_val)) = false;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_BYTE: {
                    INFO_PRINT("解析byte类型");

                    jobject field_val = nullptr;
                    *((jbyte *)(&field_val)) = 0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_CHAR: {
                    INFO_PRINT("解析char类型");

                    jobject field_val = nullptr;
                    *((jchar *)(&field_val)) = 0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_SHORT: {
                    INFO_PRINT("解析short类型");

                    jobject field_val = nullptr;
                    *((jshort *)(&field_val)) = 0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_INT: {
                    INFO_PRINT("解析int类型");

                    jobject field_val = nullptr;
                    *((jint*)(&field_val)) = 0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_FLOAT: {
                    INFO_PRINT("解析float类型");

                    jobject field_val = nullptr;
                    *((jfloat *)(&field_val)) = 0.0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_LONG: {
                    INFO_PRINT("解析long类型");

                    jobject field_val = nullptr;
                    *((jlong*)(&field_val)) = 0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                }
                case JVM_SIGNATURE_DOUBLE: {
                    INFO_PRINT("解析double类型");

                    jobject field_val = nullptr;
                    *((jdouble*)(&field_val)) = 0.0;
                    mirror->put_field(class_name, f_name, field_val);
                    break;
                    break;
                }
                default: {
                    ERROR_PRINT("无法识别的元素类型: %c", *(f_descriptor->as_C_string()));
                    exit(-1);

                }
            }
        }
    }
}

int InstanceKlass::non_static_filed_count(KlassHandle k) {
    // InstanceMirrorKlass storage static fields
    InstanceKlass* instance = (InstanceKlass*)k();
    int non_static_filed_count = 0;
    while (instance != nullptr) {
        non_static_filed_count += instance->non_static_filed_count();
        instance = (InstanceKlass *)(instance->get_super_klass());
    }
    return non_static_filed_count;
}

void InstanceKlass::initialize() {
    if (this->is_not_initialized()) {
        initialize_impl(this);
    }

}

/**
 *何时初始化？
    主动使用时
  - new、getstatic、putstatic、invokestatic
  - 反射
  - 初始化一个类的子类会去加载其父类
  - 启动类（main函数所在的类）
 * */
void InstanceKlass::initialize_impl(InstanceKlass* this_oop) {
    this_oop->link_class();

    // TODO: locked

    this_oop->set_init_state(ClassState::being_initialized);

    // 执行<clinit>方法
    const char* name = "<clinit>";
    const char* descriptor = "()V";

    Symbol* method_name = new (strlen(name)) Symbol(name, strlen(name));
    Symbol* method_descriptor = new (strlen(descriptor)) Symbol(descriptor, strlen(descriptor));
    ConstantPool* constant_pool = this_oop->get_constant_pool();
    Symbol* class_name = constant_pool->get_class_name_by_class_ref(this_oop->get_this_class());

    INFO_PRINT("class name: %s", class_name->as_C_string());

    // TODO: 没有处理继承java包下的类的情况，如果需要加载java包下的类需要用JNI调用<clinit>方法
    if (class_name->start_with("java")) {
        INFO_PRINT("不初始化 java包的类");
        return;
    }

    // call this class <clinit>
    Method* method = JavaNativeInterface::get_method(this_oop, method_name, method_descriptor);
    if (method == nullptr) {
        INFO_PRINT("class %s 不存在<clinit>方法，跳过", class_name->as_C_string());
        return;
    }
    JavaNativeInterface::call_method(this_oop, method);

    this_oop->set_init_state(ClassState::fully_initialized);

    // initialize super class
    InstanceKlass* super_klass = (InstanceKlass*)this_oop->get_super_klass();
    if (super_klass == nullptr) return;
    initialize_impl(super_klass);

    // 判断缓存中是否已经存在了，没有存在就解析，并存入缓存
}

void InstanceKlass::initialize_vtable() {
    this->initialize_super_vtable(this);

    ConstantPool* constant_pool = this->get_constant_pool();

    Array<Method*>* methods = this->get_methods();
    for (int i = 0; i < methods->size(); i++) {
        Method* method = methods->get_at(i);

        // protected/public and non-static/non-final method
        if (method->access_flags().is_protected() || method->access_flags().is_public()) {
            // skip <init> / <clinit> method
            if (constant_pool->symbol_at(method->name_index())->start_with("<init>") ||
            constant_pool->symbol_at(method->name_index())->start_with("<clinit>")) {
                INFO_PRINT("skip <init> / <clinit> method");
                continue;
            }

            // skip final / static method
            if (method->access_flags().is_final() || method->access_flags().is_static()) {
                INFO_PRINT("skip final / static method");
                continue;
            }

            // add to vtable
            INFO_PRINT("add to vtable method: %s", constant_pool->symbol_at(method->name_index())->as_C_string());
            this->vtable()->add(method);
        } else {
            INFO_PRINT("not add to vatble method: %s", constant_pool->symbol_at(method->name_index())->as_C_string());
        }
    }
}

void InstanceKlass::initialize_super_vtable(InstanceKlass* klass) {
    InstanceKlass* super_klass = (InstanceKlass*) klass->get_super_klass();

    // if super klass is Object, the super_klass is null, because java package class not link
    if (super_klass == nullptr) {
        ConstantPool* constant_pool = klass->get_constant_pool();
        INFO_PRINT("%s not have super class", constant_pool->get_class_name_by_class_ref(this->get_this_class())->as_C_string());
        return;
    } else {
        ConstantPool* constant_pool = super_klass->get_constant_pool();

        if (super_klass->get_super_klass() != nullptr) {
            INFO_PRINT("%s also have super class",constant_pool->get_class_name_by_class_ref(super_klass->get_this_class())->as_C_string());
            this->initialize_super_vtable(super_klass);
        } else {
            INFO_PRINT("%s not have super class",constant_pool->get_class_name_by_class_ref(super_klass->get_this_class())->as_C_string());
        }
    }

    Array<Method*>* methods = super_klass->get_methods();
    for (int i = 0; i < methods->size(); i++) {
        Method* method = methods->get_at(i);

        // protected/public and non-static/non-final method
        if (method->access_flags().is_protected() || method->access_flags().is_public()) {
            // skip <init> / <clinit> method
            if (method->name()->start_with("<init>") ||
                    method->name()->start_with("<clinit>")) {
                INFO_PRINT("skip <init> / <clinit> method");
                continue;
            }

            // skip final / static method
            if (method->access_flags().is_final() || method->access_flags().is_static()) {
                INFO_PRINT("skip final / static method");
                continue;
            }

            // add to vtable
            INFO_PRINT("add to vtable method: %s", method->name()->as_C_string());
            this->vtable()->add(method);
        } else {
            INFO_PRINT("not add to vatble method: %s", method->name()->as_C_string());
        }
    }
}


void InstanceKlass::initialize_itable() {
    Array<InstanceKlass*>* super_class_interfaces= new Array<InstanceKlass*>();
    this->initialize_super_itable(this, super_class_interfaces);

    INFO_PRINT("initialize_itable: %s", this->get_constant_pool()->get_class_name_by_class_ref(this->get_this_class())->as_C_string());

    if (this->get_interfaces_count() == 0 && super_class_interfaces->size() == 0) {
        INFO_PRINT("没有实现接口，不需要初始化itable");
        return;
    }

    Array<Method*>* methods = this->get_methods();

    for (int i = 0; i < methods->size(); i++) {
        Method* method = methods->get_at(i);

        for (int j = 0; j < super_class_interfaces->size(); j++) {
            InstanceKlass* interface = super_class_interfaces->get_at(j);

            Array<Method*>* interface_methods = interface->get_methods();

            for (int k = 0; k < interface_methods->size(); k ++) {
                Method* interface_method = interface_methods->get_at(k);
                if (*method->name() == interface_method->name() && *method->descriptor()->descriptor_info() == interface_method->descriptor()->descriptor_info()) {
                    INFO_PRINT("add to itable method: %s", method->name()->as_C_string());
                    this->itable()->add(method);
                    goto next;
                }
            }
        }

        for (int j = 0; j < this->get_interfaces_count(); j++) {
            InstanceKlass* interface = this->get_interface_klass()->get_at(j);

            Array<Method*>* interface_methods = interface->get_methods();

            for (int k = 0; k < interface_methods->size(); k ++) {
                Method* interface_method = interface_methods->get_at(k);
                if (*method->name() == interface_method->name() && *method->descriptor()->descriptor_info() == interface_method->descriptor()->descriptor_info()) {
                    INFO_PRINT("add to itable method: %s", method->name()->as_C_string());
                    this->itable()->add(method);
                    goto next;
                }
            }
        }

        next:
        {}
    }
}

void InstanceKlass::initialize_super_itable(InstanceKlass *klass, Array<InstanceKlass*>* super_class_interfaces) {
    INFO_PRINT("initialize_itable: %s", klass->get_constant_pool()->get_class_name_by_class_ref(klass->get_this_class())->as_C_string());

    InstanceKlass* super_klass = (InstanceKlass*) klass->get_super_klass();

    // if super klass is Object, the super_klass is null, because java package class not link
    if (super_klass == nullptr) {
        ConstantPool* constant_pool = klass->get_constant_pool();
        INFO_PRINT("%s not have super class", constant_pool->get_class_name_by_class_ref(this->get_this_class())->as_C_string());
        return;
    } else {
        INFO_PRINT("initialize_itable: %s", super_klass->get_constant_pool()->get_class_name_by_class_ref(super_klass->get_this_class())->as_C_string());

        ConstantPool* constant_pool = super_klass->get_constant_pool();

        if (super_klass->get_super_klass() != nullptr) {
            INFO_PRINT("%s also have super class",constant_pool->get_class_name_by_class_ref(super_klass->get_this_class())->as_C_string());
            this->initialize_super_itable(super_klass, super_class_interfaces);
        } else {
            INFO_PRINT("%s not have super class",constant_pool->get_class_name_by_class_ref(super_klass->get_this_class())->as_C_string());
        }
    }

    Array<Method*>* methods = super_klass->get_methods();
    for (int i = 0; i < methods->size(); i++) {
        Method* method = methods->get_at(i);

        for (int j = 0; j < super_class_interfaces->size(); j++) {
            InstanceKlass* interface = super_class_interfaces->get_at(j);

            Array<Method*>* interface_methods = interface->get_methods();

            for (int k = 0; k < interface_methods->size(); k ++) {
                Method* interface_method = interface_methods->get_at(k);
                if (*method->name() == interface_method->name() && *method->descriptor()->descriptor_info() == interface_method->descriptor()->descriptor_info()) {
                    INFO_PRINT("add to itable method: %s", method->name()->as_C_string());
                    this->itable()->add(method);
                    goto next;
                }
            }
        }

        for (int j = 0; j < super_klass->get_interfaces_count(); j++) {
            InstanceKlass* interface = super_klass->get_interface_klass()->get_at(j);

            Array<Method*>* interface_methods = interface->get_methods();

            for (int k = 0; k < interface_methods->size(); k ++) {
                Method* interface_method = interface_methods->get_at(k);
                if (*method->name() == interface_method->name() && *method->descriptor()->descriptor_info() == interface_method->descriptor()->descriptor_info()) {
                    INFO_PRINT("add to itable method: %s", method->name()->as_C_string());
                    this->itable()->add(method);
                    goto next;
                }
            }
        }

        next:
        {}
    }

    for (int j = 0; j < super_klass->get_interfaces_count(); j++) {
        InstanceKlass* interface = super_klass->get_interface_klass()->get_at(j);
        super_class_interfaces->add(interface);
    }

}

