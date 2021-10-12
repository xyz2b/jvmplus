//
// Created by xyzjiao on 10/10/21.
//

#ifndef JVM_C___BYTES_X86_HPP
#define JVM_C___BYTES_X86_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

class Bytes {
public:
    static inline u2 get_native_u2(address p) { return *(u2*)p; }
    static inline u4 get_native_u4(address p) { return *(u4*)p; }
    static inline u8 get_native_u8(address p) { return *(u8*)p; }

    // Java编译器生成的字节码是大端存储的，默认x86 linux默认是小端存储的(c++跟随os)，所以需要把从字节码中读取的大端存储的数据转成小端存储的，c++才能进行处理
    static inline u2 get_java_u2(address p) { return swap_u2(get_native_u2(p)); }
    static inline u4 get_java_u4(address p) { return swap_u4(get_native_u4(p)); }
    static inline u4 get_java_u8(address p) { return swap_u8(get_native_u8(p)); }

    static inline u2 swap_u2(u2 x);
    static inline u4 swap_u4(u4 x);
    static inline u8 swap_u8(u8 x);
};

#include "../../../os_cpu/linux_x86/vm/bytes_linux_x86.inline.hpp"

#endif //JVM_C___BYTES_X86_HPP
