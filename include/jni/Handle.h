//
// Created by xyzjiao on 10/15/21.
//

#ifndef JVMPLUS_HANDLE_H
#define JVMPLUS_HANDLE_H

#include <jni.h>
#include "../../share/vm/oops/klass.hpp"
#include "../../share/vm/oops/method.hpp"
#include "../../share/vm/oops/oopsHierachy.hpp"

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
    static jobject oopToHandle(JNIEnv *env, oop o);

    static Klass* handleToKlass(JNIEnv* env, jobject klass_handle);
    static Method* handleToMethod(JNIEnv* env, jobject method_handle);
    static oop handleToOop(JNIEnv* env, jobject oop_handle);
};

#endif //JVMPLUS_HANDLE_H
