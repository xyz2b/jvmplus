//
// Created by hongyujiao on 7/12/21.
//

#ifndef MARK_CLEAR_MARK_CLEAN_H
#define MARK_CLEAR_MARK_CLEAN_H


#include "../../include/gc/interface/garbage_collect.h"
#include "../../include/memory/memory_chunk.h"

class MarkClean: public GarbageCollect {
private:
    MemoryChunk *m_mem_chunk;

public:
    MarkClean(MemoryChunk *mem_chunk);
    ~MarkClean();

public:
    MemoryChunk *get_mem_chunk();
    MarkClean *set_mem_chunk(MemoryChunk *chunk);

private:
    void mark_step();
    void clean_step();

public:
    void run() override;
};


#endif //MARK_CLEAR_MARK_CLEAN_H
