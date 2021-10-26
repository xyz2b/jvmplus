//
// Created by xyzjiao on 8/7/21.
//

#ifndef JVM_C___HASHMAP_HPP
#define JVM_C___HASHMAP_HPP

#include <cstdlib>
#include <iostream>
#include "hashFun.hpp"
#include <map>
#include <vector>
#include <cstddef>
#include "array.hpp"

using std::size_t;
using std::map;

// overwrite map operator new function --> allocate map memory in metaspace
// the operation of allocate memory in map, is can't allocate in metaspace
template<class K, class V>
class Map : public map<K, V> {
public:
    void* operator new (size_t size) {
        return Metaspace::allocate(size)->value();
    }
    void operator delete(void* p) {}
};

template <class K, class V, class HashFn = HashCode<K>>
class Hashmap : public MetaspaceObj {
private:
    const size_t upperTol = 10;
    const size_t lowerTol = 2;
    const size_t initCapacity = 7;
    Array<Map<K, V>*>* _hashtable; // map<K, V>* _hashtable[1];
    size_t _size;
    size_t  _M;

public:
    Hashmap(size_t M);
    Hashmap();
    ~Hashmap();
    Hashmap(const Hashmap<K, V>* hashMap);
    Hashmap<K, V>* operator= (const Hashmap<K, V>* hashMap);

    size_t size();
    void put(K key, V value);
    V get(K key);
    V remove(K key);
    bool contains(K key);

private:
    size_t _hash(K key);
    void resize(size_t newM);
};

template<class K, class V, class HashFn >
Hashmap<K, V>* Hashmap<K, V, HashFn>::operator=(const Hashmap<K, V> *hashMap) {
    if (_hashtable != nullptr) {
        delete _hashtable;
        _hashtable = nullptr;
    }

    _hashtable = new Array<map<K, V>*>(hashMap->_M);
    for (int i = 0 ; i < hashMap->_M; ++i) {
        Map<K, V>* m = hashMap->get(i);
        for(auto iterator = m->begin(); iterator != m->end(); iterator++) {
            Map<string, string>* newMap = _hashtable->get(i);
            (*newMap)[iterator->first] = iterator->second;
        }
    }

    return this;
}

template<class K, class V, class HashFn >
Hashmap<K, V, HashFn>::~Hashmap() {
    if (_hashtable != nullptr) {
        delete _hashtable;
        _hashtable = nullptr;
    }
}

template<class K, class V, class HashFn >
Hashmap<K, V, HashFn>::Hashmap(const Hashmap<K, V> *hashMap) {
    _hashtable = new (hashMap->_M) Array<Map<K, V>*>(hashMap->_M);
    for (int i = 0 ; i < hashMap->_M; ++i) {
        Map<K, V>* m = hashMap->get(i);
        for(auto iterator = m->begin(); iterator != m->end(); iterator++) {
            Map<string, string>* newMap = _hashtable->get(i);
            (*newMap)[iterator->first] = iterator->second;
        }
    }
}

template<class K, class V, class HashFn >
Hashmap<K, V, HashFn>::Hashmap(size_t M): _M(M), _size(0) {
    _hashtable = new (M) Array<Map<K, V>*>(M);
    for (int i = 0 ; i < M ; ++i) {
        Map<K, V>* _map = new Map<K, V>();
        _hashtable->insert(i, _map);
    }
}

template<class K, class V, class HashFn >
Hashmap<K, V, HashFn>::Hashmap(): _M(initCapacity), _size(0) {
    _hashtable = new (initCapacity) Array<Map<K, V>*>(initCapacity);
    for (int i = 0 ; i < initCapacity ; ++i) {
        _hashtable->insert(i, new Map<K, V>());
    }
}

template<class K, class V, class HashFn>
size_t Hashmap<K, V, HashFn>::_hash(K key) {
    return HashFn()(key) % _M;
}

template<class K, class V, class HashFn>
size_t Hashmap<K, V, HashFn>::size() {
    return _size;
}

template<class K, class V, class HashFn>
void Hashmap<K, V, HashFn>::put(K key, V value) {
    size_t i = _hash(key);
    Map<K, V>* m = _hashtable->get(i);

    if (m->find(key) != m->end()) {
        (*m)[key] = value;
    } else {
        (*m)[key] = value;
        _size++;

        if (_size >= upperTol * _M) {
            resize(2 * _M);
        }
    }
}

// get前需要使用contains判断是否存在，否则行为未定义
template<class K, class V, class HashFn>
V Hashmap<K, V, HashFn>::get(K key) {
    size_t i = _hash(key);
    Map<K, V>* m = _hashtable->get(i);
    if (m->find(key) != m->end()) {
        return (*m)[key];
    }
    return nullptr;
}

template<class K, class V, class HashFn>
V Hashmap<K, V, HashFn>::remove(K key) {
    Map<K, V>* m = _hashtable->get(_hash(key));
    auto i = m->find(key);
    if (i != m->end()) {
        V ret = (*m)[key];
        m->erase(i);
        _size--;

        if (_size < lowerTol * _M && _M / 2 >= initCapacity) {
            resize(_M / 2);
        }

        return ret;
    }
}

template<class K, class V, class HashFn>
bool Hashmap<K, V, HashFn>::contains(K key) {
    Map<K, V>* m = _hashtable->get(_hash(key));
    if (m->find(key) != m->end()) {
        return true;
    } else {
        return false;
    }
}

template<class K, class V, class HashFn>
void Hashmap<K, V, HashFn>::resize(size_t newM) {
    Array<Map<K, V>*>* newHashTable = new (newM) Array<Map<K, V>*>(newM);
    for (int i = 0 ; i < newM ; ++i) {
        newHashTable->insert(i, new Map<K, V>());
    }

    size_t oldM = _M;
    _M = newM;

    for (int i = 0 ; i < oldM ; ++i) {
        Map<K, V>* m = _hashtable->get(i);
        for(auto iterator = m->begin(); iterator != m->end(); iterator++) {
            Map<K, V>* newMap = newHashTable->get(_hash(iterator->first));
            (*newMap)[iterator->first] = iterator->second;
        }
    }

    delete _hashtable;

    _hashtable = newHashTable;
}

#endif //JVM_C___HASHMAP_HPP
