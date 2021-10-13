//
// Created by xyzjiao on 10/11/21.
//

#include "classFileParser.hpp"

InstanceKlassHandle
ClassFileParser::parse_class_file(Symbol *name) {
    ClassFileStream* cfs = stream();

    unsigned char* ptr = cfs->buffer();
    unsigned char* end_ptr = cfs->buffer() + cfs->length();

    cfs = new ClassFileStream(ptr, end_ptr - ptr, cfs->source());
    set_stream(cfs);

    InstanceKlass* instance_klass = new InstanceKlass();
    InstanceKlassHandle ik (instance_klass);

    // 魔数
    u4 magic = cfs->get_u4_fast();

    // 主次版本号
    u2 minion_version = cfs->get_u2_fast();
    u2 major_version = cfs->get_u2_fast();

    INFO_PRINT("major version: %d", major_version);

    // 常量池
    ConstantPoolHandle cp = parse_constant_pool();

    // 类的访问权限及属性
    jint flags = cfs->get_u2_fast();
    AccessFlags access_flags(flags);
    INFO_PRINT("access flags: %d", major_version);

    // 类名
    u2 this_class_index = cfs->get_u2_fast();
    Symbol* class_name = cp->get_class_name_by_class_index(this_class_index);
    INFO_PRINT("class name: %s", class_name->as_C_string());

    // 父类名
    u2 super_class_index = cfs->get_u2_fast();
    Symbol* super_class_name = cp->get_class_name_by_class_index(super_class_index);
    INFO_PRINT("super class name: %s", super_class_name->as_C_string());

    // 实现的接口数量
    u2 interfaces_len = cfs->get_u2_fast();
    INFO_PRINT("interfaces length: %d", interfaces_len);

    // 实现的接口列表
    Array<u2>* local_interfaces = parse_interfaces(interfaces_len);

    // 成员字段数量
    u2 fields_len = cfs->get_u2_fast();
    INFO_PRINT("fields length: %d", fields_len);

    // 字段列表
    parse_fields(fields_len);

    return ik;
}



