//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CONSTANTPOOL_HPP
#define JVMPLUS_CONSTANTPOOL_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/hashMap.hpp"
#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/adlc/adlc.hpp"
#include "../../../share/vm/utilities/constantTag.hpp"
#include "../../../share/vm/prims/jvm.h"
#include "../../../cpu/x86/vm/bytes_x86.hpp"

class Symbol;

class ConstantPool : public Metadata {
private:
    u2 _length;
    Array<u1>* _tags;
    Klass* _klass;

    // 将常量池项存放到常量池对象后面，在HotSpot中也是这么做的，不过它没有在类中声明这个字段，而是直接在申请内存时候多申请了存放常量池项的内存，申请内存的单位是一个指针的大小（对于64位操作系统来说，整数/浮点数可以直接存储在一个指针所占用的空间）
    // HotSpot中是通过对象头指针跳过常量池对象的大小，就得到了存储常量池项的内存起始地址，我们实现中是直接在常量池对象中定了一个空字段，通过该字段的引用得到存储常量池项的内存首地址
    void* _pool[0];

public:
    // 存储常量池项的内存起始地址
    intptr_t* base() const { return (intptr_t*) (_pool); }

    Array<u1>* tags() const { return _tags; }

    ConstantPool(u2 length) : _length(length) {
        _tags = new (length) Array<u1>(length);
    }

    ConstantPool(Klass* klass, u2 length) : _klass(klass), _length(length) {
        _tags = new (length) Array<u1>(length);
    }

    /**
     * new背后的逻辑
        Complex* pc = new Complex(1, 2);

        编译器解释为-->

        Complex *pc;
        try {
            void* mem = operator new(sizeof(Complex)); // allocate(分配内存)
            pc = static_cast<Complex*>(mem);    // cast(把指向新分配内存的指针进行转型)
            pc->Complex::Complex(1, 2); // construct(调用构造函数)
            // 注意: 只有编译器才可以像上面那样直接调用ctor
            // 如果想直接调用ctor，可使用placement new: new(p)Complex(1, 2);
        } catch (std::bad_alloc) {
            // 若allocation失败就不执行constructor
        }
     * */
//    void* operator new(size_t sz, int length) {
//        // 申请存放常量池项的内存，指针存储（64位，整数/浮点数可以直接存储在一个指针所占用的空间）
//        // 常量池有多少项就申请多少个指针的内存空间
//        // 同时需要申请存储常量池对象本身字段的内存（sz）
//        char* s = (char *)malloc(sz + length * sizeof(*_pool));
//        memset(s,0, sz + length * sizeof(*_pool));
//        return s;
//    }

    static ConstantPool* allocate(int length) {
        return new (length * sizeof(*_pool)) ConstantPool(length);
    }

    void set_klass(Klass* klass) { _klass = klass; }

    ConstantTag tag_at(u2 index) const {
        // 调用ConstantTag的构造方法构造ConstantTag对象
        return (ConstantTag) tags()->get_at(index);
    }

    void tag_at_put(u2 index, u1 tag) {
        _tags->set_at(index, tag);
    }

    int length() const {
        return _length;
    }

    bool is_within_bounds(int index) const {
        return 0 <= index && index < length();
    }

    intptr_t* obj_at_adr_raw(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (intptr_t*) &base()[which];
    }

