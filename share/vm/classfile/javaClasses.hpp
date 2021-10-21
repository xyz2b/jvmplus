//
// Created by xyzjiao on 10/21/21.
//

#ifndef JVMPLUS_JAVACLASSES_HPP
#define JVMPLUS_JAVACLASSES_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/memory/alloction.hpp"
#include "../../../share/vm/runtime/handles.hpp"

class java_lang_Class : public AllStatic {
private:

public:
    static void create_mirror(KlassHandle k);
};


#endif //JVMPLUS_JAVACLASSES_HPP
