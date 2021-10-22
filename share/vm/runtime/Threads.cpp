//
// Created by ziya on 2021/10/19.
//

#include "Threads.hpp"


// 所有Java基本线程（真实线程）都存储在这个List中
Vector<Thread*>*Threads::_threads = nullptr;

// 当前线程
Thread* Threads::_current_thread = nullptr;