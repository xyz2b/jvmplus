#include "share/vm/classfile/classLoader.hpp"
#include "share/vm/oops/symbol.hpp"

int main() {

    const char* file = "/home/ziya/IdeaProjects/java-research/target/classes/org/xyz/test/HelloWorld";
    Symbol* s = new (strlen(file)) Symbol((u1*)file, strlen(file));

    ClassLoader* classLoader = new ClassLoader();
    InstanceKlassHandle instanceKlassHandle = classLoader->load_classfile(s);



    return 0;
}
