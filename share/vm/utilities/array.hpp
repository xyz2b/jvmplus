//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_ARRAY_HPP
#define JVMPLUS_ARRAY_HPP
#include <stdlib.h>
#include <cstring>

template <typename T>
class Array {
private:
    const size_t initCapacity = 7;

    size_t     _size;        // the number of array elements
    T*         _data;       // the array memory
    size_t     _capacity;
public:
    Array();
    ~Array();
    Array(size_t capacity);
    Array(const Array<T> *array);
    Array* operator=(const Array<T> *array);
    void insert(size_t index, T value);
    void resize(size_t newCapacity);
    T get(size_t index);
    T remove(size_t index);
    void set(size_t index, T value);

    void insertLast(T value);
    void insertFirst(T value);
};

// 特化版本，模板类型为指针类型
template <typename T>
class Array<T*> {
private:
    const size_t initCapacity = 7;

    size_t     _size;        // the number of array elements
    T**         _data;       // the array memory
    size_t     _capacity;
public:
    Array();
    ~Array();
    Array(size_t capacity);
    Array(const Array<T*> *array);
    Array* operator=(const Array<T*> *array);
    void insert(size_t index, T* value);
    void resize(size_t newCapacity);
    T* get(size_t index);
    T* remove(size_t index);
    void set(size_t index, T* value);

    void insertLast(T* value);
    void insertFirst(T* value);
};


#endif //JVMPLUS_ARRAY_HPP
