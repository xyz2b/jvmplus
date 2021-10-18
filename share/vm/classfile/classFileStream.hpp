//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSFILESTREAM_HPP
#define JVMPLUS_CLASSFILESTREAM_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../cpu/x86/vm/bytes_x86.hpp"

// 字节码文件流
class ClassFileStream {
private:
    u1*   _buffer_start; // Buffer bottom
    u1*   _buffer_end;   // Buffer top (one past last element)
    u1*   _current;      // Current buffer position
    const char* _source; // Source of stream (class file name)

public:
    ClassFileStream(u1* buffer, int length, const char* source);
    ~ClassFileStream() {
        free(_buffer_start);
        _buffer_start = nullptr;
        _buffer_end = nullptr;
        _current = nullptr;
        free((void *) _source);
        _source = nullptr;
    }

    u1* buffer() const           { return _buffer_start; }
    int length() const           { return _buffer_end - _buffer_start; }
    const char* source() const   { return _source; }

    u1* get_u1_buffer() {
        return _current;
    }

    u1 get_u1_fast() {
        return *_current++;
    }

    u2 get_u2_fast() {
        u2 res = Bytes::get_java_u2(_current);
        _current += 2;
        return res;
    }

    u4 get_u4_fast() {
        u4 res = Bytes::get_java_u4(_current);
        _current += 4;
        return res;
    }

    u8 get_u8_fast() {
        u8 res = Bytes::get_java_u8(_current);
        _current += 8;
        return res;
    }

    void skip_u1_fast(u4 length) {
        _current += length;
    }

    void skip_u2_fast(u4 length) {
        _current += 2 * length;
    }

    void skip_u4_fast(u4 length) {
        _current += 4 * length;
    }
};



#endif //JVMPLUS_CLASSFILESTREAM_HPP
