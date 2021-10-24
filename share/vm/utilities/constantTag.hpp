//
// Created by xyzjiao on 10/12/21.
//

#ifndef JVMPLUS_CONSTANTTAG_HPP
#define JVMPLUS_CONSTANTTAG_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/prims/jvm.h"

class ConstantTag {
private:
    jbyte _tag;
public:
    bool is_klass() const             { return _tag == JVM_CONSTANT_Class; }
    bool is_field () const            { return _tag == JVM_CONSTANT_Fieldref; }
    bool is_method() const            { return _tag == JVM_CONSTANT_Methodref; }
    bool is_interface_method() const  { return _tag == JVM_CONSTANT_InterfaceMethodref; }
    bool is_string() const            { return _tag == JVM_CONSTANT_String; }
    bool is_int() const               { return _tag == JVM_CONSTANT_Integer; }
    bool is_float() const             { return _tag == JVM_CONSTANT_Float; }
    bool is_long() const              { return _tag == JVM_CONSTANT_Long; }
    bool is_double() const            { return _tag == JVM_CONSTANT_Double; }
    bool is_name_and_type() const     { return _tag == JVM_CONSTANT_NameAndType; }
    bool is_utf8() const              { return _tag == JVM_CONSTANT_Utf8; }

    bool is_invalid() const           { return _tag == JVM_CONSTANT_Invalid; }

    ConstantTag() {
        _tag = JVM_CONSTANT_Invalid;
    }

    ConstantTag(u1 tag) {
        _tag = tag;
    }

    jbyte value() const { return _tag; }
};


#endif //JVMPLUS_CONSTANTTAG_HPP
