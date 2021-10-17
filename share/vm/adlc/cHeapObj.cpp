//
// Created by xyzjiao on 10/17/21.
//

#include "cHeapObj.hpp"

void* CHeapObj::operator new(size_t size) {
    return (void *) malloc(size);
}

void CHeapObj::operator delete(void* p){
    free(p);
}