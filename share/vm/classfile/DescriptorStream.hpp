//
// Created by xyzjiao on 10/22/21.
//

#ifndef JVMPLUS_DESCRIPTORSTREAM_HPP
#define JVMPLUS_DESCRIPTORSTREAM_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/symbol.hpp"
#include "../../../share/vm/oops/DescriptorInfo.hpp"
#include "../../../share/vm/runtime/javaVFrame.hpp"
#include <vector>

class DescriptorStream {
private:
    // 描述符
    Symbol* _descriptor_info;
    // 描述符字节流
    char* _descriptor;
    // 字节流的读取索引
    int index = 0;
    // 方法参数的个数
    int _method_params_size;

    // 按顺序存储解析完的方法参数
    Array<DescriptorInfo*>* _parameters = new Array<DescriptorInfo*>();

    // 解析完的返回参数类型
    DescriptorInfo* _return_element;

    // 解析完的字段类型
    DescriptorInfo* _field;

    // method name
    Symbol* _method_name;

private:
    DescriptorInfo* parse_short_type();

    DescriptorInfo* parse_boolean_type();

    DescriptorInfo* parse_byte_type();

    DescriptorInfo* parse_char_type();

    DescriptorInfo* parse_int_type();

    DescriptorInfo* parse_float_type();

    DescriptorInfo* parse_long_type();

    DescriptorInfo* parse_double_type();

    DescriptorInfo* parse_void_type();

    /**
     * 解析引用类型
     * @return 解析出来的引用类型的信息
     * */
    DescriptorInfo* parse_reference_type();

    DescriptorInfo* parse_array_type();

public:
    int get_field_type() {
        return _field->type();
    }

    int method_params_size() { return _method_params_size; }
    Array<DescriptorInfo*>* parameters() { return _parameters; }

    int return_element_type() { _return_element->type(); }

    DescriptorStream(Symbol* descriptor_info, Symbol* method_name) : _descriptor_info(descriptor_info), _method_name(method_name) {}

    DescriptorStream(Symbol* descriptor_info) : _descriptor_info(descriptor_info) {}
    /**
     * 将不同类型的字段值压入操作数栈中
     * */
    void push_field(jobject o, JavaVFrame* frame);

    /**
      * 将不同类型的返回值压入操作数栈中
      * */
    void push_return_element(jobject o, JavaVFrame* frame);

    /**
     * 根据形参列表的类型，从操作数栈中弹出实参值
     * @return 实参值列表
     * */
    jvalue* get_params_val(JavaVFrame* frame);

    /**
      * 根据字段的类型，从操作数栈中弹出实参值
      * @return 字段值列表
      * */
    jobject get_field_val(JavaVFrame* frame);

    /**
      * 解析方法的描述符
      * */
     void parse_method();

    /**
     * 解析方法返回值类型
     * */
    void parse_return();

    /**
     * 解析字段类型
     * */
    void parse_filed();

    /**
     * 解析方法参数类型
     * */
    void parse_method_params();

    /**
    * 真正解析描述符方法
    * @return 解析完之后的列表
    * */
    Array<DescriptorInfo*>* do_parse();

};


#endif //JVMPLUS_DESCRIPTORSTREAM_HPP
