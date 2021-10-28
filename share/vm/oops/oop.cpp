//
// Created by xyzjiao on 10/17/21.
//

#include "oop.hpp"
#include "../../../share/vm/prims/unsafe.cpp"

jobject oopDesc::get_field(Symbol* class_name, Symbol* field_name) {
    const char * contact = "#";
    Symbol* field_full_name = *(*class_name + contact) + field_name;
    INFO_PRINT("field full name: %s", field_full_name->as_C_string());

    jobject field_val = _fields->get(field_full_name);

    if (field_val == nullptr) {
        ERROR_PRINT("not find field: %s", field_full_name->as_C_string());
        return nullptr;
    }

    return field_val;
}

bool oopDesc::find_field(Symbol* class_name, Symbol* field_name) {
    const char * contact = "#";
    Symbol* field_full_name = *(*class_name + contact) + field_name;
    INFO_PRINT("field full name: %s", field_full_name->as_C_string());

    return _fields->contains(field_full_name);
}

void oopDesc::put_field(Symbol *class_name, Symbol *field_name, jobject value) {
    const char * contact = "#";
    Symbol* field_full_name = *(*class_name + contact) + field_name;

    INFO_PRINT("field_full_name: %s", field_full_name->as_C_string());
    _fields->put(field_full_name, value);
    INFO_PRINT("field put success");
}
