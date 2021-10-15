//
// Created by hongyujiao on 7/12/21.
//

#include "../../include/gc/gc_factory.h"
#include "../../include/memory/memory_chunk.h"
#include "../../include/gc/mark_clean.h"
#include "../../include/gc/mark_compact.h"
#include "../../include/gc/mark_copy.h"

GC_STATUS GCFactory::get_status() {
    return m_status;
}

GCFactory *GCFactory::set_status(GC_STATUS status) {
    m_status = status;
    return this;
}

void GCFactory::minor_run(GC_Type type, MemoryChunk *mem_chunk) {
    INFO_PRINT("%s", __func__);

    if (get_status() == GC_STATUS_WORKING) {
        WARNING_PRINT("gc is running!!!");
        return;
    }

    set_status(GC_STATUS_WORKING);

    switch (type) {
        case GC_MARK_CLEAN:
            (new MarkClean(mem_chunk))->run();
            break;
        case GC_MARK_COLLECT:
            (new MarkCompact(mem_chunk))->run();
            break;
        case GC_MARK_COPY:
            (new MarkCopy(mem_chunk))->run();
            break;
        case GC_G1:
            break;
    }

    set_status(GC_STATUS_CLOSE);
}

void GCFactory::major_run(GC_Type type, MemoryChunk *mem_chunk) {

}

void GCFactory::full_run(GC_Type type, MemoryPool *mem_pool) {

}
