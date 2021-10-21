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

    int static_filed_count = 0;
    // 字段列表
    Array<FiledInfo*>* fields = parse_fields(fields_len, &static_filed_count);
    INFO_PRINT("static field count: %d", static_filed_count);

    // 成员方法数量
    u2 methods_len = cfs->get_u2_fast();
    INFO_PRINT("methods length: %d", methods_len);

    // 成员方法列表
    Array<Method*>* methods = parse_methods(methods_len);

    // 类属性数量
    u2 class_attribute_count = cfs->get_u2_fast();
    INFO_PRINT("class attribute count: %d", class_attribute_count);

    // 类属性列表
    Hashmap<Symbol*, AttributeInfo*, HashCode<const Symbol*>>* class_attributes = parse_class_attribute(class_attribute_count);

    InstanceKlass* instance_klass = InstanceKlass::allocate_instance_klass(magic, minion_version, major_version, cp(), access_flags, this_class_index, super_class_index,
                                                      interfaces_len, local_interfaces, fields_len, fields, methods_len, methods, class_attribute_count, class_attributes, static_filed_count);
    InstanceKlassHandle ik (instance_klass);

    for (int index = 0; index < ik->get_methods()->size(); index++) {
        ik->get_methods()->get_at(index)->set_belong_klass(instance_klass);
    }

    return ik;
}

ConstantPoolHandle ClassFileParser::parse_constant_pool() {
    INFO_PRINT("parse constant pool");
    ClassFileStream* cfs = stream();

    u2 length = cfs->get_u2_fast();
    INFO_PRINT("constant pool size: %d", length);

    ConstantPool* constant_pool = ConstantPool::allocate(length);
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

                    Symbol* s = new (utf8_length) Symbol((const char*)utf8_buffer, utf8_length);

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

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Long, 值: %ld", index-1, _cp->long_at(index-1));
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

                    INFO_PRINT("ConstantPool, 第%d项, 类型: Double, 值: %lf", index-1, _cp->double_at(index-1));
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
        return new (0) Array<u2>();
    ClassFileStream* cfs = stream();

    Array<u2>* interfaces_index = new (length) Array<u2>(length);

    for (int index = 0; index < length; index++) {
        u2 interface_index = cfs->get_u2_fast();

        interfaces_index->add(interface_index);

        INFO_PRINT("Interface, 第%d项, 值: %X，名称: %s", index, interface_index, _cp->get_class_name_by_class_index(interface_index)->as_C_string());
    }

    return interfaces_index;
}

