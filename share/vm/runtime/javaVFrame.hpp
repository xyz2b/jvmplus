//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_JAVAVFRAME_HPP
#define JVMPLUS_JAVAVFRAME_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/runtime/stackValue.hpp"
#include "../../../share/vm/runtime/VFrame.hpp"

class Method;

class JavaVFrame : public VFrame {
private:
    Array<StackValue*>* _local_variable_table;

    Array<StackValue*>* _operand_stack;

    Method* _method;

public:
    JavaVFrame(int max_locals, Method* method) : _method(method) {
        if (max_locals > 0) {
            _local_variable_table = new Array<StackValue*>(max_locals);
        }
        _operand_stack = new Array<StackValue*>();
    }

    Array<StackValue*>* get_local_variable_table() { return _local_variable_table; }
    Array<StackValue*>* get_operand_stack() { return _operand_stack; }

    void set_local_variable_table(int index, StackValue* value) {
        _local_variable_table->set_at(index, value);
    }
    StackValue* get_local_variable_table(int index) {
        return _local_variable_table->get_at(index);
    }

    int operand_stack_size() {
        return _operand_stack->size();
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
