//
// Created by xyzjiao on 8/7/21.
//

#ifndef JVM_C___HASHFUN_HPP
#define JVM_C___HASHFUN_HPP
#include <cstdlib>
#include <string>
#include "../../../share/vm/oops/symbol.hpp"

using std::size_t;
using std::string;

// 等同于hash类（class hash {};）
template<class K>
struct HashCode { };

// 仿函数
template<>
struct HashCode<string> {
    size_t
    operator()(const string& s) const {
        unsigned long h = 0;
        for (char c : s) {
            h = 5 * h + c;
        }
        return size_t(h);
    };
};

inline size_t
hash_string(const char* s)
{
    unsigned long h = 0;
    for ( ; *s; ++s)
        h = 5 * h + *s;
    return size_t(h);
}

template<>
struct HashCode<char*>
{
    size_t
    operator()(const char* s) const
    { return hash_string(s); }
};

template<>
struct HashCode<const char*>
{
    size_t
    operator()(const char* s) const
    { return hash_string(s); }
};

template<>
struct HashCode<const Symbol*>
{
    size_t
    operator()(const Symbol* s) const
    { return hash_string(s->as_C_string()); }
};

template<>
struct HashCode<char>
{
    size_t
    operator()(char x) const
    { return x; }
};

template<>
struct HashCode<unsigned char>
{
    size_t
    operator()(unsigned char x) const
    { return x; }
};

template<>
struct HashCode<signed char>
{
    size_t
    operator()(unsigned char x) const
    { return x; }
};

template<>
struct HashCode<short>
{
    size_t
    operator()(short x) const
    { return x; }
};

template<>
struct HashCode<unsigned short>
{
    size_t
    operator()(unsigned short x) const
    { return x; }
};

template<>
struct HashCode<int>
{
    size_t
    operator()(int x) const
    { return x; }
};

template<>
struct HashCode<unsigned int>
{
    size_t
    operator()(unsigned int x) const
    { return x; }
};

template<>
struct HashCode<long>
{
    size_t
    operator()(long x) const
    { return x; }
};

template<>
struct HashCode<unsigned long>
{
    size_t
    operator()(unsigned long x) const
    { return x; }
};

template<>
struct HashCode<float>
{
    size_t
    operator()(float x) const
    {return (int) x;}
};

template<>
struct HashCode<double>
{
    size_t
    operator()(double x) const
    {return (long) x;}
};

#endif //JVM_C___HASHFUN_HPP