Array<FiledInfo*>* ClassFileParser::parse_fields(int length, int* static_filed_count) {
    INFO_PRINT("parse fields");
    if (length == 0)
        return new (0) Array<FiledInfo*>();

    ClassFileStream* cfs = stream();

    Array<FiledInfo*>* filed_infos = new (length) Array<FiledInfo*>(length);

    for (int index = 0; index < length; index++) {
        u2 access_flag = cfs->get_u2_fast();
        u2 name_index = cfs->get_u2_fast();
        u2 signature_index = cfs->get_u2_fast();
        u2 field_attributes_count = cfs->get_u2_fast();

        AccessFlags ac = AccessFlags((jint)access_flag);
        if (ac.is_static()) {
            (*static_filed_count)++;
        }

        FiledInfo* filed_info = new FiledInfo(ac, name_index, signature_index, field_attributes_count);

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
        return new (0) Array<Method*>();

    ClassFileStream* cfs = stream();

    Array<Method*>* methods = new (length) Array<Method*>(length);

    for (int index = 0; index < length; index++) {
        u2 access_flag = cfs->get_u2_fast();
        u2 name_index = cfs->get_u2_fast();
        u2 signature_index = cfs->get_u2_fast();
        u2 method_attributes_count = cfs->get_u2_fast();

        INFO_PRINT("Method, 第%d项, name: %s, name_index: %X，signature_index: %X, attributes_count: %d", index, _cp->symbol_at(name_index)->as_C_string(), name_index, signature_index, method_attributes_count);

        Method* method = new Method(access_flag, name_index, signature_index, method_attributes_count);

        if (method_attributes_count > 0) {
            parse_method_attributes(method_attributes_count, method);
        }

        methods->add(method);
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
        INFO_PRINT("method attribute name: %s", method_attribute_name->as_C_string());
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
            CodeAttribute* code_attributes = new (code_length) CodeAttribute(method_attribute_name_index, method_attribute_length, code_attributes_count, max_stack, max_locals, code_length, exception_table_length);
            code_attributes->set_code(code_start);
            code_attributes->set_exception_tables(exception_table);

            // 解析code的属性
            for (int i = 0; i < code_attributes_count; i++) {
                u2 code_attribute_name_index = cfs->get_u2_fast();
                u4 code_attribute_length = cfs->get_u4_fast();

                Symbol* code_attribute_name = _cp->symbol_at(code_attribute_name_index);

                INFO_PRINT("start parse method attribute, type: %s", code_attribute_name->as_C_string());
                AttributeInfo *code_attribute = nullptr;
                if (*code_attribute_name == JVM_ATTRIBUTE_LocalVariableTable) {
                    u2 local_variable_table_length = cfs->get_u2_fast();

                    LocalVariableTableAttribute* local_variable_table_attribute = new LocalVariableTableAttribute(code_attribute_name_index, code_attribute_length, local_variable_table_length);

                    Array<LocalVariable*>* local_variable_table = nullptr;
                    if (local_variable_table_length > 0) {
                        local_variable_table = parse_local_variable_table(local_variable_table_length);
                    }
                    local_variable_table_attribute->set_local_variable_table(local_variable_table);

                    code_attribute = local_variable_table_attribute;
                    INFO_PRINT("Code Attribute, 第%d项, type: LocalVariableTable，local_variable_table_length: %X", i, local_variable_table_length);
                } else if (*code_attribute_name == JVM_ATTRIBUTE_LineNumberTable) {
                    u2 line_number_table_length = cfs->get_u2_fast();

                    LineNumberTableAttribute* line_number_table_attribute = new LineNumberTableAttribute(code_attribute_name_index, code_attribute_length, line_number_table_length);

                    Array<LineNumber*>* line_number_table = nullptr;
                    if (line_number_table_length > 0) {
                        line_number_table = parse_line_number_table(line_number_table_length);
                    }
                    line_number_table_attribute->set_line_number_table(line_number_table);

                    code_attribute = line_number_table_attribute;
                    INFO_PRINT("Code Attribute, 第%d项, type: LineNumberTable，local_variable_table_length: %X", i, line_number_table_length);
                } else if (*code_attribute_name == JVM_ATTRIBUTE_StackMapTable) {
                    // 该属性直接跳过，不做解析
                    cfs->skip_u1_fast(code_attribute_length);

                    StackMapTableAttribute* stack_map_table_attribute = new StackMapTableAttribute(code_attribute_name_index, code_attribute_length);

                    code_attribute = stack_map_table_attribute;

                    INFO_PRINT("Code Attribute, 第%d项, type: StackMapTable", i);
                }
                code_attributes->put_attribute(code_attribute_name, code_attribute);
            }
            method_attribute = code_attributes;
            INFO_PRINT("Method Attribute, 第%d项, type: Code，max_stack: %X, max_locals: %X, code_length: %X, exception_table_length: %X, attributes_count: %X", index, max_stack, max_locals, code_length, exception_table_length, code_attributes_count);
        } else if (*method_attribute_name == JVM_ATTRIBUTE_Exceptions) {
            u2 number_of_exceptions = cfs->get_u2_fast();

            ExceptionAttribute* exception_attribute = new ExceptionAttribute(method_attribute_name_index, method_attribute_length, number_of_exceptions);

            Array<u2>* exception_index_table = new (number_of_exceptions) Array<u2>(number_of_exceptions);
            for (int i = 0; i < number_of_exceptions; i++) {
                u2 exception_index = cfs->get_u2_fast();
                exception_index_table->add(exception_index);
            }
            exception_attribute->set_exception_index_table(exception_index_table);

            method_attribute = exception_attribute;
            INFO_PRINT("Method Attribute, 第%d项, type: Exception, number_of_exceptions: %X", index, number_of_exceptions);
        }
        method->put_attribute(method_attribute_name, method_attribute);
    }
}

