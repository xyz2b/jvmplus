//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_OOP_HPP
#define JVMPLUS_OOP_HPP

#include <jni.h>
#include "../../../share/vm/utilities/hashMap.hpp"

class Klass;

class oopDesc {
private:
    Klass* _klass;
    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* _fields;

public:
    oopDesc(Klass* klass) : _klass(klass) {}

    void set_fields(Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* fields) { _fields = fields; }

    Klass* klass() const { return _klass; }
    void set_klass(Klass* klass) { _klass = klass; }


    jobject get_field(Symbol* class_name, Symbol* field_name);
    void add_field(Symbol* class_name, Symbol* field_name, jobject value);
};


#endif //JVMPLUS_OOP_HPP
