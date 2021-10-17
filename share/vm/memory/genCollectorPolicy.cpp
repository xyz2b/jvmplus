//
// Created by xyzjiao on 10/17/21.
//

#include "genCollectorPolicy.hpp"
#include "../../../share/vm/memory/genCollectedHeap.hpp"

HeapWord* GenCollectorPolicy::mem_allocate_work(size_t size) {
    GenCollectedHeap *gch = GenCollectedHeap::heap();

    // 新生代分配
    MemoryChunk* gen0 = gch->get_gen(0);

    MemoryCell* obj = gen0->malloc(size);
    if (NULL == obj) {
        // 触发GC后再分配
    }

    return new HeapWord(obj);
}

HeapWord* GenCollectorPolicy::satisfy_failed_allocation(size_t size) {
    return NULL;
}
