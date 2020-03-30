//
// Created by tom on 30.03.20.
//

#include <iostream>
#include <cstring>
#include <cassert>
#include "mySet.h"

int cmpfunc (const void * a, const void * b) {
    const char * l = *(const char **)a;
    const char * r = *(const char **)b;
    int res = strcmp(l, r);

    std::cout << "> left: " << l << ", right: " << r << " | res = " << res << std::endl;

    return res;
}

template<typename T>
mySet<T>::mySet() {
    const int INIT_SIZE = 2;
    items = new T[INIT_SIZE];
    size = INIT_SIZE;
    occupied = 0;
}

template<typename T>
bool mySet<T>::insert(T item) {
    if (occupied == size)
        resize();

    if (idExist(item))
        return false;

    items[occupied] = item;
    occupied ++;
    qsort(items, occupied, sizeof(T), cmpfunc);

    return true;
}

template<typename T>
mySet<T>::~mySet() {
    delete [] items;
}

template<typename T>
T mySet<T>::operator[](int index) {
    return items[index];
}

template<typename T>
void mySet<T>::resize() {
    auto * tmp = new T[2 * size];

    for (int i = 0; i < size; i++)
        tmp[i] = items[i];

    delete [] items;
    items = tmp;
    size *= 2;
}

template<typename T>
bool mySet<T>::idExist(const char *id) {
    if (occupied == 0)
        return false;
    return findId(id, 0, occupied) != nullptr;
}

template<typename T>
T mySet<T>::findId(const char *id, int from, int to) {
    if (to < from)
        return nullptr;
    int mid = (from + to) / 2;
    int comparison = strcmp(id, items[mid]);
    if (comparison == 0)
        return items[mid];
    if (comparison > 0)
        return findId(id, (mid + 1), to);
    return findId(id, from, (mid - 1));
}


int main (){
    /*mySet <int> a;
    a.insert(5);
    a.insert(6);
    a.insert(500);

    std::cout << a[0] << a[1] << a[2] << std::endl;*/

    mySet <const char *> a;
    a.insert("q");
    a.insert("bee");
    a.insert("c");
    a.insert("ab");
    a.insert("d");
    a.insert("aaaa");

    assert(!(a.insert("q")));


    std::cout << a[0] << " " << a[1] << " " << a[2] << " " << a[3] << " " << a[4] << " " << a[5] << std::endl;


    return 0;
}