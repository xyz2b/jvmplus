#include "share/vm/classfile/classLoader.hpp"
#include "share/vm/oops/symbol.hpp"
#include "share/vm/memory/universe.hpp"

int main() {

    const char* file = "/home/xyzjiao/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
    Symbol* s = new (strlen(file)) Symbol((u1*)file, strlen(file));

    // 初始化堆内存以及元空间
    Universe::initialize_heap();
    Metaspace::initialize();

    ClassLoader* classLoader = new ClassLoader();
    InstanceKlassHandle instanceKlassHandle = classLoader->load_classfile(s);
    INFO_PRINT("version: %d", instanceKlassHandle->get_major_version());

    u2 major_version = ((InstanceKlass*) instanceKlassHandle->get_methods()->get_at(0)->get_belong_klass())->get_major_version();
    INFO_PRINT("%d", major_version);
    return 0;
}
