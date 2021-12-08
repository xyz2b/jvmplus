//
// Created by ziya on 2021/10/19.
//

#include "bytecodeInterpreter.hpp"

#include "../../../share/vm/oops/codeAttribute.hpp"
#include "../../../share/vm/oops/constantPool.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"
#include "../../../share/vm/prims/unsafe.cpp"
#include "../../../include/jni/JniTools.h"
#include "../../../share/vm/prims/JavaNativeInterface.hpp"
#include "../../../share/vm/classfile/systemDictionary.h"

extern JNIEnv* g_env;

void BytecodeInterpreter::run(JavaThread* current_thread, Method* method) {
    Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
    CodeAttribute* codeAttribute = (CodeAttribute*) method->get_attribute(c);

    BaseBytecodeStream* code = codeAttribute->code_stream();

    JavaVFrame* frame = (JavaVFrame*) current_thread->top_frame();

    Array<StackValue*>* operand_stack = frame->get_operand_stack();
    Array<StackValue*>* local_variable_table = frame->get_local_variable_table();
    Klass* klass = method->get_belong_klass();
    ConstantPool* constant_pool = ((InstanceKlass*)klass)->get_constant_pool();

    INFO_PRINT("run %s::%s", constant_pool->get_class_name_by_class_ref(((InstanceKlass*)klass)->get_this_class())->as_C_string(),constant_pool->symbol_at(method->name_index())->as_C_string());


    while (!code->end()) {
        u1 opcode = code->get_u1_code();

        switch (opcode) {
            case NOP:
                {
                    INFO_PRINT("执行指令: nop，该指令功能: 什么也不做");
                }
                break;
            case ACONST_NULL:
                {
                    INFO_PRINT("执行指令: aconst_null，该指令功能: 将一个null对象引用压入栈顶");
                    JavaVFrame* frame = (JavaVFrame*) current_thread->top_frame();

                    frame->push_operand_stack(new StackValue(T_OBJECT, nullptr));
                }
                break;
            case GETSTATIC:
                {
                    INFO_PRINT("执行指令: getstatic，该指令功能为: 获取类的静态字段值并压入操作数栈");
                    // 取出操作数，getstatic指令的操作数是常量池的索引（Fieldref），占两个字节
                    u2 operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_field_ref(operand);
                    Symbol* field_name = constant_pool->get_filed_name_by_field_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_descriptor_by_field_ref(operand);

                    // 解析字段描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name);
                    descriptor_stream->parse_filed();

                    jobject field_val = nullptr;
                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clazz出错")
                            exit(-1);
                        }

                        // 2.获取属性信息
                        jfieldID field = g_env->GetStaticFieldID(clazz, field_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == field) {
                            ERROR_PRINT("获取field出错了")
                            exit(-1);
                        }

                        // 3.读属性值（静态属性值在Class对象中）
                        switch (descriptor_stream->get_field_type()) {
                            case T_BOOLEAN:
                                {
                                    jboolean bool_val = g_env->GetStaticBooleanField(clazz, field);
                                    *((jboolean*)(&field_val)) = bool_val;
                                }
                                break;
                            case T_SHORT:
                                {
                                    jshort short_val = g_env->GetStaticShortField(clazz, field);
                                    *((jshort*)(&field_val)) = short_val;
                                }
                                break;
                            case T_CHAR:
                                {
                                    jchar char_val = g_env->GetStaticCharField(clazz, field);
                                    *((jchar*)(&field_val)) = char_val;
                                }
                                break;
                            case T_BYTE:
                                {
                                    jbyte byte_val = g_env->GetStaticByteField(clazz, field);
                                    *((jbyte*)(&field_val)) = byte_val;
                                }
                                break;
                            case T_INT:
                                {
                                    jint int_val = g_env->GetStaticIntField(clazz, field);
                                    *((jint*)(&field_val)) = int_val;
                                }
                                break;
                            case T_LONG:
                                {
                                    jlong long_val = g_env->GetStaticLongField(clazz, field);
                                    *((jlong*)(&field_val)) = long_val;
                                }
                                break;
                            case T_FLOAT:
                                {
                                    jfloat float_val = g_env->GetStaticFloatField(clazz, field);
                                    *((jfloat*)(&field_val)) = float_val;
                                }
                                break;
                            case T_DOUBLE:
                                {
                                    jdouble double_val = g_env->GetStaticDoubleField(clazz, field);
                                    *((jdouble*)(&field_val)) = double_val;
                                }
                                break;
                            case T_OBJECT:
                                {
                                    jobject object_val = g_env->GetStaticObjectField(clazz, field);
                                    field_val = object_val;
                                }
                                break;
                            default:
                                ERROR_PRINT("无法识别的field类型%s", descriptor_name->as_C_string());
                                exit(-1);
                        }
                    } else {
                        // 可能存在只是去操作父类静态变量的情况，此时子类不需要初始化，父类需要初始化
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->link_class();
//                        klass->initialize();

                        while (klass != nullptr) {
                            ConstantPool* constant_pool = klass->get_constant_pool();
                            Symbol* class_name = constant_pool->get_class_name_by_class_ref(klass->get_this_class());


                            if (class_name->start_with("java")) {
                                INFO_PRINT("不在 java包的类中查找静态变量");
                                return;
                            }

                            oop mirror = klass->java_mirror();

                            if (mirror->find_field(class_name, field_name)) {
                                // 找到了这个静态变量属于哪个父类，此时需要初始化父类，执行父类的clinit方法，不然后面获取到的值可能不对
                                klass->initialize();
                                field_val = mirror->get_field(class_name, field_name);
                                INFO_PRINT("找到了 field %s.%s = %p", class_name->as_C_string(), field_name->as_C_string(), field_val);
                                break;
                            }

                            klass = (InstanceKlass*)klass->get_super_klass();
                        }

                    }
                    INFO_PRINT("static field: %s, value: %p", field_name->as_C_string(), field_val);
                    descriptor_stream->push_field(field_val, frame);
                }
                break;
            case PUTSTATIC:
                {
                    INFO_PRINT("执行指令: putstatic，该指令功能为: 为指定类的静态字段赋值");
                    // 取出操作数，putstatic指令的操作数是常量池的索引（Fieldref），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_field_ref(operand);
                    Symbol* field_name = constant_pool->get_filed_name_by_field_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_descriptor_by_field_ref(operand);

                    // 解析字段描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name);
                    descriptor_stream->parse_filed();

                    // 从栈中取出对应类型的字段值
                    jobject value = descriptor_stream->get_field_val(frame);

                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clazz出错")
                            exit(-1);
                        }

                        // 2.获取属性信息
                        jfieldID field = g_env->GetStaticFieldID(clazz, field_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == field) {
                            ERROR_PRINT("获取field出错了")
                            exit(-1);
                        }

                        // 3.读属性值（静态属性值在Class对象中）
                        switch (descriptor_stream->get_field_type()) {
                            case T_BOOLEAN:
                                {
                                    jboolean field_val = *((jboolean *)(&value));
                                    g_env->SetStaticBooleanField(clazz, field, field_val);
                                }
                                break;
                            case T_SHORT:
                                {
                                    jshort field_val = *((jshort *)(&value));
                                    g_env->SetStaticShortField(clazz, field, field_val);
                                }
                                break;
                            case T_CHAR:
                                {
                                    jchar field_val = *((jchar *)(&value));
                                    g_env->SetStaticCharField(clazz, field, field_val);
                                }
                                break;
                            case T_BYTE:
                                {
                                    jbyte field_val = *((jbyte*)(&value));
                                    g_env->SetStaticByteField(clazz, field, field_val);
                                }
                                break;
                            case T_INT:
                                {
                                    jint field_val = *((jint*)(&value));
                                    g_env->SetStaticIntField(clazz, field, field_val);
                                }
                                break;
                            case T_LONG:
                                {
                                    jlong field_val = *((jlong*)(&value));
                                    g_env->SetStaticLongField(clazz, field, field_val);
                                }
                                break;
                            case T_FLOAT:
                                {
                                    jfloat field_val = *((jfloat*)(&value));
                                    g_env->SetStaticFloatField(clazz, field, field_val);
                                }
                                break;
                            case T_DOUBLE:
                                {
                                    jdouble field_val = *((jdouble*)(&value));
                                    g_env->SetStaticDoubleField(clazz, field, field_val);
                                }
                                break;
                            case T_OBJECT:
                                {
                                    g_env->SetStaticObjectField(clazz, field, value);
                                }
                                break;
                            default:
                                ERROR_PRINT("无法识别的field类型%s", descriptor_name->as_C_string());
                                exit(-1);
                        }
                    } else {
                        // 可能存在只是去操作父类静态变量的情况，此时子类不需要初始化，父类需要初始化
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->link_class();
//                        klass->initialize();

                        while (klass != nullptr) {
                            ConstantPool* constant_pool = klass->get_constant_pool();
                            Symbol* class_name = constant_pool->get_class_name_by_class_ref(klass->get_this_class());

                            if (class_name->start_with("java")) {
                                INFO_PRINT("不在 java包的类中查找静态变量");
                                return;
                            }

                            oop mirror = klass->java_mirror();

                            if (mirror->find_field(class_name, field_name)) {
                                // 找到了这个静态变量属于哪个父类，此时需要初始化父类，执行父类的clinit方法
                                klass->initialize();
                                INFO_PRINT("找到了 field %s.%s", class_name->as_C_string(), field_name->as_C_string());
                                mirror->put_field(class_name, field_name, value);
                                break;
                            }

                            klass = (InstanceKlass*)klass->get_super_klass();
                        }
                    }
                }
                break;
            case LDC2_W:
                {
                    INFO_PRINT("执行指令: ldc2_w，该指令功能为: 从运行时常量池中提取long或double数据并压入操作数栈（宽索引）中");
                    // 取出操作数，两个无符号byte类型整数，合并之后的值就是 运行时常量池中的索引号，占两个字节
                    int operand = code->get_u2_code();

                    ConstantTag tag = constant_pool->tag_at(operand);

                    if (tag.is_long()) {
                        long l = constant_pool->get_long_by_long_ref(operand);
                        frame->push_operand_stack(new StackValue(T_LONG, l));
                    } else if (tag.is_double()) {
                        double d = constant_pool->get_double_by_double_ref(operand);
                        frame->push_operand_stack(new StackValue(T_DOUBLE, d));
                    } else {
                        ERROR_PRINT("无法识别的格式: %d", tag.value());
                    }
                }
                break;
            case LDC:
                {
                    INFO_PRINT("执行指令: ldc，该指令功能为: 从运行时常量池中提取数据并压入操作数栈");

                    // 取出操作数，LDC指令的操作数是常量池的索引，一个字节
                    int operand = code->get_u1_code();
                    // 从常量池中取出操作数的类型tag，即常量池项的类型tag
                    ConstantTag tag = constant_pool->tag_at(operand);

                    switch (tag.value()) {
                        case JVM_CONSTANT_Integer:
                            {
                                int content = constant_pool->get_int_by_int_ref(operand);
                                operand_stack->push(new StackValue(T_INT, content));
                            }
                            break;
                        case JVM_CONSTANT_Float:
                            {
                                float content = constant_pool->get_float_by_float_ref(operand);
                                operand_stack->push(new StackValue(T_FLOAT, content));
                            }
                            break;
                        case JVM_CONSTANT_String:
                            {
                                Symbol* content = constant_pool->get_string_by_string_ref(operand);
                                operand_stack->push(new StackValue(T_OBJECT, JniTools::charsToJavaString(content->as_C_string())));
                            }
                            break;
                        case JVM_CONSTANT_Class:
                            {
                                Symbol* class_name = constant_pool->get_class_name_by_class_ref(operand);
                                // JVM加载器加载的类（java开头），通过JNI
                                if (class_name->start_with("java")) {
                                    // 获取类元信息
                                    jclass clazz = g_env->FindClass(class_name->as_C_string());
                                    if (nullptr == clazz) {
                                        ERROR_PRINT("获取clazz出错")
                                        exit(-1);
                                    }
                                    operand_stack->push(new StackValue(T_OBJECT, clazz));
                                } else {
                                    InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                                    klass->initialize();

                                    operand_stack->push(new StackValue(T_OBJECT, (jobject)klass));
                                }
                            }
                            break;
                        default:
                            ERROR_PRINT("无法识别的常量池类型: %d", tag);
                            exit(-1);
                    }
                }
                break;
            case NEW:
                {
                    INFO_PRINT("执行指令: new，该指令功能为: 创建一个对象，并将其引用压入栈顶");
                    // 取出操作数，new指令的操作数是常量池的索引（Class），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_class_ref(operand);

                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clz出错")
                            exit(-1);
                        }

                        // 获取方法
                        jmethodID construct = g_env->GetMethodID(clazz, "<init>", "()V");
                        if (nullptr == method) {
                            ERROR_PRINT("获取non-param construct方法出错")
                            exit(-1);
                        }

                        // call construct
                        jobject object = g_env->NewObject(clazz, construct);

                        frame->push_operand_stack(new StackValue(T_OBJECT, object));
                    } else {
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->initialize();

                        INFO_PRINT("生成oop实例")
                        instanceOop oop = klass->allocate_instance(klass);


                        frame->push_operand_stack(new StackValue(T_OBJECT, (jobject)oop));
                    }
                }
                break;
            case DUP:
                {
                    INFO_PRINT("执行指令: dup，该指令功能为: 复制操作数栈顶的值，并插入到栈顶（不弹出栈顶的值）");
                    StackValue* value = frame->top_operand_stack();
                    frame->push_operand_stack(value);
                }
                break;
            case ASTORE:
                {
                    INFO_PRINT("执行指令: astore，该指令功能为: 将操作数栈栈顶的引用类型的元素存入局部变量表中对应索引（索引通过操作数给出）的位置");
                    // 取出操作数，无符号byte类型整数，标识局部变量表中的索引号，占一个字节
                    int index = code->get_u1_code();

                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(index, value);
                }
                break;
            case ASTORE_0:
                {
                    INFO_PRINT("执行指令: astore_0，该指令功能为: 将操作数栈栈顶的引用类型的元素存入局部变量表中索引为0的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(0, value);
                }
                break;
            case ASTORE_1:
                {
                    INFO_PRINT("执行指令: astore_1，该指令功能为: 将操作数栈栈顶的引用类型的元素存入局部变量表中索引为1的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(1, value);
                }
                break;
            case ASTORE_2:
                {
                    INFO_PRINT("执行指令: astore_2，该指令功能为: 将操作数栈栈顶的引用类型的元素存入局部变量表中索引为2的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(2, value);
                }
                break;
            case ASTORE_3:
                {
                    INFO_PRINT("执行指令: astore_3，该指令功能为: 将操作数栈栈顶的引用类型的元素存入局部变量表中索引为3的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(3, value);
                }
                break;
            case ISTORE_1:
                {
                    INFO_PRINT("执行指令: istore_1，该指令功能为: 将操作数栈栈顶的int类型的元素存入局部变量表中索引为1的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(1, value);
                }
                break;
            case ISTORE_2:
                {
                    INFO_PRINT("执行指令: istore_2，该指令功能为: 将操作数栈栈顶的int类型的元素存入局部变量表中索引为2的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(2, value);
                }
                break;
            case ISTORE_3:
                {
                    INFO_PRINT("执行指令: istore_3，该指令功能为: 将操作数栈栈顶的int类型的元素存入局部变量表中索引为3的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(3, value);
                }
                break;
            case ILOAD_0:
                {
                    INFO_PRINT("执行指令: iload_0，该指令功能为: 将局部变量表中索引为0的值（int类型）压入操作数栈中");
                    // 从局部变量表中索引为1的值
                    StackValue* value = frame->get_local_variable_table(0);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ILOAD_1:
                {
                    INFO_PRINT("执行指令: iload_1，该指令功能为: 将局部变量表中索引为1的值（int类型）压入操作数栈中");
                    // 从局部变量表中索引为1的值
                    StackValue* value = frame->get_local_variable_table(1);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ILOAD_2:
                {
                    INFO_PRINT("执行指令: iload_2，该指令功能为: 将局部变量表中索引为2的值（int类型）压入操作数栈中");
                    // 从局部变量表中索引为1的值
                    StackValue* value = frame->get_local_variable_table(2);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ILOAD_3:
                {
                    INFO_PRINT("执行指令: iload_3，该指令功能为: 将局部变量表中索引为3的值（int类型）压入操作数栈中");
                    // 从局部变量表中索引为1的值
                    StackValue* value = frame->get_local_variable_table(3);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ICONST_M1:
                {
                    INFO_PRINT("执行指令: iconst_m1，该指令功能为: 将int类型的常量-1压入操作数栈中");
                    // 将1（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, -1));
                }
                break;
            case ICONST_0:
                {
                    INFO_PRINT("执行指令: iconst_0，该指令功能为: 将int类型的常量0压入操作数栈中");
                    // 将1（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 0));
                }
                break;
            case ICONST_1:
                {
                    INFO_PRINT("执行指令: iconst_1，该指令功能为: 将int类型的常量1压入操作数栈中");
                    // 将1（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 1));
                }
                break;
            case ICONST_2:
                {
                    INFO_PRINT("执行指令: iconst_2，该指令功能为: 将int类型的常量2压入操作数栈中");
                    // 将2（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 2));
                }
                break;
            case ICONST_3:
                {
                    INFO_PRINT("执行指令: iconst_3，该指令功能为: 将int类型的常量3压入操作数栈中");
                    // 将3（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 3));
                }
                break;
            case ICONST_4:
                {
                    INFO_PRINT("执行指令: iconst_4，该指令功能为: 将int类型的常量4压入操作数栈中");
                    // 将4（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 4));
                }
                break;
            case ICONST_5:
                {
                    INFO_PRINT("执行指令: iconst_5，该指令功能为: 将int类型的常量5压入操作数栈中");
                    // 将5（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 5));
                }
                break;
            case LCONST_0:
                {
                    INFO_PRINT("执行指令: lconst_0，该指令功能为: 将long类型的常量0压入操作数栈中");
                    // 将0（long）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_LONG, (long)0));
                }
                break;
            case LCONST_1:
                {
                    INFO_PRINT("执行指令: lconst_1，该指令功能为: 将long类型的常量1压入操作数栈中");
                    // 将1（long）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_LONG, (long)1));
                }
                break;
            case FCONST_0:
                {
                    INFO_PRINT("执行指令: fconst_0，该指令功能为: 将float类型的常量0压入操作数栈中");
                    // 将0（float）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_FLOAT, (float)0));
                }
                break;
            case FCONST_1:
                {
                    INFO_PRINT("执行指令: fconst_1，该指令功能为: 将float类型的常量1压入操作数栈中");
                    // 将1（float）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_FLOAT, (float)1));
                }
                break;
            case ALOAD:
                {
                    INFO_PRINT("执行指令: aload，该指令功能为: 将局部变量表中对应索引（操作数中给出）位置的值（引用类型）压入操作数栈中");
                    int index = code->get_u1_code();
                    StackValue* value = frame->get_local_variable_table(index);
                    frame->push_operand_stack(value);
                }
                break;
            case ALOAD_0:
                {
                    INFO_PRINT("执行指令: aload_0，该指令功能为: 将局部变量表中索引为0的值（引用类型）压入操作数栈中");
                    // 从局部变量表中索引为0的值
                    StackValue* value = frame->get_local_variable_table(0);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ALOAD_1:
                {
                    INFO_PRINT("执行指令: aload_1，该指令功能为: 将局部变量表中索引为1的值（引用类型）压入操作数栈中");
                    // 从局部变量表中索引为0的值
                    StackValue* value = frame->get_local_variable_table(1);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ALOAD_2:
                {
                    INFO_PRINT("执行指令: aload_2，该指令功能为: 将局部变量表中索引为2的值（引用类型）压入操作数栈中");
                    // 从局部变量表中索引为0的值
                    StackValue* value = frame->get_local_variable_table(2);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ALOAD_3:
                {
                    INFO_PRINT("执行指令: aload_3，该指令功能为: 将局部变量表中索引为3的值（引用类型）压入操作数栈中");
                    // 从局部变量表中索引为0的值
                    StackValue* value = frame->get_local_variable_table(3);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case BIPUSH:
                {
                    INFO_PRINT("执行指令: bipush，该指令功能为: 将立即数byte带符号扩展为一个int类型的值，然后压入操作数栈中");
                    // 取出操作数，byte类型的立即数，占一个字节，然后直接转成int类型
                    int value = code->get_u1_code();
                    frame->push_operand_stack(new StackValue(T_INT, value));
                }
                break;
            case POP:
                {
                    INFO_PRINT("执行指令: pop，该指令功能为: 将栈顶元素出栈");
                    frame->pop_operand_stack();
                }
                break;
            case RETURN:
                {
                    INFO_PRINT("执行指令: return，该指令功能为: 从方法中返回void，恢复调用者的栈帧，并且把程序的控制权交回调用者");
                    // pop出栈帧
                    current_thread->pop_frame();
                    INFO_PRINT("剩余栈帧数量: %d", current_thread->frame_size());
                }
                break;
            case IRETURN:
                {
                    INFO_PRINT("执行指令: ireturn，该指令功能为: 从方法中返回int类型数据，恢复调用者的栈帧，并且把程序的控制权交回调用者");
                    StackValue* ret = frame->pop_operand_stack();

                    // pop出当前栈帧
                    current_thread->pop_frame();
                    INFO_PRINT("剩余栈帧数量: %d", current_thread->frame_size());

                    // 将返回值压入调用者栈帧
                    ((JavaVFrame*) current_thread->top_frame())->push_operand_stack(ret);
                }
                break;
            case ARETURN:
                {
                    INFO_PRINT("执行指令: areturn，该指令功能为: 从方法中返回引用类型数据，恢复调用者的栈帧，并且把程序的控制权交回调用者");
                    StackValue* ret = frame->pop_operand_stack();

                    // pop出当前栈帧
                    current_thread->pop_frame();
                    INFO_PRINT("剩余栈帧数量: %d", current_thread->frame_size());

                    // 将返回值压入调用者栈帧
                    ((JavaVFrame*) current_thread->top_frame())->push_operand_stack(ret);
                }
                break;
            case PUTFIELD:
                {
                    INFO_PRINT("执行指令: putField，该指令功能为: 设置对象的属性值");
                    // 取出操作数，putfield指令的操作数是常量池的索引（Fieldref），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_field_ref(operand);
                    Symbol* field_name = constant_pool->get_filed_name_by_field_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_descriptor_by_field_ref(operand);

                    // 解析字段描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name);
                    descriptor_stream->parse_filed();

                    // 从栈中取出对应类型的字段值
                    jobject value = descriptor_stream->get_field_val(frame);

                    // 从操作数栈中弹出 属性所属类的对象，即this指针
                    jobject obj = (jobject)frame->pop_operand_stack()->data();

                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clazz出错")
                            exit(-1);
                        }

                        // 2.获取属性信息
                        jfieldID field = g_env->GetFieldID(clazz, field_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == field) {
                            ERROR_PRINT("获取field出错了")
                            exit(-1);
                        }

                        // 4.赋值新的值（非静态属性值在对象中）
                        switch (descriptor_stream->get_field_type()) {
                            case T_BOOLEAN:
                                {
                                    g_env->SetBooleanField(obj, field, (jlong)value);
                                }
                                break;
                            case T_SHORT:
                                {
                                    g_env->SetShortField(obj, field, (jlong)value);
                                }
                                break;
                            case T_CHAR:
                                {
                                     g_env->SetCharField(obj, field, (jlong)value);
                                }
                                break;
                            case T_BYTE:
                                {
                                    g_env->SetByteField(obj, field, (jlong)value);
                                }
                                break;
                            case T_INT:
                                {
                                    g_env->SetIntField(obj, field, (jlong)value);
                                }
                                break;
                            case T_LONG:
                                {
                                    g_env->SetLongField(obj, field, (jlong)value);
                                }
                                break;
                            case T_FLOAT:
                                {
                                    g_env->SetFloatField(obj, field, *((jfloat*)&value));
                                }
                                break;
                            case T_DOUBLE:
                                {
                                    g_env->SetDoubleField(obj, field, *((jdouble*)&value));
                                }
                                break;
                            case T_OBJECT:
                                {
                                    g_env->SetObjectField(obj, field, value);
                                }
                                break;
                            default:
                                ERROR_PRINT("无法识别的field类型%s", descriptor_name->as_C_string());
                                exit(-1);
                        }
                    } else {
                        INFO_PRINT("set object field value, %s.%s = %ld", class_name->as_C_string(), field_name->as_C_string(), (jlong)value);
                        instanceOop instance_oop = (instanceOop)obj;
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->initialize();

                        while (klass != nullptr) {
                            ConstantPool* constant_pool = klass->get_constant_pool();
                            Symbol* class_name = constant_pool->get_class_name_by_class_ref(klass->get_this_class());

                            if (class_name->start_with("java")) {
                                INFO_PRINT("不在 java包的类中查找静态变量");
                                return;
                            }

                            if (instance_oop->find_field(class_name, field_name)) {
                                INFO_PRINT("找到了 field %s.%s", class_name->as_C_string(), field_name->as_C_string());
                                instance_oop->put_field(class_name, field_name, value);
                                break;
                            }

                            klass = (InstanceKlass*)klass->get_super_klass();
                        }
                        INFO_PRINT("set object field value success")
                    }
                }
                break;
            case GETFIELD:
                {
                    INFO_PRINT("执行指令: getField，该指令功能为: 获取对象的属性值并压入操作数栈");
                    // 取出操作数，getfield指令的操作数是常量池的索引（Fieldref），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_field_ref(operand);
                    Symbol* field_name = constant_pool->get_filed_name_by_field_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_descriptor_by_field_ref(operand);

                    // 解析字段描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name);
                    descriptor_stream->parse_filed();

                    // 从操作数栈中弹出 属性所属类的对象，即this指针
                    jobject obj = (jobject)frame->pop_operand_stack()->data();

                    jobject field_val = nullptr;
                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clazz出错")
                            exit(-1);
                        }

                        // 2.获取属性信息
                        jfieldID field = g_env->GetFieldID(clazz, field_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == field) {
                            ERROR_PRINT("获取field出错了")
                            exit(-1);
                        }

                        // 3.读取属性值（非静态属性值在对象中）
                        switch (descriptor_stream->get_field_type()) {
                            case T_BOOLEAN:
                            {
                                jboolean bool_val = g_env->GetBooleanField(clazz, field);
                                *((jboolean*)(&field_val)) = bool_val;
                            }
                                break;
                            case T_SHORT:
                            {
                                jshort short_val = g_env->GetShortField(clazz, field);
                                *((jshort*)(&field_val)) = short_val;
                            }
                                break;
                            case T_CHAR:
                            {
                                jchar char_val = g_env->GetCharField(clazz, field);
                                *((jchar*)(&field_val)) = char_val;
                            }
                                break;
                            case T_BYTE:
                            {
                                jbyte byte_val = g_env->GetByteField(clazz, field);
                                *((jbyte*)(&field_val)) = byte_val;
                            }
                                break;
                            case T_INT:
                            {
                                jint int_val = g_env->GetIntField(clazz, field);
                                *((jint*)(&field_val)) = int_val;
                            }
                                break;
                            case T_LONG:
                            {
                                jlong long_val = g_env->GetLongField(clazz, field);
                                *((jlong*)(&field_val)) = long_val;
                            }
                                break;
                            case T_FLOAT:
                            {
                                jfloat float_val = g_env->GetFloatField(clazz, field);
                                *((jfloat*)(&field_val)) = float_val;
                            }
                                break;
                            case T_DOUBLE:
                            {
                                jdouble double_val = g_env->GetDoubleField(clazz, field);
                                *((jdouble*)(&field_val)) = double_val;
                            }
                                break;
                            case T_OBJECT:
                            {
                                jobject object_val = g_env->GetObjectField(clazz, field);
                                field_val = object_val;
                            }
                                break;
                            default:
                                ERROR_PRINT("无法识别的field类型%s", descriptor_name->as_C_string());
                                exit(-1);
                        }
                    } else {
                        instanceOop instance_oop = (instanceOop)obj;
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->initialize();

                        while (klass != nullptr) {
                            ConstantPool* constant_pool = klass->get_constant_pool();
                            Symbol* class_name = constant_pool->get_class_name_by_class_ref(klass->get_this_class());

                            if (class_name->start_with("java")) {
                                INFO_PRINT("不在 java包的类中查找静态变量");
                                return;
                            }

                            if (instance_oop->find_field(class_name, field_name)) {
                                field_val = instance_oop->get_field(class_name, field_name);
                                INFO_PRINT("找到了 field %s.%s = %p, ", class_name->as_C_string(), field_name->as_C_string(), field_val);
                                break;
                            }

                            klass = (InstanceKlass*)klass->get_super_klass();
                        }
                    }
                    INFO_PRINT("static field: %s, value: %p", field_name->as_C_string(), field_val);
                    descriptor_stream->push_field(field_val, frame);
                }
                break;
            case INVOKESPECIAL:
                {
//                    if (strcmp(constant_pool->symbol_at(method->name_index())->as_C_string(), "<init>") == 0 &&
//                        strcmp(constant_pool->get_class_name_by_class_ref(((InstanceKlass*)klass)->get_this_class())->as_C_string(),
//                               "org/xyz/jvm/example/Father") == 0)
//                        return;

                    INFO_PRINT("执行指令: invokespecial，该指令功能为: 调用实例方法，专门用来调用父类方法、私有方法和实例初始化方法");
                    // 取出操作数，invokestatic指令的操作数是常量池的索引（Methodref），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_method_ref(operand);
                    Symbol* method_name = constant_pool->get_method_name_by_method_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_method_descriptor_by_method_ref(operand);

                    // 解析描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name, method_name);
                    descriptor_stream->parse_method();

                    INFO_PRINT("执行方法: %s:%s#%s", class_name->as_C_string(), method_name->as_C_string(), descriptor_name->as_C_string());

                    // 系统加载的类走JNI
                    if (class_name->start_with("java")) {
                        // 调用java包下的父类构造方法，不做处理
                        if (method_name->start_with("<init>")) {
                            break;
                        }
                        // TODO: 调用java包下的父类的实例方法，不做处理，因为用子类对象调用父类方法，jni不支持

                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clz出错")
                            exit(-1);
                        }

                        // 获取方法
                        jmethodID method = g_env->GetMethodID(clazz, method_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == method) {
                            ERROR_PRINT("获取方法出错")
                            exit(-1);
                        }

                        // params
                        jvalue* params = descriptor_stream->get_params_val(frame);

                        // 从操作数栈中弹出 被调方法所属类的对象，即this指针
                        jobject obj = (jobject)frame->pop_operand_stack()->data();

                        jobject return_val;
                        switch (descriptor_stream->return_element_type()) {
                            case T_BOOLEAN:
                            {
                                jboolean bool_val = g_env->CallBooleanMethodA(obj, method, params);
                                *((jboolean*)(&return_val)) = bool_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_SHORT:
                            {
                                jshort short_val = g_env->CallShortMethodA(obj, method, params);
                                *((jshort*)(&return_val)) = short_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_CHAR:
                            {
                                jchar char_val = g_env->CallCharMethodA(obj, method, params);
                                *((jchar*)(&return_val)) = char_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_BYTE:
                            {
                                jbyte byte_val = g_env->CallByteMethodA(obj, method, params);
                                *((jbyte*)(&return_val)) = byte_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_INT:
                            {
                                jint int_val = g_env->CallIntMethodA(obj, method, params);
                                *((jint*)(&return_val)) = int_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_LONG:
                            {
                                jlong long_val = g_env->CallLongMethodA(obj, method, params);
                                *((jlong*)(&return_val)) = long_val;
                                frame->push_operand_stack(new StackValue(T_LONG, return_val));
                            }
                                break;
                            case T_FLOAT:
                            {
                                jfloat float_val = g_env->CallFloatMethodA(obj, method, params);
                                *((jfloat*)(&return_val)) = float_val;
                                frame->push_operand_stack(new StackValue(T_FLOAT, return_val));
                            }
                                break;
                            case T_DOUBLE:
                            {
                                jdouble double_val = g_env->CallDoubleMethodA(obj, method, params);
                                *((jdouble*)(&return_val)) = double_val;
                                frame->push_operand_stack(new StackValue(T_DOUBLE, return_val));
                            }
                                break;
                            case T_OBJECT:
                            {
                                jobject object_val = g_env->CallObjectMethodA(obj, method, params);
                                return_val = object_val;
                                frame->push_operand_stack(new StackValue(T_OBJECT, return_val));
                            }
                                break;
                            case T_VOID:
                            {
                                g_env->CallVoidMethodA(obj, method, params);
                            }
                                break;
                            default:
                                ERROR_PRINT("无法识别的return类型%d", descriptor_stream->return_element_type());
                                exit(-1);
                        }
                    } else {
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->initialize();

                        // 在对应的类中找到对应的方法
                        Method* method = JavaNativeInterface::get_method(klass, method_name, descriptor_name);
                        if (nullptr == method) {
                            ERROR_PRINT("不存在的方法: %s#%s", method_name->as_C_string(), descriptor_name->as_C_string());
                            exit(-1);
                        }

                        // 同一个方法重复调用问题: 该方法的程序计数器如果没有重置，会导致下一次调用是从上一次调用完之后的指令位置开始，导致出错
                        // 调用某一个方法之前，需要重置该方法的程序计数器，避免上面所说的重复调用的问题
                        Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
                        CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);
                        // 重置程序计数器
                        code_attribute->code_stream()->reset();

                        JavaNativeInterface::call_method(klass, method);
                    }
                }
                break;
            case INVOKEVIRTUAL:
                {
                    INFO_PRINT("执行指令: invokevirtual，该指令功能为: 调用实例方法，依据实例的类型进行分派，这个方法不能使实例初始化方法也不能是类或接口的初始化方法（静态初始化方法）");
                    // 取出操作数，invokevirtual指令的操作数是常量池的索引（Methodref），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_method_ref(operand);
                    Symbol* method_name = constant_pool->get_method_name_by_method_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_method_descriptor_by_method_ref(operand);

                    // 解析描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name, method_name);
                    descriptor_stream->parse_method();

                    INFO_PRINT("执行方法: %s:%s#%s", class_name->as_C_string(), method_name->as_C_string(), descriptor_name->as_C_string());

                    // 系统加载的类走JNI
                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clz出错")
                            exit(-1);
                        }

                        // 获取方法
                        jmethodID method = g_env->GetMethodID(clazz, method_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == method) {
                            ERROR_PRINT("获取方法出错")
                            exit(-1);
                        }

                        // params
                        jvalue* params = descriptor_stream->get_params_val(frame);

                        // 从操作数栈中弹出 被调方法所属类的对象，即this指针
                        jobject obj = (jobject)frame->pop_operand_stack()->data();

                        jobject return_val;
                        switch (descriptor_stream->return_element_type()) {
                            case T_BOOLEAN:
                            {
                                jboolean bool_val = g_env->CallBooleanMethodA(obj, method, params);
                                *((jboolean*)(&return_val)) = bool_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_SHORT:
                            {
                                jshort short_val = g_env->CallShortMethodA(obj, method, params);
                                *((jshort*)(&return_val)) = short_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_CHAR:
                            {
                                jchar char_val = g_env->CallCharMethodA(obj, method, params);
                                *((jchar*)(&return_val)) = char_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_BYTE:
                            {
                                jbyte byte_val = g_env->CallByteMethodA(obj, method, params);
                                *((jbyte*)(&return_val)) = byte_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_INT:
                            {
                                jint int_val = g_env->CallIntMethodA(obj, method, params);
                                *((jint*)(&return_val)) = int_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_LONG:
                            {
                                jlong long_val = g_env->CallLongMethodA(obj, method, params);
                                *((jlong*)(&return_val)) = long_val;
                                frame->push_operand_stack(new StackValue(T_LONG, return_val));
                            }
                                break;
                            case T_FLOAT:
                            {
                                jfloat float_val = g_env->CallFloatMethodA(obj, method, params);
                                *((jfloat*)(&return_val)) = float_val;
                                frame->push_operand_stack(new StackValue(T_FLOAT, return_val));
                            }
                                break;
                            case T_DOUBLE:
                            {
                                jdouble double_val = g_env->CallDoubleMethodA(obj, method, params);
                                *((jdouble*)(&return_val)) = double_val;
                                frame->push_operand_stack(new StackValue(T_DOUBLE, return_val));
                            }
                                break;
                            case T_OBJECT:
                            {
                                jobject object_val = g_env->CallObjectMethodA(obj, method, params);
                                return_val = object_val;
                                frame->push_operand_stack(new StackValue(T_OBJECT, return_val));
                            }
                                break;
                            case T_VOID:
                            {
                                g_env->CallVoidMethodA(obj, method, params);
                            }
                                break;
                            default:
                                ERROR_PRINT("无法识别的return类型%d", descriptor_stream->return_element_type());
                                exit(-1);
                        }
                    } else {
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->initialize();

                        // 在对应的类中找到对应的方法
                        Method* method = JavaNativeInterface::get_vmethod(klass, method_name, descriptor_name);
                        if (nullptr == method) {
                            ERROR_PRINT("不存在的方法: %s#%s", method_name->as_C_string(), descriptor_name->as_C_string());
                            exit(-1);
                        }

                        // 同一个方法重复调用问题: 该方法的程序计数器如果没有重置，会导致下一次调用是从上一次调用完之后的指令位置开始，导致出错
                        // 调用某一个方法之前，需要重置该方法的程序计数器，避免上面所说的重复调用的问题
                        Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
                        CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);
                        // 重置程序计数器
                        code_attribute->code_stream()->reset();

                        JavaNativeInterface::call_method(klass, method);
                    }
                }
                break;
            case INVOKESTATIC:
                {
                    INFO_PRINT("执行指令: invokestatic( java体系的借助反射实现，自己定义的类自己实现 )");
                    // 取出操作数，invokevirtual指令的操作数是常量池的索引（Methodref），占两个字节
                    int operand = code->get_u2_code();

                    Symbol* class_name = constant_pool->get_class_name_by_method_ref(operand);
                    Symbol* method_name = constant_pool->get_method_name_by_method_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_method_descriptor_by_method_ref(operand);

                    // 解析描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name, method_name);
                    descriptor_stream->parse_method();

                    INFO_PRINT("执行方法: %s:%s#%s", class_name->as_C_string(), method_name->as_C_string(), descriptor_name->as_C_string());

                    // 系统加载的类走JNI
                    if (class_name->start_with("java")) {
                        // 获取类元信息
                        jclass clazz = g_env->FindClass(class_name->as_C_string());
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clz出错")
                            exit(-1);
                        }

                        // 获取方法
                        jmethodID method = g_env->GetStaticMethodID(clazz, method_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == method) {
                            ERROR_PRINT("获取方法出错")
                            exit(-1);
                        }

                        // params
                        jvalue* params = descriptor_stream->get_params_val(frame);

                        jobject return_val;
                        switch (descriptor_stream->return_element_type()) {
                            case T_BOOLEAN:
                            {
                                jboolean bool_val = g_env->CallStaticBooleanMethodA(clazz, method, params);
                                *((jboolean*)(&return_val)) = bool_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_SHORT:
                            {
                                jshort short_val = g_env->CallStaticShortMethodA(clazz, method, params);
                                *((jshort*)(&return_val)) = short_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_CHAR:
                            {
                                jchar char_val = g_env->CallStaticCharMethodA(clazz, method, params);
                                *((jchar*)(&return_val)) = char_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_BYTE:
                            {
                                jbyte byte_val = g_env->CallStaticByteMethodA(clazz, method, params);
                                *((jbyte*)(&return_val)) = byte_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_INT:
                            {
                                jint int_val = g_env->CallStaticIntMethodA(clazz, method, params);
                                *((jint*)(&return_val)) = int_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_LONG:
                            {
                                jlong long_val = g_env->CallStaticLongMethodA(clazz, method, params);
                                *((jlong*)(&return_val)) = long_val;
                                frame->push_operand_stack(new StackValue(T_LONG, return_val));
                            }
                                break;
                            case T_FLOAT:
                            {
                                jfloat float_val = g_env->CallStaticFloatMethodA(clazz, method, params);
                                *((jfloat*)(&return_val)) = float_val;
                                frame->push_operand_stack(new StackValue(T_FLOAT, return_val));
                            }
                                break;
                            case T_DOUBLE:
                            {
                                jdouble double_val = g_env->CallStaticDoubleMethodA(clazz, method, params);
                                *((jdouble*)(&return_val)) = double_val;
                                frame->push_operand_stack(new StackValue(T_DOUBLE, return_val));
                            }
                                break;
                            case T_OBJECT:
                            {
                                jobject object_val = g_env->CallStaticObjectMethodA(clazz, method, params);
                                return_val = object_val;
                                frame->push_operand_stack(new StackValue(T_OBJECT, return_val));
                            }
                                break;
                            case T_VOID:
                            {
                                g_env->CallStaticVoidMethodA(clazz, method, params);
                            }
                                break;
                            default:
                                ERROR_PRINT("无法识别的return类型%d", descriptor_stream->return_element_type());
                                exit(-1);
                        }
                    } else {
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);
                        klass->initialize();

                        // 在对应的类中找到对应的方法
                        Method* method = JavaNativeInterface::get_method(klass, method_name, descriptor_name);
                        if (nullptr == method) {
                            ERROR_PRINT("不存在的方法: %s#%s", method_name->as_C_string(), descriptor_name->as_C_string());
                            exit(-1);
                        }

                        // 同一个方法重复调用问题: 该方法的程序计数器如果没有重置，会导致下一次调用是从上一次调用完之后的指令位置开始，导致出错
                        // 调用某一个方法之前，需要重置该方法的程序计数器，避免上面所说的重复调用的问题
                        Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
                        CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);
                        // 重置程序计数器
                        code_attribute->code_stream()->reset();

                        JavaNativeInterface::call_method(klass, method);
                    }
                }
                break;
            case INVOKEINTERFACE:
                {
                    INFO_PRINT("执行指令: invokeinterface，该指令功能为: 调用接口方法");
                    // 取出操作数，invokeinterface指令的操作数是常量池的索引（InterfaceMethodref），占两个字节
                    int operand = code->get_u2_code();
                    code->get_u1_code();
                    code->get_u1_code();

                    Symbol* class_name = constant_pool->get_class_name_by_interface_method_ref(operand);
                    Symbol* method_name = constant_pool->get_method_name_by_interface_method_ref(operand);
                    Symbol* descriptor_name = constant_pool->get_method_descriptor_by_interface_method_ref(operand);

                    // 解析描述符
                    DescriptorStream* descriptor_stream = new DescriptorStream(descriptor_name, method_name);
                    descriptor_stream->parse_method();

                    // 系统加载的类走JNI
                    if (class_name->start_with("java")) {
                        // TODO: 接口实现类为java包下的类暂时不做处理
                        break;
                        // params
                        jvalue* params = descriptor_stream->get_params_val(frame);

                        // 从操作数栈中弹出 被调方法所属类的对象，即this指针
                        jobject obj = (jobject)frame->pop_operand_stack()->data();

                        // 获取类元信息
                        jclass clazz = g_env->GetObjectClass(obj);
                        if (nullptr == clazz) {
                            ERROR_PRINT("获取clz出错")
                            exit(-1);
                        }

                        // 获取方法
                        jmethodID method = g_env->GetMethodID(clazz, method_name->as_C_string(), descriptor_name->as_C_string());
                        if (nullptr == method) {
                            ERROR_PRINT("获取方法出错")
                            exit(-1);
                        }

                        jobject return_val;
                        switch (descriptor_stream->return_element_type()) {
                            case T_BOOLEAN:
                            {
                                jboolean bool_val = g_env->CallBooleanMethodA(obj, method, params);
                                *((jboolean*)(&return_val)) = bool_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_SHORT:
                            {
                                jshort short_val = g_env->CallShortMethodA(obj, method, params);
                                *((jshort*)(&return_val)) = short_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_CHAR:
                            {
                                jchar char_val = g_env->CallCharMethodA(obj, method, params);
                                *((jchar*)(&return_val)) = char_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_BYTE:
                            {
                                jbyte byte_val = g_env->CallByteMethodA(obj, method, params);
                                *((jbyte*)(&return_val)) = byte_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_INT:
                            {
                                jint int_val = g_env->CallIntMethodA(obj, method, params);
                                *((jint*)(&return_val)) = int_val;
                                frame->push_operand_stack(new StackValue(T_INT, return_val));
                            }
                                break;
                            case T_LONG:
                            {
                                jlong long_val = g_env->CallLongMethodA(obj, method, params);
                                *((jlong*)(&return_val)) = long_val;
                                frame->push_operand_stack(new StackValue(T_LONG, return_val));
                            }
                                break;
                            case T_FLOAT:
                            {
                                jfloat float_val = g_env->CallFloatMethodA(obj, method, params);
                                *((jfloat*)(&return_val)) = float_val;
                                frame->push_operand_stack(new StackValue(T_FLOAT, return_val));
                            }
                                break;
                            case T_DOUBLE:
                            {
                                jdouble double_val = g_env->CallDoubleMethodA(obj, method, params);
                                *((jdouble*)(&return_val)) = double_val;
                                frame->push_operand_stack(new StackValue(T_DOUBLE, return_val));
                            }
                                break;
                            case T_OBJECT:
                            {
                                jobject object_val = g_env->CallObjectMethodA(obj, method, params);
                                return_val = object_val;
                                frame->push_operand_stack(new StackValue(T_OBJECT, return_val));
                            }
                                break;
                            case T_VOID:
                            {
                                g_env->CallVoidMethodA(obj, method, params);
                            }
                                break;
                            default:
                                ERROR_PRINT("无法识别的return类型%d", descriptor_stream->return_element_type());
                                exit(-1);
                        }
                    } else {
                        // TODO: 在栈中的object属于的类中找实现方法，栈中还有参数
                        Array<StackValue*>* params = nullptr;
                        if (descriptor_stream->method_params_size() != 0) {
                            params = new Array<StackValue*>(descriptor_stream->method_params_size());
                            for (int i = 0; i < descriptor_stream->method_params_size(); i ++) {
                                params->add(frame->pop_operand_stack());
                            }
                        }

                        oop obj = (oop)frame->top_operand_stack()->data();

                        InstanceKlass* klass = (InstanceKlass*)obj->klass();
                        klass->initialize();

                        INFO_PRINT("执行方法: %s:%s#%s", klass->get_constant_pool()->get_class_name_by_class_ref(klass->get_this_class())->as_C_string(), method_name->as_C_string(), descriptor_name->as_C_string());

                        // 在对应的类中找到对应的方法
                        Method* method = JavaNativeInterface::get_imethod(klass, method_name, descriptor_name);
                        if (nullptr == method) {
                            ERROR_PRINT("不存在的方法: %s#%s", method_name->as_C_string(), descriptor_name->as_C_string());
                            exit(-1);
                        }

                        // 同一个方法重复调用问题: 该方法的程序计数器如果没有重置，会导致下一次调用是从上一次调用完之后的指令位置开始，导致出错
                        // 调用某一个方法之前，需要重置该方法的程序计数器，避免上面所说的重复调用的问题
                        Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
                        CodeAttribute* code_attribute = (CodeAttribute*) method->get_attribute(c);
                        // 重置程序计数器
                        code_attribute->code_stream()->reset();

                        JavaNativeInterface::call_method(klass, method, params);
                    }
                }
                break;
            default:
                ERROR_PRINT("not bytecode %d", opcode);
                exit(-1);
        }
    }

    INFO_PRINT("byte code run success");
}
