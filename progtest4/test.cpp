#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;
#endif /* __PROGTEST__ */

template <typename T>
class myVector {
    T * items;
    int size;

    void resize ();
public:
    int occupied;
    myVector ();
    ~ myVector();
    bool push_back (T item);
    void clear();
    T operator [](int index);
    myVector<T> & operator = (const myVector<T> & vector);
};


template <typename T>
class mySet {
    T * items;
    int size;

    void resize ();
    bool idExist (const char * id);
public:
    int occupied;
    mySet ();
    ~ mySet();
    bool insert (T item);
    T findId (const char * id, int from, int to);
    T operator [](int index);
    mySet<T> & operator = (const mySet<T> & item);
};


class CTransaction{
public:
    int pointNum = 0;
    const char * from;
    const char * to;
    unsigned int amount;
    const char * id;
    CTransaction(const char *debAccID, const char *credAccID, unsigned int amount, const char *signature);
    ~CTransaction();
    CTransaction &operator = (const CTransaction & transaction);
};


class CAccount{
    int initDeposit;
    int balance;
    myVector <CTransaction *> transactions;
    friend int cmpfunc (const void * a, const void * b);
public:
    const char * id;
    unsigned Balance();
    CAccount (const char * c, int initBal);
    ~CAccount();
    void addTransaction (CTransaction * transaction);
    void trimTransactions();
    friend ostream & operator << (ostream & os, CAccount & acc);
    CAccount &operator = (CAccount const & acc);
};


class CBank
{
public:
    // default constructor
    CBank  ();
    // copy constructor
    CBank  (CBank const & bank);
    // destructor
        // todo?
    // operator =
    CBank &operator = (const CBank & bank);

    bool   NewAccount    ( const char * accID,
                           int          initialBalance );
    bool   Transaction   ( const char * debAccID,
                           const char * credAccID,
                           unsigned int amount,
                           const char * signature );
    bool   TrimAccount   ( const char * accID );
    CAccount & Account (const char * accID );
private:
    mySet <CAccount * > accounts;
};


int cmpfunc (const void * a, const void * b) {
    const CAccount * aa = *(const CAccount **)a;
    const CAccount * bb = *(const CAccount **)b;
    const char * l = aa->id;
    const char * r = bb->id;
    int res = strcmp(l, r);

    //std::cout << "> left: " << l << ", right: " << r << " | res = " << res << std::endl;

    return res;
}

/* =================== DEFINITIONS =================== */

template<typename T>
T myVector<T>::operator[](int index) {
    return items[index];
}

template<typename T>
myVector<T>::~myVector() {
    for (int i = 0; i < occupied; i++){
        if (items[i]->pointNum == 1)
            delete items[i];
        else
            items[i]->pointNum --;
    }
     delete [] items;
}

template<typename T>
myVector<T>::myVector() {
    const int INIT_SIZE = 100;
    items = new T[INIT_SIZE];
    size = INIT_SIZE;
    occupied = 0;
}

template<typename T>
bool myVector<T>::push_back(T item) {
    if (occupied == size)
        resize();

    item->pointNum ++;
    items[occupied] = item;
    occupied ++;

    return true;
}

template<typename T>
void myVector<T>::resize() {
    auto * tmp = new T[2 * size];

    for (int i = 0; i < size; i++)
        tmp[i] = items[i];


    delete [] items;
    items = tmp;
    size *= 2;
}

template<typename T>
void myVector<T>::clear() {
    for (int i = 0; i < occupied; i++){
        if (items[i]->pointNum == 1)
            delete items[i];
        else
            items[i]->pointNum --;
    }
    delete [] items;

    //*this = myVector();   -- WHY DOESNT WORK?

    const int INIT_SIZE = 100;
    items = new T[INIT_SIZE];
    size = INIT_SIZE;
    occupied = 0;
}

template<typename T>
myVector<T> & myVector<T>::operator=(const myVector<T> &vector) {
    size = vector.size;
    occupied = vector.occupied;

    for (int i = 0; i < occupied; i++){
        auto * acc = new  CTransaction("", "", 0, "");
        *acc = *vector.items[i];
        //CAccount tmp = *item.items[i];
        items[i] = acc;
    }

    return *this;
}

/* ---------------------------------------------------- */

template<typename T>
mySet<T>::mySet() {
    const int INIT_SIZE = 100;
    items = new T[INIT_SIZE];
    size = INIT_SIZE;
    occupied = 0;
}

