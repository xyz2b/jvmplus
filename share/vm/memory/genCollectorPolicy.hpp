//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_GENCOLLECTORPOLICY_HPP
#define JVMPLUS_GENCOLLECTORPOLICY_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/memory/collectorPolicy.hpp"

class GenCollectorPolicy: public CollectorPolicy {

public:
    HeapWord* mem_allocate_work(size_t size);

public:
    HeapWord* satisfy_failed_allocation(size_t size);
};


#endif //JVMPLUS_GENCOLLECTORPOLICY_HPP
