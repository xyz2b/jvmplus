//
// Created by xyzjiao on 10/14/21.
//

#ifndef JVMPLUS_EXCEPTIONATTRIBUTE_H
#define JVMPLUS_EXCEPTIONATTRIBUTE_H

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/attribute.hpp"
#include "../../../share/vm/utilities/array.hpp"

class ExceptionAttribute : public AttributeInfo {
private:
    u2 _number_of_exceptions;
    Array<u2>* _exception_index_table;
public:
    ExceptionAttribute(u2 attribute_name_index, u4 attribute_length, u2 number_of_exceptions): AttributeInfo(attribute_name_index, attribute_length), _number_of_exceptions(number_of_exceptions) {}

    void set_exception_index_table(Array<u2>* exception_index_table) {
        _exception_index_table = exception_index_table;
    }

    u2 get_number_of_exceptions() { return _number_of_exceptions; }

    u2 get_exception_index_from_table(int index) { return _exception_index_table->get(index); }
    void add_exception_index_to_table(u2 exception_index) { _exception_index_table->add(exception_index); }
};

#endif //JVMPLUS_EXCEPTIONATTRIBUTE_H
