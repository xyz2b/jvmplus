//
// Created by ziya on 2021/10/14.
//

#ifndef JVMPLUS_INNERCLASSATTRIBUTE_HPP
#define JVMPLUS_INNERCLASSATTRIBUTE_HPP

#include "../../../share/vm/oops/attribute.hpp"
#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"
#include "../../../share/vm/utilities/accessFlags.hpp"

class InnerClass : public Metadata {
private:
    u2 _inner_class_info_index;
    u2 _outer_class_info_index;
    u2 _inner_class_name_index;
    AccessFlags _inner_class_access_flags;

public:
    InnerClass(u2 inner_class_info_index, u2 outer_class_info_index, u2 inner_class_name_index, AccessFlags inner_class_access_flags) : _inner_class_info_index(inner_class_info_index), _outer_class_info_index(outer_class_info_index), _inner_class_name_index(inner_class_name_index), _inner_class_access_flags(inner_class_access_flags) {}

    u2 get_inner_class_info_index() { return _inner_class_info_index; }
    u2 get_outer_class_info_index() { return _outer_class_info_index; }
    u2 get_inner_class_name_index() { return _inner_class_name_index; }
    AccessFlags get_inner_class_access_flags() { return _inner_class_access_flags; }
};

class InnerClassAttribute : public AttributeInfo {
private:
    u2 _number_of_classes;
    Array<InnerClass*>* _classes;
public:
    InnerClassAttribute(u2 attribute_name_index, u4 attribute_length, u2 number_of_classes) :AttributeInfo(attribute_name_index, attribute_length), _number_of_classes(number_of_classes) {}

    void set_classes(Array<InnerClass*>* classes) { _classes = classes; }
    InnerClass* get_bootstrap_method(int index) { return _classes->get(index); }
    void add_bootstrap_method(InnerClass* bootstrap_method) { _classes->add(bootstrap_method); }
};

#endif //JVMPLUS_INNERCLASSATTRIBUTE_HPP
