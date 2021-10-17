//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_CHEAPOBJ_HPP
#define JVMPLUS_CHEAPOBJ_HPP
#include "../../../share/vm/utilities/globalDefinitions.hpp"

class CHeapObj {
public:
    void* operator new(size_t size);
    void  operator delete(void* p);
};


#endif //JVMPLUS_CHEAPOBJ_HPP
