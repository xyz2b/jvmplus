//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CONSTANTPOOL_HPP
#define JVMPLUS_CONSTANTPOOL_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/hashMap.hpp"
#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/adlc/adlc.hpp"
#include "../../../share/vm/utilities/constantTag.hpp"
#include "../../../share/vm/prims/jvm.h"

class Symbol;

class ConstantPool {
private:
    u2 _length;
    Array<u1>* _tags;
    Klass* _klass;

    void* _pool[0];

public:
    intptr_t* base() const { return (intptr_t*) (_pool); }

    Array<u1>* tags() const { return _tags; }

    ConstantPool(u2 length) : _length(length) {
        _tags = new Array<u1>(length);
    }

    ConstantPool(Klass* klass, u2 length) : _klass(klass), _length(length) {
        _tags = new Array<u1>(length);
    }

    void* operator new(size_t sz, int length) {
        // 申请存放常量池项的内存，指针存储（整数/浮点数可以直接存储在一个指针所占用的空间）
        // 常量池有多少项就申请多少个指针的内存空间
        char* s = (char *)malloc(sz + length * sizeof(*_pool));
        memset(s,0, sz + length * sizeof(*_pool));
        return s;
    }

    void set_klass(Klass* klass) { _klass = klass; }

    ConstantTag tag_at(u2 index) const {
        // 调用ConstantTag的构造方法构造ConstantTag对象
        return (ConstantTag) tags()->get_at(index);
    }

    void tag_at_put(u2 index, u1 tag) {
        _tags->set_at(index, tag);
    }

    int length() const {
        return _length;
    }

    bool is_within_bounds(int index) const {
        return 0 <= index && index < length();
    }

    Symbol** symbol_at_addr(int which) const {
        assert(is_within_bounds(which), "index out of bounds");
        return (Symbol**) &base()[which];
    }

    void symbol_at_put(int which, Symbol* s) {
        tag_at_put(which, JVM_CONSTANT_Utf8);
        *symbol_at_addr(which) = s;
    }

    Symbol* symbol_at(int which) {
        assert(tag_at(which).is_utf8(), "Corrupted constant pool");
        return *symbol_at_addr(which);
    }

};


#endif //JVMPLUS_CONSTANTPOOL_HPP
