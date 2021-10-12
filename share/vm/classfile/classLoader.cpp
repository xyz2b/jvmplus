//
// Created by xyzjiao on 10/11/21.
//

#include "classLoader.hpp"

InstanceKlassHandle ClassLoader::load_classfile(Symbol *h_name) {
    const char* class_name = h_name->as_C_string();

    stringStream st;
    st.print_raw(h_name->as_utf8());
    st.print_raw(".class");
    const char* file_name = st.as_string();

    ClassFileStream* stream = nullptr;
    InstanceKlassHandle h;

    stream = open_stream(file_name);

    if (stream != nullptr) {
        printf("start parse class file: %s\n", file_name);
        // class file found, parse it
        ClassFileParser parser(stream);
        InstanceKlassHandle result = parser.parseClassFile(h_name);

    } else {
        printf("open class file failed\n");
    }

    return h;
}

ClassFileStream *ClassLoader::open_stream(const char *name) {
    // check if file exists
    struct stat st;
    if (os::stat(name, &st) == 0) {
        // found file, open it
        int file_handle = os::open(name, 0, 0);
        if (file_handle != -1) {
            // read contents into resource array
            u1* buffer = (u1*) calloc(st.st_size, sizeof(u1));
            size_t num_read = os::read(file_handle, (char*)buffer, st.st_size);
            // clos file;
            os::close(file_handle);
            // construct ClassFileStream
            if (num_read == (size_t)st.st_size) {
                return new ClassFileStream(buffer, st.st_size, name);
            }
        }
    }
    return nullptr;
}

