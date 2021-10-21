//
// Created by xyzjiao on 10/21/21.
//

#ifndef JVMPLUS_INSTANCEMIRRORKLASS_HPP
#define JVMPLUS_INSTANCEMIRRORKLASS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/oopsHierachy.hpp"
#include "../../../share/vm/oops/instanceKlass.hpp"
#include "../../../share/vm/runtime/handles.hpp"

class InstanceMirrorKlass : public InstanceKlass {
private:

public:
    static InstanceMirrorKlass* cast(Klass* k) {
        return (InstanceMirrorKlass*) k;
    }


    static HeapWord* start_of_static_fields(oop obj) {

    }

    int instance_size(KlassHandle k);

    instanceOop allocate_instance(KlassHandle k);
};


#endif //JVMPLUS_INSTANCEMIRRORKLASS_HPP
