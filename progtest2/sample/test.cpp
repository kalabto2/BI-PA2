#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

class Citizen
{
private:
    vector <int> income;
    vector <int> expense;
public:
    bool isUnique (Citizen *cit2);
    bool setIncome (int value);
    bool setExpense (int value);
    int getSumIncome ();
    int getSumExpense ();
    string account;
    string name;
    string addr;
    Citizen(string  name, string  addr, string  account);
};

bool Citizen::isUnique(Citizen *cit2) {
    return !(this->account == cit2->account || (this->name == cit2->name && this->addr == cit2->addr));
}

bool Citizen::setIncome(int value) {
    income.push_back(value);
    return true;
}

bool Citizen::setExpense(int value) {
    expense.push_back(value);
    return true;
}

int Citizen::getSumIncome() {
    int sum = 0;
    for (auto it : income)
        sum += it;
    return sum;
}

int Citizen::getSumExpense() {
    int sum = 0;
    for (auto it : expense)
        sum += it;
    return sum;
}

Citizen::Citizen(string name, string addr, string account) {
    this->name = std::move(name);
    this->addr = std::move(addr);
    this->account = std::move(account);
}

class CIterator
{
  public:
    bool                     AtEnd                         ( void ) const;
    void                     Next                          ( void );
    string                   Name                          ( void ) const;
    string                   Addr                          ( void ) const;
    string                   Account                       ( void ) const;
                             CIterator                     ( vector <Citizen *> vector);
  private:
    std::vector<Citizen*> myVector;         // FIXME aby se nekopirovalo ...
    std::vector<Citizen*>::iterator it;
};

bool CIterator::AtEnd(void) const {
    return (it == myVector.end());
}

void CIterator::Next(void) {
    it = it + 1;
}

string CIterator::Name(void) const {
    return (*it)->name;
}

string CIterator::Addr(void) const {
    return (*it)->addr;
}

string CIterator::Account(void) const {
    return (*it)->account;
}

CIterator::CIterator(std::vector<Citizen *> myVector) {
    this->myVector = myVector;
    it = this->myVector.begin();
}

bool accountComp (Citizen * citizen, const string& acc) {
    //cout << "jou " << (citizen->account < acc) << endl;
    return citizen->account < acc;
}

bool nameAddrComp (Citizen * citizen, Citizen * citizen2) {
    if (citizen->name == citizen2->name)
        return citizen->addr < citizen2->addr;
    return citizen->name < citizen2->name;
}

class CTaxRegister
{
  public:
                             ~CTaxRegister();
    bool                     Birth                         ( const string    & name,
                                                             const string    & addr,
                                                             const string    & account );
    bool                     Death                         ( const string    & name,
                                                             const string    & addr );
    bool                     Income                        ( const string    & account,
                                                             int               amount );
    bool                     Income                        ( const string    & name,
                                                             const string    & addr,
                                                             int               amount );
    bool                     Expense                       ( const string    & account,
                                                             int               amount );
    bool                     Expense                       ( const string    & name,
                                                             const string    & addr,
                                                             int               amount );
    bool                     Audit                         ( const string    & name,
                                                             const string    & addr,
                                                             string          & account,
                                                             int             & sumIncome,
                                                             int             & sumExpense ) const;
    CIterator                ListByName                    ( void ) const;
  private:
    vector <Citizen*> accountSorted;
    vector <Citizen*> nameAddrSorted;
};

bool CTaxRegister::Birth(const string &name, const string &addr, const string &account) {
    auto * citizen = new Citizen(name, addr, account);

    if (accountSorted.empty() && nameAddrSorted.empty()){
        accountSorted.push_back(citizen);
        nameAddrSorted.push_back(citizen);
        return true;
    }

    auto low1 = lower_bound(accountSorted.begin(),accountSorted.end(), account, accountComp);
    auto low2 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);

    if ((low1 != accountSorted.end() &&  ! citizen->isUnique(*(low1) )) ||
        (low2 != nameAddrSorted.end() && ! citizen->isUnique(*(low2) ))){
            delete citizen;
            return false;
        }

    nameAddrSorted.insert(low2, citizen);
    accountSorted.insert(low1, citizen);

    return true;
}

bool CTaxRegister::Death(const string &name, const string &addr) {
    auto * citizen = new Citizen(name, addr, "");

    auto low2 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ((low2 == nameAddrSorted.end()) || ((*low2)->name != name || (*low2)->addr != addr)) {
        delete citizen;
        return false;
    }

    Citizen * tmp = *low2;
    string account = (*low2)->account;
    //auto low1 = lower_bound(accountSorted.begin(),accountSorted.end(), account, accountComp);

    auto low1 = find(accountSorted.begin(), accountSorted.end(), *low2);

    nameAddrSorted.erase(low2);
    accountSorted.erase(low1);

    delete citizen;
    delete tmp;
    return true;
}

