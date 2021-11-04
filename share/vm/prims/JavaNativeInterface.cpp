//
// Created by xyzjiao on 10/20/21.
//

#include "JavaNativeInterface.hpp"
#include "../../../share/vm/runtime/Threads.hpp"
#include "../../../share/vm/oops/codeAttribute.hpp"
#include "../../../share/vm/interpreter/bytecodeInterpreter.hpp"

Method* JavaNativeInterface::get_method(InstanceKlass* klass, Symbol* method_name, Symbol* descriptor_name) {
    Array<Method*>* methods = klass->get_methods();

    for (int index = 0; index < klass->get_methods_count(); index++) {
        Method* method = methods->get_at(index);
        Symbol* tmp_method_name = klass->get_constant_pool()->symbol_at(method->name_index());
        Symbol* tmp_descriptor_name = klass->get_constant_pool()->symbol_at(method->signature_index());

        if (*tmp_method_name == method_name && *tmp_descriptor_name == descriptor_name) {
            INFO_PRINT("找到了方法: %s#%s", tmp_method_name->as_C_string(), tmp_descriptor_name->as_C_string());
            return method;
        }
    }

    ERROR_PRINT("没有找到方法: %s#%s", method_name->as_C_string(), descriptor_name->as_C_string());
    return nullptr;
}

void JavaNativeInterface::call_static_method(InstanceKlass *klass, Method *method) {
    // 调用方栈帧
    JavaVFrame* caller_frame = nullptr;
    // 获取当前线程
    JavaThread* current_thread = Threads::current_thread();

    ConstantPool* constant_pool = klass->get_constant_pool();

    if (!method->access_flags().is_static()) {
        ERROR_PRINT("只能调用静态方法");
    }

    // 判断是否有参数
    if (0 != method->descriptor()->method_params_size()) {
        // 这个判断是为了过滤调用main方法的情况，因为调用main方法时线程虚拟机栈中还是空的，没有任何栈帧，main方法的参数由JVM自动传入
        if (0 != current_thread->frame_size()) {
            // 实参在调用方的操作数栈中，需要将实参存到被调用方的局部变量表中，而不是压到被调用方的操作数栈中
            // 此时调用方的方法栈帧在当前线程虚拟机栈的栈顶，因为被调用方的方法栈帧还未压入虚拟机栈
            INFO_PRINT("获取调用方的方法栈帧");
            // 获取调用方的方法栈帧
            caller_frame = (JavaVFrame*) current_thread->top_frame();
        }
    } else {    // 无参数
        INFO_PRINT("方法 [%s] 没有参数", constant_pool->symbol_at(method->name_index())->as_C_string());
    }

    Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
    // 获取当前方法的Code属性
    CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);

    // 创建被调用方方法栈帧
    JavaVFrame* callee_frame = new JavaVFrame(code_attribute->get_max_locals(), method);

    // 使用调用方操作数栈中的实参给被调用方局部变量中的形参赋值
    // 调用方操作数栈栈顶的实参 对应 被调用方局部变量索引最大的形参
    // 同时非静态方法第一个形参是this指针，注意赋值
    // not handle main params
    if (nullptr != caller_frame) {
        // 由于是静态方法，所以参数在被调用方局部变量表中的位置是从0开始
        for (int i = method->descriptor()->method_params_size() - 1; i >= 0; i--) {
            callee_frame->set_local_variable_table(i, caller_frame->pop_operand_stack());
        }
    }

    // 将栈帧压入当前线程的操作数栈
    current_thread->push_frame(callee_frame);

    INFO_PRINT("第 %d 个栈帧", current_thread->frame_size());

    // 执行方法的任务交给字节码解释器
    BytecodeInterpreter::run(current_thread, method);

}

void JavaNativeInterface::call_method(InstanceKlass *klass, Method *method) {
    // 调用方栈帧
    JavaVFrame* caller_frame = nullptr;
    // 获取当前线程
    JavaThread* current_thread = Threads::current_thread();

    ConstantPool* constant_pool = klass->get_constant_pool();

    /*
     * 需要获取上一个方法栈帧的情况：
     * 1.非静态方法。因为需要给this赋值
     * 2.需要传参
     */
    if (!method->access_flags().is_static() || 0 != method->descriptor()->method_params_size()) {
        // 这个判断是为了过滤调用main方法的情况，因为调用main方法时线程虚拟机栈中还是空的，没有任何栈帧，main方法的参数由JVM自动传入
        if (0 != current_thread->frame_size()) {
            // 实参在调用方的操作数栈中，需要将实参存到被调用方的局部变量表中，而不是压到被调用方的操作数栈中
            // 此时调用方的方法栈帧在当前线程虚拟机栈的栈顶，因为被调用方的方法栈帧还未压入虚拟机栈
            INFO_PRINT("获取调用方的方法栈帧");
            // 获取调用方的方法栈帧
            caller_frame = (JavaVFrame*) current_thread->top_frame();
        }
    } else {    // 无参数
        INFO_PRINT("方法 [%s] 没有参数", constant_pool->symbol_at(method->name_index())->as_C_string());
    }

    Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
    // 获取当前方法的Code属性
    CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);

    // 创建被调用方方法栈帧
    JavaVFrame* callee_frame = new JavaVFrame(code_attribute->get_max_locals(), method);

    // 使用调用方操作数栈中的实参给被调用方局部变量中的形参赋值
    // 调用方操作数栈栈顶的实参 对应 被调用方局部变量索引最大的形参
    // 同时非静态方法第一个形参是this指针，注意赋值
    // not handle main params，main方法的参数
    if (nullptr != caller_frame) {
        if (method->access_flags().is_static()) {
            // 由于是静态方法，所以参数在被调用方局部变量表中的位置是从0开始
            for (int i = method->descriptor()->method_params_size() - 1; i >= 0; i--) {
                callee_frame->set_local_variable_table(i, caller_frame->pop_operand_stack());
            }
        } else {
            // 由于是非静态方法，被调用方局部变量表索引为0的位置是this，所以参数的位置是从1开始
            for (int i = method->descriptor()->method_params_size(); i > 0; i--) {
                callee_frame->set_local_variable_table(i, caller_frame->pop_operand_stack());
            }
            // 给this赋值
            callee_frame->set_local_variable_table(0, caller_frame->pop_operand_stack());
        }

    }

    // 将栈帧压入当前线程的操作数栈
    current_thread->push_frame(callee_frame);

    INFO_PRINT("第 %d 个栈帧", current_thread->frame_size());

    // 执行方法的任务交给字节码解释器
    BytecodeInterpreter::run(current_thread, method);
}

