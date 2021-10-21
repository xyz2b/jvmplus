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
    static Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* _dictionary;
public:
    static Klass* resolve_or_null(Symbol* class_name);
    static Klass* resolve_instance_class_or_null(Symbol *class_name);

    static Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* dictionary() { return _dictionary; }
    static void set_dictionary(Hashmap<Symbol*, Klass*, HashCode<const Symbol*>>* dictionary) { _dictionary = dictionary; }
};


#endif //JVMPLUS_SYSTEMDICTIONARY_H
