//
// Created by xyzjiao on 10/13/21.
//

#ifndef JVMPLUS_LOCALVARIABLETABLEATTRIBUTE_HPP
#define JVMPLUS_LOCALVARIABLETABLEATTRIBUTE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/oops/attribute.hpp"

class LocalVariable {
private:
    u2 _start_pc;
    u2 _length;

    u2 _name_index;

    u2 _descriptor_index;

    u2 _index;

public:
    LocalVariable(u2 start_pc, u2 length, u2 name_index, u2 descriptor_index, u2 index) :_start_pc(start_pc), _length(length), _name_index(name_index), _descriptor_index(descriptor_index), _index(index) {}

    u2 get_start_pc() { return _start_pc; }
    u2 get_length() { return _length; }
    u2 get_name_index() { return _name_index; }
    u2 get_descriptor_index() { return _descriptor_index; }
    u2 get_index() { return _index; }
};

class LocalVariableTableAttribute : public AttributeInfo {
private:
    u2 _local_variable_table_length;
    Array<LocalVariable*>* _local_variable_table;
public:
    LocalVariableTableAttribute(u2 attribute_name_index, u4 attribute_length, u2 local_variable_table_length):
    AttributeInfo(attribute_name_index, attribute_length), _local_variable_table_length(local_variable_table_length) {}

    void set_local_variable_table(Array<LocalVariable*>* local_variable_table) {
        _local_variable_table = local_variable_table;
    }

    u2 get_local_variable_table_length() { return _local_variable_table_length; }

    LocalVariable* get_local_variable_from_table(int index) { return _local_variable_table->get(index); }
    void add_local_variable_to_table(LocalVariable* local_variable) { _local_variable_table->add(local_variable); }
};

#endif //JVMPLUS_LOCALVARIABLETABLEATTRIBUTE_HPP
