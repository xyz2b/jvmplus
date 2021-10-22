//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_JAVATHREAD_HPP
#define JVMPLUS_JAVATHREAD_HPP

#include "../../../share/vm/runtime/VFrame.hpp"
#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/runtime/Thread.hpp"
#include "../../../share/vm/utilities/stack.hpp"

class JavaThread : public Thread {
private:
    // 当前线程的虚拟机栈，VFrame是栈帧
    Stack<VFrame*>* _stack;

public:
    JavaThread() {
        _stack = new Stack<VFrame*>();
    }

    VFrame* pop_frame() {
        VFrame* frame = _stack->top();
        _stack->pop();
        return frame;
    }

    void push_frame(VFrame* frame) {
        _stack->push(frame);
    }

    VFrame* top_frame() {
        return _stack->top();
    }

    int frame_size() {
        return _stack->size();
    }

};


#endif //JVMPLUS_JAVATHREAD_HPP
