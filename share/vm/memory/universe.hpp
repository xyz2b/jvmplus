//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_UNIVERSE_HPP
#define JVMPLUS_UNIVERSE_HPP

#include "../../share/vm/gc_interface/collectedHeap.hpp"
#include "../../share/vm/memory/markSweepPolicy.hpp"
#include "../../share/vm/memory/genCollectorPolicy.hpp"

class Universe {
private:
    static CollectedHeap* _collectedHeap;

public:
    static CollectedHeap* heap() {
        return _collectedHeap;
    }

public:
    static void initialize_heap();
};


#endif //JVMPLUS_UNIVERSE_HPP
