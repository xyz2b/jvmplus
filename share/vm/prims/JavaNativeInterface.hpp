//
// Created by xyzjiao on 10/20/21.
//

#ifndef JVMPLUS_JAVANATIVEINTERFACE_HPP
#define JVMPLUS_JAVANATIVEINTERFACE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"

class JavaNativeInterface {
public:
    static Method* get_method(InstanceKlass* klass, Symbol* method_name, Symbol* descriptor_name);
};


#endif //JVMPLUS_JAVANATIVEINTERFACE_HPP
