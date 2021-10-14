//
// Created by xyzjiao on 10/14/21.
//

#ifndef JVMPLUS_LINENUMBERTABLEATTRIBUTE_HPP
#define JVMPLUS_LINENUMBERTABLEATTRIBUTE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/oops/attribute.hpp"

class LineNumber {
private:
    u2 _start_pc;
    u2 _line_number;
public:
    LineNumber(u2 start_pc, u2 line_number): _start_pc(start_pc), _line_number(line_number) {}

    u2 get_start_pc() { return _start_pc; }
    u2 get_line_number() { return _line_number; };
};

class LineNumberTableAttribute: public AttributeInfo {
private:
    u2 _line_number_table_length;
    Array<LineNumber*>* _line_number_table;
public:
    LineNumberTableAttribute(u2 attribute_name_index, u4 attribute_length, u2 line_number_table_length):
    AttributeInfo(attribute_name_index, attribute_length), _line_number_table_length(line_number_table_length) {}

    void set_line_number_table(Array<LineNumber*>* line_number_table) {
        _line_number_table = line_number_table;
    }

    u2 get_line_number_table_length() { return _line_number_table_length; }

    LineNumber* get_line_number_from_table(int index) { return _line_number_table->get(index); }
    void add_line_number_to_table(LineNumber* line_number) { _line_number_table->add(line_number); }
};

#endif //JVMPLUS_LINENUMBERTABLEATTRIBUTE_HPP
