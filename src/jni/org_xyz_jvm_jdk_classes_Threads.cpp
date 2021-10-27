//
// Created by xyzjiao on 8/2/21.
//

#include "../../include/jni/org_xyz_jvm_jdk_classes_Threads.h"
#include "../../share/vm/memory/universe.hpp"
#include "../../share/vm/memory/metaspace.hpp"
#include "../../share/vm/classfile/systemDictionary.h"
#include "../../share/vm/runtime/javaThread.hpp"
#include "../../share/vm/runtime/Threads.hpp"

JNIEnv* g_env;

/*
 * Class:     org_xyz_jvm_jdk_Threads
 * Method:    createVm
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_xyz_jvm_jdk_classes_Threads_createVm
        (JNIEnv *env, jclass clazz) {
    g_env = env;
    Universe::initialize_heap();
    Metaspace::initialize();
    SystemDictionary::set_dictionary(new Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>());

    // 创建线程，此处仅为模拟
    JavaThread* thread = new JavaThread();

    Threads::set_threads(new Array<Thread*>());
    // 将新创建的线程存放到线程管理器中
    Threads::add_thread(thread);
    // 设置线程管理器的当前线程
    Threads::set_current_thread(thread);
}