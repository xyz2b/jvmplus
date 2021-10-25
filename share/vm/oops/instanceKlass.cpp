//
// Created by xyzjiao on 10/11/21.
//

#include "instanceKlass.hpp"

instanceOop InstanceKlass::allocate_instance(KlassHandle k) {
    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(k, sizeof(instanceOopDesc));
    return i;
}

FiledInfo* InstanceKlass::find_field(Symbol* field_name, Symbol* descriptor_name) {
    for (int index = 0; index < _fields->size(); index++) {
        FiledInfo* f = _fields->get(index);

        if (*(f->name()) == field_name && *(f->descriptor()) == descriptor_name) {
            return f;
        }
    }

    return nullptr;
}