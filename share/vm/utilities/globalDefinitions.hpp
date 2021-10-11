//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_GLOBALDEFINITIONS_HPP
#define JVMPLUS_GLOBALDEFINITIONS_HPP

#include <cstdlib>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <climits>
#include <unistd.h>
#include <stdio.h>

#include "../../../share/vm/utilities/globalDefinitions_gcc.hpp"

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


class ClassLoader;
class ClassFileParser;
class ClassFileStream;

class ConstantPool;
class InstanceKlass;
class Klass;
class Metadata;
class Method;
class Symbol;

class os;
class TimeStamp;

class AccessFlags;
class stringStream;

#include "../../../share/vm/utilities/array.hpp"

#include "../../../share/vm/runtime/handles.hpp"

#include "../../../share/vm/adlc/adlc.hpp"

#include "../../../share/vm/classfile/classLoader.hpp"
#include "../../../share/vm/classfile/classFileStream.hpp"
#include "../../../share/vm/classfile/classFileParser.hpp"

#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"
#include "../../../share/vm/oops/metadata.hpp"
#include "../../../share/vm/oops/constantPool.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/oops/symbol.hpp"

#include "../../../share/vm/runtime/os.hpp"
#include "../../../share/vm/runtime/timer.hpp"

#include "../../../share/vm/utilities/accessFlags.hpp"

#include "../../../share/vm/utilities/ostream.hpp"

#endif //JVMPLUS_GLOBALDEFINITIONS_HPP
