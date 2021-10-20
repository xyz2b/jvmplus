//
// Created by xyzjiao on 10/19/21.
//

#include "../../include/jni/org_xyz_jvm_jdk_classes_JniEnv.h"
#include "../../include/common.h"
#include "../../include/jni/JniTools.h"
#include "../../share/vm/oops/klass.hpp"
#include "../../share/vm/classfile/systemDictionary.h"
#include "../../include/jni/Handle.h"

JNIEXPORT jobject JNICALL Java_org_xyz_jvm_jdk_classes_JniEnv_loadClassFile
        (JNIEnv *env, jclass clazz, jstring class_name) {
    const char* name = JniTools::getCharsFromJString(class_name, true);
    Symbol* s = new (strlen(name)) Symbol(name, strlen(name));
    Klass* klass = SystemDictionary::resolve_or_null(s);

    INFO_PRINT("AppClassLoader load klass, %s : %p", name, klass);

    return Handle::klassToHandle(env, klass);
}