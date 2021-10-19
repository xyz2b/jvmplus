//
// Created by xyzjiao on 10/18/21.
//

#ifndef JVMPLUS_SYSTEMDICTIONARY_H
#define JVMPLUS_SYSTEMDICTIONARY_H

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/klass.hpp"
#include "../../../share/vm/oops/symbol.hpp"

class SystemDictionary {
public:
    static Klass* resolve_or_null(Symbol* class_name);
};


#endif //JVMPLUS_SYSTEMDICTIONARY_H
