//
// Created by xyzjiao on 8/2/21.
//

#include "../../include/jni/org_xyz_jvm_example_jni_HelloWorld.h"
#include "../../include/common.h"

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    showVal
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_showVal
        (JNIEnv *env, jclass clazz) {
    // 1.获取属性信息
    jfieldID val_field = env->GetStaticFieldID(clazz, "val", "I");
    if (nullptr == val_field) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 2.读属性值（静态属性值在Class对象中）
    jint val = env->GetStaticIntField(clazz, val_field);

    // 3.打印
     INFO_PRINT("%d", val);
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    getVal
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_getVal
        (JNIEnv *env, jclass clazz) {
    // 1.获取属性信息
    jfieldID val_field = env->GetStaticFieldID(clazz, "val", "I");
    if (nullptr == val_field) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 2.读属性值（静态属性值在Class对象中）
    jint val = env->GetStaticIntField(clazz, val_field);

    // 3.返回
    return val;
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    setVal
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_setVal
        (JNIEnv *env, jclass clazz, jint new_value) {
    // 1.获取属性信息
    jfieldID val_field = env->GetStaticFieldID(clazz, "val", "I");
    if (nullptr == val_field) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 2.读属性值（静态属性值在Class对象中）
    jint old_val = env->GetStaticIntField(clazz, val_field);

    // 3.赋值新的值（静态属性值在Class对象中）
    env->SetStaticIntField(clazz, val_field, new_value);

    // 4.返回老的值
    return old_val;
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    showVal2
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_showVal2
        (JNIEnv *env, jobject object) {
    // 1.获取Class对象
    jclass clazz = env->GetObjectClass(object);
    if (nullptr == clazz) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 2.获取属性信息
    jfieldID val_field = env->GetFieldID(clazz, "val2", "I");
    if (nullptr == val_field) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 3.读取属性值（非静态属性值在对象中）
    jint val = env->GetIntField(object, val_field);

    // 4.打印
    INFO_PRINT("%d", val);
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    getVal2
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_getVal2
        (JNIEnv *env, jobject object){
    // 1.获取Class对象
    jclass clazz = env->GetObjectClass(object);
    if (nullptr == clazz) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 2.获取属性信息
    jfieldID val_field = env->GetFieldID(clazz, "val2", "I");
    if (nullptr == val_field) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 3.读取属性值（非静态属性值在对象中）
    jint val = env->GetIntField(object, val_field);

    // 4.返回
    return val;
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    setVal2
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_setVal2
        (JNIEnv *env, jobject object, jint new_value) {
    // 1.获取Class对象
    jclass clazz = env->GetObjectClass(object);
    if (nullptr == clazz) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 2.获取属性信息
    jfieldID val_field = env->GetFieldID(clazz, "val2", "I");
    if (nullptr == val_field) {
        ERROR_PRINT("出错了")
        exit(-1);
    }

    // 3.读取属性值（非静态属性值在对象中）
    jint old_val = env->GetIntField(object, val_field);

    // 4.赋值新的值（非静态属性值在对象中）
    env->SetIntField(object, val_field, new_value);

    // 5.返回老的值
    return old_val;
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    createThread
 * Signature: ()Lorg/xyz/jvm/example/jni/JavaThread;
 */
JNIEXPORT jobject JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_createThread
        (JNIEnv *env, jclass clazz) {
    // 1.获取JavaThread clazz（JNI传过来的clazz是HelloWorld的clazz，因为是在HelloWorld中定义的native方法）
    jclass java_thread_clazz = env->FindClass("org/xyz/jvm/example/jni/JavaThread");

    // 2.获取构造方法
    jmethodID construct = env->GetMethodID(java_thread_clazz, "<init>", "()V");
    if (nullptr == construct) {
        ERROR_PRINT("construct出错了")
        exit(-1);
    }

    // 3.创建对象
    // env->AllocObject(java_thread_clazz);    // 只申请内存，即只执行了new字节码指令，后面的还未执行
    jobject object = env->NewObject(java_thread_clazz, construct);  // 创建对象，包括申请内存，执行构造函数

    // 4.返回
    return object;
}

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    threadRunFast
 * Signature: (Lorg/xyz/jvm/example/jni/JavaThread;)V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_threadRunFast
        (JNIEnv *env, jclass clz, jobject object) {
    // clz是本JNI native方法所在类的Class对象，即HelloWorld类的Class对象

    // 这里的object是JNI native方法传入的参数，就是JavaThread对象，因为native传入的参数就是JavaThread对象

    // 1.获取JavaThread的Class对象
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

/*
 * Class:     org_xyz_jvm_example_jni_HelloWorld
 * Method:    throwException
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_HelloWorld_throwException
        (JNIEnv *env, jobject object) {
    // 1.获取类的元信息
    jclass c = env->FindClass("java/lang/RuntimeException");

    // 4.抛出异常（自动创建异常对象）
    env->ThrowNew(c, "test runtime exception");
}