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

/**
 * Represents 1 citizen.
 */
class Citizen
{
private:
    vector <int> income;
    vector <int> expense;
    string account;
    string name;
    string addr;
public:
    bool isUnique (Citizen *cit2);
    bool setIncome (int value);
    bool setExpense (int value);
    int getSumIncome ();
    int getSumExpense ();
    const string &getAccount() const;
    const string &getName() const;
    const string &getAddr() const;
    Citizen(string  name, string  addr, string  account);
};

/**
 * Decides, if this instance of citizen isn't the same name and address or same account, as another given one.
 * @param cit2 - another Citizen to be compared.
 * @return true, if Citizens are uniqe. Else false.
 */
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

const string &Citizen::getAccount() const {
    return account;
}

const string &Citizen::getName() const {
    return name;
}

const string &Citizen::getAddr() const {
    return addr;
}

/**
 * Encapsulates sorted vector<Citizens*>, and makes "interface". Can easily manipulate with.
 */
class CIterator
{
  public:
    bool                     AtEnd                         (  ) const;
    void                     Next                          (  );
    string                   Name                          (  ) const;
    string                   Addr                          (  ) const;
    string                   Account                       (  ) const;
                             explicit CIterator            ( vector <Citizen *> vector);
  private:
    std::vector<Citizen*> myVector;
    std::vector<Citizen*>::iterator it;
};

/**
 * @return true, if iterator is at the end.
 */
bool CIterator::AtEnd() const {
    return (it == myVector.end());
}

/**
 * Moves iterator to another record.
 */
void CIterator::Next() {
    it = it + 1;
}

string CIterator::Name() const {
    return (*it)->getName();
}

string CIterator::Addr() const {
    return (*it)->getAddr();
}

string CIterator::Account() const {
    return (*it)->getAccount();
}

/**
 * Constructor
 * @param myVector - sets vector to iterate.
 */
CIterator::CIterator(std::vector<Citizen *> myVector) {
    this->myVector = std::move(myVector);
    it = this->myVector.begin();
}

/**
 * Compares "alphabeticaly" accounts of 2 citizens.
 * @param citizen
 * @param acc - account
 * @return true, if account of citizen should be before account. Else false.
 */
bool accountComp (Citizen * citizen, const string& acc) {
    return citizen->getAccount() < acc;
}

/**
 * Compares "alphabeticaly" names of 2 citizens. If they equals, function compare their addresses
 * @param citizen1 - citizen1
 * @param citizen2 - citizen2
 * @return true, if citizen1 should be before citizen2. Else false.
 */
bool nameAddrComp (Citizen * citizen1, Citizen * citizen2) {
    if (citizen1->getName() == citizen2->getName())
        return citizen1->getAddr() < citizen2->getAddr();
    return citizen1->getName() < citizen2->getName();
}

/**
 * Represents database of taxpayers. Records their identification, income and expense.
 */
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
    CIterator                ListByName                    ( ) const;
  private:
    vector <Citizen*> accountSorted;
    vector <Citizen*> nameAddrSorted;
};

/**
 * Creates account to citizen (at birth). Accounts numbers and name + addr. must be unique.
 * @param name - name to be recorded
 * @param addr - address to be recorded
 * @param account - account to be recorded
 * @return false, if account already exists (doesn't have unique name+addr/account). Else true.
 */
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

/**
 * Delete account, information about citizen.
 * @param name - citizen is searched by this parameter
 * @param addr - citizen is searched by this parameter
 * @return false, if account doesn't exist. Else true
 */
bool CTaxRegister::Death(const string &name, const string &addr) {
    auto * citizen = new Citizen(name, addr, "");

    auto low2 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ((low2 == nameAddrSorted.end()) || ((*low2)->getName() != name || (*low2)->getAddr() != addr)) {
        delete citizen;
        return false;
    }

    Citizen * tmp = *low2;
    string account = (*low2)->getAccount();

    auto low1 = find(accountSorted.begin(), accountSorted.end(), *low2);

    nameAddrSorted.erase(low2);
    accountSorted.erase(low1);

    delete citizen;
    delete tmp;
    return true;
}

