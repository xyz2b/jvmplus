//
// Created by hongyujiao on 2021/7/13.
//

#ifndef MARK_CLEAR_MARK_COPY_H
#define MARK_CLEAR_MARK_COPY_H

#include "../../include/memory/memory_chunk.h"
#include "../../include/gc/interface/garbage_collect.h"

class MarkCopy: public GarbageCollect {
private:
    MemoryChunk *m_mem_chunk;

public:
    MarkCopy(MemoryChunk *chunk);
    ~MarkCopy();

public:
    MemoryChunk *get_mem_chunk();
    MarkCopy *set_mem_chunk(MemoryChunk *chunk);

private:
    void mark_step();
    void stop_the_world();
    void memory_sweep_step();

public:
    void run() override;

};


#endif //MARK_CLEAR_MARK_COPY_H
