//
// Created by xyzjiao on 10/17/21.
//

#include "metaspace.hpp"

void SpaceManager::initialize() {
    _memory_pool = new MemoryPool;

    _metaspace = _memory_pool->new_chunk(81920);
}

MetaWord *SpaceManager::allocate_work(size_t size) {
    MemoryCell* obj = _metaspace->malloc(size);
    if (NULL == obj) {
        // 触发GC后再分配
    }

    return new MetaWord(obj);
}

MetaWord *SpaceManager::allocate(size_t size) {
    return allocate_work(size);
}


SpaceManager* Metaspace::_vsm;
void Metaspace::initialize() {
    _vsm = new SpaceManager;
    _vsm->initialize();
}

MetaWord *Metaspace::allocate(size_t size) {
    return _vsm->allocate(size);
}
