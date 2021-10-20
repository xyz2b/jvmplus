//
// Created by xyzjiao on 10/19/21.
//

#include "../../include/jni/org_xyz_jvm_jdk_classes_JniEnv.h"
#include "../../include/common.h"
#include "../../include/jni/JniTools.h"
#include "../../share/vm/oops/klass.hpp"
#include "../../share/vm/classfile/systemDictionary.h"
#include "../../include/jni/Handle.h"
#include "../../share/vm/prims/JavaNativeInterface.hpp"

JNIEXPORT jobject JNICALL Java_org_xyz_jvm_jdk_classes_JniEnv_loadClassFile
        (JNIEnv *env, jclass clazz, jstring class_name) {
    const char* name = JniTools::getCharsFromJString(class_name, true);
    Symbol* s = new (strlen(name)) Symbol(name, strlen(name));
    Klass* klass = SystemDictionary::resolve_or_null(s);

    INFO_PRINT("AppClassLoader load klass, %s : %p", name, klass);

    return Handle::klassToHandle(env, klass);
}

JNIEXPORT jobject JNICALL Java_org_xyz_jvm_jdk_classes_JniEnv_getMethodId
        (JNIEnv *env, jclass clazz, jobject klass_handle, jstring method_name, jstring descriptor_name) {
    InstanceKlass* instance_klass = (InstanceKlass*) Handle::handleToKlass(env, klass_handle);
    Symbol* method_n = JniTools::jstringToSymbol(method_name, true);
    Symbol* descriptor_n = JniTools::jstringToSymbol(descriptor_name, true);

    Method* method = JavaNativeInterface::get_method(instance_klass, method_n, descriptor_n);

    return Handle::methodToHandle(env, method);
}

/*
 * Class:     org_xyz_jvm_jdk_classes_JniEnv
 * Method:    callStaticVoidMethod
 * Signature: (Lorg/xyz/jvm/jdk/classes/Handle;Lorg/xyz/jvm/jdk/classes/Handle;)V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_jdk_classes_JniEnv_callStaticVoidMethod
        (JNIEnv *env, jclass clazz, jobject klass_handle, jobject method_handle) {

}