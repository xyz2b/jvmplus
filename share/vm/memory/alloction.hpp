//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_ALLOCTION_HPP
#define JVMPLUS_ALLOCTION_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/memory/genCollectedHeap.hpp"

// JDK* metaspace在直接内存中
class MetaspaceObj {

public:
    void* operator new (size_t sz);
    void* operator new(size_t sz, size_t length);

    void operator delete(void* p) { free(p); }
};


#endif //JVMPLUS_ALLOCTION_HPP
