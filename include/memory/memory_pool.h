//
// Created by hongyujiao on 7/12/21.
//

#ifndef MARK_CLEAR_MEMORY_POOL_H
#define MARK_CLEAR_MEMORY_POOL_H

#include <list>
#include "memory_chunk.h"

using namespace std;

class MemoryPool {
private:
    list<MemoryChunk *> m_chunk;

public:
    ~MemoryPool();

public:
    MemoryChunk* new_chunk(uint mem_size);

public:
    void print_chunks();
    void free_chunks();
};


#endif //MARK_CLEAR_MEMORY_POOL_H