template<typename T>
bool mySet<T>::insert(T item) {
    if (occupied == size)
        resize();

    if (idExist(item->id))
        return false;

    items[occupied] = item;
    occupied ++;
    qsort(items, occupied, sizeof(T), cmpfunc);

    return true;
}

template<typename T>
mySet<T>::~mySet() {
    for (int i = 0; i < occupied; i++){
        delete items[i];
    }
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
    if (mid == occupied)
        return nullptr;
    int comparison = strcmp(id, items[mid]->id);
    if (comparison == 0)
        return items[mid];
    if (comparison > 0)
        return findId(id, (mid + 1), to);
    return findId(id, from, (mid - 1));
}

template<typename T>
mySet<T> & mySet<T>::operator = (const mySet<T> &item) {
    size = item.size;
    occupied = item.occupied;

    items = new T[size];

    for (int i = 0; i < occupied; i++){
        auto * acc = new  CAccount("", 0);
        *acc = *item.items[i];
        //CAccount tmp = *item.items[i];
        items[i] = acc;
    }

    return *this;
}

/* ---------------------------------------------------- */

CTransaction::CTransaction(const char *debAccID, const char *credAccID, unsigned int amount, const char *signature)
        : amount(amount) {
    char * tmp = new char[strlen(signature) + 1];
    memcpy(tmp, signature, sizeof(char) * (strlen(signature) + 1));
    id = tmp;

    char * tmp2 = new char[strlen(credAccID) + 1];
    memcpy(tmp2, credAccID, sizeof(char) * (strlen(credAccID) + 1));
    to = tmp2;

    char * tmp3 = new char[strlen(debAccID) + 1];
    memcpy(tmp3, debAccID, sizeof(char) * (strlen(debAccID) + 1));
    from = tmp3;
}

CTransaction::~CTransaction() {
    delete [] from;
    delete [] to;
    delete [] id;
}

CTransaction &CTransaction::operator=(const CTransaction & transaction) {
    amount = transaction.amount;
    pointNum = 1;

    delete [] id;
    char * tmp = new char [strlen(transaction.id) + 1];
    memcpy(tmp, transaction.id, sizeof(char) * (strlen(transaction.id) + 1));
    id = tmp;

    delete [] from;
    char * tmp2 = new char [strlen(transaction.from) + 1];
    memcpy(tmp2, transaction.from, sizeof(char) * (strlen(transaction.from) + 1));
    from = tmp2;

    delete [] to;
    char * tmp3 = new char [strlen(transaction.to) + 1];
    memcpy(tmp3, transaction.to, sizeof(char) * (strlen(transaction.to) + 1));
    to = tmp3;

    return *this;
}

/* ---------------------------------------------------- */

CAccount::CAccount(const char *c, int initBal) : initDeposit(initBal), balance(initBal) {
    char * tmp = new char[strlen(c) + 1];
    // strncpy(tmp, c, strlen(c));
    memcpy(tmp, c, sizeof(char) * (strlen(c) + 1));
    id = tmp;
}

void CAccount::addTransaction(CTransaction *transaction) {
    transactions.push_back(transaction);

    if (strcmp(transaction->from, id) == 0)
        balance -= transaction->amount;
    else
        balance += transaction->amount;
}

unsigned CAccount::Balance() {
    return balance;
}

void CAccount::trimTransactions() {
    initDeposit = balance;
    transactions.clear();
}

/* ---------------------------------------------------- */

bool CBank::NewAccount(const char *accID, int initialBalance) {
    auto * acc = new  CAccount(accID, initialBalance);

    if (!accounts.insert(acc)){
        delete acc;
        return false;
    }

    return true;
}

bool CBank::Transaction(const char *debAccID, const char *credAccID, unsigned int amount, const char *signature) {
    if (strcmp(debAccID,credAccID) == 0)
        return false;

    CAccount * deb = accounts.findId(debAccID, 0, accounts.occupied);
    CAccount * cred = accounts.findId(credAccID, 0, accounts.occupied);

    if (deb == nullptr || cred == nullptr)
        return false;

    auto * transaction = new CTransaction(debAccID, credAccID, amount, signature);

    deb->addTransaction(transaction);
    cred->addTransaction(transaction);

    return true;
}

CAccount & CBank::Account(const char *accID) {
    CAccount * res = accounts.findId(accID, 0, accounts.occupied);
    if (res == nullptr)
        throw exception {    };
    return *res;
}

bool CBank::TrimAccount(const char *accID) {
    CAccount * res = accounts.findId(accID, 0, accounts.occupied);
    if (res == nullptr)
        return false;

    res->trimTransactions();

    return true;
}

