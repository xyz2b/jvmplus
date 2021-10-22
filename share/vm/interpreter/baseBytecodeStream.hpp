//
// Created by ziya on 2021/10/19.
//

#ifndef JVMPLUS_BASEBYTECODESTREAM_HPP
#define JVMPLUS_BASEBYTECODESTREAM_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/oops/codeAttribute.hpp"
#include "../../../cpu/x86/vm/bytes_x86.hpp"

class BaseBytecodeStream {
private:
    u1*   _code_start; // Buffer bottom
    u1*   _code_end;   // Buffer top (one past last element)
    u1*   _current;      // Current buffer position

    Method* _method;
public:
    BaseBytecodeStream(u1* code_start, u1* code_end, Method* method): _code_start(code_start), _code_end(code_end), _method(method), _current(code_start) {}

    u1 get_u1_code() {
        checkIndex();

        return *_current;
    }

    int8_t get_u1_code2() {
        checkIndex();

        return *(int8_t*)*_current;
    }

    u2 get_u2_code() {
        checkIndex();

        u2 res = Bytes::get_java_u2(_current);
        _current += 2;
        return res;
    }

    void checkIndex() {
        if (_current < _code_start || _current > _code_end) {
            ERROR_PRINT("字节码指令的索引超过了最大值");
            exit(-1);
        }
    }

    void reset() {
        _current = _code_start;
    }

    bool end() {
        return _current > _code_end;
    }

    void inc(size_t step) {
        _current +=  step;
    }

    void setIndex(size_t index) {
        _current = _code_end;
        _current += index;
    }

    size_t current_index() {
        return _current - _code_start;
    }
};

#endif //JVMPLUS_BASEBYTECODESTREAM_HPP
