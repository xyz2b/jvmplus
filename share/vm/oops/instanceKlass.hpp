//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_INSTANCEKLASS_HPP
#define JVMPLUS_INSTANCEKLASS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/oops/constantPool.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/oops/field.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/utilities/hashMap.hpp"
#include "../../../share/vm/utilities/hashFun.hpp"
#include "../../../share/vm/oops/attribute.hpp"
#include "../../../share/vm/oops/symbol.hpp"
#include "../../../share/vm/oops/oopsHierachy.hpp"
#include "../../../share/vm/gc_interface/collectedHeap.hpp"

class InstanceKlass : public Klass {
private:
    u4      _magic;
    u2      _minor_version;         // minor version number of class file
    u2      _major_version;         // major version number of class file

    // Constant pool for this class.
    ConstantPool*   _constant_pool;

    AccessFlags _access_flags;

    u2 _this_class;
    u2 _super_class;

    u2 _interfaces_count;
    // Interface (Klass*s) this class declares to implement.
    Array<u2>*  _interfaces;

    u2 _fields_count;
    // Instance and static variable information.
    Array<FiledInfo*>*      _fields;

    u2 _methods_count;
    // Method Array;
    Array<Method*>* _methods;

    u2 _attributes_count;
    Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* _attributes;

    // 静态属性的数量，可以从解析完成的field中判断
    int _static_filed_count;
    int _static_field_size;

    // static fields，一个指针的大小可以存储任何类型的数据（long double 指针），按照常量池那种写法
    void* _static_fields[0];
public:
    int static_field_size() const            { return _static_field_size; }
    void set_static_field_size(int size)     { _static_field_size = size; }

    int static_filed_count() { return _static_filed_count; }
    void set_static_filed_count(int count) { _static_filed_count = count; }

    // 存储静态属性值的内存首地址
    intptr_t* static_fields_base() const { return (intptr_t*) (_static_fields); }

    bool is_within_bounds(int index) const {
        return 0 <= index && index < _fields_count;
    }

