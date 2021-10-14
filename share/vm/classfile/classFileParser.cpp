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

    // 成员方法数量
    u2 methods_len = cfs->get_u2_fast();
    INFO_PRINT("methods length: %d", methods_len);

    // 成员方法列表
    Array<Method*>* methods = parse_methods(methods_len);

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
        u2 field_attributes_count = cfs->get_u2_fast();

        FiledInfo* filed_info = new FiledInfo(access_flag, name_index, signature_index, field_attributes_count);

        if (field_attributes_count > 0) {
            parse_field_attributes(field_attributes_count, filed_info);
        }

        filed_infos->add(filed_info);
        INFO_PRINT("Filed, 第%d项, name: %s， name_index: %X，signature_index: %X, attributes_count: %d", index, _cp->symbol_at(name_index)->as_C_string(), name_index,signature_index, field_attributes_count);

    }

    return filed_infos;
}

void ClassFileParser::parse_field_attributes(u2 attributes_count, FiledInfo* filed_info) {
    ClassFileStream* cfs = stream();

    for (int index = 0; index < attributes_count; index++) {
        u2 field_attribute_name_index = cfs->get_u2_fast();
        u4 field_attribute_length = cfs->get_u4_fast();

        Symbol* field_attribute_name = _cp->symbol_at(field_attribute_name_index);

        AttributeInfo *field_attribute = nullptr;
        if (*field_attribute_name == JVM_ATTRIBUTE_ConstantValue) {
            u2 constant_value_index = cfs->get_u2_fast();

            ConstantValueAttribute* constant_value_attribute = new ConstantValueAttribute(field_attribute_name_index, field_attribute_length,constant_value_index);

            field_attribute = constant_value_attribute;
            INFO_PRINT("Filed Attribute, 第%d项，type: ConstantValue, constant_value_index: %X，", index, constant_value_index);
        }

        filed_info->put_attribute(field_attribute_name, field_attribute);
    }
}

Array<Method*>* ClassFileParser::parse_methods(int length) {
    INFO_PRINT("parse methods");
    if (length == 0)
        return new Array<Method*>();

    ClassFileStream* cfs = stream();

    Array<Method*>* methods = new Array<Method*>(length);

    for (int index = 0; index < length; index++) {
        u2 access_flag = cfs->get_u2_fast();
        u2 name_index = cfs->get_u2_fast();
        u2 signature_index = cfs->get_u2_fast();
        u2 method_attributes_count = cfs->get_u2_fast();

        Method* method = new Method(access_flag, name_index, signature_index, method_attributes_count);

        if (method_attributes_count > 0) {
            parse_method_attributes(method_attributes_count, method);
        }

        methods->add(method);
        INFO_PRINT("Method, 第%d项, name_index: %X，signature_index: %X, attributes_count: %d", index, name_index, signature_index, method_attributes_count);
    }
    return methods;
}

void ClassFileParser::parse_method_attributes(u2 method_attributes_count, Method* method) {
    ClassFileStream *cfs = stream();

    for (int index = 0; index < method_attributes_count; index++) {
        u2 method_attribute_name_index = cfs->get_u2_fast();
        u4 method_attribute_length = cfs->get_u4_fast();

        Symbol* method_attribute_name = _cp->symbol_at(method_attribute_name_index);

        AttributeInfo *method_attribute = nullptr;
        if (*method_attribute_name == JVM_ATTRIBUTE_Code) {
            u2 max_stack = cfs->get_u2_fast();
            u2 max_locals = cfs->get_u2_fast();

            u4 code_length = cfs->get_u4_fast();
            u1* code_start = cfs->get_u1_buffer();
            cfs->skip_u1_fast(code_length);

            u2 exception_table_length = cfs->get_u2_fast();
            Array<ExceptionHandler*>* exception_table = nullptr;
            if (exception_table_length > 0) {
                exception_table = parse_exception_table(exception_table_length);
            }

            u2 code_attributes_count = cfs->get_u2_fast();
            CodeAttribute* code_attributes = new (code_length) CodeAttribute(method_attribute_name_index, method_attribute_length, exception_table_length, code_attributes_count, max_stack, max_locals, code_length);
            code_attributes->set_code(code_start);

            // 解析code的属性
            for (int index = 0; index < code_attributes_count; index++) {
                u2 code_attribute_name_index = cfs->get_u2_fast();
                u4 code_attribute_length = cfs->get_u4_fast();

                Symbol* code_attribute_name = _cp->symbol_at(code_attribute_name_index);

                AttributeInfo *code_attribute = nullptr;
                if (*code_attribute_name == JVM_ATTRIBUTE_LocalVariableTable) {



//                    code_attribute = ;
                    INFO_PRINT("Code Attribute, 第%d项, type: LocalVariableTable，", index);
                }
                code_attributes->put_attribute(code_attribute_name, code_attribute);
            }
            method_attribute = code_attributes;
            INFO_PRINT("Method Attribute, 第%d项, type: Code，max_stack: %X, max_locals: %X, code_length: %X, exception_table_length: %X, attributes_count: %X", index, max_stack, max_locals, code_length, exception_table_length, code_attributes_count);
        }
        method->put_attribute(method_attribute_name, method_attribute);
    }
}

Array<ExceptionHandler*>* ClassFileParser::parse_exception_table(u2 exception_table_length) {
    ClassFileStream *cfs = stream();

    Array<ExceptionHandler*>* exception_table = new Array<ExceptionHandler*>(exception_table_length);

    for (int index = 0; index < exception_table_length; index++) {
        u2 start_pc = cfs->get_u2_fast();
        u2 end_pc = cfs->get_u2_fast();
        u2 handler_pc = cfs->get_u2_fast();
        u2 catch_type = cfs->get_u2_fast();

        ExceptionHandler* exception_handler = new ExceptionHandler(start_pc, end_pc, handler_pc, catch_type);
        exception_table->add(exception_handler);

        INFO_PRINT("ExceptionHandler, 第%d项, start_pc: %X，end_pc: %X, handler_pc: %X, catch_type: %X", index, start_pc, end_pc, handler_pc, catch_type);

    }

    return exception_table;
}