//
// Created by xyzjiao on 10/21/21.
//

#include "InstanceMirrorKlass.hpp"

instanceOop InstanceMirrorKlass::allocate_instance(KlassHandle k) {
    int size = instance_size(k);

    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(k, size);

    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* fields = new Hashmap<Symbol*, jobject, HashCode<const Symbol*>>(static_filed_count(k));
    i->set_fields(fields);

    return i;
}

int InstanceMirrorKlass::instance_size(KlassHandle k) {
    return sizeof(oopDesc);
}

int InstanceMirrorKlass::static_filed_count(KlassHandle k) {
    // InstanceMirrorKlass storage static fields
    InstanceKlass* instance = (InstanceKlass*)k();
    int static_filed_count = instance->static_filed_count();
    return static_filed_count;
}
