//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_GENCOLLECTEDHEAP_HPP
#define JVMPLUS_GENCOLLECTEDHEAP_HPP

#include "../../../share/vm/memory/sharedHeap.hpp"
#include "../../../share/vm/memory/genCollectorPolicy.hpp"
#include "../../../include/memory/memory_chunk.h"
#include "../../../include/memory/memory_pool.h"
#include "../.././include/gc/gc_factory.h"

class GenCollectedHeap : public SharedHeap {
protected:
    static GenCollectedHeap* _gch;

private:
    GenCollectorPolicy* _gen_policy;

    int _n_gens;
    MemoryChunk** _gens;
    MemoryPool* _memory_pool;

public:
    GenCollectedHeap(GenCollectorPolicy* policy);

    void initialize();

    void post_initialize();

    HeapWord* mem_allocate(size_t size);
    HeapWord* attempt_allocation(size_t size);

    static GenCollectedHeap* heap();

public:
    MemoryChunk* get_gen(int i) const {
        return _gens[i];
    }

    GenCollectorPolicy* gen_policy() const {
        return _gen_policy;
    }

    CollectorPolicy* collector_policy() const {
        return (CollectorPolicy *) gen_policy();
    }

public:
    HeapWord* satisfy_failed_allocation(std::size_t size);

    void do_collection(bool is_new);
    void do_full_collection();
};


#endif //JVMPLUS_GENCOLLECTEDHEAP_HPP
