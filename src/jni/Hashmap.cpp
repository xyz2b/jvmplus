//
// Created by xyzjiao on 9/25/21.
//

#include "../../include/jni/JniTools.h"
#include "../../include/jni/Hashmap.h"

extern JNIEnv* g_env;

jclass hashmap::_clazz = NULL;
jmethodID hashmap::_no_param_construct = NULL;
jmethodID hashmap::_put_method = NULL;
jmethodID hashmap::_get_method = NULL;

void hashmap::initialize() {
    // 获取HashMap类元信息
    _clazz = g_env->FindClass("java/util/HashMap");
    if (nullptr == _clazz) {
        ERROR_PRINT("获取clz出错")
        exit(-1);
    }

    // 获取构造方法
    _no_param_construct = g_env->GetMethodID(_clazz, "<init>", "()V");
    if (nullptr == _no_param_construct) {
        ERROR_PRINT("获取构造方法出错")
        exit(-1);
    }

    // 获取get方法
    _get_method = g_env->GetMethodID(_clazz, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
    if (nullptr == _get_method) {
        ERROR_PRINT("获取get方法出错")
        exit(-1);
    }

    // 获取put方法
    _put_method = g_env->GetMethodID(_clazz, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    if (nullptr == _put_method) {
        ERROR_PRINT("获取put方法出错")
        exit(-1);
    }
}

jobject hashmap::object() {
    if (nullptr == _clazz) {
        ERROR_PRINT("未执行初始化")
        exit(-1);
    }

    // 创建对象
    return g_env->NewObject(_clazz, _no_param_construct);
}

void hashmap::put(jobject obj, jobject key, jobject value) {
    if (nullptr == _clazz) {
        ERROR_PRINT("未执行初始化")
        exit(-1);
    }

    // 调用put方法
    g_env->CallObjectMethod(obj, _put_method, key, value);
}

void hashmap::put(jobject obj, const string &key, jobject value) {
    if (nullptr == _clazz) {
        ERROR_PRINT("未执行初始化")
        exit(-1);
    }

    // c++ string --> java string
    put(obj, JniTools::stringToJavaString(key), value);
}

jobject hashmap::get(jobject obj, jobject key) {
    if (nullptr == _clazz) {
        ERROR_PRINT("未执行初始化")
        exit(-1);
    }

    // 调用get方法，并返回结果
    return g_env->CallObjectMethod(obj, _get_method, key);
}

jobject hashmap::get(jobject obj, const string &key) {
    if (nullptr == _clazz) {
        ERROR_PRINT("未执行初始化")
        exit(-1);
    }

    // c++ string --> java string
    return get(obj, JniTools::stringToJavaString(key));
}