Array<ExceptionHandler*>* ClassFileParser::parse_exception_table(u2 exception_table_length) {
    ClassFileStream *cfs = stream();

    Array<ExceptionHandler*>* exception_table = new (exception_table_length) Array<ExceptionHandler*>(exception_table_length);

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

Array<LocalVariable*>* ClassFileParser::parse_local_variable_table(u2 local_variable_table_length) {
    ClassFileStream *cfs = stream();

    Array<LocalVariable*>* local_variable_table = new (local_variable_table_length) Array<LocalVariable*>(local_variable_table_length);

    for (int index = 0; index < local_variable_table_length; index++) {
        u2 start_pc = cfs->get_u2_fast();
        u2 length = cfs->get_u2_fast();
        u2 name_index = cfs->get_u2_fast();
        u2 descriptor_index = cfs->get_u2_fast();
        u2 local_variable_index = cfs->get_u2_fast();

        LocalVariable* local_variable = new LocalVariable(start_pc, length, name_index, descriptor_index, local_variable_index);

        local_variable_table->add(local_variable);
        INFO_PRINT("LocalVariable, 第%d项, start_pc: %X，length: %X, name_index: %X, descriptor_index: %X, local_variable_index: %X", index, start_pc, length, name_index, descriptor_index, local_variable_index);
    }

    return local_variable_table;
}

Array<LineNumber *> *ClassFileParser::parse_line_number_table(u2 line_number_table_length) {
    ClassFileStream *cfs = stream();

    Array<LineNumber*>* line_number_table = new (line_number_table_length) Array<LineNumber*>(line_number_table_length);

    for (int index = 0; index < line_number_table_length; index++) {
        u2 start_pc = cfs->get_u2_fast();
        u2 line_number = cfs->get_u2_fast();

        LineNumber* line_number_entry = new LineNumber(start_pc, line_number);

        line_number_table->add(line_number_entry);
        INFO_PRINT("LineNumber, 第%d项, start_pc: %X，line_number: %X", index, start_pc, line_number);
    }

    return line_number_table;
}

Hashmap<Symbol *, AttributeInfo *, HashCode<const Symbol *>>*
ClassFileParser::parse_class_attribute(u2 class_attribute_count) {
    ClassFileStream *cfs = stream();

    Hashmap<Symbol *, AttributeInfo *, HashCode<const Symbol *>>* class_attributes = new Hashmap<Symbol *, AttributeInfo *, HashCode<const Symbol *>>(class_attribute_count);

    for (int index = 0; index < class_attribute_count; index++) {
        u2 class_attribute_name_index = cfs->get_u2_fast();
        u4 class_attribute_length = cfs->get_u4_fast();

        Symbol* class_attribute_name = _cp->symbol_at(class_attribute_name_index);

        AttributeInfo *class_attribute = nullptr;
        INFO_PRINT("class attribute name: %s", class_attribute_name->as_C_string());
        if (*class_attribute_name == JVM_ATTRIBUTE_SourceFile) {
            u2 source_file_index = cfs->get_u2_fast();

            SourceFileAttribute* source_file_attribute = new SourceFileAttribute(class_attribute_name_index, class_attribute_length, source_file_index);

            class_attribute = source_file_attribute;
            INFO_PRINT("Class Attribute, 第%d项, type: SourceFile，source_file_index: %X", index, source_file_index);
        } else if (*class_attribute_name == JVM_ATTRIBUTE_BootstrapMethods) {
            u2 number_of_bootstrap_methods = cfs->get_u2_fast();

            Array<BootstrapMethod*>* bootstrap_methods_entry = nullptr;
            if (number_of_bootstrap_methods > 0) {
                bootstrap_methods_entry = parse_bootstrap_method(number_of_bootstrap_methods);
            }

            BootstrapMethods* bootstrap_methods = new BootstrapMethods(class_attribute_name_index, class_attribute_length, number_of_bootstrap_methods);
            bootstrap_methods->set_bootstrap_methods(bootstrap_methods_entry);
            class_attribute = bootstrap_methods;
            INFO_PRINT("Class Attribute, 第%d项, type: BootstrapMethods，number_of_bootstrap_methods: %X", index, number_of_bootstrap_methods);
        } else if (*class_attribute_name == JVM_ATTRIBUTE_InnerClasses) {
            u2 number_of_classes = cfs->get_u2_fast();

            Array<InnerClass*>* inner_class_entry = nullptr;
            if (number_of_classes > 0) {
                inner_class_entry = parse_inner_classes(number_of_classes);
            }

            InnerClassAttribute* inner_class = new InnerClassAttribute(class_attribute_name_index, class_attribute_length, number_of_classes);
            inner_class->set_classes(inner_class_entry);
            class_attribute = inner_class;
            INFO_PRINT("Class Attribute, 第%d项, type: InnerClasses，number_of_classes: %X", index, number_of_classes);
        }
        class_attributes->put(class_attribute_name, class_attribute);
    }

    return class_attributes;
}

Array<BootstrapMethod *> *ClassFileParser::parse_bootstrap_method(u2 number_of_bootstrap_methods) {
    ClassFileStream *cfs = stream();

    Array<BootstrapMethod*>* bootstrap_methods_entry = new (number_of_bootstrap_methods) Array<BootstrapMethod*>(number_of_bootstrap_methods);

    for (int index = 0; index < number_of_bootstrap_methods; index++) {
        u2 bootstrap_method_ref = cfs->get_u2_fast();
        u2 num_bootstrap_arguments = cfs->get_u2_fast();

        Array<u2>* bootstrap_arguments = nullptr;
        if (num_bootstrap_arguments > 0) {
            bootstrap_arguments = new (num_bootstrap_arguments) Array<u2>(num_bootstrap_arguments);

            for (int i = 0; i < num_bootstrap_arguments; i++) {
                u2 bootstrap_argument = cfs->get_u2_fast();
                bootstrap_arguments->add(bootstrap_argument);

                INFO_PRINT("Bootstrap Argument, 第%d项，bootstrap_argument: %X", index, bootstrap_argument);
            }
        }

        BootstrapMethod* bootstrap_method = new BootstrapMethod(bootstrap_method_ref, num_bootstrap_arguments);
        bootstrap_method->set_bootstrap_arguments(bootstrap_arguments);

        bootstrap_methods_entry->add(bootstrap_method);
        INFO_PRINT("Bootstrap Method, 第%d项，bootstrap_method_ref: %X,num_bootstrap_arguments: %X", index, bootstrap_method_ref, num_bootstrap_arguments);
    }

    return bootstrap_methods_entry;
}

Array<InnerClass *> *ClassFileParser::parse_inner_classes(u2 number_of_classes) {
    ClassFileStream *cfs = stream();

    Array<InnerClass*>* classes = new (number_of_classes) Array<InnerClass*>(number_of_classes);

    for (int index = 0; index < number_of_classes; index++) {
        u2 inner_class_info_index = cfs->get_u2_fast();
        u2 outer_class_info_index = cfs->get_u2_fast();
        u2 inner_name_index = cfs->get_u2_fast();
        u2 inner_class_access_flags = cfs->get_u2_fast();

        InnerClass* inner_class = new InnerClass(inner_class_info_index, outer_class_info_index, inner_name_index, inner_class_access_flags);
        classes->add(inner_class);
        INFO_PRINT("Inner Class, 第%d项，inner_class_info_index: %X, outer_class_info_index: %X, inner_name_index: %X, inner_class_access_flags: %X", index, inner_class_info_index, outer_class_info_index, inner_name_index, inner_class_access_flags);
    }

    return classes;
}
