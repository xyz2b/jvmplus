//
// Created by xyzjiao on 9/25/21.
//

#ifndef JVM_C___HASHMAP_H
#define JVM_C___HASHMAP_H
#include "jni.h"
#include "../../include/common.h"

class hashmap {
private:
    static jclass _clazz;
    static jmethodID _no_param_construct;
    static jmethodID _put_method;
    static jmethodID _get_method;
public:
    static void initialize();

    static jobject object();
    static void put(jobject obj, jobject key, jobject value);
    static void put(jobject obj, const string& key, jobject value);
    static jobject get(jobject obj, jobject key);
    static jobject get(jobject obj, const string& key);
};

#endif //JVM_C___HASHMAP_H
