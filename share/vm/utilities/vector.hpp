//
// Created by ziya on 2021/10/22.
//

#ifndef JVMPLUS_VECTOR_HPP
#define JVMPLUS_VECTOR_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/memory/metaspace.hpp"
#include <vector>

template<class K>
class Vector : public std::vector<K> {
public:
    void* operator new (size_t size) {
        return Metaspace::allocate(size)->value();
    }
    void operator delete(void* p) {}
};

#endif //JVMPLUS_VECTOR_HPP
