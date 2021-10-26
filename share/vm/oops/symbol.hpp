//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_SYMBOL_HPP
#define JVMPLUS_SYMBOL_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/adlc/adlc.hpp"
#include "../../../share/vm/memory/alloction.hpp"

class SymbolBase : public MetaspaceObj {
public:
    unsigned short  _length;

    unsigned short set_length(int length) { _length = length; }
};

// 字符串
class Symbol : public SymbolBase {
public:
    jbyte   _body[0];

public:
    // 将字符存到Symbol底层字符串的对应索引位置
    void byte_at_put(int index, int value) {
        assert(index >=0 && index < _length, "symbol index overflow");
        _body[index] = value;
    }

    Symbol() {}
    Symbol(const char* name, int length);

    Symbol(char name);

    bool operator == (const char* s) const {
        return strcmp(this->as_C_string(), s) == 0;
    }

    Symbol* operator + (const Symbol* s) const {
        size_t new_size = this->size() + s->size();
        Symbol* new_symbol = new (new_size) Symbol();
        new_symbol->set_length(new_size);

        for(int i = 0; i < this->size(); i++) {
            new_symbol->byte_at_put(i, this->byte_at(i));
        }

        for (int i = 0; i < s->size(); i++) {
            new_symbol->byte_at_put(i + this->size(), s->byte_at(i));
        }

        return new_symbol;
    }

    Symbol* operator + (const char* s) const {
        size_t new_size = this->size() + strlen(s);
        Symbol* new_symbol = new (new_size) Symbol();
        new_symbol->set_length(new_size);

        for(int i = 0; i < this->size(); i++) {
            new_symbol->byte_at_put(i, this->byte_at(i));
        }

        for (int i = 0; i < strlen(s); i++) {
            new_symbol->byte_at_put(i + this->size(), *(s + i));
        }

        return new_symbol;
    }


    bool operator == (const Symbol* s) const {
        return strcmp(this->as_C_string(), s->as_C_string()) == 0;
    }

    bool operator< (const Symbol& s) const {
        return strcmp(this->as_C_string(), s.as_C_string());
    }

    int find_char_index(char c);

    Symbol* sub_symbol(int start, int end);

    // 获取Symbol底层字符串首字符的地址
    const jbyte* base() const { return &_body[0]; }

    // 返回经过Symbol包装后的字符串的大小（添加了对象的一些额外信息）
    int size() const { return utf8_length(); }

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

    bool start_with(Symbol* s);
    bool start_with(const char* s);

    char* as_utf8() const { return as_C_string(); }
};

using std::less;
namespace std {
    template<>
    struct less<Symbol*> {
    public:
        bool operator() (const Symbol* s1, const Symbol* s2) const {
            if (strcmp(s1->as_C_string(), s2->as_C_string()) < 0) {
                return true;
            }
            return false;
        }
    };
}

#endif //JVMPLUS_SYMBOL_HPP
