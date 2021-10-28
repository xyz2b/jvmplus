//
// Created by xyzjiao on 10/11/21.
//

#include "instanceKlass.hpp"
#include "../../../share/vm/prims/JavaNativeInterface.hpp"

instanceOop InstanceKlass::allocate_instance(KlassHandle k) {
    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(k, sizeof(instanceOopDesc));

    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* fields_map = new Hashmap<Symbol*, jobject, HashCode<const Symbol*>>(non_static_filed_count(k));
    i->set_fields(fields_map);

    InstanceKlass* klass = (InstanceKlass*) i->klass();
    InstanceKlassHandle this_klass(klass);
    put_field_to_oop(i, this_klass);
    return i;
}

void InstanceKlass::put_field_to_oop(instanceOop child_oop, InstanceKlassHandle this_klass) {
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

    return put_field_to_oop(child_oop, super_oop);
}


FiledInfo* InstanceKlass::find_field(Symbol* field_name, Symbol* descriptor_name) {
    for (int index = 0; index < _fields->size(); index++) {
        FiledInfo* f = _fields->get(index);

        if (*(f->name()) == field_name && *(f->descriptor()) == descriptor_name) {
            return f;
        }
    }

    return nullptr;
}

void InstanceKlass::link_class() {
    if (!this->is_linked()) {
        InstanceKlassHandle this_oop(this);
        link_class_impl(this_oop);
    }
}

bool InstanceKlass::link_class_impl(InstanceKlassHandle this_oop) {
    // 为静态变量分配内存、赋初值（零值）
    // 将静态变量存储到InstanceMirrorKlass对象中
    ConstantPool* constant_pool = this_oop->get_constant_pool();
    Array<FiledInfo*>* fields = this_oop->get_fields();
    Symbol* class_name = constant_pool->get_class_name_by_class_ref(this_oop->get_this_class());
    oop mirror = this_oop->java_mirror();

    INFO_PRINT("link class: %s", class_name->as_C_string());

    if (class_name->start_with("java")) {
        INFO_PRINT("不link java包的类");
        return true;
    }

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
    this_oop->set_init_state(ClassState::linked);
    Klass* super_klass = this_oop->get_super_klass();
    if (super_klass == nullptr) return true;
    InstanceKlassHandle super_oop(super_klass);

    return link_class_impl(super_oop);
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
        InstanceKlassHandle this_oop(this);

        initialize_impl(this_oop);
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
void InstanceKlass::initialize_impl(InstanceKlassHandle this_oop) {
    this_oop->set_init_state(ClassState::being_initialized);
    // 执行<clinit>方法
    const char* name = "<clinit>";
    const char* descriptor = "()V";

    Symbol* method_name = new (strlen(name)) Symbol(name, strlen(name));
    Symbol* method_descriptor = new (strlen(descriptor)) Symbol(descriptor, strlen(descriptor));
    ConstantPool* constant_pool = this_oop->get_constant_pool();
    Symbol* class_name = constant_pool->get_class_name_by_class_ref(this_oop->get_this_class());

    INFO_PRINT("class name: %s", class_name->as_C_string());

    if (class_name->start_with("java")) {
        INFO_PRINT("不初始化 java包的类");
        return;
    }

    Method* method = JavaNativeInterface::get_method(this_oop(), method_name, method_descriptor);
    JavaNativeInterface::call_method(this_oop(), method);

    this_oop->set_init_state(ClassState::fully_initialized);

    Klass* super_klass = this_oop->get_super_klass();
    if (super_klass == nullptr) return;
    InstanceKlassHandle super_oop(super_klass);
    initialize_impl(super_oop);

    // 判断缓存中是否已经存在了，没有存在就解析，并存入缓存
}
