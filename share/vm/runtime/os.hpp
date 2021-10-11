//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_OS_HPP
#define JVMPLUS_OS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

class os {
public:
    static int open(const char* path, int oflag, int mode);
    static int stat(const char* path, struct stat* sbuf);
    static size_t read(int fd, void* buf, unsigned int nBytes);
    static int close(int fd);
};


#endif //JVMPLUS_OS_HPP
