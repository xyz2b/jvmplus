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

enum BasicType {
    T_BOOLEAN     =  4,
    T_CHAR        =  5,
    T_FLOAT       =  6,
    T_DOUBLE      =  7,
    T_BYTE        =  8,
    T_SHORT       =  9,
    T_INT         = 10,
    T_LONG        = 11,
    T_OBJECT      = 12,
    T_ARRAY       = 13,
    T_VOID        = 14,
    T_ADDRESS     = 15,
    T_NARROWOOP   = 16,
    T_METADATA    = 17,
    T_NARROWKLASS = 18,
    T_CONFLICT    = 19, // for stack value type with conflicting contents
    T_ILLEGAL     = 99
};

enum {
    ILLEGAL = -1,

    NOP = 0,    // 0x00

    ACONST_NULL = 1,    // 0x01

    ICONST_M1 = 2,   // 0x02
    ICONST_0 = 3,   // 0x03
    ICONST_1 = 4,   // 0x04
    ICONST_2 = 5,   // 0x05
    ICONST_3 = 6,   // 0x06
    ICONST_4 = 7,   // 0x07
    ICONST_5 = 8,   // 0x08

    LCONST_0 = 9,   // 0x09
    LCONST_1 = 10,   // 0x0a

    FCONST_0 = 11,   // 0x0b
    FCONST_1 = 12,   // 0x0c
    FCONST_2 = 13,   // 0x0e

    DCONST_0 = 14,   // 0x0e
    DCONST_1 = 15,   // 0x0f

    BIPUSH = 16,    // 0x10
    SIPUSH = 17,       // 0x11

    LDC = 18,       // 0x12
    LDC_W = 19,     // 0x13
    LDC2_W = 20,    // 0x14

    ILOAD = 21,   // 0x15
    LLOAD = 22,   // 0x16
    FLOAD = 23,   // 0x17
    DLOAD = 24,   // 0x18
    ALOAD = 25,   // 0x19

    ILOAD_0 = 26,   // 0x1a
    ILOAD_1 = 27,   // 0x1b
    ILOAD_2 = 28,   // 0x1c
    ILOAD_3 = 29,   // 0x1d

    LLOAD_0 = 30,   // 0x1e
    LLOAD_1 = 31,   // 0x1f
    LLOAD_2 = 32,   // 0x20
    LLOAD_3 = 33,   // 0x21

    FLOAD_0 = 34,   // 0x22
    FLOAD_1 = 35,   // 0x23
    FLOAD_2 = 36,   // 0x24
    FLOAD_3 = 37,   // 0x25

    DLOAD_0 = 38,   // 0x26
    DLOAD_1 = 39,   // 0x27
    DLOAD_2 = 40,   // 0x28
    DLOAD_3 = 41,   // 0x29

    ALOAD_0 = 42,   // 0x2a
    ALOAD_1 = 43,   // 0x2b
    ALOAD_2 = 44,   // 0x2c
    ALOAD_3 = 45,   // 0x2d

    IALOAD = 46,    // 0x2e
    LALOAD = 47,    // 0x2f
    FALOAD = 48,    // 0x30
    DALOAD = 49,    // 0x31
    AALOAD = 50,    // 0x32
    BALOAD = 51,    // 0x33
    CALOAD = 52,    // 0x34
    SALOAD = 53,    // 0x35

    ISTORE = 54,  // 0x36
    LSTORE = 55, // 0x37
    FSTORE = 56, // 0x38
    DSTORE = 57,  // 0x39
    ASTORE = 58,  // 0x3a

    ISTORE_0 = 59,  // 0x3b
    ISTORE_1 = 60,  // 0x3c
    ISTORE_2 = 61,  // 0x3d
    ISTORE_3 = 62,  // 0x3e

    LSTORE_0 = 63, // 0x3f
    LSTORE_1 = 64, // 0x40
    LSTORE_2 = 65, // 0x41
    LSTORE_3 = 66, // 0x42

    FSTORE_0 = 67, // 0x43
    FSTORE_1 = 68, // 0x44
    FSTORE_2 = 69, // 0x45
    FSTORE_3 = 70, // 0x46

    DSTORE_0 = 71, // 0x47
    DSTORE_1 = 72, // 0x48
    DSTORE_2 = 73, // 0x49
    DSTORE_3 = 74, // 0x4a

    ASTORE_0 = 75, // 0x4b
    ASTORE_1 = 76, // 0x4c
    ASTORE_2 = 77, // 0x4d
    ASTORE_3 = 78, // 0x4e

    IASTORE = 79, // 0x4f
    LASTORE = 80, // 0x50
    FASTORE = 81, // 0x51
    DASTORE = 82, // 0x52
    AASTORE = 83, // 0x53
    BASTORE = 84, // 0x54
    CASTORE = 85, // 0x55
    SASTORE = 86, // 0x56

