//
// Created by xyzjiao on 10/17/21.
//

#include "alloction.hpp"

void *MetaspaceObj::operator new(size_t sz) {
    void* metaspace = calloc(1, sz);
    return metaspace;
}

void *MetaspaceObj::operator new(size_t sz, size_t extra_length) {
    void* metaspace = calloc(1, sz + extra_length);
    return metaspace;
}