//
// Created by xyzjiao on 8/2/21.
//
#include "../../include/jni/org_xyz_jvm_jdk_classes_sun_misc_Unsafe.h"
#include "../../include/common.h"
#include "../../include/memory/memory_pool.h"

extern MemoryChunk* g_newGeneration;
extern MemoryChunk* g_oldGeneration;


/*
 * Class:     org_xyz_jvm_jdk_classes_sun_misc_Unsafe
 * Method:    allocateMemory
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_org_xyz_jvm_jdk_classes_sun_misc_Unsafe_allocateMemory
        (JNIEnv *env, jclass clazz, jlong size) {
    MemoryCell* cell = g_newGeneration->malloc(size);
    // 指针8字节，long也是8字节，所以可以直接转，不会丢失精度
    return reinterpret_cast<jlong>(cell->ptr());
}
