//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_SYMBOL_HPP
#define JVMPLUS_SYMBOL_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/adlc/adlc.hpp"

// 字符串
class Symbol {
private:
    unsigned short  _length;
    jbyte   _body[1];

    // 将字符存到Symbol底层字符串的对应索引位置
    void byte_at_put(int index, int value) {
        assert(index >=0 && index < _length, "symbol index overflow");
        _body[index] = value;
    }

    Symbol(const u1* name, int length);
    void* operator new(size_t sz, int len) throw();

    void operator delete (void* p);

public:
    // 获取Symbol底层字符串首字符的地址
    const jbyte* base() const { return &_body[0]; }

    // 返回经过Symbol包装后的字符串的大小（添加了对象的一些额外信息）
    int size() { return utf8_length(); }

    // 返回Symbol底层字符串的长度
    int utf8_length() const { return _length; }

    // 将Symbol底层的字符串转成C字符串(末尾需要加'0')
    char* as_C_string() const;
    char* as_C_string(char* buf, int size) const;


    int byte_at(int index) const {
        assert(index >=0 && index < _length, "symbol index overflow");
        // 获取底层字符串索引为index处的字符
        return base()[index];
    }

    char* as_utf8() const { return as_C_string(); }
};


#endif //JVMPLUS_SYMBOL_HPP
