//
// Created by ziya on 2021/10/23.
//

#ifndef JVMPLUS_JNIHANDLES_H
#define JVMPLUS_JNIHANDLES_H

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/oop.hpp"
#include <jni.h>

class JNIHandles {

public:
    // Resolve handle into oop
    inline static oopDesc* resolve(jobject handle);
};


#endif //JVMPLUS_JNIHANDLES_H
