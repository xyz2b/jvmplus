//
// Created by hongyujiao on 7/12/21.
//

#ifndef MARK_CLEAR_MEMORY_CHUNK_H
#define MARK_CLEAR_MEMORY_CHUNK_H

#include <list>
#include <cstdlib>
#include <cstring>
#include "./memory_cell.h"

using namespace std;

class MemoryChunk {
private:
    // 创建Chunk的文件名
    char *m_filename;

    // 创建Chunk的文件位置
    uint m_line;

    // chunk memory size
    uint m_size;
    // align size
    uint m_align_size;
    // total cell num of chunk
    uint m_cell_num;
    // used cell num of chunk
    uint m_used_cell_num;

    // 当前Cell的起始位置，复制算法用
    uint m_cell_start;
private:
    // 真正存储数据的地方
    pvoid m_data;

    // cell
    list<MemoryCell *> m_available_table;
    list<MemoryCell *> m_used_table;
    // 整理内存时中转用，暂存打了标记的对象，对象移动后释放
    list<MemoryCell *> m_transfer_table;
    // 空闲的内存，复制算法用
    list<MemoryCell *> m_idle_table;
public:
    MemoryChunk(uint size, char *filename, uint line);
    ~MemoryChunk();

public:
    uint get_size() const;
    uint get_align_size() const;
    pvoid get_data();

    uint get_cell_start() const;
    MemoryChunk *set_cell_start(uint val);

    list<MemoryCell *> *get_available_table();
    MemoryChunk* set_available_table(list<MemoryCell *> table);
    list<MemoryCell *> *get_used_table();
    list<MemoryCell *> *get_transfer_table();
    list<MemoryCell *> *get_idle_table();

    MemoryCell* malloc(uint size);

    MemoryCell* malloc_after_gc(MemoryCell* transfer_cell);

    uint get_new_cell_start() const;
    // old_cell_start 和 new_cell_start 正好相反
    uint get_old_cell_start() const;
    MemoryChunk *renew_cell_start();

public:
    void free_available_table();

private:
    void free_used_table();
    MemoryCell* real_malloc(MemoryCell* cell, uint cell_num);

public:
    MemoryChunk* inc_used_cell_num(uint step);
    MemoryChunk* desc_used_cell_num(uint step);
    MemoryChunk* set_used_cell_num(uint val);
    uint get_cell_num() const;

public:
    void print_available_table();
    void print_used_table();
    void print_all_table();
    void print_idle_table();
    void print_transfer_table();
};


#endif //MARK_CLEAR_MEMORY_CHUNK_H