ConstantPoolHandle ClassFileParser::parse_constant_pool() {
    INFO_PRINT("parse constant pool");
    ClassFileStream* cfs = stream();

    u2 length = cfs->get_u2_fast();
    INFO_PRINT("constant pool size: %d", length);

    ConstantPool* constant_pool = new (length) ConstantPool(length);
    _cp = constant_pool;
    ConstantPoolHandle cp (constant_pool);

    parse_constant_pool_entries(length);

    return cp;
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

                    _cp->symbol_at_put(index, s);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Utf8, 值: %s", index, _cp->symbol_at(index)->as_C_string());
                }
                break;
            case JVM_CONSTANT_Integer:
                {
                    u4 bytes = cfs->get_u4_fast();
                    _cp->int_at_put(index, (jint) bytes);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Integer, 值: %d", index, _cp->int_at(index));
                }
                break;
            case JVM_CONSTANT_Long:
                {
                    u8 bytes = cfs->get_u8_fast();
                    _cp->long_at_put(index, (jlong) bytes);
                    // 因为long在常量池中占两个槽位，上面解析是一次解析完成的，所以之后需要将遍历索引自增1
                    index++;

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Long, 值: %ld", index, _cp->long_at(index));
                }
            case JVM_CONSTANT_Float:
                {
                    u4 bytes = cfs->get_u4_fast();
                    // c++ bytes(二进制bit流，表示一个float浮点数) --> float : *((float*) &bytes)，先将bit流的指针转成float*，然后再取值完成转换
                    _cp->float_at_put(index, *((jfloat*) &bytes));

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Float, 值: %f", index, _cp->float_at(index));
                }
                break;
            case JVM_CONSTANT_Double:
                {
                    u8 bytes = cfs->get_u8_fast();
                    _cp->double_at_put(index, *((jdouble*) &bytes));
                    // 因为double在常量池中占两个槽位，上面解析是一次解析完成的，所以之后需要将遍历索引自增1
                    index++;

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Double, 值: %lf", index, _cp->double_at(index));
                }
                break;
            case JVM_CONSTANT_Methodref:
                {
                    u2 class_index = cfs->get_u2_fast();
                    u2 name_and_type_index = cfs->get_u2_fast();
                    _cp->method_at_put(index, class_index, name_and_type_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Methodref, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_Fieldref:
                {
                    u2 class_index = cfs->get_u2_fast();
                    u2 name_and_type_index = cfs->get_u2_fast();
                    _cp->field_at_put(index, class_index, name_and_type_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Fieldref, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_String:
                {
                    u2 string_index = cfs->get_u2_fast();
                    _cp->string_index_at_put(index, string_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: String, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_Class:
                {
                    u2 name_index = cfs->get_u2_fast();
                    _cp->class_index_at_put(index, name_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Class, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_NameAndType:
                {
                    u2 name_index = cfs->get_u2_fast();
                    u2 signature_index = cfs->get_u2_fast();
                    _cp->name_and_type_at_put(index, name_index, signature_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: NameAndType, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_InterfaceMethodref:
                {
                    u2 class_index = cfs->get_u2_fast();
                    u2 name_and_type_index = cfs->get_u2_fast();
                    _cp->interface_method_at_put(index, class_index, name_and_type_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: InterfaceMethodref, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_InvokeDynamic:
                {
                    u2 bootstrap_method_attr_index = cfs->get_u2_fast();
                    u2 name_and_type_index = cfs->get_u2_fast();
                    _cp->invoke_dynamic_at_put(index, bootstrap_method_attr_index, name_and_type_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: InvokeDynamic, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_MethodType:
                {
                    u2 ref_index = cfs->get_u2_fast();
                    _cp->method_type_index_at_put(index, ref_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: MethodType, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            case JVM_CONSTANT_MethodHandle:
                {
                    u1 ref_kind = cfs->get_u1_fast();
                    u2 ref_index = cfs->get_u2_fast();
                    _cp->method_handle_index_at_put(index, ref_kind, ref_index);

                    INFO_PRINT("ConstantPool, 第%d项, 类型: MethodHandle, 值: %X", index, *_cp->int_at_adr(index));
                }
                break;
            default:
                ERROR_PRINT("Unknown constant tag %u in class file %s", tag, _stream->source());
                break;
        }
    }
}

ClassFileParser::~ClassFileParser() {

}

Array<u2>* ClassFileParser::parse_interfaces(int length) {
    INFO_PRINT("parse interfaces");
    if (length == 0)
        return new Array<u2>();
    ClassFileStream* cfs = stream();

    Array<u2>* interfaces_index = new Array<u2>(length);

    for (int index = 0; index < length; index++) {
        u2 interface_index = cfs->get_u2_fast();

        interfaces_index->add(interface_index);

        INFO_PRINT("Interface, 第%d项, 值: %X，名称: %s", index, interface_index, _cp->get_class_name_by_class_index(interface_index)->as_C_string());
    }

    return interfaces_index;
}

Array<FiledInfo*>* ClassFileParser::parse_fields(int length) {
    INFO_PRINT("parse fields");
    if (length == 0)
        return new Array<FiledInfo*>();

    ClassFileStream* cfs = stream();

    Array<FiledInfo*>* filed_infos = new Array<FiledInfo*>(length);

    for (int index = 0; index < length; index++) {
        u2 access_flag = cfs->get_u2_fast();
        u2 name_index = cfs->get_u2_fast();
        u2 signature_index = cfs->get_u2_fast();
        u2 attributes_count = cfs->get_u2_fast();

        FiledInfo* filed_info = new FiledInfo(access_flag, name_index, signature_index, attributes_count);

        if (attributes_count > 0) {
            parse_field_attributes(attributes_count, filed_info);
        }

        filed_infos->add(filed_info);
        INFO_PRINT("Filed, 第%d项, name_index: %X，signature_index: %X, attributes_count: %d", index, name_index,signature_index, attributes_count);

    }

    return filed_infos;
}

void ClassFileParser::parse_field_attributes(u2 attributes_count, FiledInfo* filed_info) {
    ClassFileStream* cfs = stream();

    u2 attribute_name_index = cfs->get_u2_fast();
    u4 attribute_length = cfs->get_u4_fast();

    Symbol* attribute_name = _cp->symbol_at(attribute_name_index);

    AttributeInfo* attribute;
    if (*attribute_name == JVM_ATTRIBUTE_ConstantValue) {
        u2 constant_value_index = cfs->get_u2_fast();

        attribute = new ConstantValueAttribute(attribute_name_index, attribute_length,constant_value_index);

        INFO_PRINT("Filed Attributes, type: ConstantValue, constant_value_index: %X，", constant_value_index);
    }

    filed_info->put_attribute(attribute_name, attribute);
}