#include "share/vm/classfile/classLoader.hpp"
#include "share/vm/oops/symbol.hpp"

int main() {

    const char* file = "/home/ziya/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
    Symbol* s = new (strlen(file)) Symbol((u1*)file, strlen(file));

    ClassLoader* classLoader = new ClassLoader();
    InstanceKlassHandle instanceKlassHandle = classLoader->load_classfile(s);
    INFO_PRINT("version: %d", instanceKlassHandle->get_major_version());

    return 0;
}
