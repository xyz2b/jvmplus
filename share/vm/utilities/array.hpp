//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_ARRAY_HPP
#define JVMPLUS_ARRAY_HPP
#include <stdlib.h>
#include <cstring>
#include "constantTag.hpp"

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
    T get_at(size_t index);
    T remove(size_t index);
    void set(size_t index, T value);
    void set_at(size_t index, T value);

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
    T* get_at(size_t index);
    T* remove(size_t index);
    void set(size_t index, T* value);
    void set_at(size_t index, T* value);

    void insertLast(T* value);
    void insertFirst(T* value);
};

template<typename T>
Array<T>::Array(): _size(0), _capacity(initCapacity) {
    _data = new T[initCapacity];
}

template<typename T>
Array<T>::~Array() {
    if(_data != nullptr) {
        delete [] _data;
        _data = nullptr;
    }
}

template<typename T>
Array<T>::Array(const Array<T> *array) {
    _data = new T[array->_size];

    for (int i = 0; i < array->_size; ++i) {
        *(_data + i) = array->get(i);
    }
}

template<typename T>
Array<T> * Array<T>::operator=(const Array<T> *array) {
    if (_data != nullptr) {
        delete [] _data;
        _data = nullptr;
    }

    _data = new T[array->_size];

    for (int i = 0; i < array->_size; ++i) {
        *(_data + i) = array->get(i);
    }

    return this;
}

template<typename T>
Array<T>::Array(size_t capacity): _size(0), _capacity(capacity) {
    _data = new T[capacity];
}

template<typename T>
void Array<T>::insert(size_t index, T value) {
    if(index < 0 || index > _size)
        // index索引非法
        exit(-1);

    if (_size == _capacity)
        resize(2 * _capacity);

    *(_data + index) = value;

    _size++;

}

template<typename T>
void Array<T>::resize(size_t newCapacity) {
    T* newArray = new T[newCapacity];

//    memcpy(newArray, _data, (_capacity) * sizeof(T));

    for (int i = 0 ; i < _capacity ; ++i) {
        *(newArray + i) = *(_data + i);
    }

    delete [] _data;

    _data = newArray;
    _capacity = newCapacity;
}


template<typename T>
T Array<T>::get(size_t index) {
    if(index < 0 || index >= _size)
        // index索引非法
        exit(-1);

    return *(_data + index);
}

template<typename T>
T Array<T>::get_at(size_t index) {
    return *(_data + index);
}

template<typename T>
T Array<T>::remove(size_t index) {
    if(index < 0 || index >= _size)
        // index索引非法
        exit(-1);

    T ret = *(_data + index);

    delete (_data + index);

    void *src = (char*)_data + (index + 1) * sizeof(T);
    void *dst = (char*)_data + index * sizeof(T);

    memmove(dst, src, (_size - (index + 1)) * sizeof(T));

    _size--;

    if (_size == _capacity / 4 && _capacity /2 != 0)
        resize(_capacity / 2);

    return ret;
}

template<typename T>
void Array<T>::set(size_t index, T value) {
    if(index < 0 || index >= _size)
        // index索引非法
        exit(-1);

    *(_data + index) = value;
}

template<typename T>
void Array<T>::set_at(size_t index, T value) {
    *(_data + index) = value;
}

template<typename T>
void Array<T>::insertLast(T value) {
    insert(_size, value);
}

template<typename T>
void Array<T>::insertFirst(T value) {
    insert(0, value);
}




template<typename T>
Array<T*>::Array(): _size(0), _capacity(initCapacity) {
    _data = new T*[initCapacity];
}

template<typename T>
Array<T*>::~Array() {
    if(_data != nullptr) {
        delete [] _data;
        _data = nullptr;
    }
}

template<typename T>
Array<T*>::Array(const Array<T*> *array) {
    _data = new T*[array->_size];

    for (int i = 0; i < array->_size; ++i) {
        *(_data + i) = array->get(i);
    }
}

template<typename T>
Array<T*> * Array<T*>::operator=(const Array<T*> *array) {
    if (_data != nullptr) {
        delete [] _data;
        _data = nullptr;
    }

    _data = new T*[array->_size];

    for (int i = 0; i < array->_size; ++i) {
        *(_data + i) = array->get(i);
    }

    return this;
}

template<typename T>
Array<T*>::Array(size_t capacity): _size(0), _capacity(capacity) {
    _data = new T*[capacity];
}

template<typename T>
void Array<T*>::insert(size_t index, T* value) {
    if(index < 0 || index > _size)
        // index索引非法
        exit(-1);

    if (_size == _capacity)
        resize(2 * _capacity);

    *(_data + index) = value;

    _size++;

}

template<typename T>
void Array<T*>::resize(size_t newCapacity) {
    T** newArray = new T*[newCapacity];

//    memcpy(newArray, _data, (_capacity) * sizeof(T));

    for (int i = 0 ; i < _capacity ; ++i) {
        *(newArray + i) = new T(*(*(_data + i)));
    }

    delete [] _data;

    _data = newArray;
    _capacity = newCapacity;
}


template<typename T>
T* Array<T*>::get(size_t index) {
    if(index < 0 || index >= _size)
        // index索引非法
        exit(-1);

    return *(_data + index);
}

template<typename T>
T* Array<T*>::get_at(size_t index) {
    return *(_data + index);
}

template<typename T>
T* Array<T*>::remove(size_t index) {
    if(index < 0 || index >= _size)
        // index索引非法
        exit(-1);

    T* e = *(_data + index);

    T* ret = new T(*e);

    delete (_data + index);

    void *src = (char*)_data + (index + 1) * sizeof(T);
    void *dst = (char*)_data + index * sizeof(T);

    memmove(dst, src, (_size - (index + 1)) * sizeof(T));

    _size--;

    if (_size == _capacity / 4 && _capacity /2 != 0)
        resize(_capacity / 2);

    return ret;
}

template<typename T>
void Array<T*>::set(size_t index, T* value) {
    if(index < 0 || index >= _size)
        // index索引非法
        exit(-1);

    *(_data + index) = value;
}

template<typename T>
void Array<T*>::set_at(size_t index, T* value) {
    *(_data + index) = value;
    _size++;
}

template<typename T>
void Array<T*>::insertLast(T* value) {
    insert(_size, value);
}

template<typename T>
void Array<T*>::insertFirst(T* value) {
    insert(0, value);
}


#endif //JVMPLUS_ARRAY_HPP
