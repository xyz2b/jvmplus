//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_STACKVALUE_HPP
#define JVMPLUS_STACKVALUE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include <jni.h>

class StackValue {
private:
    BasicType _type;
    char _data[8];

public:
    StackValue(BasicType type, char v) {
        _type = type;
        *_data = v;
    }

    StackValue(BasicType type, int v) {
        _type = type;
        *(int*)_data = v;
    }

    StackValue(BasicType type, long v) {
        _type = type;
        *(jlong*)_data = v;
    }

    StackValue(BasicType type, double v) {
        _type = type;
        *(double*)_data = v;
    }

    StackValue(BasicType type, jobject v) {
        _type = type;
        *(jlong*)_data = (jlong) v;
    }

    jlong data() {
        return *(jlong*)_data;
    }

};


#endif //JVMPLUS_STACKVALUE_HPP
