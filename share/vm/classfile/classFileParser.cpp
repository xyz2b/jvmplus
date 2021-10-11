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

    printf("%x\n", magic);
}

ConstantPoolHandle ClassFileParser::parse_constant_pool() {

}

void ClassFileParser::parse_constant_pool_entries(int length) {

}

ClassFileParser::~ClassFileParser() {

}