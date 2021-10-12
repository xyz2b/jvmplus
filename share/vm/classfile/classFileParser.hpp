//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSFILEPARSER_HPP
#define JVMPLUS_CLASSFILEPARSER_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/runtime/handles.hpp"
#include "../../../share/vm/classfile/classFileStream.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/oops/constantPool.hpp"
#include "../../../share/vm/oops/symbol.hpp"

// 解析器
class ClassFileParser {
private:
    ConstantPool* _cp;

    ClassFileStream* _stream;
    // Accessors
    ClassFileStream* stream()       { return _stream; }
    void set_stream(ClassFileStream* st)    { _stream = st; }

public:

    InstanceKlassHandle parseClassFile(Symbol* name);

    ConstantPoolHandle parse_constant_pool();

    void parse_constant_pool_entries(int length);

public:
    ClassFileParser(ClassFileStream* st) { set_stream(st); };
    ~ClassFileParser();
};


#endif //JVMPLUS_CLASSFILEPARSER_HPP
