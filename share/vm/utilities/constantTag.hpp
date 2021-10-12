//
// Created by xyzjiao on 10/12/21.
//

#ifndef JVMPLUS_CONSTANTTAG_HPP
#define JVMPLUS_CONSTANTTAG_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/prims/jvm.h"

class ConstantTag {
private:
    jbyte _tag;
public:
    bool is_utf8() { return _tag == JVM_CONSTANT_Utf8; }

    ConstantTag() {
        _tag = JVM_CONSTANT_Invalid;
    }

    ConstantTag(u1 tag) {
        _tag = tag;
    }

    jbyte value() const { return _tag; }
};


#endif //JVMPLUS_CONSTANTTAG_HPP
