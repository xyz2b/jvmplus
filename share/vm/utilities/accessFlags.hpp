//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_ACCESSFLAGS_HPP
#define JVMPLUS_ACCESSFLAGS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

enum {
    // See jvm.h for shared JVM_ACC_XXX access flags

    // HotSpot-specific access flags

    // flags actually put in .class file
    JVM_ACC_WRITTEN_FLAGS = 0x00007FFF,
};

class AccessFlags {
private:
    jint _flags;

public:
    AccessFlags() {}
    AccessFlags(jint flags) : _flags(flags) {}

    bool is_static() const { return (_flags & JVM_ACC_STATIC) != 0; }

    jint get_flags() const { return (_flags & JVM_ACC_WRITTEN_FLAGS); }
    void set_flags(jint flags) { _flags = (flags & JVM_ACC_WRITTEN_FLAGS); }
};


#endif //JVMPLUS_ACCESSFLAGS_HPP
