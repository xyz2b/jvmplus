//
// Created by xyzjiao on 8/2/21.
//

#include "../../include/jni/org_xyz_jvm_jdk_Threads.h"
#include "../../../share/vm/memory/universe.hpp"


/*
 * Class:     org_xyz_jvm_jdk_Threads
 * Method:    createVm
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_jdk_Threads_createVm
        (JNIEnv * env, jclass clazz) {
    Universe::initialize_heap();
}