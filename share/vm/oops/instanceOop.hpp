//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_INSTANCEOOP_HPP
#define JVMPLUS_INSTANCEOOP_HPP

#include "../../../share/vm/oops/oop.hpp"

class Klass;

class instanceOopDesc : public oopDesc {

public:
    instanceOopDesc(Klass* klass) : oopDesc(klass) {}
};


#endif //JVMPLUS_INSTANCEOOP_HPP
