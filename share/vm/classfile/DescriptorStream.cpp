//
// Created by xyzjiao on 10/22/21.
//

#include "DescriptorStream.hpp"

DescriptorInfo* DescriptorStream::parse_short_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_SHORT);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_SHORT);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_boolean_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_BOOLEAN);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_BOOLEAN);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_byte_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_BYTE);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_BYTE);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_char_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_CHAR);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_CHAR);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_int_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_INT);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_INT);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_float_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_FLOAT);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_FLOAT);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_long_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_LONG);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_LONG);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_double_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_DOUBLE);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_DOUBLE);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

DescriptorInfo* DescriptorStream::parse_void_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_VOID);
    Symbol* d = new (1) Symbol(JVM_SIGNATURE_VOID);
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}

/**
 * 解析引用类型
 * @return 解析出来的引用类型的信息
 * */
DescriptorInfo* DescriptorStream::parse_reference_type() {
    DescriptorInfo* descriptorInfo = new DescriptorInfo();
    descriptorInfo->set_type(T_OBJECT);
    // 前面已经解析过了'L'，所以继续往后解析，需要将index+1，后面才是真正的引用类型信息
    index++;
    char* reference = (char*) Metaspace::allocate((strlen(_descriptor)) * sizeof(char))->value();
    // 往后循环遍历，直到遇到';'，标识引用类型的结尾
    int reference_index = 0;
    for (; index < strlen(_descriptor); index++) {
        if (*(_descriptor + index) != JVM_SIGNATURE_END_CLASS) {
            *(reference + reference_index) = *(_descriptor + index);
            reference_index++;
        } else {
            break;
        }
    }
    Symbol* d = new (strlen(reference)) Symbol(reference, strlen(reference));
    descriptorInfo->set_type_desc(d);
    descriptorInfo->set_is_resolved(true);
    return descriptorInfo;
}


/**
 * 将不同类型的字段值压入操作数栈中
 * */
void DescriptorStream::push_field(jobject o, JavaVFrame* frame) {
    switch (_field->type()) {
        case T_BOOLEAN:
        case T_SHORT:
        case T_CHAR:
        case T_BYTE:
        case T_INT:
            frame->push_operand_stack(new StackValue(T_INT, *((int*)o)));
            break;
        case T_LONG:
            frame->push_operand_stack(new StackValue(T_LONG, *((long*)o)));
            break;
        case T_DOUBLE:
            frame->push_operand_stack(new StackValue(T_DOUBLE, *((double*)o)));
            break;
        case T_OBJECT:
            frame->push_operand_stack(new StackValue(T_OBJECT, o));
            break;
        default:
            ERROR_PRINT("无法识别的参数类型%d", _field->type());
            exit(-1);
    }
}

/**
  * 将不同类型的返回值压入操作数栈中
  * */
void DescriptorStream::push_return_element(jobject o, JavaVFrame* frame) {
    switch (_return_element->type()) {
        case T_BOOLEAN:
        case T_SHORT:
        case T_CHAR:
        case T_BYTE:
        case T_INT:
            frame->push_operand_stack(new StackValue(T_INT, *((int*)o)));
            break;
        case T_LONG:
            frame->push_operand_stack(new StackValue(T_LONG, *((long*)o)));
            break;
        case T_DOUBLE:
            frame->push_operand_stack(new StackValue(T_DOUBLE, *((double *)o)));
            break;
        case T_OBJECT:
            frame->push_operand_stack(new StackValue(T_OBJECT, o));
            break;
        default:
            ERROR_PRINT("无法识别的参数类型%d", _field->type());
            exit(-1);
    }
}

/**
 * 根据形参列表的类型，从操作数栈中弹出实参值
 * @return 实参值列表
 * */
Array<jobject>* DescriptorStream::get_params_val(JavaVFrame* frame) {
    Array<jobject>* values = new Array<jobject>[method_params_size()];

    for (int i = 0; i < method_params_size(); i++) {
        DescriptorInfo* info = parameters()->at(i);
        values->set(i, (jobject) (frame->pop_operand_stack()->data()));
    }

    return values;
}

