//
// Created by xyzjiao on 10/20/21.
//

#include "JavaNativeInterface.hpp"

Method* JavaNativeInterface::get_method(InstanceKlass* klass, Symbol* method_name, Symbol* descriptor_name) {
    Array<Method*>* methods = klass->get_methods();

    for (int index = 0; index < klass->get_methods_count(); index++) {
        Method* method = methods->get_at(index);
        Symbol* tmp_method_name = klass->get_constant_pool()->symbol_at(method->name_index());
        Symbol* tmp_descriptor_name = klass->get_constant_pool()->symbol_at(method->signature_index());

        if (*tmp_method_name == method_name && *tmp_descriptor_name == descriptor_name) {
            INFO_PRINT("找到了方法: %s#%s", tmp_method_name->as_C_string(), tmp_descriptor_name->as_C_string());
            return method;
        }
    }

    ERROR_PRINT("没有找到方法: %s#%s", method_name->as_C_string(), descriptor_name->as_C_string());
    return nullptr;
}