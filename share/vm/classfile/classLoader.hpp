//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSLOADER_HPP
#define JVMPLUS_CLASSLOADER_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/runtime/handles.hpp"
#include "../../../share/vm/utilities/ostream.hpp"
#include "../../../share/vm/runtime/os.hpp"
#include "../../../share/vm/classfile/classFileParser.hpp"
#include "../../../share/vm/oops/symbol.hpp"
#include "../../../share/vm/classfile/classFileStream.hpp"

// 类加载器
class ClassLoader {
public:
    static InstanceKlassHandle load_classfile(Symbol* h_name);
    static ClassFileStream* open_stream(const char* name);
    static bool is_load_class(Symbol* class_name);
};


#endif //JVMPLUS_CLASSLOADER_HPP
