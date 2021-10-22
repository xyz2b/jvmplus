//
// Created by ziya on 2021/10/19.
//

#include "bytecodeInterpreter.hpp"

#include "../../../share/vm/oops/codeAttribute.hpp"
#include "../../../share/vm/interpreter/baseBytecodeStream.hpp"
#include "../../../share/vm/runtime/javaVFrame.hpp"

extern JNIEnv* g_env;

void BytecodeInterpreter::run(JavaThread* current_thread, Method* method) {
    Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
    CodeAttribute* codeAttribute = (CodeAttribute*) method->get_attribute(c);

    BaseBytecodeStream* code = new BaseBytecodeStream((u1*)codeAttribute->code_base(), (u1*)codeAttribute->code_end(), method);

    while (!code->end()) {
        u1 opcode = code->get_u1_code();

        switch (opcode) {
            case NOP:
                {
                    INFO_PRINT("执行指令: nop，该指令功能: 什么也不做");
                }
                break;
            case ACONST_NULL:
                {
                    INFO_PRINT("执行指令: aconst_null，该指令功能: 将一个null对象引用压入栈顶");
                    JavaVFrame* frame = (JavaVFrame*) current_thread->top_frame();

                    frame->push_operand_stack(new StackValue(T_OBJECT, nullptr));
                }
                break;
            default:
                ERROR_PRINT("not bytecode");
                exit(-1);
        }
    }
}
