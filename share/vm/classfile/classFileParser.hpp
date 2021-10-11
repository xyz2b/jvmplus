//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_CLASSFILEPARSER_HPP
#define JVMPLUS_CLASSFILEPARSER_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"

class ClassFileParser {
private:
    u2   _major_version;
    u2   _minor_version;

    Array<u2>*          _fields;
    Array<Method*>*     _methods;
    InstanceKlassHandle* _super_klass;
    ConstantPool*       _cp;
    InstanceKlass*      _klass;


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
