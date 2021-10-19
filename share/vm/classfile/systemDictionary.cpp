//
// Created by xyzjiao on 10/18/21.
//

#include "systemDictionary.h"
#include "../../../share/vm/classfile/classLoader.hpp"

Klass *SystemDictionary::resolve_or_null(Symbol *class_name) {
    return ClassLoader::load_classfile(class_name)();
}
