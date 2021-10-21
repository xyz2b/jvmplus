//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_COLLECTEDHEAP_HPP
#define JVMPLUS_COLLECTEDHEAP_HPP

#include "../../../share/vm/oops/oopsHierachy.hpp"
#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/runtime/handles.hpp"

class CollectedHeap {
public:
    static oop obj_allocate(KlassHandle k, size_t size);

    static HeapWord* common_mem_allocate_init(KlassHandle k, size_t size);

    static HeapWord* common_mem_allocate_noinit(KlassHandle k, size_t size);

    static void post_allocation_setup_obj(KlassHandle handle, HeapWord *pWord, size_t size);

    static void post_allocation_setup_common(KlassHandle handle, HeapWord *pWord);

    static void post_allocation_install_obj_klass(KlassHandle handle, oopDesc *pDesc);
};


#endif //JVMPLUS_COLLECTEDHEAP_HPP
