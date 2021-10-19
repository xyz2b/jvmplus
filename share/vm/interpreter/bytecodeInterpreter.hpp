//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_BYTECODEINTERPRETER_HPP
#define JVMPLUS_BYTECODEINTERPRETER_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/runtime/javaThread.hpp"
#include "../../../share/vm/oops/method.hpp"

class BytecodeInterpreter {
public:
    static void run(JavaThread* currentThread, Method* method);
};


#endif //JVMPLUS_BYTECODEINTERPRETER_HPP
