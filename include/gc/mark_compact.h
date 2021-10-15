//
// Created by hongyujiao on 7/12/21.
//

#ifndef MARK_CLEAR_MARK_COMPACT_H
#define MARK_CLEAR_MARK_COMPACT_H

#include "../../include/gc/interface/garbage_collect.h"
#include "../../include/memory/memory_chunk.h"

class MarkCompact: public GarbageCollect {
private:
    MemoryChunk* m_mem_chunk;
public:
    MarkCompact(MemoryChunk *chunk);
    ~MarkCompact();

private:
    void memory_compact_before();
    void memory_compact_after();

public:
    MemoryChunk *get_mem_chunk();
    MarkCompact *set_mem_chunk(MemoryChunk *chunk);

private:
    void mark_step();
    void clean_step();
    void memory_compact_step();
    void data_compact_step();

public:
    void run() override;
};


#endif //MARK_CLEAR_MARK_COMPACT_H
