//
// Created by ziya on 2021/10/13.
//

#ifndef JVMPLUS_CONSTANTVALUEATTRIBUTE_HPP
#define JVMPLUS_CONSTANTVALUEATTRIBUTE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/attribute.hpp"

class ConstantValueAttribute : public AttributeInfo {
private:
    u2 _constant_value_index;
public:
    ConstantValueAttribute(u2 attribute_name_index, u4 attribute_length, u2 constant_value_index) : AttributeInfo(attribute_name_index, attribute_length) {
        _constant_value_index = constant_value_index;
    }

    void set_constant_value_index(u2 constant_value_index) { _constant_value_index = constant_value_index; }
    u2 get_constant_value_index() { return _constant_value_index; }
};

#endif //JVMPLUS_CONSTANTVALUEATTRIBUTE_HPP