/**
 * Records 1 income to given citizen.
 * @param account - citizen is searched by this parameter
 * @param amount - amount of income
 * @return false, if account doesn't exist. Else true.
 */
bool CTaxRegister::Income(const string &account, int amount) {
    auto low1 = lower_bound(accountSorted.begin(),accountSorted.end(), account, accountComp);
    if ((low1 == accountSorted.end()) || ((*low1)->getAccount() != account))
        return false;

    (*(low1))->setIncome(amount);
    return true;
}

/**
 * Records 1 income to given citizen.
 * @param name - citizen is searched by this parameter
 * @param addr - citizen is searched by this parameter
 * @param amount - amount of income
 * @return false, if account doesn't exist. Else true.
 */
bool CTaxRegister::Income(const string &name, const string &addr, int amount) {
    auto * citizen = new Citizen(name, addr, "");

    auto low1 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ( (low1 == nameAddrSorted.end()) || ((*low1)->getName() != name || (*low1)->getAddr() != addr)) {
        delete citizen;
        return false;
    }

    (*(low1))->setIncome(amount);
    delete citizen;
    return true;
}

/**
 * Records 1 expense to given citizen.
 * @param account - citizen is searched by this parameter
 * @param amount  - ammount of expense
 * @return false, if account doesn't exist. Else true.
 */
bool CTaxRegister::Expense(const string &account, int amount) {
    auto low1 = lower_bound(accountSorted.begin(),accountSorted.end(), account, accountComp);
    if ((low1 == accountSorted.end()) || ((*low1)->getAccount() != account))
        return false;

    (*(low1))->setExpense(amount);
    return true;
}

/**
 * Records 1 expense to given citizen.
 * @param name - citizen is searched by this parameter
 * @param addr - citizen is searched by this parameter
 * @param amount - ammount of expense
 * @return false, if account doesn't exist. Else true.
 */
bool CTaxRegister::Expense(const string &name, const string &addr, int amount) {
    auto * citizen = new Citizen(name, addr, "");

    auto low1 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ((low1 == nameAddrSorted.end()) || ((*low1)->getName() != name || (*low1)->getAddr() != addr)) {
        delete citizen;
        return false;
    }

    (*(low1))->setExpense(amount);
    delete citizen;
    return true;
}

/**
 * Writes information about citizen to forwarded parameters.
 * @param name  - citizen is searched by this parameter
 * @param addr  - citizen is searched by this parameter
 * @param account - here is written account number
 * @param sumIncome - here is written sum of citizen's all incomes
 * @param sumExpense - here is written sum of citizen's all expense
 * @return false, if account doesn't exist. Else true.
 */
bool
CTaxRegister::Audit(const string &name, const string &addr, string &account, int &sumIncome, int &sumExpense) const {
    auto * citizen = new Citizen(name, addr, "");

    auto low1 = lower_bound(nameAddrSorted.begin(),nameAddrSorted.end(), citizen, nameAddrComp);
    if ((low1 == nameAddrSorted.end()) || ((*low1)->getName() != name || (*low1)->getAddr() != addr)) {
        delete citizen;
        return false;
    }

    account = (*low1)->getAccount();
    sumIncome = (*low1)->getSumIncome();
    sumExpense = (*low1)->getSumExpense();
    delete citizen;
    return true;
}

/**
 * Returns CIterator on list of Citizen sorted by Name and address (if names are equal) ascending.
 * @return CIterator instance.
 */
CIterator CTaxRegister::ListByName() const {
    return CIterator(nameAddrSorted);
}

/**
 * Destructor
 */
CTaxRegister::~CTaxRegister() {
    for (auto it: accountSorted)
        delete it;
}

#ifndef __PROGTEST__
int main ()
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
