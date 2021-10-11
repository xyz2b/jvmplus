//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_INSTANCEKLASS_HPP
#define JVMPLUS_INSTANCEKLASS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/oops/constantPool.hpp"
#include "../../../share/vm/oops/method.hpp"


class InstanceKlass : public Klass {
public:
    u4      _magic;
    u2      _minor_version;         // minor version number of class file
    u2      _major_version;         // major version number of class file

    // Constant pool for this class.
    ConstantPool*   _constants;

    u2 _access_flags;

    u2 _this_class;
    u2 _super_class;

    u2 _interfaces_count;
    // Interface (Klass*s) this class declares to implement.
    Array<Klass*>*  _interfaces;

    u2 _fields_count;
    // Instance and static variable information.
    Array<u2>*      _fields;

    u2 _methods_count;
    // Method Array;
    Array<Method*>* _methods;

    u2 _attributes_count;
};


#endif //JVMPLUS_INSTANCEKLASS_HPP
