//
// Created by xyzjiao on 8/2/21.
//

#ifndef JVM_C___JNITOOLS_H
#define JVM_C___JNITOOLS_H

#include <jni.h>
#include "../../include/common.h"
#include "../../share/vm/oops/symbol.hpp"
#include "../../share/vm/oops/klass.hpp"


class JniTools {
public:
    // 字符串处理
    static const char* getCharsFromJString(jstring str, jboolean is_copy);
    static jstring charsToJstring(const char* str);
    static string* jstringToString(jstring str, jboolean is_copy);
    static jstring stringToJString(const string& str);
    static jobject stringToJavaString(const string& s);
    static string *charsToString(const char *s);
    static jobject charsToJavaString(const char* s);
    static const char *stringToChars(const string &str);
    static jobject jstringToJavaString(jstring string, jboolean is_copy);
    static Symbol* jstringToSymbol(jstring string, jboolean is_copy);

    static jmethodID get_method(const char* class_name, const char* method_name, const char* descriptor_name);
    static jboolean is_load_class(Symbol* class_name);
    static Klass* load_class(Symbol* class_name);
    static Klass* find_load_class(Symbol* class_name);
};


#endif //JVM_C___JNITOOLS_H
