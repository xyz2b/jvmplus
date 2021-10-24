//
// Created by xyzjiao on 10/17/21.
//

#ifndef JVMPLUS_OOP_HPP
#define JVMPLUS_OOP_HPP

#include <jni.h>

class Klass;

class oopDesc {
private:
    Klass* _klass;

public:
    oopDesc(Klass* klass) : _klass(klass) {}

    Klass* klass() const { return _klass; }
    void set_klass(Klass* klass) { _klass = klass; }


    jobject get_field(int type, int offset);
};


#endif //JVMPLUS_OOP_HPP
