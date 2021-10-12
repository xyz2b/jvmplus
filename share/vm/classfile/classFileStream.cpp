//
// Created by xyzjiao on 10/11/21.
//

#include "classFileStream.hpp"

ClassFileStream::ClassFileStream(u1 *buffer, int length, const char *source) {
    _buffer_start = buffer;
    _buffer_end = buffer + length;
    _current = buffer;
    _source = source;
}



