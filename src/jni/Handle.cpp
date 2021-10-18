//
// Created by xyzjiao on 10/15/21.
//

#include "../../include/jni/Handle.h"

static jobject dataToHandle(JNIEnv* env, jlong data, int data_type) {
    char* handle_class_name = "org/xyz/jvm/jdk/classes/Handle";

    jclass java_handle = env->FindClass(handle_class_name);
    if (nullptr == java_handle) {
        ERROR_PRINT("不存在的类: %s", handle_class_name);
        exit(-1);
    }

    jmethodID construct = env->GetMethodID(java_handle, "<init>", "()V");
    if (nullptr == construct) {
        ERROR_PRINT("该类不存在默认的构造方法: %s", handle_class_name);
        exit(-1);
    }

    jobject object = env->NewObject(java_handle, construct);
    if (nullptr == object) {
        ERROR_PRINT("创建对象失败: %s", handle_class_name);
        exit(-1);
    }

    jfieldID p = env->GetFieldID(java_handle, "p", "J");
    if (nullptr == p) {
        ERROR_PRINT("该类不存在属性p: %s", handle_class_name);
        exit(-1);
    }
    env->SetLongField(object, p, data);

    jfieldID type = env->GetFieldID(java_handle, "type", "I");
    if (nullptr == p) {
        ERROR_PRINT("该类不存在属性type: %s", handle_class_name);
        exit(-1);
    }
    env->SetIntField(object, type, data_type);

    return object;
}


jobject Handle::klassToHandle(JNIEnv *env, Klass *klass) {
    jlong data = reinterpret_cast<jlong>(klass);

    return dataToHandle(env, data, HANDLE_TYPE_KLASS);
}

jobject Handle::methodToHandle(JNIEnv *env, Method *method) {
    jlong data = reinterpret_cast<jlong>(method);

    return dataToHandle(env, data, HANDLE_TYPE_METHOD);
}


jobject Handle::oopToHandle(JNIEnv *env, oop o) {
    jlong data = reinterpret_cast<jlong>(o);

    return dataToHandle(env, data, HANDLE_TYPE_OOP);
}

template<class T>
T* handleToData(JNIEnv *env, jobject handle) {
    char* handle_class_name = "org/xyz/jvm/jdk/classes/Handle";

    jclass java_handle = env->FindClass(handle_class_name);
    if (nullptr == java_handle) {
        ERROR_PRINT("不存在的类: %s", handle_class_name);
        exit(-1);
    }

    jfieldID p = env->GetFieldID(java_handle, "p", "J");
    if (nullptr == p) {
        ERROR_PRINT("该类不存在属性p: %s", handle_class_name);
        exit(-1);
    }
    jlong data = env->GetLongField(handle, p);

    return (T*) data;
}

Klass *Handle::handleToKlass(JNIEnv *env, jobject klass_handle) {
    return handleToData<Klass>(env, klass_handle);
}

Method* Handle::handleToMethod(JNIEnv* env, jobject method_handle) {
    return handleToData<Method>(env, method_handle);
}

oop Handle::handleToOop(JNIEnv *env, jobject oop_handle) {
    return handleToData<oopDesc>(env, oop_handle);
}