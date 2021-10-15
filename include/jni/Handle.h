//
// Created by xyzjiao on 10/15/21.
//

#ifndef JVMPLUS_HANDLE_H
#define JVMPLUS_HANDLE_H

#include <jni.h>
#include "../../share/vm/oops/klass.hpp"
#include "../../share/vm/oops/method.hpp"

class Handle {
public:
    enum {
        HANDLE_TYPE_KLASS = 1,
        HANDLE_TYPE_OOP = 2,
        HANDLE_TYPE_METHOD = 3,
    };
public:
    static jobject klassToHandle(JNIEnv* env, Klass* klass);
    static jobject methodToHandle(JNIEnv* env, Method* method);

    static Klass* klass(JNIEnv* env, jobject klass_handle);
    static Method* method(JNIEnv* env, jobject method_handle);
};

#endif //JVMPLUS_HANDLE_H
