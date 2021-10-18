//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_METASPACE_HPP
#define JVMPLUS_METASPACE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/adlc/cHeapObj.hpp"
#include "../../../include/memory/memory_pool.h"

// 管理metaspace元空间
class SpaceManager : public CHeapObj {
private:
    MemoryChunk* _metaspace;
    MemoryPool* _memory_pool;
public:
    void initialize();

    MetaWord* allocate_work(size_t size);
    MetaWord* allocate(size_t size);
};


class Metaspace : public CHeapObj {
private:
    static SpaceManager* _vsm;
public:
    static void initialize();

    static MetaWord* allocate(size_t size);
};


#endif //JVMPLUS_METASPACE_HPP
