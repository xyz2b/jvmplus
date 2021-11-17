//
// Created by xyzjiao on 10/17/21.
//

#include "collectedHeap.hpp"
#include "../../../share/vm/memory/genCollectedHeap.hpp"
#include "../../../share/vm/memory/universe.hpp"

oop CollectedHeap::obj_allocate(KlassHandle k, size_t size) {
    HeapWord* obj = common_mem_allocate_init(k, size);
    post_allocation_setup_obj(k, obj, size);
    return (oop) (obj->value());
}

HeapWord *CollectedHeap::common_mem_allocate_init(KlassHandle k, size_t size) {
    HeapWord* obj = common_mem_allocate_noinit(k, size);
    return obj;
}

HeapWord *CollectedHeap::common_mem_allocate_noinit(KlassHandle k, size_t size) {
    HeapWord* result = nullptr;

    // TODO: 先尝试从TLAB中分配内存，后面实现

    // 直接去堆中-新生代分配内存
    // 获取堆
    GenCollectedHeap* heap = (GenCollectedHeap*) Universe::heap();
    // 在堆中新生代分配内存
    result = heap->mem_allocate(size);
    if (result == nullptr) {

    }

    return result;
}

void CollectedHeap::post_allocation_setup_obj(KlassHandle klass, HeapWord *obj, size_t size) {
    post_allocation_setup_common(klass, obj);
}

void CollectedHeap::post_allocation_setup_common(KlassHandle klass, HeapWord *obj) {
    post_allocation_install_obj_klass(klass, (oop)(obj->value()));
    post_allocation_setup_no_klass_install(klass, (oop)(obj->value()));
}

void CollectedHeap::post_allocation_install_obj_klass(KlassHandle klass, oop obj) {
    obj->set_klass(klass());
}

void CollectedHeap::post_allocation_setup_no_klass_install(KlassHandle klass, oop obj) {

}

