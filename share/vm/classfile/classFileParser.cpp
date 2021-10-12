//
// Created by xyzjiao on 10/11/21.
//

#include "classFileParser.hpp"

InstanceKlassHandle
ClassFileParser::parseClassFile(Symbol *name) {
    ClassFileStream* cfs = stream();

    unsigned char* ptr = cfs->buffer();
    unsigned char* end_ptr = cfs->buffer() + cfs->length();

    cfs = new ClassFileStream(ptr, end_ptr - ptr, cfs->source());
    set_stream(cfs);

    u4 magic = cfs->get_u4_fast();

    u2 minion_version = cfs->get_u2_fast();
    u2 major_version = cfs->get_u2_fast();

    printf("major version: %d\n", major_version);

    ConstantPoolHandle cp = parse_constant_pool();

//    InstanceKlass* instanceKlass = new InstanceKlass();
//    InstanceKlassHandle instanceKlassHandle(instanceKlass);
//    instanceKlassHandle->_access_flags;

}

ConstantPoolHandle ClassFileParser::parse_constant_pool() {
    ClassFileStream* cfs = stream();

    u2 length = cfs->get_u2_fast();
    printf("constant pool size: %d\n", length);
}

void ClassFileParser::parse_constant_pool_entries(int length) {

}

ClassFileParser::~ClassFileParser() {

}