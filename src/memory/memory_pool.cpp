//
// Created by hongyujiao on 7/12/21.
//

#include "../../include/memory/memory_pool.h"

MemoryChunk *MemoryPool::new_chunk(uint mem_size) {
    MemoryChunk* mem_chunk = new MemoryChunk(mem_size, __FILE__, __LINE__);
    this->m_chunk.push_front(mem_chunk);
    return mem_chunk;
}

MemoryPool::~MemoryPool() {
    INFO_PRINT("[call destructor %s] release resources", __func__);
    free_chunks();
}

void MemoryPool::free_chunks() {
    for(MemoryChunk* chunk : m_chunk) {
        delete(chunk);
    }

    m_chunk.clear();
}

void MemoryPool::print_chunks() {
    INFO_PRINT("print chunk");

    for (MemoryChunk* chunk : m_chunk) {
        INFO_PRINT("[chunk]memory size %d bytes", chunk->get_size());
    }

    INFO_PRINT("print chunk end");
}
