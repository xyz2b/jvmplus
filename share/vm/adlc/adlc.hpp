//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_ADLC_HPP
#define JVMPLUS_ADLC_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

#undef assert
#define assert(cond, msg) { if (!(cond)) { fprintf(stderr, "assert fails %s %d: %s\n", __FILE__, __LINE__, msg); abort(); }}

#endif //JVMPLUS_ADLC_HPP
