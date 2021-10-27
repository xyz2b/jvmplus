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

    Klass* _super_klass;

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

    int _non_static_filed_count;

    u1 _init_state;
public:
    int static_field_size() const            { return _static_field_size; }
    void set_static_field_size(int size)     { _static_field_size = size; }

    int static_filed_count() { return _static_filed_count; }
    void set_static_filed_count(int count) { _static_filed_count = count; }

    int non_static_filed_count() { return _non_static_filed_count; }
    void set_non_static_filed_count(int count) { _non_static_filed_count = count; }

    FiledInfo* find_field(Symbol* field_name, Symbol* descriptor_name);

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
                  u2 methods_count, Array<Method*>* methods, u2 attributes_count, Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* attributes, int static_filed_count, int non_static_filed_count):
                  Klass(nullptr), _magic(magic), _minor_version(minor_version), _major_version(major_version), _constant_pool(constant_pool), _access_flags(access_flags),
                  _this_class(this_class), _super_class(super_class), _interfaces_count(interfaces_count), _interfaces(interfaces), _fields_count(fields_count),
                  _fields(fields), _methods_count(methods_count), _methods(methods), _attributes_count(attributes_count), _attributes(attributes), _static_filed_count(static_filed_count), _non_static_filed_count(non_static_filed_count) {}

    static InstanceKlass* allocate_instance_klass() {
        return new InstanceKlass();
    }

    static InstanceKlass* allocate_instance_klass(u4 magic, u2 minor_version, u2 major_version, ConstantPool* constant_pool, AccessFlags access_flags,
                                                  u2 this_class, u2 super_class, u2 interfaces_count, Array<u2>* interfaces, u2 fields_count, Array<FiledInfo*>* fields,
                                                  u2 methods_count, Array<Method*>* methods, u2 attributes_count, Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* attributes, int static_filed_count, int non_static_filed_count) {

        size_t extra_memory_size = (fields_count) * sizeof(void*);
        return new (extra_memory_size) InstanceKlass(magic, minor_version, major_version, constant_pool, access_flags, this_class, super_class, interfaces_count, interfaces, fields_count, fields, methods_count, methods, attributes_count, attributes, static_filed_count, non_static_filed_count);
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

    void set_super_klass(Klass* klass) { _super_klass = klass; }
    Klass* get_super_klass() { return _super_klass; }

    void set_init_state(ClassState state) { _init_state = (u1)state; }

    // initialization state
    bool is_loaded() const                   { return _init_state >= loaded; }
    bool is_linked() const                   { return _init_state >= linked; }
    bool is_initialized() const              { return _init_state == fully_initialized; }
    bool is_not_initialized() const          { return _init_state <  being_initialized; }
    bool is_being_initialized() const        { return _init_state == being_initialized; }
    bool is_in_error_state() const           { return _init_state == initialization_error; }

    ClassState  init_state()                 { return (ClassState)_init_state; }

    instanceOop allocate_instance(KlassHandle k);
    void put_field_to_oop(instanceOop child_oop, InstanceKlassHandle this_klass);

    int non_static_filed_count(KlassHandle k);
    void link_class();
    bool link_class_impl(InstanceKlassHandle this_oop);

    void initialize();
    void initialize_impl(InstanceKlassHandle this_oop);
};


#endif //JVMPLUS_INSTANCEKLASS_HPP
