//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_ALLOCTION_HPP
#define JVMPLUS_ALLOCTION_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/adlc/cHeapObj.hpp"
#include "../../../share/vm/memory/metaspace.hpp"

class AllStatic {

};

// JDK* metaspace在直接内存中
class MetaspaceObj : public CHeapObj {
public:
    void* operator new (size_t size, size_t length) {
        return Metaspace::allocate(size + length)->value();
    }

    void* operator new (size_t size) {
        return Metaspace::allocate(size)->value();
    }
};


#endif //JVMPLUS_ALLOCTION_HPP
