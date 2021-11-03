//
// Created by xyzjiao on 10/18/21.
//

#include "systemDictionary.h"
#include "../../../share/vm/classfile/classLoader.hpp"

#define SEARCH_PATH "/home/xyzjiao/Desktop/project/jvm/target/classes/"

Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* SystemDictionary::_dictionary = nullptr;

Klass *SystemDictionary::resolve_or_null(Symbol *class_name) {
    Symbol* search_path = new (strlen(SEARCH_PATH)) Symbol(SEARCH_PATH, strlen(SEARCH_PATH));
    INFO_PRINT("resolve class %s", class_name->as_C_string());
    Symbol* full_class_name = *search_path + class_name;
    return SystemDictionary::resolve_instance_class_or_null(full_class_name);
}


Klass* SystemDictionary::resolve_instance_class_or_null(Symbol *class_name) {
    Klass* klass = nullptr;

    INFO_PRINT("resolve instance class %s", class_name->as_C_string());

    klass = dictionary()->get(class_name);
    if (klass != nullptr) {
        return klass;
    }

    INFO_PRINT("load class: %s", class_name->as_C_string());
    klass = ClassLoader::load_classfile(class_name)();
    if (nullptr == klass) {
        ERROR_PRINT("不存在文件： %s", class_name->as_C_string());

        exit(-1);
    }

    ((InstanceKlass*)klass)->set_init_state(InstanceKlass::ClassState::loaded);

//    load_super_class(klass);

    dictionary()->put(class_name, klass);

    return klass;
}

void SystemDictionary::load_super_class(Klass* klass) {
    InstanceKlass* instance = (InstanceKlass*)klass;

    int index = instance->get_super_class();
    Symbol* super_class_name = instance->get_constant_pool()->get_class_name_by_class_ref(index);
    Symbol* class_name = instance->get_constant_pool()->get_class_name_by_class_ref(instance->get_this_class());
    INFO_PRINT("%s super_class_name: %s", class_name->as_C_string(), super_class_name->as_C_string());

    // TODO: 没有处理继承java包下的类的情况，如果需要加载java包下的类需要用JNI获取klass，FindClass函数
    if (super_class_name->start_with("java")) {
        INFO_PRINT("不加载java包的类");
        return;
    }

    Klass* super_klass = SystemDictionary::resolve_or_null(super_class_name);
    instance->set_super_klass(super_klass);
}
