//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_JVM_H
#define JVMPLUS_JVM_H

enum {
    JVM_CONSTANT_Invalid = 0,
    JVM_CONSTANT_Utf8,
    JVM_CONSTANT_Unicode,               /* unused */
    JVM_CONSTANT_Integer,
    JVM_CONSTANT_Float,
    JVM_CONSTANT_Long,
    JVM_CONSTANT_Double,
    JVM_CONSTANT_Class,
    JVM_CONSTANT_String,
    JVM_CONSTANT_Fieldref,
    JVM_CONSTANT_Methodref,
    JVM_CONSTANT_InterfaceMethodref,
    JVM_CONSTANT_NameAndType,
    JVM_CONSTANT_MethodHandle           = 15,  // JSR 292
    JVM_CONSTANT_MethodType             = 16,  // JSR 292
    //JVM_CONSTANT_(unused)             = 17,  // JSR 292 early drafts only
    JVM_CONSTANT_InvokeDynamic          = 18,  // JSR 292
    JVM_CONSTANT_ExternalMax            = 18   // Last tag found in classfiles
};

enum {
    JVM_ACC_PUBLIC        = 0x0001,
    JVM_ACC_PRIVATE       = 0x0002,
    JVM_ACC_PROTECTED     = 0x0004,
    JVM_ACC_STATIC        = 0x0008,
    JVM_ACC_FINAL         = 0x0010,
    JVM_ACC_SYNCHRONIZED  = 0x0020,
    JVM_ACC_SUPER         = 0x0020,
    JVM_ACC_VOLATILE      = 0x0040,
    JVM_ACC_BRIDGE        = 0x0040,
    JVM_ACC_TRANSIENT     = 0x0080,
    JVM_ACC_VARARGS       = 0x0080,
    JVM_ACC_NATIVE        = 0x0100,
    JVM_ACC_INTERFACE     = 0x0200,
    JVM_ACC_ABSTRACT      = 0x0400,
    JVM_ACC_STRICT        = 0x0800,
    JVM_ACC_SYNTHETIC     = 0x1000,
    JVM_ACC_ANNOTATION    = 0x2000,
    JVM_ACC_ENUM          = 0x4000
};

// 描述符
enum {
    JVM_SIGNATURE_ARRAY = '[',
    JVM_SIGNATURE_BYTE = 'B',
    JVM_SIGNATURE_CHAR = 'C',
    JVM_SIGNATURE_CLASS = 'L',
    JVM_SIGNATURE_END_CLASS = ';',
    JVM_SIGNATURE_ENUM = 'E',
    JVM_SIGNATURE_FLOAT = 'F',
    JVM_SIGNATURE_DOUBLE = 'D',
    JVM_SIGNATURE_START_FUNC = '(',
    JVM_SIGNATURE_END_FUNC = ')',
    JVM_SIGNATURE_INT = 'I',
    JVM_SIGNATURE_LONG = 'J',
    JVM_SIGNATURE_SHORT = 'S',
    JVM_SIGNATURE_VOID = 'V',
    JVM_SIGNATURE_BOOLEAN = 'Z'
};
#endif //JVMPLUS_JVM_H
