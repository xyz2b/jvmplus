//
// Created by xyzjiao on 10/17/21.
//

#include "oop.hpp"
#include "../../../share/vm/prims/unsafe.cpp"

jobject oopDesc::get_field(int type, int field_offset) {
    int offset = sizeof(oopDesc) + field_offset;
    jobject  field_val;
    switch (type) {
        case T_BOOLEAN:
        {
            GET_FIELD((jobject)this, offset, jboolean, bool_val);
            field_val = reinterpret_cast<jobject>(&bool_val);
        }
            break;
        case T_SHORT:
        {
            GET_FIELD((jobject)this, offset, jshort, short_val);
            field_val = reinterpret_cast<jobject>(&short_val);
        }
            break;
        case T_CHAR:
        {
            GET_FIELD((jobject)this, offset, jchar, char_val);
            field_val = reinterpret_cast<jobject>(&char_val);
        }
            break;
        case T_BYTE:
        {
            GET_FIELD((jobject)this, offset, jbyte, byte_val);
            field_val = reinterpret_cast<jobject>(&byte_val);
        }
            break;
        case T_INT:
        {
            GET_FIELD((jobject)this, offset, jint, int_val);
            field_val = reinterpret_cast<jobject>(&int_val);
        }
            break;
        case T_LONG:
        {
            GET_FIELD((jobject)this, offset, jlong, long_val);
            field_val = reinterpret_cast<jobject>(&long_val);
        }
            break;
        case T_DOUBLE:
        {
            GET_FIELD((jobject)this, offset, jdouble, double_val);
            field_val = reinterpret_cast<jobject>(&double_val);
        }
            break;
        case T_OBJECT:
        {
            GET_FIELD((jobject)this, offset, jobject, object_val);
            field_val = object_val;
        }
            break;
        default:
            ERROR_PRINT("无法识别的field类型%d", type);
            exit(-1);
    }
    return field_val;
}