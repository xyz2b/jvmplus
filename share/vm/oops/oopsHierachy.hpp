//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_OOPSHIERACHY_HPP
#define JVMPLUS_OOPSHIERACHY_HPP

#include "../../../share/vm/oops/instanceOop.hpp"
#include "../../../share/vm/oops/oop.hpp"

typedef class oopDesc*              oop;
typedef class instanceOopDesc*      instanceOop;


template<class T>
inline T cast_from_oop(oop o) {
    return (T) ((void*) o);
}

#endif //JVMPLUS_OOPSHIERACHY_HPP