CBank::CBank() {
        // todo
}

CBank::CBank(CBank const & bank) {
    accounts.~mySet();
    accounts.operator=(bank.accounts);
}

CBank &CBank::operator=(const CBank & bank) {
    accounts.~mySet();
    accounts.operator=(bank.accounts);

    return *this;
}

/* ---------------------------------------------------- */

ostream & operator << (ostream & os, CAccount & acc){
    os << acc.id << ":" << endl << "   " << acc.initDeposit << endl;

    for (int i = 0; i < acc.transactions.occupied; i++){
        if (strcmp(acc.transactions[i]->from, acc.id) == 0)
            os << " - " << acc.transactions[i]->amount << ", to: " << acc.transactions[i]->to;
        else
            os << " + " << acc.transactions[i]->amount << ", from: " << acc.transactions[i]->from;
        os << ", sign: " << acc.transactions[i]->id << endl;
    }

    os << " = " << acc.balance << endl;
    return os;
}

CAccount &CAccount::operator=(CAccount const &acc) {
    cout << ">>>" << initDeposit << " " <<  acc.initDeposit << endl;
    initDeposit = acc.initDeposit;
    balance = acc.balance;

    delete [] id;
    char * tmp = new char [strlen(acc.id) + 1];
    memcpy(tmp, acc.id, sizeof(char) * (strlen(acc.id) + 1));
    id = tmp;

    transactions = acc.transactions;

    return *this;
}

CAccount::~CAccount() {
    delete [] id;
}

/* ---------------------------------------------------- */

