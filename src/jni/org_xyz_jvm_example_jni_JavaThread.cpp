//
// Created by xyzjiao on 8/2/21.
//

#include "../../include/jni/org_xyz_jvm_example_jni_JavaThread.h"
#include "../../include/common.h"

/*
 * Class:     org_xyz_jvm_example_jni_JavaThread
 * Method:    start0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_JavaThread_start0
        (JNIEnv *env, jobject object) {
    // 1.获取Class对象
    jclass clazz = env->GetObjectClass(object);
    if (nullptr == clazz) {
        ERROR_PRINT("clazz出错了")
        exit(-1);
    }

    // 2.获取run方法
    jmethodID run_method = env->GetMethodID(clazz, "run", "()V");
    if (nullptr == run_method) {
        ERROR_PRINT("run_method出错了")
        exit(-1);
    }

    // 3.调用run方法（非静态方法属于对象）
    env->CallVoidMethod(object, run_method);
}