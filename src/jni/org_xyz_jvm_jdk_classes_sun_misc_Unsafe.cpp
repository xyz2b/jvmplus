//
// Created by xyzjiao on 8/2/21.
//
#include "../../include/jni/org_xyz_jvm_jdk_classes_sun_misc_Unsafe.h"
#include "../../../share/vm/memory/universe.hpp"

/*
 * Class:     org_xyz_jvm_jdk_classes_sun_misc_Unsafe
 * Method:    allocateMemory
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_org_xyz_jvm_jdk_classes_sun_misc_Unsafe_allocateMemory
        (JNIEnv *env, jclass clazz, jlong size) {

    // 指针8字节，long也是8字节，所以可以直接转，不会丢失精度
    return 1;
//    return Handle::klassToHandle(env, cell);
}

JNIEXPORT jobject JNICALL Java_org_xyz_jvm_jdk_classes_sun_misc_Unsafe_allocateObject
        (JNIEnv *env, jclass clazz) {
    InstanceKlass* instanceKlass = new InstanceKlass;

    instanceOop i = instanceKlass->allocate_instance(16);

    return Handle::oopToHandle(env, i);
}