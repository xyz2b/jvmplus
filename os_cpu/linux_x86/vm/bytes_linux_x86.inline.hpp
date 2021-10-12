//
// Created by xyzjiao on 10/10/21.
//

#ifndef JVM_C___BYTES_LINUX_X86_INLINE_HPP
#define JVM_C___BYTES_LINUX_X86_INLINE_HPP

#include "../../../cpu/x86/vm/bytes_x86.hpp"

inline u2 Bytes::swap_u2(u2 x) {
    return bswap_16(x);
//    u2 ret;
//    __asm__ __volatile__ {
//        "movw %0, %%ax;"
//        "xchg %%al, %%ah;"  // 交换al和ah寄存器的内容
//        "movw %%ax, %0;"
//        :"=r" (ret)     // output: register 0 => ret
//        :"0" (x)        // input: x => register 0
//        :"ax", "0"      // 影响的寄存器
//    }
//    return ret;
}

inline u4 Bytes::swap_u4(u4 x) {
    return bswap_32(x);
//    u4 ret;
//    __asm__ __volatile__ {
//    "bswap %0"      // 32位寄存器内的字节次序变反
//    :"=r" (ret)     // output: register 0 => ret
//    :"0" (x)        // input: x => register 0
//    :"0"      // 影响的寄存器
//    }
//    return ret;
}


inline u8 Bytes::swap_u8(u8 x) {
    return bswap_64(x);
}


#endif //JVM_C___BYTES_LINUX_X86_INLINE_HPP
