//
// Created by ziya on 2021/10/14.
//

#ifndef JVMPLUS_BOOTSTRAPMETHODS_HPP
#define JVMPLUS_BOOTSTRAPMETHODS_HPP

#include "../../../share/vm/oops/attribute.hpp"
#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/utilities/array.hpp"

class BootstrapMethod : public Metadata {
private:
    u2 _bootstrap_method_ref;
    u2 _num_of_bootstrap_arguments;
    Array<u2>* _bootstrap_arguments;
public:
    BootstrapMethod(u2 bootstrap_method_ref, u2 num_of_bootstrap_arguments): _bootstrap_method_ref(bootstrap_method_ref), _num_of_bootstrap_arguments(num_of_bootstrap_arguments) {}

    u2 get_bootstrap_method_ref() { return _bootstrap_method_ref; }
    u2 get_num_of_bootstrap_arguments() { return _num_of_bootstrap_arguments; }

    void set_bootstrap_arguments(Array<u2>* bootstrap_arguments) { _bootstrap_arguments = bootstrap_arguments; }
    u2 get_bootstrap_argument(int index) { return _bootstrap_arguments->get(index); }
    void add_bootstrap_argument(u2 bootstrap_argument) { _bootstrap_arguments->add(bootstrap_argument); }
};

class BootstrapMethods : public AttributeInfo {
private:
    u2 _number_of_bootstrap_methods;
    Array<BootstrapMethod*>* _bootstrap_methods;
public:
    BootstrapMethods(u2 attribute_name_index, u4 attribute_length, u2 number_of_bootstrap_methods) :AttributeInfo(attribute_name_index, attribute_length), _number_of_bootstrap_methods(number_of_bootstrap_methods) {}

    void set_bootstrap_methods(Array<BootstrapMethod*>* bootstrap_methods) { _bootstrap_methods = bootstrap_methods; }
    BootstrapMethod* get_bootstrap_method(int index) { return _bootstrap_methods->get(index); }
    void add_bootstrap_method(BootstrapMethod* bootstrap_method) { _bootstrap_methods->add(bootstrap_method); }
};

#endif //JVMPLUS_BOOTSTRAPMETHODS_HPP
