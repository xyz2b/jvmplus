//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_METHOD_HPP
#define JVMPLUS_METHOD_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/accessFlags.hpp"
#include "../../../share/vm/oops/symbol.hpp"
#include "../../../share/vm/oops/attribute.hpp"
#include "../../../share/vm/utilities/hashFun.hpp"
#include "../../../share/vm/utilities/hashMap.hpp"

class Klass;

class Method {
private:
    AccessFlags _access_flags;
    u2 _name_index;
    u2 _signature_index;
    // Back pointer to the Klass*
    Klass* _belong_klass;
    Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* _attributes;

public:
    Method(AccessFlags access_flags, u2 name_index, u2 signature_index, u2 attributes_count) : _access_flags(access_flags), _name_index(name_index), _signature_index(signature_index) {
        _attributes = new Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>(attributes_count);
    }

    AccessFlags access_flags() { return _access_flags; }
    u2 name_index() { return _name_index; }
    u2 signature_index() { return _signature_index; }

    void set_access_flags(AccessFlags accessFlags) { _access_flags = accessFlags; }
    void set_name_index(u2 name_index) { _name_index = name_index; }
    void set_signature_index(u2 signature_index) { _signature_index = signature_index; }

    AttributeInfo* get_attribute(Symbol* name) { return _attributes->get(name); }
    void put_attribute(Symbol* name, AttributeInfo* attribute) { _attributes->put(name, attribute); }

    void set_belong_klass(Klass* klass) { _belong_klass = klass; }
    Klass* get_belong_klass() { return _belong_klass; }
};


#endif //JVMPLUS_METHOD_HPP
