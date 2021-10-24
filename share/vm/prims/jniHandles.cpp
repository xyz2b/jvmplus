//
// Created by ziya on 2021/10/23.
//

#include "jniHandles.h"

oopDesc* JNIHandles::resolve(jobject handle) {
    oopDesc* result = (handle == nullptr ? (oopDesc*) nullptr : *(oopDesc**)handle);
    return result;
}
