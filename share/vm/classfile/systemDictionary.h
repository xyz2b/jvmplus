//
// Created by xyzjiao on 10/18/21.
//

#ifndef JVMPLUS_SYSTEMDICTIONARY_H
#define JVMPLUS_SYSTEMDICTIONARY_H

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/oops/symbol.hpp"
#include "../../../share/vm/utilities/hashMap.hpp"

class SystemDictionary {
private:
    // 该类加载器的加载路径，多个路径以分号分隔，只有一个的话，分号可省略。注意路径后面的斜杠不可丢
    static Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* _dictionary;
public:
    static Klass* resolve_or_null(Symbol* class_name);
    static Klass* resolve_instance_class_or_null(Symbol *class_name);
    static void load_super_class(Klass* klass);

    static Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* dictionary() { return _dictionary; }
    static void set_dictionary(Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* dictionary) { _dictionary = dictionary; }
};


#endif //JVMPLUS_SYSTEMDICTIONARY_H
