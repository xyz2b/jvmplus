#include "share/vm/classfile/classLoader.hpp"
#include "share/vm/oops/symbol.hpp"
#include "share/vm/memory/universe.hpp"
#include "share/vm/prims/JavaNativeInterface.hpp"
#include "share/vm/classfile/systemDictionary.h"

int main() {
    // 初始化堆内存以及元空间
    Universe::initialize_heap();
    Metaspace::initialize();
    SystemDictionary::set_dictionary(new Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>());

    const char* file = "/home/xyzjiao/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
    Symbol* s = new (strlen(file)) Symbol(file, strlen(file));


    ClassLoader* classLoader = new ClassLoader();
    InstanceKlassHandle instanceKlassHandle = classLoader->load_classfile(s);
    INFO_PRINT("version: %d", instanceKlassHandle->get_major_version());

    u2 major_version = ((InstanceKlass*) instanceKlassHandle->get_methods()->get_at(0)->get_belong_klass())->get_major_version();
    INFO_PRINT("%d", major_version);

    const char* method = "main";
    const char* descriptor = "([Ljava/lang/String;)V";
    Symbol* method_n = new (strlen(method)) Symbol(method, strlen(method));
    Symbol* descriptor_n = new (strlen(descriptor)) Symbol(descriptor, strlen(descriptor));

    Method* method_info = JavaNativeInterface::get_method(instanceKlassHandle(), method_n, descriptor_n);

    instanceKlassHandle->java_mirror();
    INFO_PRINT("%d", instanceKlassHandle->get_fields()->get(0)->offset());
    INFO_PRINT("%d", instanceKlassHandle->get_fields()->get(1)->offset());
    INFO_PRINT("%d", instanceKlassHandle->get_fields()->get(2)->offset());
    INFO_PRINT("%d", instanceKlassHandle->get_fields()->get(3)->offset());
    return 0;
}
