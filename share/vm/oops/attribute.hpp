//
// Created by ziya on 2021/10/13.
//

#ifndef JVMPLUS_ATTRIBUTE_HPP
#define JVMPLUS_ATTRIBUTE_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/metadata.hpp"

#define JVM_ATTRIBUTE_ConstantValue "ConstantValue"
#define JVM_ATTRIBUTE_Code "Code"
#define JVM_ATTRIBUTE_Exceptions "Exceptions"
#define JVM_ATTRIBUTE_LineNumberTable "LineNumberTable"
#define JVM_ATTRIBUTE_LocalVariableTable "LocalVariableTable"
#define JVM_ATTRIBUTE_SourceFile "SourceFile"
#define JVM_ATTRIBUTE_StackMapTable "StackMapTable"
#define JVM_ATTRIBUTE_BootstrapMethods "BootstrapMethods"
#define JVM_ATTRIBUTE_InnerClasses "InnerClasses"

class AttributeInfo : public Metadata {
private:
    u2 _attribute_name_index;
    u4 _attribute_length;

public:
    AttributeInfo(u2 attribute_name_index, u4 attribute_length) : _attribute_name_index(attribute_name_index), _attribute_length(attribute_length) {}

    void set_attribute_length(u4 attribute_length) { _attribute_length = attribute_length; }
    void set_attribute_name_index(u2 attribute_name_index) { _attribute_name_index = attribute_name_index; }

    u2 get_attribute_name_index() { return _attribute_name_index; }
    u4 get_attribute_length() { return _attribute_length; }
};

#endif //JVMPLUS_ATTRIBUTE_HPP
