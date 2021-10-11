//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_HANDLES_HPP
#define JVMPLUS_HANDLES_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

class Klass;
class Method;
class ConstantPool;

class KlassHandle {
    Klass*      _value;
};

class InstanceKlassHandle : public KlassHandle {

};

#define DEF_METADATA_HANDLE(name, type)          \
  class name##Handle;                            \
  class name##Handle {         \
    type*     _value;                            \
  };

DEF_METADATA_HANDLE(Method, Method)
DEF_METADATA_HANDLE(ConstantPool, ConstantPool)

#endif //JVMPLUS_HANDLES_HPP