    // utf8
    Symbol** symbol_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (Symbol**) &base()[which];
    }

    jint* int_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jint*) &base()[which];
    }

    jlong* long_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jlong*) &base()[which];
    }

    jfloat* float_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jfloat*) &base()[which];
    }

    jdouble* double_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jdouble*) &base()[which];
    }

    // utf8
    void symbol_at_put(int which, Symbol* s) {
        tag_at_put(which, JVM_CONSTANT_Utf8);
        *symbol_at_adr(which) = s;
    }

    void int_at_put(int which, jint i) {
        tag_at_put(which, JVM_CONSTANT_Integer);
        *int_at_adr(which) = i;
    }

    void long_at_put(int which, jlong l) {
        tag_at_put(which, JVM_CONSTANT_Long);
        Bytes::put_native_u8((address) long_at_adr(which), *((u8*)&l));
    }

    void float_at_put(int which, jfloat f) {
        tag_at_put(which, JVM_CONSTANT_Float);
        *float_at_adr(which) = f;
    }

    void double_at_put(int which, jdouble d) {
        tag_at_put(which, JVM_CONSTANT_Double);
        Bytes::put_native_u8((address) double_at_adr(which), *((u8*)&d));
    }

    void method_at_put(int which, int class_index, int name_and_type_index) {
        tag_at_put(which, JVM_CONSTANT_Methodref);
        *int_at_adr(which) = ((jint) class_index << 16) | name_and_type_index;
    }

    void interface_method_at_put(int which, int class_index, int name_and_type_index) {
        tag_at_put(which, JVM_CONSTANT_InterfaceMethodref);
        *int_at_adr(which) = ((jint) class_index << 16) | name_and_type_index;
    }

    void field_at_put(int which, int class_index, int name_and_type_index) {
        tag_at_put(which, JVM_CONSTANT_Fieldref);
        *int_at_adr(which) = ((jint) class_index << 16) | name_and_type_index;
    }

    void string_index_at_put(int which, int string_index) {
        tag_at_put(which, JVM_CONSTANT_String);
        *int_at_adr(which) = string_index;
    }

    void class_index_at_put(int which, int name_index) {
        tag_at_put(which, JVM_CONSTANT_Class);
        *int_at_adr(which) = name_index;
    }

    void name_and_type_at_put(int which, int name_index, int signature_index) {
        tag_at_put(which, JVM_CONSTANT_NameAndType);
        *int_at_adr(which) = ((jint) name_index << 16) | signature_index;
    }

    void method_type_index_at_put(int which, int ref_index) {
        tag_at_put(which, JVM_CONSTANT_MethodType);
        *int_at_adr(which) = ref_index;
    }

    void method_handle_index_at_put(int which, int ref_kind, int ref_index) {
        tag_at_put(which, JVM_CONSTANT_MethodHandle);
        *int_at_adr(which) = ((jint) ref_kind << 16) | ref_index;
    }

    void invoke_dynamic_at_put(int which, int bootstrap_specifier_index, int name_and_type_index) {
        tag_at_put(which, JVM_CONSTANT_InvokeDynamic);
        *int_at_adr(which) = ((jint) bootstrap_specifier_index << 16) | name_and_type_index;
    }

    Symbol* symbol_at(int which) {
        assert(tag_at(which).is_utf8(), "Corrupted constant pool");
        return *symbol_at_adr(which);
    }

    jint int_at(int which) {
        assert(tag_at(which).is_int(), "Corrupted constant pool");
        return *int_at_adr(which);
    }

    jlong long_at(int which) {
        assert(tag_at(which).is_long(), "Corrupted constant pool");
        u8 tmp = Bytes::get_native_u8((address) &base()[which]);
        return *((jlong*)&tmp);
    }

    jfloat float_at(int which) {
        assert(tag_at(which).is_float(), "Corrupted constant pool");
        return *float_at_adr(which);
    }

    jdouble double_at(int which) {
        assert(tag_at(which).is_double(), "Corrupted constant pool");
        u8 tmp = Bytes::get_native_u8((address) &base()[which]);
        return *((jdouble*)&tmp);
    }

    jint class_at(int which) {
        assert(tag_at(which).is_klass(), "Corrupted constant pool");
        return *int_at_adr(which);
    }

    jint field_at(int which) {
        assert(tag_at(which).is_field(), "Corrupted constant pool");
        return *int_at_adr(which);
    }

    jint string_at(int which) {
        assert(tag_at(which).is_string(), "Corrupted constant pool");
        return *int_at_adr(which);
    }

    jint name_and_type_at(int which) {
        assert(tag_at(which).is_name_and_type(), "Corrupted constant pool");
        return *int_at_adr(which);
    }

    jint method_at(int which) {
        assert(tag_at(which).is_method(), "Corrupted constant pool");
        return *int_at_adr(which);
    }

    double get_int_by_int_ref(int which) {
        return int_at(which);
    }

    double get_float_by_float_ref(int which) {
        return float_at(which);
    }

    double get_long_by_long_ref(int which) {
        return long_at(which);
    }

    double get_double_by_double_ref(int which) {
        return double_at(which);
    }

    Symbol* get_string_by_string_ref(int which) {
        return symbol_at(string_at(which));
    }

    // 在常量池中通过class index查找class name
    Symbol* get_class_name_by_class_ref(int which) {
        return symbol_at(class_at(which));
    }

    /**
    * CONSTANT_Fieldref_info: class_index + nameAndType_index
    * @param index Fieldref 结构在常量池中的索引
    * @return 字段所属类的类名
    * */
    Symbol* get_class_name_by_field_ref(int which) {
        // 获取 Fieldref 在常量池中的信息(class_index + nameAndType_index)
        jint data = field_at(which);

        // 获取 class_index，int的前2个字节
        jint class_index = data >> 16;

        return get_class_name_by_class_ref(class_index);
    }

    Symbol* get_filed_name_by_field_ref(int which) {
        // 获取 Fieldref 在常量池中的信息(class_index + nameAndType_index)
        jint data = field_at(which);

        // 获取 nameAndType_index，int的后2个字节
        jint name_and_type_index = data & 0xFF;

        return get_name_by_name_and_type_ref(name_and_type_index);
    }

    Symbol* get_descriptor_by_field_ref(int which) {
        // 获取 Fieldref 在常量池中的信息(class_index + nameAndType_index)
        jint data = field_at(which);

        // 获取 nameAndType_index，int的后2个字节
        jint name_and_type_index = data & 0xFF;

        return get_descriptor_by_name_and_type_ref(name_and_type_index);
    }


    Symbol* get_name_by_name_and_type_ref(int which) {
        // 获取 NameAndType 在常量池中的信息(name_index + descriptor_index)
        jint data = name_and_type_at(which);

        // 获取 name_index，int的前2个字节
        int name_index = data >> 16;

        return symbol_at(name_index);
    }

    Symbol* get_descriptor_by_name_and_type_ref(int which) {
        // 获取 NameAndType 在常量池中的信息(name_index + descriptor_index)
        jint data = name_and_type_at(which);

        // 获取 descriptor_index，int的后2个字节
        int descriptor_index = data & 0xFF;

        return symbol_at(descriptor_index);
    }

    Symbol* get_method_name_by_method_ref(int index) {
        // 获取 Methodref 在常量池中的信息(class_index + nameAndType_index)
        jint data = name_and_type_at(index);

        // 获取 nameAndType_index，int的后2个字节
        jint name_and_type_index = data & 0xFF;

        return get_name_by_name_and_type_ref(name_and_type_index);
    }

    Symbol* get_method_descriptor_by_method_ref(int index) {
        // 获取 Methodref 在常量池中的信息(class_index + nameAndType_index)
        jint data = method_at(index);

        // 获取 nameAndType_index，int的后2个字节
        jint name_and_type_index = data & 0xFF;

        return get_descriptor_by_name_and_type_ref(name_and_type_index);
    }

    Symbol* get_class_name_by_method_ref(int index) {
        // 获取 Methodref 在常量池中的信息(class_index + nameAndType_index)
        jint data = method_at(index);

        // 获取 class_index，int的前2个字节
        jint class_index = data >> 16;

        return get_class_name_by_class_ref(class_index);
    }
};


#endif //JVMPLUS_CONSTANTPOOL_HPP
