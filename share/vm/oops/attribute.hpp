//
// Created by ziya on 2021/10/13.
//

#ifndef JVMPLUS_ATTRIBUTE_HPP
#define JVMPLUS_ATTRIBUTE_HPP

#define JVM_ATTRIBUTE_ConstantValue "ConstantValue"

class AttributeInfo {
private:
    u2 _attribute_name_index;
    u4 _attribute_length;

public:
    AttributeInfo(u2 attribute_name_index, u4 attribute_length) : _attribute_name_index(attribute_name_index), _attribute_length(attribute_length) {}

    void set_attribute_length(u4 attribute_length) { _attribute_length = attribute_length; }
    void set_attribute_name_index(u2 attribute_name_index) { _attribute_name_index = attribute_name_index; }

    u2 get_attribute_name_index() { return _attribute_name_index; }
    u4 get_attribute_length() { return _attribute_length; }
};

#endif //JVMPLUS_ATTRIBUTE_HPP
