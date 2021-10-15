//
// Created by hongyujiao on 7/12/21.
//

#ifndef MARK_CLEAR_GC_FACTORY_H
#define MARK_CLEAR_GC_FACTORY_H

#include "../../include/memory/memory_chunk.h"
#include "../../include/memory/memory_pool.h"

typedef enum {
    GC_STATUS_CLOSE,
    GC_STATUS_WORKING
} GC_STATUS;

class GCFactory {
private:
    GC_STATUS m_status;
public:
    // Singleton
    static GCFactory* get_instance() {
        static GCFactory gcFactory;
        return &gcFactory;
    }

public:
    GC_STATUS get_status();
    GCFactory* set_status(GC_STATUS status);

public:
    void minor_run(GC_Type type, MemoryChunk *mem_chunk);
    void major_run(GC_Type type, MemoryChunk *mem_chunk);
    void full_run(GC_Type type, MemoryPool *mem_pool);
};


#endif //MARK_CLEAR_GC_FACTORY_H
