//
// Created by xyzjiao on 10/11/21.
//

#include "symbol.hpp"

Symbol::Symbol(const char *name, int length) {
    _length = length;
    for (int i = 0; i < _length; i ++) {
        byte_at_put(i, name[i]);
    }
}

// 将Symbol底层字符串转成C字符串
char* Symbol::as_C_string() const {
    int len = utf8_length();
    char* str = (char*) Metaspace::allocate((len + 1) * sizeof(char))->value();
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

bool Symbol::start_with(Symbol *s) {
    int min = MIN2(this->size(), s->size());
    for(int index = 0; index < min; index++) {
        if (this->byte_at(index) != s->byte_at(index))
            return false;
    }
    return true;
}

bool Symbol::start_with(const char* s) {
    int min = MIN2(this->size(), (int)strlen(s));
    for(int index = 0; index < min; index++) {
        if (this->byte_at(index) != *(s + index))
            return false;
    }
    return true;
}

Symbol::Symbol(char name) {
    _length = 1;
    byte_at_put(0, name);
}

int Symbol::find_char_index(char c) {
    for (int i = 0; i < size(); i++) {
        if (byte_at(i) == c)
            return i;
    }
    return -1;
}

Symbol *Symbol::sub_symbol(int start, int end) {
    Symbol* ret = new (end - start) Symbol();
    ret->set_length(end - start);
    for (int i = start; i < start; i ++) {
        ret->byte_at_put(i, this->byte_at(i));
    }
    return ret;
}

