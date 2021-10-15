//
// Created by hongyujiao on 2021/7/13.
//

#include "../../include/gc/mark_copy.h"

MarkCopy::MarkCopy(MemoryChunk *chunk): m_mem_chunk(chunk) {

}

MarkCopy::~MarkCopy() {
    INFO_PRINT("[call destructor %s] release resources", __func__);
}

MemoryChunk *MarkCopy::get_mem_chunk() {
    return m_mem_chunk;
}

MarkCopy *MarkCopy::set_mem_chunk(MemoryChunk *chunk) {
    m_mem_chunk = chunk;
    return this;
}

void MarkCopy::mark_step() {
    INFO_PRINT("start mark step")

    list<MemoryCell *> *used_table = get_mem_chunk()->get_used_table();
    for (MemoryCell* cell : *used_table) {
        if (cell->get_size() == 4 || cell->get_size() == 1) {
            cell->set_mark(true);
        }
    }
}

void MarkCopy::stop_the_world() {
    INFO_PRINT("\t GC中，暂停所有用户线程");
}

// 将idle_table和available_table置换，然后将used_table中被标记的对象在新的available_table中重新分配
void MarkCopy::memory_sweep_step() {
    // 检测To区是否有碎片
    if (1 != m_mem_chunk->get_idle_table()->size()) {
        ERROR_PRINT("To区有内存碎片，无法完成GC\n");

        exit(1);
    }

    // 修改cell_start
    // 0 --> cell_num/2 ; cell_num/2 --> 0
    get_mem_chunk()->renew_cell_start();
    get_mem_chunk()->set_used_cell_num(0);

    // 释放available_table
    get_mem_chunk()->free_available_table();

    // 空间置换，将idle_table复制一份给available_table，因为传参默认是复制的，所以只需要传参时，传idle_table实体即可，不要传它的指针
    get_mem_chunk()->set_available_table(*get_mem_chunk()->get_idle_table());

    // 重新初始化idle_table
    // 将idle_table中上一次初始化的cell弹出（只有一个）
    get_mem_chunk()->get_idle_table()->pop_front();
    // 重新初始化idle_table，此时idle_table初始化时的cell_start是上一次的cell_start，即old_cell_start，和available_table相反，对换
    get_mem_chunk()->get_idle_table()->push_front(new MemoryCell(get_mem_chunk()->get_old_cell_start(), get_mem_chunk()->get_cell_num() / 2));

    // 将use_table中有标记的cell重新在新的available_table中分配内存即可（置换后的available_table）
    // 未打标记的cell直接清除即可
    list<MemoryCell *>::iterator used_table_ite = get_mem_chunk()->get_used_table()->begin();
    while (used_table_ite != get_mem_chunk()->get_used_table()->end()) {
        MemoryCell *used_cell = *used_table_ite;

        used_cell->to_string();

        if (!used_cell->get_mark()) {
            INFO_PRINT("\t 未打标记，开始释放资源\n");

            delete used_cell;
        } else {
            INFO_PRINT("\t 打了标记，开始处理\n");
            // 将use_table中有标记的cell重新在新的available_table中分配内存
            get_mem_chunk()->malloc_after_gc(used_cell);
        }

        // 从used_table中移除老的cell对象
        used_table_ite = get_mem_chunk()->get_used_table()->erase(used_table_ite);
    }
}

void MarkCopy::run() {
    // 打标记
    mark_step();

    // 暂停所有用户线程
    stop_the_world();

    // 内存交换
    memory_sweep_step();
}
