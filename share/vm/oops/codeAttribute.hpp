//
// Created by xyzjiao on 10/13/21.
//

#ifndef JVMPLUS_CODEATTRIBUTE_HPP
#define JVMPLUS_CODEATTRIBUTE_HPP

#include "../../../share/vm/oops/attribute.hpp"
#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"

class ExceptionHandler : public Metadata {
private:
    u2 _start_pc;
    u2 _end_pc;

    u2 _handler_pc;

    u2 _catch_type;
public:
    ExceptionHandler(u2 start_pc, u2 end_pc, u2 handler_pc, u2 catch_Type) : _start_pc(start_pc), _end_pc(end_pc), _handler_pc(handler_pc), _catch_type(catch_Type) {}

    u2 get_start_pc() { return _start_pc; }
    u2 get_end_pc() { return _end_pc; }
    u2 get_handler_pc() { return _handler_pc; }
    u2 get_catch_type() { return _catch_type; }
};

class CodeAttribute : public AttributeInfo {
private:
    u2 _max_stack;
    u2 _max_locals;

    u4 _code_length;

    u2 _exception_table_length;
    Array<ExceptionHandler*>* _exception_tables;

    u2 _attribute_count;
    Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* _attributes;

    jbyte   _code[0];

public:
    CodeAttribute(u2 attribute_name_index, u4 attribute_length, u2 attribute_count,
                  u2 max_stack, u2 max_locals, u4 code_length, u2 exception_table_length):
                  AttributeInfo(attribute_name_index, attribute_length), _attribute_count(attribute_count),
                  _max_stack(max_stack), _max_locals(max_locals), _code_length(code_length), _exception_table_length(exception_table_length) {
        _attributes = new Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>(attribute_count);
    }

    address code_base() const { return (address) &_code[0]; }
    address code_end() const { return code_base() + code_size(); }
    u4 code_size() const { return _code_length; }

    void set_code(address code) {
        if (code_size() > 0) {
            memcpy(code_base(), code, code_size());
        }
    }

    void set_exception_tables(Array<ExceptionHandler*>* exception_table) {
        _exception_tables = exception_table;
    }

    u2 get_max_stack() { return _max_stack; }
    u2 get_max_locals() { return _max_locals; }
    u2 get_exception_table_length() { return _exception_table_length; }
    u2 get_attribute_count() { return _attribute_count; }

    ExceptionHandler* get_exception_form_table(int index) { return _exception_tables->get(index); }
    void add_exception_to_table(ExceptionHandler* exception_handler) { _exception_tables->add(exception_handler); }

    AttributeInfo* get_attribute(Symbol* name) { return _attributes->get(name); }
    void put_attribute(Symbol* name, AttributeInfo* attribute) { _attributes->put(name, attribute); }
};



#endif //JVMPLUS_CODEATTRIBUTE_HPP
