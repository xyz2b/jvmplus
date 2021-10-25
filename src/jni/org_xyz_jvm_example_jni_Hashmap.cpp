//
// Created by xyzjiao on 9/25/21.
//

#include "../../include/jni/org_xyz_jvm_example_jni_Hashmap.h"
#include "../../include/common.h"
#include "../../include/jni/JniTools.h"
#include "../../include/jni/Hashmap.h"

/*
 * Class:     org_xyz_jvm_example_jni_Hashmap
 * Method:    createHashmap
 * Signature: ()Ljava/util/Hashmap;
 */
JNIEXPORT jobject JNICALL Java_org_xyz_jvm_example_jni_Hashmap_createHashmap
        (JNIEnv *env, jclass clz) {

    hashmap::initialize();

    return hashmap::object();
}

/*
 * Class:     org_xyz_jvm_example_jni_Hashmap
 * Method:    get
 * Signature: (Ljava/util/Hashmap;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_xyz_jvm_example_jni_Hashmap_get__Ljava_util_HashMap_2Ljava_lang_String_2
        (JNIEnv *env, jclass clz, jobject hp, jstring key) {
    // 调用get方法，并返回结果
    // jstring --> c++ string
    return hashmap::get(hp, *JniTools::jstringToString(key, 0));
}

/*
 * Class:     org_xyz_jvm_example_jni_Hashmap
 * Method:    get
 * Signature: (Ljava/util/Hashmap;Ljava/lang/Object;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_xyz_jvm_example_jni_Hashmap_get__Ljava_util_HashMap_2Ljava_lang_Object_2
        (JNIEnv *env, jclass clz, jobject hp, jobject key) {
    return hashmap::get(hp, key);
}

/*
 * Class:     org_xyz_jvm_example_jni_Hashmap
 * Method:    put
 * Signature: (Ljava/util/Hashmap;Ljava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_Hashmap_put__Ljava_util_HashMap_2Ljava_lang_String_2Ljava_lang_Object_2
        (JNIEnv *env, jclass clz, jobject hp, jstring key, jobject value) {
    hashmap::put(hp, *JniTools::jstringToString(key, 0), value);
}

/*
 * Class:     org_xyz_jvm_example_jni_Hashmap
 * Method:    put
 * Signature: (Ljava/util/Hashmap;Ljava/lang/Object;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_example_jni_Hashmap_put__Ljava_util_HashMap_2Ljava_lang_Object_2Ljava_lang_Object_2
        (JNIEnv *env, jclass clz, jobject hp, jobject key, jobject value) {
    // jstring --> c++ string
    hashmap::put(hp, key, value);
}

