//
// Created by ziya on 2021/10/22.
//

#ifndef JVMPLUS_STACK_HPP
#define JVMPLUS_STACK_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/memory/metaspace.hpp"
#include <stack>

template<class K>
class Stack : public std::stack<K> {
public:
    void* operator new (size_t size) {
        return Metaspace::allocate(size)->value();
    }
    void operator delete(void* p) {}
};


#endif //JVMPLUS_STACK_HPP
