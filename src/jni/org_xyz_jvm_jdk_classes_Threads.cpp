//
// Created by xyzjiao on 8/2/21.
//

#include "../../include/jni/org_xyz_jvm_jdk_classes_Threads.h"
#include "../../share/vm/memory/universe.hpp"
#include "../../share/vm/memory/metaspace.hpp"

JNIEnv* g_env;

/*
 * Class:     org_xyz_jvm_jdk_Threads
 * Method:    createVm
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_jdk_classes_Threads_createVm
        (JNIEnv *env, jclass clazz) {
    g_env = env;
    Universe::initialize_heap();
    Metaspace::initialize();
}