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
    // 静态属性存储在各个类的mirror class的oop对象中
    // 而子父类的非静态属性都存储在子类对象oop中，因为不会创建父类对象
    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* _fields;

public:
    oopDesc(Klass* klass) : _klass(klass) {}

    void set_fields(Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* fields) { _fields = fields; }
    size_t fields_size() { return _fields->size(); }

    Klass* klass() const { return _klass; }
    void set_klass(Klass* klass) { _klass = klass; }


    jobject get_field(Symbol* class_name, Symbol* field_name);
    void put_field(Symbol* class_name, Symbol* field_name, jobject value);
    bool find_field(Symbol* class_name, Symbol* field_name);
};


#endif //JVMPLUS_OOP_HPP