bool CTaxRegister::Income(const string &account, int amount) {
    auto low1 = lower_bound(accountSorted.begin(),accountSorted.end(), account, accountComp);
    if ((low1 == accountSorted.end()) || ((*low1)->account != account))
        return false;

    (*(low1))->setIncome(amount);
    return true;
}

bool CTaxRegister::Income(const string &name, const string &addr, int amount) {
    auto * citizen = new Citizen(name, addr, "");

    auto low1 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ( (low1 == nameAddrSorted.end()) || ((*low1)->name != name || (*low1)->addr != addr)) {
        delete citizen;
        return false;
    }

    (*(low1))->setIncome(amount);
    delete citizen;
    return true;
}

bool CTaxRegister::Expense(const string &account, int amount) {
    auto low1 = lower_bound(accountSorted.begin(),accountSorted.end(), account, accountComp);
    if ((low1 == accountSorted.end()) || ((*low1)->account != account))
        return false;

    (*(low1))->setExpense(amount);
    return true;
}

bool CTaxRegister::Expense(const string &name, const string &addr, int amount) {
    auto * citizen = new Citizen(name, addr, "");

    auto low1 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ((low1 == nameAddrSorted.end()) || ((*low1)->name != name || (*low1)->addr != addr)) {
        delete citizen;
        return false;
    }

    (*(low1))->setExpense(amount);
    delete citizen;
    return true;
}

bool
CTaxRegister::Audit(const string &name, const string &addr, string &account, int &sumIncome, int &sumExpense) const {
    auto * citizen = new Citizen(name, addr, "");

    auto low1 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ((low1 == nameAddrSorted.end()) || ((*low1)->name != name || (*low1)->addr != addr)) {
        delete citizen;
        return false;
    }

    account = (*low1)->account;
    sumIncome = (*low1)->getSumIncome();
    sumExpense = (*low1)->getSumExpense();
    delete citizen;
    return true;
}

CIterator CTaxRegister::ListByName(void) const {
    return CIterator(nameAddrSorted);
}

CTaxRegister::~CTaxRegister() {
    for (auto it: accountSorted)
        delete it;
}

#ifndef __PROGTEST__
int main ( void )
{
  string acct;
  int    sumIncome, sumExpense;
  CTaxRegister b1;
  assert ( b1 . Birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b1 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( b1 . Birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  assert ( b1 . Birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
  assert ( b1 . Income ( "Xuj5#94", 1000 ) );
  assert ( b1 . Income ( "634oddT", 2000 ) );
  assert ( b1 . Income ( "123/456/789", 3000 ) );
  assert ( b1 . Income ( "634oddT", 4000 ) );
  assert ( b1 . Income ( "Peter Hacker", "Main Street 17", 2000 ) );
  assert ( b1 . Expense ( "Jane Hacker", "Main Street 17", 2000 ) );
  assert ( b1 . Expense ( "John Smith", "Main Street 17", 500 ) );
  assert ( b1 . Expense ( "Jane Hacker", "Main Street 17", 1000 ) );
  assert ( b1 . Expense ( "Xuj5#94", 1300 ) );
  assert ( b1 . Audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "123/456/789" );
  assert ( sumIncome == 3000 );
  assert ( sumExpense == 0 );
  assert ( b1 . Audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 1000 );
  assert ( sumExpense == 4300 );
  assert ( b1 . Audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "634oddT" );
  assert ( sumIncome == 8000 );
  assert ( sumExpense == 0 );
  assert ( b1 . Audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Z343rwZ" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 500 );
  CIterator it = b1 . ListByName ();
  assert ( ! it . AtEnd ()
           && it . Name () == "Jane Hacker"
           && it . Addr () == "Main Street 17"
           && it . Account () == "Xuj5#94" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "John Smith"
           && it . Addr () == "Main Street 17"
           && it . Account () == "Z343rwZ" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "John Smith"
           && it . Addr () == "Oak Road 23"
           && it . Account () == "123/456/789" );
  it . Next ();
  assert ( ! it . AtEnd ()
           && it . Name () == "Peter Hacker"
           && it . Addr () == "Main Street 17"
           && it . Account () == "634oddT" );
  it . Next ();
  assert ( it . AtEnd () );

  assert ( b1 . Death ( "John Smith", "Main Street 17" ) );

  CTaxRegister b2;
  assert ( b2 . Birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b2 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( !b2 . Income ( "634oddT", 4000 ) );
  assert ( !b2 . Expense ( "John Smith", "Main Street 18", 500 ) );
  assert ( !b2 . Audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
  assert ( !b2 . Death ( "Peter Nowak", "5-th Avenue" ) );
  assert ( !b2 . Birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
  assert ( !b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b2 . Death ( "Jane Hacker", "Main Street 17" ) );
  assert ( b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b2 . Audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 0 );
  assert ( !b2 . Birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

  return 0;
}
#endif /* __PROGTEST__ */
