#include "share/vm/classfile/classLoader.hpp"
#include "share/vm/oops/symbol.hpp"
#include "share/vm/memory/universe.hpp"
#include "share/vm/prims/JavaNativeInterface.hpp"
#include "share/vm/classfile/systemDictionary.h"
#include "../../../share/vm/runtime/Threads.hpp"

int main() {
    // double 转 long 再转double
//    double d = 2.1;
//    long l = *((long*)(&d));
//    double d2 = *(double *)&l;

//    jdouble double_val = 1.1;
//    jobject field_val = nullptr;
//    *((jdouble*)(&field_val)) = double_val;
//    jdouble double_val2 = *((jdouble*)(&field_val));


    // 初始化堆内存以及元空间
    Universe::initialize_heap();
    Metaspace::initialize();
    SystemDictionary::set_dictionary(new Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>);

//    Array<int>* a = new Array<int>();
//
//    for (int i = 0; i < 15; i++) {
//        a->add(i);
//    }
//    int r = a->get(7);
//    int st = a->size();
//    int c = a->capacity();
//    int rp = a->peek();
//    int rs = a->pop();
//    for(int i = 0; i < 10; i++) {
//        int fs = a->pop();
//    }
//    int d = a->peek();

//    Hashmap<int ,int>* hp = new Hashmap<int ,int>();
//    for (int i = 0; i < 8; i++) {
//        hp->put(i, i);
//    }
//    int r = hp->get(7);
//    int ht = hp->size();


//    const char* class_file = "/home/xyzjiao/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
//    Symbol* class_files = new (strlen(class_file)) Symbol(class_file, strlen(class_file));
//
//    Hashmap<Symbol*, jobject, HashCode<const Symbol*>>* dictionary = new Hashmap<Symbol*, jobject, HashCode<const Symbol*>>();
//
//    dictionary->put(class_files, (jobject)1);
//
//
//    const char* class_file2 = "/home/xyzjiao/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
//    Symbol* class_files2 = new (strlen(class_file2)) Symbol(class_file2, strlen(class_file2));
//
//    jobject  i = dictionary->get(class_files2);

    // 创建线程，此处仅为模拟
    JavaThread* thread = new JavaThread();

    // 将新创建的线程存放到线程管理器中
    Threads::set_threads(new Array<Thread*>());
    Threads::add_thread(thread);
    // 设置线程管理器的当前线程
    Threads::set_current_thread(thread);

    const char* file = "org/xyz/jvm/example/HelloWorld";
    Symbol* s = new (strlen(file)) Symbol(file, strlen(file));

//    ClassLoader* classLoader = new ClassLoader();
//    InstanceKlassHandle instanceKlassHandle = classLoader->load_classfile(s);
//    INFO_PRINT("version: %d", instanceKlassHandle->get_major_version());

    InstanceKlass* ins = (InstanceKlass*)SystemDictionary::resolve_or_null(s);

//    const char* class_file = "/home/xyzjiao/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
//    Symbol* class_files = new (strlen(class_file)) Symbol(class_file, strlen(class_file));
//    INFO_PRINT("%p", SystemDictionary::dictionary())
//    Klass* r = SystemDictionary::dictionary()->get(class_files);
//    int i = SystemDictionary::dictionary()->size();

//    SystemDictionary::resolve_or_null(s);



//    u2 major_version = ((InstanceKlass*) instanceKlassHandle->get_methods()->get_at(0)->get_belong_klass())->get_major_version();
//    INFO_PRINT("%d", major_version);
//
    const char* method = "main";
    const char* descriptor = "([Ljava/lang/String;)V";
    Symbol* method_n = new (strlen(method)) Symbol(method, strlen(method));
    Symbol* descriptor_n = new (strlen(descriptor)) Symbol(descriptor, strlen(descriptor));

    Method* method_info = JavaNativeInterface::get_method(ins, method_n, descriptor_n);

//    instanceKlassHandle->java_mirror();
//
    JavaNativeInterface::call_static_method(ins, method_info);
    return 0;
}
