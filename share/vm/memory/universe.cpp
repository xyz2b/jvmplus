//
// Created by xyzjiao on 10/17/21.
//

#include "universe.hpp"
#include "../../../share/vm/memory/genCollectedHeap.hpp"

CollectedHeap*  Universe::_collectedHeap = NULL;

void Universe::initialize_heap() {
    INFO_PRINT("initialize_heap\n");

    if (UseParallelGC) {

    } else if (UseG1GC) {

    } else {
        GenCollectorPolicy *gc_policy;

        if (UseSerialGC) {

        } else if (UseConcMarkSweepGC) {

        } else {
            gc_policy = new MarkSweepPolicy();
        }

        GenCollectedHeap* heap = new GenCollectedHeap(gc_policy);

        Universe::_collectedHeap = heap;

        heap->initialize();
    }
}