    POP = 87, // 0x57
    POP2 = 88, // 0x58

    DUP = 89,       // 0x59
    DUP_X1 = 90,       // 0x5a
    DUP_X2 = 91,       // 0x5b
    DUP2 = 92,       // 0x5c
    DUP2_X1 = 93,       // 0x5d
    DUP2_X2 = 94,       // 0x5e
    SWAP = 95,       // 0x5f

    IADD = 96,      // 0x60
    LADD = 97,      // 0x61
    FADD = 98,      // 0x62
    DADD = 99,      // 0x63

    ISUB = 100,      // 0x64
    LSUB = 101,      // 0x65
    FSUB = 102,      // 0x66
    DSUB = 103,      // 0x67

    IMUL = 104,      // 0x68
    LMUL = 105,      // 0x69
    FMUL = 106,      // 0x6a
    DMUL = 107,      // 0x6b

    IDIV = 108,      // 0x6c
    LDIV = 109,      // 0x6d
    FDIV = 110,      // 0x6e
    DDIV = 111,      // 0x6f

    IREM = 112,      // 0x70
    LREM = 113,      // 0x71
    FREM = 114,      // 0x72
    DREM = 115,      // 0x73

    INEG = 116,      // 0x74
    LNEG = 117,      // 0x75
    FNEG = 118,      // 0x76
    DNEG = 119,      // 0x77

    ISHL = 120,      // 0x78
    LSHL = 121,      // 0x79

    ISHR = 122,      // 0x7a
    LSHR = 123,      // 0x7b

    IUSHR = 124,      // 0x7c
    LUSHR = 125,      // 0x7d

    IAND = 126,      // 0x7e
    LAND = 127,      // 0x7f

    IOR = 128,      // 0x80
    LOR = 129,      // 0x81

    IXOR = 130,      // 0x82
    LXOR = 131,      // 0x83

    IINC = 132,     // 0x84


    I2L = 133,      // 0x85
    I2F = 134,      // 0x86
    I2D = 135,      // 0x87     // ?????????int??????????????????double??????????????????????????????
    L2I = 136,      // 0x88
    L2F = 137,      // 0x89
    L2D = 138,      // 0x8a
    F2I = 139,      // 0x8b
    F2L = 140,      // 0x8c
    F2D = 141,      // 0x8d
    D2I = 142,      // 0x8e
    D2L = 143,      // 0x8f
    D2F = 144,      // 0x90
    I2B = 145,      // 0x91
    I2C = 146,      // 0x92
    I2S = 147,      // 0x93

    LCMP = 148,     // 0x94
    FCMPL = 149,    // 0x95
    FCMPG = 150,    // 0x96
    DCMPL = 151,    // 0x97
    DCMPG = 152,    // 0x98

    IFEQ = 153,    // 0x99
    IFNE = 154,    // 0x9a
    IFLT = 155,    // 0x9b
    IFGE = 156,    // 0x9c
    IFGT = 157,    // 0x9d
    IFLE = 158,    // 0x9e

    IF_ICMPEQ = 159,    // 0x9f
    IF_ICMPNE = 160,    // 0xa0
    IF_ICMPLT = 161,    // 0xa1
    IF_ICMPGE = 162,    // 0xa2
    IF_ICMPGT = 163,    // 0xa3
    IF_ICMPLE = 164,    // 0xa4
    IF_ACMPEQ = 165,    // 0xa5
    IF_ACMPNE = 166,    // 0xa6

    GOTO = 167,    // 0xa7

    IRETURN = 172,      // 0xac
    LRETURN = 173,      // 0xad
    FRETURN = 174,      // 0xae
    DRETURN = 175,      // 0xaf
    ARETURN = 176,      // 0xb0
    RETURN = 177,       // 0xb1

    GETSTATIC = 178,    // 0xb2
    PUTSTATIC = 179,    // 0xb3
    GETFIELD = 180,     // 0xb4
    PUTFIELD = 181,     // 0xb5

    INVOKEVIRTUAL = 182,        // 0xb6
    INVOKESPECIAL = 183,        // 0xb7
    INVOKESTATIC = 184,         // 0xb8
    INVOKEINTERFACE = 185,      // 0xb9
    INVOKEDYNAMIC = 186,        // 0xba

    NEW = 187,          // 0xbb
    NEWARRAY = 188,     // 0xbc
    ANEWARRAY = 189,    // 0xbd
    ARRAYLENGTH = 190,  // 0xbe

    ATHROW = 191,       // 0xbf
    CHECKCAST = 192,    // 0xc0

    IFNULL = 198,    // 0xc6
    IFNONNULL = 199    // 0xc7
};

#include "../../../share/vm/prims/jvm.h"
#include "../../../include/common.h"


#endif //JVMPLUS_GLOBALDEFINITIONS_HPP
