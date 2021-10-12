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

    INFO_PRINT("major version: %d\n", major_version)

    ConstantPoolHandle cp = parse_constant_pool();

//    InstanceKlass* instanceKlass = new InstanceKlass();
//    InstanceKlassHandle instanceKlassHandle(instanceKlass);
//    instanceKlassHandle->_access_flags;

}

ConstantPoolHandle ClassFileParser::parse_constant_pool() {
    INFO_PRINT("parse constant pool")
    ClassFileStream* cfs = stream();

    u2 length = cfs->get_u2_fast();
    INFO_PRINT("constant pool size: %d\n", length)

    ConstantPool* constant_pool = new (length) ConstantPool(length);
    _cp = constant_pool;

    ConstantPoolHandle cp (constant_pool);

    parse_constant_pool_entries(length);
    

}

// 解析常量池的项
void ClassFileParser::parse_constant_pool_entries(int length) {

    ClassFileStream* cfs = stream();
    
    for (int index = 1; index < length; index++) {
        u1 tag = cfs->get_u1_fast();
        switch (tag) {
            case JVM_CONSTANT_Utf8:
                {
                    u2 utf8_length = cfs->get_u2_fast();
                    u1* utf8_buffer = cfs->get_u1_buffer();
                    cfs->skip_u1_fast(utf8_length);

                    Symbol* s = new (utf8_length) Symbol(utf8_buffer, utf8_length);

                    INFO_PRINT("constant utf8: %s\n", s->as_C_string())

                    _cp->symbol_at_put(index, s);
                }
                break;
            case JVM_CONSTANT_Integer:
                {
                    u4 bytes = cfs->get_u4_fast();

                }
                break;
            case JVM_CONSTANT_Float:
                {
                    u4 bytes = cfs->get_u4_fast();

                }
                break;
        }
    }
}

ClassFileParser::~ClassFileParser() {

}