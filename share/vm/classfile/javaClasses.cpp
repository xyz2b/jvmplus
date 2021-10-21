//
// Created by xyzjiao on 10/21/21.
//

#include "javaClasses.hpp"
#include "../../../share/vm/oops/InstanceMirrorKlass.hpp"

void java_lang_Class::create_mirror(KlassHandle k) {
    assert(k->java_mirror() == nullptr, "should only assign mirror once");

    Handle mirror = InstanceMirrorKlass::cast(k())->allocate_instance(k);

    k->set_java_mirror(mirror());
}
