//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSFILESTREAM_HPP
#define JVMPLUS_CLASSFILESTREAM_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

// 读取字节码文件的流信息
class ClassFileStream {
private:
    u1*   _buffer_start; // Buffer bottom
    u1*   _buffer_end;   // Buffer top (one past last element)
    u1*   _current;      // Current buffer position
    const char* _source; // Source of stream (class file name)

public:
    ClassFileStream(u1* buffer, int length, const char* source);

    u1 get_u1_fast();

    u2 get_u2_fast();


};



#endif //JVMPLUS_CLASSFILESTREAM_HPP
