//
// Created by hongyujiao on 7/12/21.
//

#include "../../include/memory/memory_chunk.h"
#include "../../include/common.h"
#include "../../include/gc/interface/garbage_collect.h"
#include "../../include/gc/gc_factory.h"

// create a chunk, divide chunk into multiple cells in units of 8 bytes
MemoryChunk::MemoryChunk(uint size, char *filename, uint line): m_filename(filename), m_line(line) {
    m_align_size = ALIGN_SIZE;
    m_cell_num = ((size - 1) / m_align_size) + 1;
    m_size = m_align_size * m_cell_num;

    this->m_data = calloc(m_size, sizeof(byte));
    if (nullptr == this->m_data) {
        ERROR_PRINT("alloc memory failed");
        exit(-1);
    }

    switch (DEFAULT_GC_TYPE) {
        case GC_MARK_CLEAN:
        case GC_MARK_COLLECT:
            m_used_cell_num = 0;
            m_available_table.push_front(new MemoryCell(0, m_cell_num));
            break;
        case GC_MARK_COPY:
            m_used_cell_num = 0;
            m_cell_start = 0;

            // 一半用作内存分配
            m_available_table.push_front(new MemoryCell(0, m_cell_num / 2));
            // 一半用作置换
            m_idle_table.push_front(new MemoryCell(m_cell_num / 2, m_cell_num / 2));

            break;
    }

    print_available_table();

    print_idle_table();
}

MemoryChunk::~MemoryChunk() {
    DEBUG_PRINT("[调用析构函数%s]释放资源\n", __func__);

    if (m_data) {
        DEBUG_PRINT("\t 释放资源, 申请内存位置( %s:%d )，内存大小:%d 字节\n", m_filename, m_line, m_size);
        free(m_data);
    }

    free_available_table();
    free_used_table();
}

void MemoryChunk::free_available_table() {
    DEBUG_PRINT("release available table");

    for (MemoryCell * cell : m_available_table) {
        delete(cell);
    }

    m_available_table.clear();
}

void MemoryChunk::free_used_table() {
    DEBUG_PRINT("release used table");

    for (MemoryCell * cell : m_used_table) {
        delete(cell);
    }

    m_used_table.clear();
}

void MemoryChunk::print_available_table() {
    DEBUG_PRINT("print available table");

    for (MemoryCell * cell : m_available_table) {
        DEBUG_PRINT("\t cell_start=%d, cell_end=%d, cell_size=%d", cell->get_start(), cell->get_end(), cell->get_size());
    }

    DEBUG_PRINT("print available table stop");
}

void MemoryChunk::print_used_table() {
    DEBUG_PRINT("print used table");

    for (MemoryCell * cell : m_used_table) {
        DEBUG_PRINT("\t cell_start=%d, cell_end=%d, cell_size=%d", cell->get_start(), cell->get_end(), cell->get_size());
    }

    DEBUG_PRINT("print used table stop");
}

void MemoryChunk::print_all_table() {
    print_available_table();

    print_used_table();

    print_transfer_table();

    print_idle_table();
}

uint MemoryChunk::get_size() const {
    return m_size;
}

uint MemoryChunk::get_align_size() const {
    return m_align_size;
}

pvoid MemoryChunk::get_data() {
    return m_data;
}

MemoryChunk *MemoryChunk::inc_used_cell_num(uint step) {
    m_used_cell_num += step;
    if (m_used_cell_num > m_cell_num) {
        ERROR_PRINT("cell of chunk size overflow")
        exit(-1);
    }
    return this;
}

MemoryChunk *MemoryChunk::desc_used_cell_num(uint step) {
    m_used_cell_num -= step;
    if (m_used_cell_num < 0) {
        ERROR_PRINT("cell of chunk size overflow")
        exit(-1);
    }
    return this;
}

MemoryCell* MemoryChunk::malloc(uint size) {
    MemoryCell* ret = nullptr;

    if (size <= 0) {
        ERROR_PRINT("malloc memory size must bigger than zero");
        exit(-1);
    }

    uint cell_num = ((size - 1) / m_align_size) + 1;
    if (cell_num > m_cell_num) {
        ERROR_PRINT("malloc memory size %d bigger than max memory %d", size, m_size);
        exit(-1);
    }

    // 遍历available_table查找满足条件的MemoryCell
    for (MemoryCell* cell : m_available_table) {
        if (cell->get_size() >= cell_num) {
            DEBUG_PRINT("find suitable MemoryCell");
            ret = real_malloc(cell, cell_num);
            break;
        }
    }

    // 没有找到合适的MemoryCell，不能分配内存，发生GC
    if (ret == nullptr) {
        ERROR_PRINT("not have suitable MemoryCell, can't malloc memory, run gc");

        GCFactory::get_instance()->minor_run(DEFAULT_GC_TYPE, this);

        ret = malloc(size);
    }

    print_all_table();

    return ret;
}