#ifndef __PROGTEST__
int main ( )
{
  ostringstream os;
  char accCpy[100], debCpy[100], credCpy[100], signCpy[100];
  CBank x0;
  assert ( x0 . NewAccount ( "123456", 1000 ) );
  assert ( x0 . NewAccount ( "987654", -500 ) );
  assert ( x0 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x0 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x0 . NewAccount ( "111111", 5000 ) );
  assert ( x0 . Transaction ( "111111", "987654", 290, "Okh6e+8rAiuT5=" ) );

    cout << x0.Account("123456");
    cout << x0.Account("987654");
    cout << x0.Account("111111");
  assert ( x0 . Account ( "123456" ). Balance ( ) ==  -2190 );
  assert ( x0 . Account ( "987654" ). Balance ( ) ==  2980 );
  assert ( x0 . Account ( "111111" ). Balance ( ) ==  4710 );
  os . str ( "" );
  os << x0 . Account ( "123456" );
  assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
  os . str ( "" );
  os << x0 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 290, from: 111111, sign: Okh6e+8rAiuT5=\n = 2980\n" ) );
  os . str ( "" );
  os << x0 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 290, to: 987654, sign: Okh6e+8rAiuT5=\n = 4710\n" ) );
  assert ( x0 . TrimAccount ( "987654" ) );
  assert ( x0 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  os . str ( "" );
  os << x0 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   2980\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 3103\n" ) );

  CBank x2;
  strncpy ( accCpy, "123456", sizeof ( accCpy ) );
  assert ( x2 . NewAccount ( accCpy, 1000 ));
  strncpy ( accCpy, "987654", sizeof ( accCpy ) );
  assert ( x2 . NewAccount ( accCpy, -500 ));
  strncpy ( debCpy, "123456", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "XAbG5uKz6E=", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 300, signCpy ) );
  strncpy ( debCpy, "123456", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "AbG5uKz6E=", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
  strncpy ( accCpy, "111111", sizeof ( accCpy ) );
  assert ( x2 . NewAccount ( accCpy, 5000 ));
  strncpy ( debCpy, "111111", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "Okh6e+8rAiuT5=", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 2890, signCpy ) );
  assert ( x2 . Account ( "123456" ). Balance ( ) ==  -2190 );
  assert ( x2 . Account ( "987654" ). Balance ( ) ==  5580 );
  assert ( x2 . Account ( "111111" ). Balance ( ) ==  2110 );
  os . str ( "" );
  os << x2 . Account ( "123456" );
  assert ( ! strcmp ( os . str () . c_str (), "123456:\n   1000\n - 300, to: 987654, sign: XAbG5uKz6E=\n - 2890, to: 987654, sign: AbG5uKz6E=\n = -2190\n" ) );
  os . str ( "" );
  os << x2 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n = 5580\n" ) );
  os . str ( "" );
  os << x2 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n = 2110\n" ) );
  assert ( x2 . TrimAccount ( "987654" ) );
  strncpy ( debCpy, "111111", sizeof ( debCpy ) );
  strncpy ( credCpy, "987654", sizeof ( credCpy ) );
  strncpy ( signCpy, "asdf78wrnASDT3W", sizeof ( signCpy ) );
  assert ( x2 . Transaction ( debCpy, credCpy, 123, signCpy ) );
  os . str ( "" );
  os << x2 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   5580\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );

  CBank x4;
  assert ( x4 . NewAccount ( "123456", 1000 ) );
  assert ( x4 . NewAccount ( "987654", -500 ) );
  assert ( !x4 . NewAccount ( "123456", 3000 ) );
  assert ( !x4 . Transaction ( "123456", "666", 100, "123nr6dfqkwbv5" ) );
  assert ( !x4 . Transaction ( "666", "123456", 100, "34dGD74JsdfKGH" ) );
  assert ( !x4 . Transaction ( "123456", "123456", 100, "Juaw7Jasdkjb5" ) );
  try
  {
    x4 . Account ( "666" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  try
  {
    os << x4 . Account ( "666" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  assert ( !x4 . TrimAccount ( "666" ) );

  CBank x6;
  assert ( x6 . NewAccount ( "123456", 1000 ) );
  assert ( x6 . NewAccount ( "987654", -500 ) );
  assert ( x6 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x6 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x6 . NewAccount ( "111111", 5000 ) );
  assert ( x6 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
  CBank x7 ( x6 );
  assert ( x6 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  assert ( x7 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
  assert ( x6 . NewAccount ( "99999999", 7000 ) );
  assert ( x6 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
    cout << x6.Account("111111");
  assert ( x6 . TrimAccount ( "111111" ) );
  assert ( x6 . Transaction ( "123456", "111111", 221, "Q23wr234ER==" ) );
  os . str ( "" );
  os << x6 . Account ( "111111" );
    cout << os.str();
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n + 221, from: 123456, sign: Q23wr234ER==\n = -1581\n" ) );
  os . str ( "" );
  os << x6 . Account ( "99999999" );
  assert ( ! strcmp ( os . str () . c_str (), "99999999:\n   7000\n + 3789, from: 111111, sign: aher5asdVsAD\n = 10789\n" ) );
  os . str ( "" );
  os << x6 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 123, from: 111111, sign: asdf78wrnASDT3W\n = 5703\n" ) );
  os . str ( "" );
  os << x7 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
  try
  {
    os << x7 . Account ( "99999999" ). Balance ( );
    assert ( "Missing exception !!" == NULL );
  }
  catch ( ... )
  {
  }
  os . str ( "" );
  os << x7 . Account ( "987654" );
  assert ( ! strcmp ( os . str () . c_str (), "987654:\n   -500\n + 300, from: 123456, sign: XAbG5uKz6E=\n + 2890, from: 123456, sign: AbG5uKz6E=\n + 2890, from: 111111, sign: Okh6e+8rAiuT5=\n + 789, from: 111111, sign: SGDFTYE3sdfsd3W\n = 6369\n" ) );

  CBank x8;
  CBank x9;
  assert ( x8 . NewAccount ( "123456", 1000 ) );
  assert ( x8 . NewAccount ( "987654", -500 ) );
  assert ( x8 . Transaction ( "123456", "987654", 300, "XAbG5uKz6E=" ) );
  assert ( x8 . Transaction ( "123456", "987654", 2890, "AbG5uKz6E=" ) );
  assert ( x8 . NewAccount ( "111111", 5000 ) );
  assert ( x8 . Transaction ( "111111", "987654", 2890, "Okh6e+8rAiuT5=" ) );
  x9 = x8;
  assert ( x8 . Transaction ( "111111", "987654", 123, "asdf78wrnASDT3W" ) );
  assert ( x9 . Transaction ( "111111", "987654", 789, "SGDFTYE3sdfsd3W" ) );
  assert ( x8 . NewAccount ( "99999999", 7000 ) );
  assert ( x8 . Transaction ( "111111", "99999999", 3789, "aher5asdVsAD" ) );
  assert ( x8 . TrimAccount ( "111111" ) );
  os . str ( "" );
  os << x8 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   -1802\n = -1802\n" ) );
  os . str ( "" );
  os << x9 . Account ( "111111" );
  assert ( ! strcmp ( os . str () . c_str (), "111111:\n   5000\n - 2890, to: 987654, sign: Okh6e+8rAiuT5=\n - 789, to: 987654, sign: SGDFTYE3sdfsd3W\n = 1321\n" ) );
    /**/
  return 0;
}
#endif /* __PROGTEST__ */
