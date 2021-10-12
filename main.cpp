#include "share/vm/classfile/classLoader.hpp"
#include "share/vm/oops/symbol.hpp"

int main() {

    const char* file = "/home/xyzjiao/Desktop/project/jvm/target/classes/org/xyz/jvm/example/HelloWorld";
    Symbol* s = new (strlen(file)) Symbol((u1*)file, strlen(file));

    ClassLoader* classLoader = new ClassLoader();
    InstanceKlassHandle instanceKlassHandle = classLoader->load_classfile(s);



    return 0;
}
