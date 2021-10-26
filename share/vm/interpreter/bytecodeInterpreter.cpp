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
    stack<StackValue*>* operand_stack = frame->get_operand_stack();
    Array<StackValue*>* local_variable_table = frame->get_local_variable_table();
    Klass* klass = method->get_belong_klass();
    ConstantPool* constant_pool = ((InstanceKlass*)klass)->get_constant_pool();

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
                                    field_val = reinterpret_cast<jobject>(&bool_val);
                                }
                                break;
                            case T_SHORT:
                                {
                                    jshort short_val = g_env->GetStaticShortField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&short_val);
                                }
                                break;
                            case T_CHAR:
                                {
                                    jchar char_val = g_env->GetStaticCharField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&char_val);
                                }
                                break;
                            case T_BYTE:
                                {
                                    jbyte byte_val = g_env->GetStaticByteField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&byte_val);
                                }
                                break;
                            case T_INT:
                                {
                                    jint int_val = g_env->GetStaticIntField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&int_val);
                                }
                                break;
                            case T_LONG:
                                {
                                    jlong long_val = g_env->GetStaticLongField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&long_val);
                                }
                                break;
                            case T_FLOAT:
                                {
                                    jfloat float_val = g_env->GetStaticFloatField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&float_val);
                                }
                                break;
                            case T_DOUBLE:
                                {
                                    jdouble double_val = g_env->GetStaticDoubleField(clazz, field);
                                    field_val = reinterpret_cast<jobject>(&double_val);
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
                        oop mirror = klass->java_mirror();
                        field_val = mirror->get_field(class_name, field_name);
                    }
                    INFO_PRINT("static field: %s, value: %s", field_name->as_C_string(), field_val);
                    descriptor_stream->push_field(field_val, frame);
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
                                    operand_stack->push(new StackValue(T_OBJECT, (jobject)klass->java_mirror()));
                                }
                            }
                            break;
                        default:
                            ERROR_PRINT("无法识别的常量池类型: %d", tag);
                            exit(-1);
                    }
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

                        // not call construct
                        jobject object = g_env->AllocObject(clazz);

                        frame->push_operand_stack(new StackValue(T_OBJECT, object));
                    } else {
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);

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
            case ASTORE_1:
                {
                    INFO_PRINT("执行指令: astore_1，该指令功能为: 将操作数栈栈顶的引用类型的元素存入局部变量表中索引为1的位置");
                    StackValue* value = frame->pop_operand_stack();
                    frame->set_local_variable_table(1, value);
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
            case ILOAD_1:
                {
                    INFO_PRINT("执行指令: iload_1，该指令功能为: 将局部变量表中索引为1的值（int类型）压入操作数栈中");
                    // 从局部变量表中索引为1的值
                    StackValue* value = frame->get_local_variable_table(1);
                    // 压入操作数栈中
                    frame->push_operand_stack(value);
                }
                break;
            case ICONST_1:
                {
                    INFO_PRINT("执行指令: iconst_1，该指令功能为: 将int类型的常量1压入操作数栈中");
                    // 将1（int）压入操作数栈中
                    frame->push_operand_stack(new StackValue(T_INT, 1));
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
            case ALOAD_1:
                {
                    INFO_PRINT("执行指令: aload_1，该指令功能为: 将局部变量表中索引为1的值（引用类型）压入操作数栈中");
                    StackValue* value = frame->get_local_variable_table(1);
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
                        instance_oop->add_field(class_name, field_name, value);
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
                        field_val = instance_oop->get_field(class_name, field_name);
                    }
                    INFO_PRINT("static field: %s, value: %p", field_name->as_C_string(), field_val);
                    descriptor_stream->push_field(field_val, frame);
                }
                break;
            case INVOKESPECIAL:
                {
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

                       g_env->CallVoidMethodA(obj, method, params);
                    } else {
                        InstanceKlass* klass = (InstanceKlass*) SystemDictionary::resolve_or_null(class_name);

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
            default:
                ERROR_PRINT("not bytecode %d", opcode);
                exit(-1);
        }
    }
}
