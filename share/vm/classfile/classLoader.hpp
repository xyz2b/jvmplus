//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSLOADER_HPP
#define JVMPLUS_CLASSLOADER_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

class ClassLoader {
public:
    static InstanceKlassHandle load_classfile(Symbol* h_name);
    static ClassFileStream* open_stream(const char* name);
};


#endif //JVMPLUS_CLASSLOADER_HPP