void JavaNativeInterface::call_method(InstanceKlass *klass, Method *method, Array<StackValue*>* params) {
    // 调用方栈帧
    JavaVFrame* caller_frame = nullptr;
    // 获取当前线程
    JavaThread* current_thread = Threads::current_thread();

    ConstantPool* constant_pool = klass->get_constant_pool();

    /*
     * 需要获取上一个方法栈帧的情况：
     * 1.非静态方法。因为需要给this赋值
     * 2.需要传参
     */
    if (!method->access_flags().is_static() || 0 != method->descriptor()->method_params_size()) {
        // 这个判断是为了过滤调用main方法的情况，因为调用main方法时线程虚拟机栈中还是空的，没有任何栈帧，main方法的参数由JVM自动传入
        if (0 != current_thread->frame_size()) {
            // 实参在调用方的操作数栈中，需要将实参存到被调用方的局部变量表中，而不是压到被调用方的操作数栈中
            // 此时调用方的方法栈帧在当前线程虚拟机栈的栈顶，因为被调用方的方法栈帧还未压入虚拟机栈
            INFO_PRINT("获取调用方的方法栈帧");
            // 获取调用方的方法栈帧
            caller_frame = (JavaVFrame*) current_thread->top_frame();
        }
    } else {    // 无参数
        INFO_PRINT("方法 [%s] 没有参数", constant_pool->symbol_at(method->name_index())->as_C_string());
    }

    Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
    // 获取当前方法的Code属性
    CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);

    // 创建被调用方方法栈帧
    JavaVFrame* callee_frame = new JavaVFrame(code_attribute->get_max_locals(), method);

    // 使用调用方操作数栈中的实参给被调用方局部变量中的形参赋值
    // 调用方操作数栈栈顶的实参 对应 被调用方局部变量索引最大的形参
    // 同时非静态方法第一个形参是this指针，注意赋值
    // not handle main params，main方法的参数
    if (nullptr != caller_frame) {
        if (method->access_flags().is_static()) {
            // 由于是静态方法，所以参数在被调用方局部变量表中的位置是从0开始
            for (int i = method->descriptor()->method_params_size() - 1, k = 0; i >= 0; i--, k++) {
                callee_frame->set_local_variable_table(i, params->get_at(k));
            }
        } else {
            // 由于是非静态方法，被调用方局部变量表索引为0的位置是this，所以参数的位置是从1开始
            for (int i = method->descriptor()->method_params_size(), k = 0; i > 0; i--, k++) {
                callee_frame->set_local_variable_table(i, params->get_at(k));
            }
            // 给this赋值
            callee_frame->set_local_variable_table(0, caller_frame->pop_operand_stack());
        }

    }

    // 将栈帧压入当前线程的操作数栈
    current_thread->push_frame(callee_frame);

    INFO_PRINT("第 %d 个栈帧", current_thread->frame_size());

    // 执行方法的任务交给字节码解释器
    BytecodeInterpreter::run(current_thread, method);
}

Method *JavaNativeInterface::get_vmethod(InstanceKlass *klass, Symbol *method_name, Symbol *descriptor_name) {
    Array<Method*>* methods = klass->vtable();
    Method* ret = nullptr;

    for (int i = 0; i < methods->size(); ++i) {
        Method* tmp = methods->get_at(i);

        if ((*tmp->name() == method_name) &&
        (*tmp->descriptor()->descriptor_info() == descriptor_name)) {
            ret = tmp;
        }
    }
    return ret;
}

Method *JavaNativeInterface::get_imethod(InstanceKlass *klass, Symbol *method_name, Symbol *descriptor_name) {
    Array<Method*>* methods = klass->itable();
    Method* ret = nullptr;

    for (int i = 0; i < methods->size(); ++i) {
        Method* tmp = methods->get_at(i);

        if ((*tmp->name() == method_name) &&
            (*tmp->descriptor()->descriptor_info() == descriptor_name)) {
            ret = tmp;
        }
    }
    return ret;
}
