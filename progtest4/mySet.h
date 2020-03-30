//
// Created by tom on 30.03.20.
//

#ifndef PROGTEST4_MYSET_H
#define PROGTEST4_MYSET_H

template <typename T>
class mySet {
    T * items;
    int size;
    int occupied;

    void resize ();
    bool idExist (const char * id);
public:
    mySet ();
    ~ mySet();
    bool insert (T item);
    T findId (const char * id, int from, int to);
    T operator [](int index);
};



#endif //PROGTEST4_MYSET_H
