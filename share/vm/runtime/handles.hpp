//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_HANDLES_HPP
#define JVMPLUS_HANDLES_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"
#include "../../../share/vm/adlc/adlc.hpp"

class Klass;
class InstanceKlass;
class Method;
class ConstantPool;

class KlassHandle {
    Klass*      _value;
protected:
    Klass* obj() const { return _value; }
    Klass* non_null_obj() const { assert(_value != nullptr, "resolving NULL _value"); return _value; }
public:
    KlassHandle() : _value(nullptr) {}
    KlassHandle(const Klass* obj) : _value(const_cast<Klass *>(obj)) {};

    Klass* operator() () const { return obj(); }
    Klass* operator-> () const { return non_null_obj(); }

    bool operator== (Klass* o) const { return obj() == o; }
    bool operator== (const KlassHandle& h) const { return obj() == h.obj(); }

    bool is_null() const { return _value == NULL; }
    bool not_null() const { return _value != NULL; }
};

class InstanceKlassHandle : public KlassHandle {
public:
    InstanceKlassHandle() : KlassHandle() {}
    InstanceKlassHandle(const Klass* k) : KlassHandle(k) {
        assert(k != nullptr, "illegal type");
    }

    InstanceKlass* operator() () const { return (InstanceKlass*)obj(); }
    InstanceKlass* operator-> () const { return (InstanceKlass*)obj(); }

};

#define DEF_METADATA_HANDLE(name, type)          \
  class name##Handle;                            \
  class name##Handle {         \
    type*     _value;                            \
  protected:                                     \
    type* obj() const { return _value; }        \
    type* non_null_obj() const { assert(_value != nullptr, "resolving NULL _value"); return _value; }    \
  public:                                        \
    name##Handle () : _value(nullptr) {}                            \
    name##Handle (type* obj) : _value(obj) {}    \
                                                 \
    type* operator() () const { return obj(); }  \
    type* operator-> () const { return non_null_obj(); }                                                 \
                                                 \
    bool operator== (type* o) const { return obj() == o; } \
    bool operator== (const name##Handle& h) const { return obj() == h.obj(); } \
                                                 \
    bool is_null() const { return _value == NULL; } \
    bool not_null() const { return _value != NULL; }                                             \
  };

DEF_METADATA_HANDLE(Method, Method)
DEF_METADATA_HANDLE(ConstantPool, ConstantPool)

#endif //JVMPLUS_HANDLES_HPP
