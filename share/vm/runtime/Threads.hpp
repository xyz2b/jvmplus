//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_THREADS_HPP
#define JVMPLUS_THREADS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/runtime/javaThread.hpp"

class Threads {
private:
    // 所有Java基本线程（真实线程）都存储在这个List中
    static Array<Thread*>* _threads;

    // 当前线程
    static Thread* _current_thread;

public:
    Threads() {}

    static void set_threads(Array<Thread*>* threads) { _threads = threads; }

    static JavaThread* current_thread() { return (JavaThread*) _current_thread; }
    static void set_current_thread(Thread* thread) { _current_thread = thread; }

    static Thread* get_thread(int index) { return _threads->get_at(index); }
    static void add_thread(Thread* thread) { _threads->add(thread); }
};


#endif //JVMPLUS_THREADS_HPP