MemoryCell* MemoryChunk::real_malloc(MemoryCell *cell, uint cell_num) {
    // memory start pointer
    pvoid ret = (pvoid)((ulong)get_data() + cell->get_start() * get_align_size());
    DEBUG_PRINT("[real malloc memory]data start address=%X, cell_start=%d, ret=%X, cell_size=%d", get_data(), cell->get_start(), ret, cell_num);

    // create used cell, add to used table
    MemoryCell* used_cell = new MemoryCell(cell->get_start(), cell_num);
    used_cell->set_belong_chunk((pvoid)this);
    m_used_table.push_front(used_cell);


    // produce cell, remove the used part from the cell
    cell->inc_start(cell_num)->desc_size(cell_num);

    // this cell is used all, remove from available table
    if (cell->get_size() == 0) {
        m_available_table.remove(cell);
        delete cell;
    }

    // update used_cell_num
    inc_used_cell_num(cell_num);

    // if the chunk is used up, empty the available_table
    if (m_cell_num == m_used_cell_num) {
        free_available_table();
    }

    return used_cell;
}

list<MemoryCell *> *MemoryChunk::get_available_table() {
    return &m_available_table;
}

list<MemoryCell *> *MemoryChunk::get_used_table() {
    return &m_used_table;
}

list<MemoryCell *> *MemoryChunk::get_transfer_table() {
    return &m_transfer_table;
}

MemoryCell *MemoryChunk::malloc_after_gc(MemoryCell *transfer_cell) {
    MemoryCell *ret = NULL;

    pvoid ptr_from = NULL;
    pvoid ptr_to = NULL;

    for (MemoryCell * cell : m_available_table) {
        if (cell->get_size() >= transfer_cell->get_size()) {
            cell->to_string("找到了满足条件的cell");

            // 原地址：原cell数据的位置
            ptr_from = transfer_cell->ptr();

            // 调整transfer cell
            transfer_cell->set_start(cell->get_start());
            transfer_cell->set_end(transfer_cell->get_start() + transfer_cell->get_size());

            // 新地址：新cell数据的位置
            ptr_to = transfer_cell->ptr();

            DEBUG_PRINT("开始拷贝内存: ptr_from=%X, ptr_to=%X, size=%d\n", ptr_from, ptr_to, transfer_cell->get_size() * get_align_size());
            // copy数据
            memcpy(ptr_to, ptr_from, transfer_cell->get_size() * get_align_size());

            // 插入used table
            get_used_table()->push_front(transfer_cell);

            // produce cell, remove the used part from the cell
            // 从找到的满足条件的cell中，切割掉使用的那块（transfer_cell->get_size()）
            cell->inc_start(transfer_cell->get_size())->desc_size(transfer_cell->get_size());

            // this cell is used all, remove from available table
            if (cell->get_size() == 0) {
                m_available_table.remove(cell);
                delete cell;
            }

            // update used_cell_num
            inc_used_cell_num(transfer_cell->get_size());

            ret = transfer_cell;

            // if the chunk is used up, empty the available_table
            if (m_cell_num == m_used_cell_num) {
                free_available_table();
            }

            break;
        }
    }
    return ret;
}

list<MemoryCell *> *MemoryChunk::get_idle_table() {
    return &m_idle_table;
}

void MemoryChunk::print_idle_table() {
    DEBUG_PRINT("print idle table");

    for (MemoryCell * cell : m_idle_table) {
        DEBUG_PRINT("\t cell_start=%d, cell_end=%d, cell_size=%d", cell->get_start(), cell->get_end(), cell->get_size());
    }

    DEBUG_PRINT("print idle table stop");
}

void MemoryChunk::print_transfer_table() {
    DEBUG_PRINT("print transfer table");

    for (MemoryCell * cell : m_transfer_table) {
        DEBUG_PRINT("\t cell_start=%d, cell_end=%d, cell_size=%d", cell->get_start(), cell->get_end(), cell->get_size());
    }

    DEBUG_PRINT("print transfer table stop");
}

uint MemoryChunk::get_cell_start() const {
    return m_cell_start;
}

MemoryChunk *MemoryChunk::set_cell_start(uint val) {
    m_cell_start = val;
    return this;
}

MemoryChunk *MemoryChunk::set_available_table(list<MemoryCell *> table) {
    m_available_table = table;
    return this;
}

uint MemoryChunk::get_new_cell_start() const {
    return (0 == m_cell_start) ? m_cell_num/2 : 0;
}

uint MemoryChunk::get_old_cell_start() const {
    return get_new_cell_start();
}

MemoryChunk *MemoryChunk::renew_cell_start() {
    m_cell_start = get_new_cell_start();
    return this;
}

MemoryChunk *MemoryChunk::set_used_cell_num(uint val) {
    m_used_cell_num = val;
    return this;
}

uint MemoryChunk::get_cell_num() const {
    return m_cell_num;
}
