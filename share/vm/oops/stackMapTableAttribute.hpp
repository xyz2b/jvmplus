//
// Created by xyzjiao on 10/14/21.
//

#ifndef JVMPLUS_STACKMAPTABLEATTRIBUTE_HPP
#define JVMPLUS_STACKMAPTABLEATTRIBUTE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/attribute.hpp"

class StackMapTableAttribute : public AttributeInfo {
private:
    u2 _number_of_entries;

public:
    StackMapTableAttribute(u2 attribute_name_index, u4 attribute_length): AttributeInfo(attribute_name_index, attribute_length) {}

    u2 get_number_of_entries() { return _number_of_entries; }
};

#endif //JVMPLUS_STACKMAPTABLEATTRIBUTE_HPP
