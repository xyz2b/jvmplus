//
// Created by xyzjiao on 10/17/21.
//

#include "collectedHeap.hpp"
#include "../../../share/vm/memory/genCollectedHeap.hpp"
#include "../../../share/vm/memory/universe.hpp"

oop CollectedHeap::obj_allocate(size_t size) {
    HeapWord* obj = common_mem_allocate_init(size);
    return (oop) (obj->value());
}

HeapWord *CollectedHeap::common_mem_allocate_init(size_t size) {
    HeapWord* obj = common_mem_allocate_noinit(size);
    return obj;
}

HeapWord *CollectedHeap::common_mem_allocate_noinit(size_t size) {
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

