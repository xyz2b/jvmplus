//
// Created by xyzjiao on 8/20/21.
//

#include "ostream.hpp"

void outputStream::update_position(const char *s, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char ch = s[i];
        // 遇到'\n'即当前行读取完了
        if (ch == '\n') {
            _newlines += 1;
            // +1，是'\n'
            _precount += _position + 1;
            _position = 0;
        } else if (ch == '\t') {    // TODO: \t的处理没懂
            // \t根据出现在行中的位置计算其最终显示的空格数，其最多显示8个空格
            /*
             * \t处于行首(_position=0，因为\t前没有别的字符了)，显示8个空格
             * \t处于行第二个位置(_position=1，\t前有一个字符)，显示7个空格
             * ...
             * \t处于行第七个位置(_position=6，\t前有六个字符)，显示2个空格
             * \t处于行第八个位置(_position=7，\t前有七个字符)，显示1个空格
             *
             * \t处于行第九个位置(_position=8，\t前有八个字符)，显示8个空格
             * \t处于行第十个位置(_position=9，\t前有九个字符)，显示7个空格
             * */
            int tw = 8 - (_position & 7);
            _position += tw;
            // 因为'\t'算是一个字符，但是_position是实根据际显示出来的字符进行计算的，而_precount是按照字符数量来算的
            // 因为_precount是遇到'\n'直接加上_position来计算的，而_position是按照实际显示的字符计算的，所以需要去掉多算的显示位置（1个\t可能会占多个显示位置）
            _precount -= tw - 1;    // _precount + _position = total count
        } else {
            _position += 1;
        }
    }
}

outputStream::outputStream(int width) {
    _width = width;
    _position = 0;
    _newlines = 0;
    _precount = 0;
    _indentation = 0;
}

outputStream::outputStream(int width, bool has_time_stamps) {
    _width = width;
    _position = 0;
    _newlines = 0;
    _precount = 0;
    _indentation = 0;
    if (has_time_stamps) _stamp.update();
}

stringStream::stringStream(size_t initial_bufsize) {
    buffer_length = initial_bufsize;
    buffer = (char*) calloc(buffer_length, sizeof(char));
    buffer_pos = 0;
    buffer_fixed = false;
}

stringStream::~stringStream() {

}

char* stringStream::as_string() {
    char* copy = (char*) calloc(buffer_pos + 1, sizeof(char));
    strncpy(copy, buffer, buffer_pos);
    copy[buffer_pos] = 0;
    return copy;
}

void stringStream::write(const char *s, size_t len) {
    size_t write_len = len;
    // +1是为了最后需要写入流结束标识符'\0'，但是buffer_pos的位置是指向'\0'之前的字符串位置的
    size_t end = buffer_pos + len + 1;

    // 如果写入的字符串长度大于buffer中剩余的长度，扩容
    if (end > buffer_length) {
        // 如果写入字符串之后的长度 小于 2倍现在的buffer的长度，则扩容到2倍现在buffer的大小，否则直接扩容到写入字符串之后的长度
        if (end < buffer_length * 2) {
            end = buffer_length * 2;
        }
        char *oldbuf = buffer;

        buffer = (char *) calloc(end, sizeof(char));
        strncpy(buffer, oldbuf, buffer_pos);
        buffer_length = end;
    }

    buffer[buffer_pos + write_len] = 0;
    strncpy(buffer + buffer_pos, s, write_len);
    buffer_pos += write_len;

    update_position(s, len);
}
