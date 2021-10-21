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
#include "../../../share/vm/utilities/accessFlags.hpp"
#include "../../../share/vm/oops/constantValueAttribute.hpp"
#include "../../../share/vm/oops/method.hpp"
#include "../../../share/vm/oops/codeAttribute.hpp"
#include "../../../share/vm/oops/localVariableTableAttribute.hpp"
#include "../../../share/vm/oops/lineNumberTableAttribute.hpp"
#include "../../../share/vm/oops/stackMapTableAttribute.hpp"
#include "../../../share/vm/oops/exceptionAttribute.h"
#include "../../../share/vm/oops/sourceFileAttribute.hpp"
#include "../../../share/vm/oops/bootstrapMethods.hpp"
#include "../../../share/vm/oops/innerClassAttribute.hpp"

// 解析器
class ClassFileParser {
private:
    ConstantPool* _cp;

    ClassFileStream* _stream;
    // Accessors
    ClassFileStream* stream()       { return _stream; }
    void set_stream(ClassFileStream* st)    { _stream = st; }

public:

    InstanceKlassHandle parse_class_file(Symbol* name);

    ConstantPoolHandle parse_constant_pool();

    void parse_constant_pool_entries(int length);

    Array<u2>* parse_interfaces(int length);

    Array<FiledInfo*>* parse_fields(int length, int* static_filed_count, int* non_static_field_count);

    void parse_field_attributes(u2 attributes_count, FiledInfo* filedInfo);

    Array<Method*>* parse_methods(int length);

    void parse_method_attributes(u2 attributes_count, Method* method);

    Array<ExceptionHandler*>* parse_exception_table(u2 exception_table_length);

    Array<LocalVariable*>* parse_local_variable_table(u2 local_variable_table_length);

    Array<LineNumber *> *parse_line_number_table(u2 length);

    Hashmap<Symbol *, AttributeInfo *, HashCode<const Symbol *>>* parse_class_attribute(u2 class_attribute_count);

    Array<BootstrapMethod *> *parse_bootstrap_method(u2 number_of_bootstrap_methods);

    Array<InnerClass *> *parse_inner_classes(u2 number_of_classes);

public:
    ClassFileParser(ClassFileStream* st) { set_stream(st); };
    ~ClassFileParser();

};


#endif //JVMPLUS_CLASSFILEPARSER_HPP
