//
// Created by xyzjiao on 10/11/21.
//

#include "instanceKlass.hpp"

instanceOop InstanceKlass::allocate_instance(KlassHandle k) {
    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(k, sizeof(instanceOopDesc));

    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* fields = new Hashmap<Symbol*, jobject, HashCode<const Symbol*>>(non_static_filed_count(k));
    i->set_fields(fields);

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

void InstanceKlass::link_class() {
    InstanceKlassHandle this_oop(this);
    link_class_impl(this_oop);
}

bool InstanceKlass::link_class_impl(InstanceKlassHandle this_oop) {
    return false;
}

int InstanceKlass::non_static_filed_count(KlassHandle k) {
    // InstanceMirrorKlass storage static fields
    InstanceKlass* instance = (InstanceKlass*)k();
    int non_static_filed_count = 0;
    while (instance != nullptr) {
        non_static_filed_count += instance->non_static_filed_count();
        instance = (InstanceKlass *)(instance->get_super_klass());
    }
    return non_static_filed_count;
}
