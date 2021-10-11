//
// Created by xyzjiao on 10/11/21.
//

#include "symbol.hpp"

Symbol::Symbol(const u1 *name, int length) {
    _length = length;
    for (int i = 0; i < _length; i ++) {
        byte_at_put(i, name[i]);
    }
}

// 将Symbol底层字符串转成C字符串
char* Symbol::as_C_string() const {
    int len = utf8_length();
    char* str = (char*) calloc(len + 1, sizeof(char));
    return as_C_string(str, len + 1);
}

// 将Symbol底层字符串转成C字符串，可以指定需要转换成C字符串的长度，会根据转成的C字符串的长度来对底层字符串进行截断
char* Symbol::as_C_string(char *buf, int size) const {
    if (size > 0) {
        int len = MIN2(size - 1, utf8_length());
        for (int i = 0; i < len; i++) {
            buf[i] = byte_at(i);
        }
        buf[len] = '\0';
    }
    return buf;
}

// new --> operator new
void *Symbol::operator new(size_t sz, int len) throw() {
    char* s = (char *)calloc(1, len);
    return s;
}

void Symbol::operator delete(void *p) {

}