/**
  * 根据字段的类型，从操作数栈中弹出实参值
  * @return 字段值列表
  * */
jobject DescriptorStream::get_field_val(JavaVFrame* frame) {
    jobject ret = (jobject) (frame->pop_operand_stack()->data());

    return ret;
}

/**
  * 解析方法的描述符
  * */
void DescriptorStream::parse_method() {
    parse_method_params();
    parse_return();
}

/**
 * 解析方法返回值类型
 * */
void DescriptorStream::parse_return() {
    // 取返回值
    int param_end_index = _descriptor_info->find_char_index(')');
    Symbol* return_str = _descriptor_info->sub_symbol(param_end_index + 1, _descriptor_info->size());

    // 调用解析方法
    _return_element = (new DescriptorStream(return_str))->do_parse()->at(0);
    INFO_PRINT("该方法的返回值: %s" , return_str->as_C_string());
}

/**
 * 解析字段类型
 * */
void DescriptorStream::parse_filed() {
    // 调用解析方法
    _field = (new DescriptorStream(_descriptor_info))->do_parse()->at(0);
    INFO_PRINT("字段: %s", _descriptor_info->as_C_string());
}

/**
 * 解析方法参数类型
 * */
void DescriptorStream::parse_method_params() {
    INFO_PRINT("解析方法描述符");
    // 找到形参列表括号的位置，左括号和右括号中间就是形参列表
    int param_start_index = _descriptor_info->find_char_index(JVM_SIGNATURE_START_FUNC);
    int param_end_index = _descriptor_info->find_char_index(JVM_SIGNATURE_END_FUNC);
    // 取形参列表
    Symbol* param_str = _descriptor_info->sub_symbol(param_start_index + 1, param_end_index);

    // 调用解析方法
    _parameters = (new DescriptorStream(param_str))->do_parse();
    _method_params_size = _parameters->size();

    INFO_PRINT("该方法描述符形参数量: %d", _method_params_size);
}

/**
* 真正解析描述符方法
* @return 解析完之后的列表
* */
vector<DescriptorInfo*>* DescriptorStream::do_parse() {
    _descriptor = _descriptor_info->as_C_string();

    vector<DescriptorInfo*>* parse_result = new vector<DescriptorInfo*>();

    for (; index < strlen(_descriptor); index++) {
        char b = *(_descriptor + index);
        switch (b) {
            case JVM_SIGNATURE_CLASS: {
                INFO_PRINT("解析reference类型");

                DescriptorInfo* ref = parse_reference_type();

                parse_result->push_back(ref);
                break;
            }
            case JVM_SIGNATURE_BOOLEAN: {
                INFO_PRINT("解析boolean类型");

                DescriptorInfo* ref = parse_boolean_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_BYTE: {
                INFO_PRINT("解析byte类型");

                DescriptorInfo* ref = parse_byte_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_CHAR: {
                INFO_PRINT("解析char类型");

                DescriptorInfo* ref = parse_char_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_SHORT: {
                INFO_PRINT("解析short类型");

                DescriptorInfo* ref = parse_short_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_INT: {
                INFO_PRINT("解析int类型");

                DescriptorInfo* ref = parse_int_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_FLOAT: {
                INFO_PRINT("解析float类型");

                DescriptorInfo* ref = parse_float_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_LONG: {
                INFO_PRINT("解析long类型");

                DescriptorInfo* ref = parse_long_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_DOUBLE: {
                INFO_PRINT("解析double类型");

                DescriptorInfo* ref = parse_double_type();

                parse_result->push_back(ref);

                break;
            }
            case JVM_SIGNATURE_VOID: {
                INFO_PRINT("解析void类型");

                DescriptorInfo* ref = parse_void_type();

                parse_result->push_back(ref);

                break;
            }
            default: {
                ERROR_PRINT("无法识别的元素类型");
                exit(-1);

            }
        }
    }
    return parse_result;
}
