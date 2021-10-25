//
// Created by hongyujiao on 7/12/21.
//

#include <cstdlib>
#include "../../include/memory/memory_cell.h"
#include "../../include/common.h"
#include "../../include/memory/memory_chunk.h"

MemoryCell::MemoryCell(uint start, uint size) : m_start(start), m_size(size)
{
    m_end = m_start + m_size;
}

MemoryCell::~MemoryCell()
{
    DEBUG_PRINT("[call destructor %s] release resources", __func__);
}

uint MemoryCell::get_start() const {
    return m_start;
}

MemoryCell *MemoryCell::set_start(const uint start) {
    m_start = start;
    return this;
}

uint MemoryCell::get_end() const {
    return m_end;
}

MemoryCell *MemoryCell::set_end(const uint end) {
    m_end = end;
    return this;
}

uint MemoryCell::get_size() const {
    return m_size;
}

MemoryCell *MemoryCell::set_size(const int size) {
    m_size = size;
    return this;
}

MemoryCell *MemoryCell::inc_start(uint step) {
    m_start += step;

    if (m_start > m_end) {
        ERROR_PRINT("cell end overflow");
        exit(-1);
    }

    return this;
}

MemoryCell *MemoryCell::desc_start(uint step) {
    m_start -= step;

    if (m_start < 0) {
        ERROR_PRINT("cell start overflow");
        exit(-1);
    }

    return this;
}

MemoryCell *MemoryCell::inc_size(uint step) {
    m_size += step;

    return this;
}

MemoryCell *MemoryCell::desc_size(uint step) {
    m_size -= step;

    if (m_size < 0) {
        ERROR_PRINT("cell size overflow");
        exit(-1);
    }

    return this;
}

bool MemoryCell::get_mark() const {
    return m_mark;
}

MemoryCell *MemoryCell::set_mark(bool mark) {
    m_mark = mark;
    return this;
}

void MemoryCell::to_string() const {
    DEBUG_PRINT("[cell info]start=%d, end=%d, size=%d", m_start, m_end, m_size)
}

void MemoryCell::to_string(char *msg) const {
    DEBUG_PRINT("[cell info]%s:start=%d, end=%d, size=%d", msg, m_start, m_end, m_size)
}

pvoid MemoryCell::get_belong_chunk() {
    return m_belong_chunk;
}

MemoryCell *MemoryCell::set_belong_chunk(pvoid chunk) {
    m_belong_chunk = chunk;
    return this;
}

bool MemoryCell::get_transfer_object() const {
    return m_transfer_object;
}

MemoryCell *MemoryCell::set_transfer_object(bool val) {
    m_transfer_object =  val;
    return this;
}

MemoryCell::MemoryCell(MemoryCell *cell) {
    m_start = cell->get_start();
    m_end = cell->get_end();
    m_size = cell->get_size();

    m_mark = false;
    m_transfer_object = true;
}

pvoid MemoryCell::ptr() {
    if (m_belong_chunk == nullptr) {
        return nullptr;
    }

    MemoryChunk* chunk = (MemoryChunk*) m_belong_chunk;

    return (pvoid)((ulong)chunk->get_data() + get_start() * chunk->get_align_size());
}

MemoryCell *MemoryCell::inc_end(uint step) {
    m_end += step;
    return this;
}

MemoryCell *MemoryCell::desc_end(uint step) {
    m_end -= step;
    if(m_end < m_start) {
        ERROR_PRINT("cell end overflow");
        exit(-1);
    }
    return this;
}

