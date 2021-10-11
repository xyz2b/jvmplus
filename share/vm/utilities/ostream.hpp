//
// Created by xyzjiao on 8/20/21.
//

#ifndef JVM_C___OSTREAM_HPP
#define JVM_C___OSTREAM_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

class outputStream {
protected:
    int     _indentation;        // 当前缩进 current indentation
    int     _width;              // 页面的宽度 width of the page (每一行的大小)
    int     _position;           // 当前行中所读取的位置 position of the current line (每一行大小知道，在第几行知道，在目前行的位置知道，就知道了当前位置在整个流中的位置)
    int     _newlines;           // 到目前为止输出的'\n'的数量 number of '\n' output so far (通过输出多少个'\n'知道目前在第几行)
    julong  _precount;           // 读取的完整行的字符数量，不包括当前行读取过的字符数量，因为当前行没有读取完
    TimeStamp   _stamp;          // for time stamps

    // 更新读取的位置
    void update_position(const char* s, size_t len);

public:
    outputStream(int width = 80);
    outputStream(int width, bool has_time_stamps);

    // 操作缩进的方法
    outputStream& indent();
    void inc() { _indentation++; }
    void dec() { _indentation--; }
    void inc(int n) { _indentation += n; }
    void dec(int n) { _indentation -= n; }
    int indentation() const { return _indentation; }
    void set_indentation(int i) { _indentation = i; }
    //
    void fill_to(int col);
    //
    void move_to(int col, int slop = 6, int min_space = 2);

    // sizing
    int width() const { return _width; }
    int position() const { return _position; }
    int newlines() const { return _newlines; }

    void print_raw(const char* str) { write(str, strlen(str)); }
    void print_raw(const char* str, int len) { write(str, len); }

    // 纯虚函数
    virtual void write(const char* str, size_t len) = 0;

};

// 可变字符串
class stringStream : public outputStream {
protected:
    char*   buffer;
    size_t  buffer_pos;
    size_t  buffer_length;
    // 是否固定长度不可变
    bool    buffer_fixed;

public:
    stringStream(size_t initial_bufsize = 256);
    ~stringStream();

    // 将从当前位置往前的字符流转成C string
    char* as_string();

    // 将c string 写入buffer，追加形式
    virtual void write(const char* str, size_t len);
};

#endif //JVM_C___OSTREAM_HPP