    intptr_t* obj_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (intptr_t*) &static_fields_base()[which];
    }

    // string
    Symbol** symbol_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (Symbol**) &static_fields_base()[which];
    }

    jbyte* byte_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jbyte*) &static_fields_base()[which];
    }

    jshort* short_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jshort*) &static_fields_base()[which];
    }

    jint* int_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jint*) &static_fields_base()[which];
    }

    jlong* long_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jlong*) &static_fields_base()[which];
    }

    jfloat* float_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jfloat*) &static_fields_base()[which];
    }

    jdouble* double_at_adr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (jdouble*) &static_fields_base()[which];
    }

    void obj_at_put(int which, intptr_t s) {
        *obj_at_adr(which) = s;
    }

    // string
    void symbol_at_put(int which, Symbol* s) {
        *symbol_at_adr(which) = s;
    }

    void int_at_put(int which, jint i) {
        *int_at_adr(which) = i;
    }

    void long_at_put(int which, jlong l) {
        Bytes::put_native_u8((address) long_at_adr(which), *((u8*)&l));
    }

    void float_at_put(int which, jfloat f) {
        *float_at_adr(which) = f;
    }

    void double_at_put(int which, jdouble d) {
        Bytes::put_native_u8((address) double_at_adr(which), *((u8*)&d));
    }

    intptr_t object_at(int which) {
        return *obj_at_adr(which);
    }

    Symbol* symbol_at(int which) {
        return *symbol_at_adr(which);
    }

    jint int_at(int which) {
        return *int_at_adr(which);
    }

    jlong long_at(int which) {
        u8 tmp = Bytes::get_native_u8((address) &static_fields_base()[which]);
        return *((jlong*)&tmp);
    }

    jfloat float_at(int which) {
        return *float_at_adr(which);
    }

    jdouble double_at(int which) {
        u8 tmp = Bytes::get_native_u8((address) &static_fields_base()[which]);
        return *((jdouble*)&tmp);
    }

    enum ClassState {
        allocated,
        loaded,
        linked,
        being_initialized,
        fully_initialized,
        initialization_error
    };

    InstanceKlass() {}
    InstanceKlass(u4 magic, u2 minor_version, u2 major_version, ConstantPool* constant_pool, AccessFlags access_flags,
                  u2 this_class, u2 super_class, u2 interfaces_count, Array<u2>* interfaces, u2 fields_count, Array<FiledInfo*>* fields,
                  u2 methods_count, Array<Method*>* methods, u2 attributes_count, Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* attributes, int static_filed_count):
                  _magic(magic), _minor_version(minor_version), _major_version(major_version), _constant_pool(constant_pool), _access_flags(access_flags),
                  _this_class(this_class), _super_class(super_class), _interfaces_count(interfaces_count), _interfaces(interfaces), _fields_count(fields_count),
                  _fields(fields), _methods_count(methods_count), _methods(methods), _attributes_count(attributes_count), _attributes(attributes), _static_filed_count(static_filed_count) {}

    static InstanceKlass* allocate_instance_klass() {
        return new InstanceKlass();
    }

    static InstanceKlass* allocate_instance_klass(u4 magic, u2 minor_version, u2 major_version, ConstantPool* constant_pool, AccessFlags access_flags,
                                                  u2 this_class, u2 super_class, u2 interfaces_count, Array<u2>* interfaces, u2 fields_count, Array<FiledInfo*>* fields,
                                                  u2 methods_count, Array<Method*>* methods, u2 attributes_count, Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* attributes, int static_filed_count) {

        size_t extra_memory_size = (fields_count) * sizeof(void*);
        return new (extra_memory_size) InstanceKlass(magic, minor_version, major_version, constant_pool, access_flags, this_class, super_class, interfaces_count, interfaces, fields_count, fields, methods_count, methods, attributes_count, attributes, static_filed_count);
    }

    void set_magic(u4 magic) { _magic = magic; }
    u4 get_magic() { return _magic; }

    void set_minor_version(u2 minor_version) { _minor_version = minor_version; }
    u2 get_minor_version() { return _minor_version; }

    void set_major_version(u2 major_version) { _major_version = major_version; }
    u2 get_major_version() { return _major_version; }

    void set_constant_pool(ConstantPool* constant_pool) { _constant_pool = constant_pool; }
    ConstantPool* get_constant_pool() { return _constant_pool; }

    void set_access_flags(AccessFlags access_flags) { _access_flags = access_flags; }
    AccessFlags get_access_flags() { return _access_flags; }

    void set_this_class(u2 this_class) { _this_class = this_class; }
    u2 get_this_class() { return _this_class; }

    void set_super_class(u2 super_class) { _super_class = super_class; }
    u2 get_super_class() { return _super_class; }

    void set_interfaces_count(u2 interfaces_count) { _interfaces_count = interfaces_count; }
    u2 get_interfaces_count() { return _interfaces_count; }

    void set_interfaces(Array<u2>* interfaces) { _interfaces = interfaces; }
    Array<u2>* get_interfaces() { return _interfaces; }

    void set_fields_count(u2 fields_count) { _fields_count = fields_count; }
    u2 get_fields_count() { return _fields_count; }

    void set_fields(Array<FiledInfo*>* fields) { _fields = fields; }
    Array<FiledInfo*>* get_fields() { return _fields; }

    void set_methods_count(u2 methods_count) { _methods_count = methods_count; }
    u2 get_methods_count() { return _methods_count; }

    void set_methods(Array<Method*>* methods) { _methods = methods; }
    Array<Method*>* get_methods() { return _methods; }

    void set_attributes_count(u2 attributes_count) { _attributes_count = attributes_count; }
    u2 get_attributes_count() { return _attributes_count; }

    void set_attributes(Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* attributes) { _attributes = attributes; }
    Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* get_attributes() { return _attributes; }

    instanceOop allocate_instance(KlassHandle k, size_t size);
};


#endif //JVMPLUS_INSTANCEKLASS_HPP
