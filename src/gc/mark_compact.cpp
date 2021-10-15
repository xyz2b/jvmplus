//
// Created by hongyujiao on 7/12/21.
//

#include <cstdlib>
#include "../../include/gc/mark_compact.h"

MarkCompact::MarkCompact(MemoryChunk *chunk): m_mem_chunk(chunk) {

}

MarkCompact::~MarkCompact() {
    INFO_PRINT("[call destructor %s] release resources", __func__);
}

MemoryChunk *MarkCompact::get_mem_chunk() {
    return m_mem_chunk;
}

MarkCompact *MarkCompact::set_mem_chunk(MemoryChunk *chunk) {
    m_mem_chunk = chunk;
    return this;
}

void MarkCompact::memory_compact_before() {
    INFO_PRINT("start compact before to head");

    list<MemoryCell *>::iterator iterator = get_mem_chunk()->get_available_table()->begin();
    list<MemoryCell *>::iterator tmp_iterator;

    while (iterator != get_mem_chunk()->get_available_table()->end()) {
        MemoryCell * cell = *iterator;

        bool flag = false;

        tmp_iterator = iterator;
        while ((++tmp_iterator) != get_mem_chunk()->get_available_table()->end()) {
            MemoryCell* next_cell = *tmp_iterator;

            INFO_PRINT("\t 比较current(%d ~ %d)与next(%d ~ %d)",
                       cell->get_start(), cell->get_end(), next_cell->get_start(), next_cell->get_end());

            if (cell->get_start() == next_cell->get_end()) {
                INFO_PRINT("\t\t 合并current(%d ~ %d)与next(%d ~ %d)",
                           cell->get_start(), cell->get_end(), next_cell->get_start(), next_cell->get_end());

                // 合并cell
                cell->desc_start(next_cell->get_size());
                cell->inc_size(next_cell->get_size());

                // 释放被合并的cell
                get_mem_chunk()->get_available_table()->erase(tmp_iterator);
                delete next_cell;

                /* 合并后打印数据 */
                get_mem_chunk()->print_all_table();

                // 控制下次循环
                // 如果有合并的，下次外层循环就从头开始，因为合并过后的cell，可能还可以再合并
                // 如果没有合并的，下次外层循环就从下一个开始
                flag = true;
                iterator = get_mem_chunk()->get_available_table()->begin();
                break;
            }
        }

        if(!flag) {
            iterator++;
        }
    }

    INFO_PRINT("compact before to head end");
}

void MarkCompact::memory_compact_after() {
    INFO_PRINT("start compact after to tail");

    list<MemoryCell *>::iterator iterator = get_mem_chunk()->get_available_table()->begin();
    list<MemoryCell *>::iterator tmp_iterator;

    while (iterator != get_mem_chunk()->get_available_table()->end()) {
        MemoryCell * cell = *iterator;

        bool flag = false;

        tmp_iterator = iterator;
        while ((++tmp_iterator) != get_mem_chunk()->get_available_table()->end()) {
            MemoryCell* next_cell = *tmp_iterator;

            INFO_PRINT("\t 比较current(%d ~ %d)与next(%d ~ %d)",
                       cell->get_start(), cell->get_end(), next_cell->get_start(), next_cell->get_end());

            if (cell->get_end() == next_cell->get_start()) {
                INFO_PRINT("\t\t 合并current(%d ~ %d)与next(%d ~ %d)",
                           cell->get_start(), cell->get_end(), next_cell->get_start(), next_cell->get_end());

                // 合并cell
                cell->inc_end(next_cell->get_size());
                cell->inc_size(next_cell->get_size());

                // 释放被合并的cell
                get_mem_chunk()->get_available_table()->erase(tmp_iterator);
                delete next_cell;

                /* 合并后打印数据 */
                get_mem_chunk()->print_all_table();

                // 控制下次循环
                // 如果有合并的，下次外层循环就从头开始，因为合并过后的cell，可能还可以再合并
                // 如果没有合并的，下次外层循环就从下一个开始
                flag = true;
                iterator = get_mem_chunk()->get_available_table()->begin();
                break;
            }
        }

        if(!flag) {
            iterator++;
        }
    }
    INFO_PRINT("compact after to tail end");
}

void MarkCompact::mark_step() {
    INFO_PRINT("start mark");

    list<MemoryCell *> *used_table = get_mem_chunk()->get_used_table();
    list<MemoryCell *>::iterator iterator;
    for (iterator = used_table->begin(); iterator != used_table->end(); iterator++) {
        MemoryCell *cell = *iterator;
        cell->to_string("simulation mark");
        if (cell->get_size() == 4 && !cell->get_transfer_object()) {
            INFO_PRINT("mark cell");
            // 将used table中被标记的cell，原样放入transfer表中（即将指向标记cell的指针放入transfer表中），cell的地址不能变，所以被标记的cell对象不能被释放
            // （用户申请内存时返回的对象为cell，用户持有指向cell对象的指针，用户后续操作申请到的内存都是使用cell来操作的，所以cell的地址不能变，即它不能被释放）
            get_mem_chunk()->get_transfer_table()->push_front(cell);

            // 把被标记的cell复制一份放到used_table原位置处，设置mark为false，这样清理used表时，它就会被清理掉
            // 这样used表就都可以被清理掉，available表中就有一块块的被回收的内存，但是不连续，需要将它们合并成连续的
            // 复制一份的原因是要保证原cell不会被释放，从而保证原cell的地址不会变化
            *iterator = new MemoryCell(cell);    // for(xx : xx)遍历时，获取的元素是原列表中元素的copy，并不是在原先列表中的元素，所以对它的修改并不会体现到原列表中

        } else {
            INFO_PRINT("not mark");
        }
    }
}

void MarkCompact::clean_step() {
    INFO_PRINT("start clean step");

    bool flag = false;

    list<MemoryCell*>::iterator iterator;
    for (iterator = get_mem_chunk()->get_used_table()->begin(); iterator != get_mem_chunk()->get_used_table()->end();) {
        MemoryCell* cell = *iterator;
        cell->to_string("check mark");

        // release not mark cell
        if(!cell->get_mark()) {
            INFO_PRINT("not mark %X", cell)

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
            INFO_PRINT("is mark, can't release");
            iterator++;
        }
    }

    if (!flag) {
        ERROR_PRINT("[after gc]there is no reclaimable memory, memory cannot be allocated, and Full GC is executed");
        exit(-1);
    }
}

void MarkCompact::memory_compact_step() {
    INFO_PRINT("start memory compact");
    memory_compact_before();
    memory_compact_after();
    INFO_PRINT("memory compact end");
}

void MarkCompact::data_compact_step() {
    INFO_PRINT("start data compact");

    list<MemoryCell*>::iterator transfer_ite = get_mem_chunk()->get_transfer_table()->begin();
    while (transfer_ite != get_mem_chunk()->get_transfer_table()->end()) {
        MemoryCell* transfer_cell = *transfer_ite;
        transfer_cell->to_string("开始整理数据");
        get_mem_chunk()->malloc_after_gc(transfer_cell);
        transfer_ite = get_mem_chunk()->get_transfer_table()->erase(transfer_ite);
    }

    INFO_PRINT("data compact end")
}

void MarkCompact::run() {
    INFO_PRINT("[mark-compact]start");
    if (get_mem_chunk() == nullptr) {
        ERROR_PRINT("chunk is null");
        exit(-1);
    }

    mark_step();
    clean_step();
    memory_compact_step();
    data_compact_step();

    INFO_PRINT("[mark-compact]end");
}
