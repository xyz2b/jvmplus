//
// Created by xyzjiao on 10/14/21.
//

#ifndef JVMPLUS_SOURCEFILEATTRIBUTE_HPP
#define JVMPLUS_SOURCEFILEATTRIBUTE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/attribute.hpp"

class SourceFileAttribute : public AttributeInfo {
private:
    u2 _source_file_index;
public:
    SourceFileAttribute(u2 attribute_name_index, u4 attribute_length, u2 source_file_index): AttributeInfo(attribute_name_index, attribute_length), _source_file_index(source_file_index) {}

    u2 get_source_file_index() { return _source_file_index; }
};

#endif //JVMPLUS_SOURCEFILEATTRIBUTE_HPP
