//
// Created by xyzjiao on 10/18/21.
//

#include "systemDictionary.h"
#include "../../../share/vm/classfile/classLoader.hpp"

Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* SystemDictionary::_dictionary = nullptr;

Klass *SystemDictionary::resolve_or_null(Symbol *class_name) {
    return SystemDictionary::resolve_instance_class_or_null(class_name);
}


Klass* SystemDictionary::resolve_instance_class_or_null(Symbol *class_name) {
    Klass* klass = nullptr;

    klass = dictionary()->get(class_name);
    if (klass != nullptr) {
        return klass;
    }

    klass = ClassLoader::load_classfile(class_name)();
    if (nullptr == klass) {
        ERROR_PRINT("不存在文件： %s", class_name->as_C_string());

        exit(-1);
    }

    dictionary()->put(class_name, klass);

    return klass;
}