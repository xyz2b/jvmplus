//
// Created by xyzjiao on 10/21/21.
//

#include "InstanceMirrorKlass.hpp"

instanceOop InstanceMirrorKlass::allocate_instance(KlassHandle k) {
    int size = instance_size(k);

    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(k, size);

    return i;
}

int InstanceMirrorKlass::instance_size(KlassHandle k) {

    return sizeof(instanceOopDesc) + ((InstanceKlass*)k())->static_filed_count();
}
