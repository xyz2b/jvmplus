//
// Created by xyzjiao on 10/11/21.
//

#include "instanceKlass.hpp"

instanceOop InstanceKlass::allocate_instance(size_t size) {
    instanceOop i = (instanceOop) CollectedHeap::obj_allocate(size);
    return i;
}
