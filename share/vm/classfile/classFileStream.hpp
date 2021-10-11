//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSFILESTREAM_HPP
#define JVMPLUS_CLASSFILESTREAM_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

// 读取字节码文件的流信息
class ClassFileStream {
private:
    u1*   _buffer_start; // Buffer bottom
    u1*   _buffer_end;   // Buffer top (one past last element)
    u1*   _current;      // Current buffer position
    const char* _source; // Source of stream (class file name)

public:
    ClassFileStream(u1* buffer, int length, const char* source);

    u1* buffer() const           { return _buffer_start; }
    int length() const           { return _buffer_end - _buffer_start; }
    const char* source() const   { return _source; }

    u1 get_u1_fast();

    u2 get_u2_fast();

    static inline u4 swap_u4(u4 x) {
        return bswap_32(x);
    }
    static inline u4   get_native_u4(address p)         { return *(u4*)p; }

    static inline u4   get_Java_u4(address p)           { return swap_u4(get_native_u4(p)); }

    u4 get_u4_fast() {
        u4 res = get_Java_u4(_current);
        _current += 4;
        return res;
    }
};



#endif //JVMPLUS_CLASSFILESTREAM_HPP
