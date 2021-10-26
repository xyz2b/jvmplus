//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_KLASS_HPP
#define JVMPLUS_KLASS_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/oops/metadata.hpp"
#include "../../../share/vm/oops/oopsHierachy.hpp"

class Klass : public Metadata {
private:
    oop _java_mirror;


public:
    Klass() {}
    Klass(oop java_mirror) { _java_mirror = java_mirror; }
    oop java_mirror() const { return _java_mirror; }
    void set_java_mirror(oop m) { _java_mirror = m; }
};


#endif //JVMPLUS_KLASS_HPP
