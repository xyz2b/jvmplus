//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_COLLECTEDHEAP_HPP
#define JVMPLUS_COLLECTEDHEAP_HPP

#include "../../../share/vm/oops/oopsHierachy.hpp"
#include "../../../share/vm/utilities/globalDefinitions.hpp"


class CollectedHeap {
public:
    static oop obj_allocate(size_t size);

    static HeapWord* common_mem_allocate_init(size_t size);

    static HeapWord* common_mem_allocate_noinit(size_t size);
};


#endif //JVMPLUS_COLLECTEDHEAP_HPP
