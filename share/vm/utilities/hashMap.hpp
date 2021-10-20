//
// Created by xyzjiao on 8/7/21.
//

#ifndef JVM_C___HASHMAP_HPP
#define JVM_C___HASHMAP_HPP

#include <cstdlib>
#include "hashFun.hpp"
#include <map>
#include <vector>
#include <cstddef>
#include "array.hpp"

using std::size_t;
using std::map;

template <class K, class V, class HashFn = HashCode<K>>
class Hashmap : public MetaspaceObj {
private:
    const size_t upperTol = 10;
    const size_t lowerTol = 2;
    const size_t initCapacity = 7;
    Array<map<K, V>*>* _hashtable; // map<K, V>* _hashtable[1];
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
        map<K, V>* m = hashMap->get(i);
        for(auto iterator = m->begin(); iterator != m->end(); iterator++) {
            map<string, string>* newMap = _hashtable->get(i);
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
    _hashtable = new (hashMap->_M) Array<map<K, V>*>(hashMap->_M);
    for (int i = 0 ; i < hashMap->_M; ++i) {
        map<K, V>* m = hashMap->get(i);
        for(auto iterator = m->begin(); iterator != m->end(); iterator++) {
            map<string, string>* newMap = _hashtable->get(i);
            (*newMap)[iterator->first] = iterator->second;
        }
    }
}

template<class K, class V, class HashFn >
Hashmap<K, V, HashFn>::Hashmap(size_t M): _M(M), _size(0) {
    _hashtable = new (M) Array<map<K, V>*>(M);
    for (int i = 0 ; i < M ; ++i) {
        map<K, V>* _map = new map<K, V>();
        _hashtable->insert(i, _map);
    }
}

template<class K, class V, class HashFn >
Hashmap<K, V, HashFn>::Hashmap(): _M(initCapacity), _size(0) {
    _hashtable = new (initCapacity) Array<map<K, V>*>(initCapacity);
    for (int i = 0 ; i < initCapacity ; ++i) {
        _hashtable->insert(i, new map<K, V>());
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
    map<K, V>* m = _hashtable->get(i);

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
    map<K, V>* m = _hashtable->get(_hash(key));
    if (m->find(key) != m->end()) {
        return (*m)[key];
    }
}

template<class K, class V, class HashFn>
V Hashmap<K, V, HashFn>::remove(K key) {
    map<K, V>* m = _hashtable->get(_hash(key));
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
    map<K, V>* m = _hashtable->get(_hash(key));
    if (m->find(key) != m->end()) {
        return true;
    } else {
        return false;
    }
}

template<class K, class V, class HashFn>
void Hashmap<K, V, HashFn>::resize(size_t newM) {
    Array<map<K, V>*>* newHashTable = new (newM) Array<map<K, V>*>(newM);
    for (int i = 0 ; i < newM ; ++i) {
        newHashTable->insert(i, new map<K, V>());
    }

    size_t oldM = _M;
    _M = newM;

    for (int i = 0 ; i < oldM ; ++i) {
        map<K, V>* m = _hashtable->get(i);
        for(auto iterator = m->begin(); iterator != m->end(); iterator++) {
            map<K, V>* newMap = newHashTable->get(_hash(iterator->first));
            (*newMap)[iterator->first] = iterator->second;
        }
    }

    delete _hashtable;

    _hashtable = newHashTable;
}

#endif //JVM_C___HASHMAP_HPP
