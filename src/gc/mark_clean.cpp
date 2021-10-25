//
// Created by hongyujiao on 7/12/21.
//

#include <cstdlib>
#include "../../include/gc/mark_clean.h"

MarkClean::MarkClean(MemoryChunk *mem_chunk): m_mem_chunk(mem_chunk)
{

}

MarkClean::~MarkClean()
{
    DEBUG_PRINT("[call destructor %s] release resources", __func__);
}

MemoryChunk *MarkClean::get_mem_chunk() {
    return m_mem_chunk;
}

MarkClean *MarkClean::set_mem_chunk(MemoryChunk *chunk) {
    m_mem_chunk = chunk;
    return this;
}

void MarkClean::mark_step() {
    DEBUG_PRINT("start mark step")

    list<MemoryCell *> *used_table = get_mem_chunk()->get_used_table();
    for (MemoryCell* cell : *used_table) {
        if (cell->get_size() == 4) {
            cell->set_mark(true);
        }
    }
}

void MarkClean::clean_step() {
    DEBUG_PRINT("start clean step");

    bool flag = false;

    list<MemoryCell*>::iterator iterator;
    for (iterator = get_mem_chunk()->get_used_table()->begin(); iterator != get_mem_chunk()->get_used_table()->end();) {
        MemoryCell* cell = *iterator;
        cell->to_string("check mark");

        // release not mark cell
        if(!cell->get_mark()) {
            DEBUG_PRINT("not mark")

            // remove not mark cell from used_table
            // list::erase return is an iterator pointing to the element that followed the last element erased by the function call
            iterator = get_mem_chunk()->get_used_table()->erase(iterator);

            // add not mark cell to available_table
            get_mem_chunk()->get_available_table()->push_front(new MemoryCell(cell->get_start(), cell->get_size()));

            // desc used cell num
            get_mem_chunk()->desc_used_cell_num(cell->get_size());

            // release object
            delete cell;

            flag = true;
        } else {
            DEBUG_PRINT("is mark, can't release");
            iterator++;
        }
    }

    if (!flag) {
        ERROR_PRINT("[after gc]there is no reclaimable memory, memory cannot be allocated, and Full GC is executed");
        exit(-1);
    }

}

void MarkClean::run() {
    DEBUG_PRINT("mark clean start");
    if (get_mem_chunk() == nullptr) {
        ERROR_PRINT("chunk is null");
        exit(-1);
    }

    mark_step();
    clean_step();
    get_mem_chunk()->print_all_table();

    DEBUG_PRINT("mark clean stop");
}
