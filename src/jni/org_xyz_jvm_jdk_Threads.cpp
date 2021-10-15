//
// Created by xyzjiao on 8/2/21.
//

#include "../../include/jni/org_xyz_jvm_jdk_Threads.h"
#include "../../include/common.h"
#include "../../include/memory/memory_pool.h"

MemoryPool* g_memoryPool;
MemoryChunk* g_newGeneration;
MemoryChunk* g_oldGeneration;

/*
 * Class:     org_xyz_jvm_jdk_Threads
 * Method:    createVm
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_jdk_Threads_createVm
        (JNIEnv * env, jclass clazz) {
    // 创建内存池
    g_memoryPool = new MemoryPool();
    // 从操作系统拿内存
    // 新生代
    g_newGeneration = g_memoryPool->new_chunk(80);
    // 老年代
    g_oldGeneration = g_memoryPool->new_chunk(160);
}