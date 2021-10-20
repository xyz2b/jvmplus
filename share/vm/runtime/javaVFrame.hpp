//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_JAVAVFRAME_HPP
#define JVMPLUS_JAVAVFRAME_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/runtime/stackValue.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/runtime/VFrame.hpp"
#include <stack>

class JavaVFrame : public VFrame {
private:
    Array<StackValue*>* _local_variable_table;

    stack<StackValue*>* _operand_stack;

    Method* _method;

public:
    JavaVFrame(int max_locals, int max_stacks, Method* method) : _method(method) {
        _local_variable_table = new (max_locals) Array<StackValue*>(max_locals);
        _operand_stack = new stack<StackValue*>;
    }

    Array<StackValue*>* get_local_variable_table() { return _local_variable_table; }
    stack<StackValue*>* get_operand_stack() { return _operand_stack; }

    void set_local_variable_table(StackValue* value,int index) {
        _local_variable_table->set_at(index, value);
    }
    StackValue* get_local_variable_table(int index) {
        return _local_variable_table->get_at(index);
    }

    StackValue* pop_operand_stack() {
        StackValue* value = _operand_stack->top();
        _operand_stack->pop();
        return value;
    }
    void push_operand_stack(StackValue* value) {
        _operand_stack->push(value);
    }
    StackValue* top_operand_stack() {
        return _operand_stack->top();
    }
};


#endif //JVMPLUS_JAVAVFRAME_HPP