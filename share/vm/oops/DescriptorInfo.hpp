//
// Created by xyzjiao on 10/22/21.
//

#ifndef JVMPLUS_DESCRIPTORINFO_HPP
#define JVMPLUS_DESCRIPTORINFO_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/metadata.hpp"
#include "../../../share/vm/oops/symbol.hpp"

class DescriptorInfo  : public Metadata {
private:
    //是否完成解析并赋值，默认false
    bool _is_resolved = false;

    // 本描述符在本JVM内部的类型
    int _type;

    // 数组维度，只有数组才有该属性
    int _array_dimension;

    // 引用类型的类型，只有引用类型才有该属性
    Symbol* _type_desc;

    // 数组元素的类型，只有数组类型才有该属性
    DescriptorInfo* _array_element_type;
public:
    void inc_array_dimension() {
        _array_dimension++;
    }

    void set_array_element_type(DescriptorInfo* d) { _array_element_type = d; }

    DescriptorInfo() {
    }

    DescriptorInfo(bool is_resolved, int type) {
        _is_resolved = is_resolved;
        _type = type;
    }

    DescriptorInfo(bool is_resolved, int type, Symbol* type_desc) {
        _is_resolved = is_resolved;
        _type = type;
        _type_desc = type_desc;
    }

    int type() { return _type; }
    void set_type(int type) { _type = type; }

    Symbol* type_desc() { return _type_desc; }
    void set_type_desc(Symbol* type_desc) { _type_desc = type_desc; }

    bool is_resolved() { return _is_resolved; }
    void set_is_resolved(bool is_resolved) { _is_resolved = is_resolved; }
};


#endif //JVMPLUS_DESCRIPTORINFO_HPP
