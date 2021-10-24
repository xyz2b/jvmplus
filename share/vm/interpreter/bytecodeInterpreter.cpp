//
// Created by ziya on 2021/10/19.
//

#include "bytecodeInterpreter.hpp"

#include "../../../share/vm/oops/codeAttribute.hpp"
#include "../../../share/vm/interpreter/baseBytecodeStream.hpp"
#include "../../../share/vm/runtime/javaVFrame.hpp"
#include "../../../share/vm/oops/constantPool.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"
#include "../../../share/vm/prims/unsafe.cpp"

extern JNIEnv* g_env;

void BytecodeInterpreter::run(JavaThread* current_thread, Method* method) {
    Symbol* c = new (strlen(JVM_ATTRIBUTE_Code)) Symbol(JVM_ATTRIBUTE_Code, strlen(JVM_ATTRIBUTE_Code));
    CodeAttribute* codeAttribute = (CodeAttribute*) method->get_attribute(c);

    BaseBytecodeStream* code = new BaseBytecodeStream((u1*)codeAttribute->code_base(), (u1*)codeAttribute->code_end(), method);

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
                        FiledInfo* f = ((InstanceKlass*)klass)->find_field(field_name, descriptor_name);
                        if (f == nullptr) {
                            ERROR_PRINT("can not find static field: %s#%s", field_name->as_C_string(), descriptor_name->as_C_string());
                            exit(-1);
                        }

                        field_val = mirror->get_field(*(descriptor_name->as_C_string()), f->offset());
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
                                operand_stack->push(new StackValue(T_OBJECT, (jobject)(content->as_C_string())));
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
                                    return_val = reinterpret_cast<jobject>(&bool_val);
                                    frame->push_operand_stack(new StackValue(T_INT, return_val));
                                }
                                break;
                            case T_SHORT:
                                {
                                    jshort short_val = g_env->CallShortMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&short_val);
                                    frame->push_operand_stack(new StackValue(T_INT, return_val));
                                }
                                break;
                            case T_CHAR:
                                {
                                    jchar char_val = g_env->CallCharMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&char_val);
                                    frame->push_operand_stack(new StackValue(T_INT, return_val));
                                }
                                break;
                            case T_BYTE:
                                {
                                    jbyte byte_val = g_env->CallByteMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&byte_val);
                                    frame->push_operand_stack(new StackValue(T_INT, return_val));
                                }
                                break;
                            case T_INT:
                                {
                                    jint int_val = g_env->CallIntMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&int_val);
                                    frame->push_operand_stack(new StackValue(T_INT, return_val));
                                }
                                break;
                            case T_LONG:
                                {
                                    jlong long_val = g_env->CallLongMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&long_val);
                                    frame->push_operand_stack(new StackValue(T_LONG, return_val));
                                }
                                break;
                            case T_FLOAT:
                                {
                                    jfloat float_val = g_env->CallFloatMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&float_val);
                                    frame->push_operand_stack(new StackValue(T_FLOAT, return_val));
                                }
                                break;
                            case T_DOUBLE:
                                {
                                    jdouble double_val = g_env->CallDoubleMethodA(obj, method, params);
                                    return_val = reinterpret_cast<jobject>(&double_val);
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
                                    INFO_PRINT("params: %s", (char*)(*params).l);
                                    INFO_PRINT("%p", obj);
                                    INFO_PRINT("class_name: %s", class_name->as_C_string());
                                    INFO_PRINT("%p, %s, %s, %d", method, method_name->as_C_string(), descriptor_name->as_C_string(), descriptor_stream->return_element_type());
                                    g_env->CallVoidMethodA(obj, method, params);
                                }
                                break;
                            default:
                                ERROR_PRINT("无法识别的return类型%d", descriptor_stream->return_element_type());
                                exit(-1);
                        }
                    } else {

                    }
                }
                break;
            case RETURN:
                INFO_PRINT("执行指令: return，该指令功能为: 从方法中返回void，恢复调用者的栈帧，并且把程序的控制权交回调用者");
                // pop出栈帧
                current_thread->pop_frame();
                INFO_PRINT("剩余栈帧数量: %d", current_thread->frame_size());
                break;
            default:
                ERROR_PRINT("not bytecode");
                exit(-1);
        }
    }
}
