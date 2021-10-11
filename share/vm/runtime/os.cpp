//
// Created by xyzjiao on 10/11/21.
//

#include "os.hpp"

#ifndef O_DELETE
#define O_DELETE 0x10000
#endif

int os::open(const char *path, int oflag, int mode) {
    if (strlen(path) > PATH_MAX - 1) {
        errno = ENAMETOOLONG;
        return -1;
    }

    int fd;
    int o_delete = (oflag & O_DELETE);
    oflag = oflag & ~O_DELETE;

    fd = open64(path, oflag, mode);
    if (fd == -1) return -1;

    {
        struct stat64 buf64;
        int ret = fstat64(fd, &buf64);
        int st_mode = buf64.st_mode;

        if (ret != -1) {
            if ((st_mode & S_IFMT) == S_IFDIR) {
                errno = EISDIR;
                ::close(fd);
                return -1;
            }
        } else {
            ::close(fd);
            return -1;
        }
    }

    if (o_delete != 0) {
        unlink(path);
    }

    return fd;
}

int os::stat(const char* path, struct stat* sbuf) {
    char pathbuf[PATH_MAX];
    if (strlen(path) > PATH_MAX - 1) {
        errno = ENAMETOOLONG;
        return -1;
    }
    strcpy(pathbuf, path);
    return ::stat(pathbuf, sbuf);
}

size_t os::read(int fd, void* buf, unsigned int nBytes) {
    return ::read(fd, buf, nBytes);
}

int os::close(int fd) {
    return ::close(fd);
}