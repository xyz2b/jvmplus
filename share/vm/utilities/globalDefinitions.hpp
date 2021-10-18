//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_GLOBALDEFINITIONS_HPP
#define JVMPLUS_GLOBALDEFINITIONS_HPP

#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <climits>
#include <unistd.h>
#include <cstdio>
#include <byteswap.h>

#include "../../../share/vm/utilities/globalDefinitions_gcc.hpp"
#include "../../../include/memory/memory_cell.h"

class HeapWord {
private:
    MemoryCell* _cell;
    void* _i;
public:
    HeapWord(MemoryCell* p) {
        _cell = p;
        _i = _cell->ptr();
    }

    char* value() {  return (char*)_i; }
};

class MetaWord {
private:
    MemoryCell* _cell;
    void* _i;
public:
    MetaWord(MemoryCell* p) {
        _cell = p;
        _i = _cell->ptr();
    }

    char* value() {  return (char*)_i; }
};

template<class T> inline T MIN2(T a, T b)           { return (a < b) ? a : b; }

typedef jubyte  u1;
typedef jushort u2;
typedef juint   u4;
typedef julong  u8;

const jubyte    max_jubyte = (jubyte) - 1;       // 0xFF                    Largest jubyte
const jushort   max_jushort = (jushort) - 1;     // 0xFFFF                  Largest jushort
const juint     max_juint = (juint) - 1;         // 0xFFFF_FFFF             Largest juit
const julong     max_julong = (julong) - 1;      // 0xFFFF_FFFF_FFFF_FFFF   Largest julong

typedef jbyte   s1;
typedef jshort  s2;
typedef jint    s4;
typedef jlong   s8;

typedef u_char* address;


#include "../../../share/vm/prims/jvm.h"
#include "../../../include/common.h"


#endif //JVMPLUS_GLOBALDEFINITIONS_HPP
