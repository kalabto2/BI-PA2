#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

class CBigInt
{
private:
    bool positiveSign = true;
    vector<char> number;
    string bcd () const;
  public:
    // default constructor
    CBigInt ();
    // copying/assignment/destruction
        //CBigInt (const CBigInt & cBigInt);
    // int constructor
    CBigInt (const int & num);
    // string constructor
    CBigInt (const char * num);
    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    CBigInt operator + (const CBigInt & num) const;
        //CBigInt operator + (int num) const;
        //CBigInt operator + (string num) const;
    friend CBigInt operator + (int l, CBigInt & r);
    friend CBigInt operator + (const char * l, CBigInt & r);
    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    CBigInt operator * (const CBigInt & num) const;
        //CBigInt operator * (int num) const;
        //CBigInt operator * (string num) const;
    friend CBigInt operator * (int l, CBigInt & r);
    friend CBigInt operator * (const char * l, CBigInt & r);
    // operator +=, any of {CBigInt/int/string}
    CBigInt operator += (const CBigInt & num);
        //CBigInt operator += (int num);
        //CBigInt operator += (string num);
    // operator *=, any of {CBigInt/int/string}
    CBigInt operator *= (const CBigInt & num);
        //CBigInt operator *= (int num);
        //CBigInt operator *= (string num);
    // comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    bool operator == (const CBigInt & cBigInt) const;
        //bool operator == (const int & l) const;
        //bool operator == (const char * l) const;
    friend bool operator == (int l, const CBigInt & r);
    friend bool operator == (const char * l, const CBigInt & r);
    bool operator <= (const CBigInt & cBigInt) const;
        //bool operator <= (const int & l) const;
        //bool operator <= (const char * l) const;
    friend bool operator <= (int l, const CBigInt & r);
    friend bool operator <= (const char * l, const CBigInt & r);
    bool operator < (const CBigInt & cBigInt) const;
        //bool operator < (const int & l) const;
        //bool operator < (const char * l) const;
    friend bool operator < (int l, const CBigInt & r);
    friend bool operator < (const char * l, const CBigInt & r);
    bool operator >= (const CBigInt & cBigInt) const;
        //bool operator >= (const int & l) const;
        //bool operator >= (const char * l) const;
    friend bool operator >= (int l, const CBigInt & r);
    friend bool operator >= (const char * l, const CBigInt & r);
    bool operator > (const CBigInt & cBigInt) const;
        //bool operator > (const int & l) const;
        //bool operator > (const char * l) const;
    friend bool operator > (int l, const CBigInt & r);
    friend bool operator > (const char * l, const CBigInt & r);
    bool operator != (const CBigInt & cBigInt) const;
        //bool operator != (const int & l) const;
        //bool operator != (const char * l) const;
    friend bool operator != (int l, const  CBigInt & r);
    friend bool operator != (const char * l, const CBigInt & r);

    // output operator <<
    friend ostream & operator << (ostream & os, const CBigInt & cBigInt);
    // input operator >>
    friend istream & operator >> (istream & is, CBigInt & cBigInt);
};

CBigInt::CBigInt() {
    positiveSign = true;
    number.push_back(0);
}

CBigInt::CBigInt(const int & num) {
    int tmp = num;
    if (num < 0)
        positiveSign = false;

    while (tmp > 0){
        number.push_back(tmp % 2);
        tmp >>= 1;
    }
}

CBigInt::CBigInt(const char *num) {
    string clearNum;
    bool signPlace = true, skipWS = true;

    for (int i = 0; num[i] != '\0'; i++){
        if (skipWS &&
                (num[i] == ' ' || num[i] == '\f' || num[i] == '\n' || num[i] == '\r' || num[i] == '\t' || num[i] == '\v' ))
            continue;

        if (signPlace && (num[i] == '-')) { // fixme mozna by se mel nastavit 'skipWS' na false ...
            positiveSign = false;
            signPlace = false;
            continue;
        }

        if (num[i] == '0' || num[i] == '1' || num[i] == '2' || num[i] == '3' || num[i] == '4' || num[i] == '5'
                || num[i] == '6' || num[i] == '7' || num[i] == '8' || num[i] == '9'){
            signPlace = false;
            skipWS = false;

            clearNum += num[i];

            continue;
        }
        throw invalid_argument("Invalid argument");
    }

    int i = 0;
    for (auto rit = clearNum.rbegin(); rit != clearNum.rend(); ++rit){
        char tmp = *rit;
        string tmp2;
        tmp2 += tmp;
        int a = atoi(tmp2.c_str()) * pow(10, i);
        *this += a;
        i++;
    }

}

CBigInt CBigInt::operator+(const CBigInt &num) const {      // fixme
    CBigInt tmp;
    tmp.number.pop_back();

    if (!this->positiveSign && !num.positiveSign)
        tmp.positiveSign = false;



    auto biggerNum  = ( num > *this ? num.number.begin() : this->number.begin());
    auto smallerNum = ( num < *this ? num.number.begin() : this->number.begin());
    char add = 0;

    for (; biggerNum != ( num > *this ? num.number.end() : this->number.end()); biggerNum++){
        char sum;

        sum = add + *biggerNum + (smallerNum == ( num < *this ? num.number.end() : this->number.end()) ? 0 : *smallerNum);

        if (add == 1)
            add = 0;

        if (sum > 1){
            sum -= 2;
            add = 1;
        }

        tmp.number.push_back(sum);

        if (smallerNum != ( num < *this ? num.number.end() : this->number.end()))
            smallerNum++;
    }

    if (add == 1)
        tmp.number.push_back(1);

    /*for (auto it1 : this->number){
        char sum;

        sum = it1 + *biggerNum + add;

        if (add == 1)
            add = 0;

        if (sum > 1){
            sum -= 2;
            add = 1;
        }

        tmp.number.push_back(sum);
        biggerNum++;
    }

    for (biggerNum; biggerNum != num.number.end(); biggerNum++){
        char sum = *biggerNum + add;

        if (add == 1)
            add = 0;

        if (sum > 1){
            sum -= 2;
            add = 1;
        }
        tmp.number.push_back(sum);
    }
*/
    return tmp;
}

CBigInt CBigInt::operator*(const CBigInt &num) const {
    CBigInt res;

    if ((this->positiveSign && !num.positiveSign) || (!this->positiveSign && num.positiveSign))
        res.positiveSign = false;

    CBigInt tmp = *this;
    for (auto it: num.number){
        if (it == 0){
            tmp.number.insert(tmp.number.begin(), 0);
            continue;
        }
        if (it == 1) {
            res += tmp;
        }
        tmp.number.insert(tmp.number.begin(), 0);
    }

    return res;
}


CBigInt CBigInt::operator+=(const CBigInt &num) {
    *this = *this + num;
    return *this;
}

CBigInt CBigInt::operator*=(const CBigInt &num) {
    *this = *this * num;
    return *this;
}

bool CBigInt::operator==(const CBigInt &cBigInt) const {
    return positiveSign == cBigInt.positiveSign && number == cBigInt.number;
}

bool CBigInt::operator<=(const CBigInt &cBigInt) const {
    if (!this->positiveSign && cBigInt.positiveSign)
        return true;

    if (this->positiveSign && !cBigInt.positiveSign)
        return false;

    if (this->positiveSign && cBigInt.positiveSign ){
        if (this->number.size() == cBigInt.number.size()){
            auto itCB = cBigInt.number.begin();
            for (auto it : this->number){
                if (it > *itCB)
                    return false;
                if (it < *itCB)
                    return true;
                itCB ++;
            }
            return true;
        }
        return this->number.size() < cBigInt.number.size();
    }

    if (!this->positiveSign && !cBigInt.positiveSign ){
        if (this->number.size() == cBigInt.number.size()){
            auto itCB = cBigInt.number.begin();
            for (auto it : this->number){
                if (it < *itCB)
                    return false;
                if (it > *itCB)
                    return true;
                itCB ++;
            }
            return true;
        }
        return this->number.size() > cBigInt.number.size();
    }
}

bool CBigInt::operator<(const CBigInt &cBigInt) const {
    return *this <= cBigInt && !(*this == cBigInt);
}

bool CBigInt::operator>=(const CBigInt &cBigInt) const {
    return !(*this < cBigInt);
}

bool CBigInt::operator>(const CBigInt &cBigInt) const {
    return !(*this <= cBigInt);
}

bool CBigInt::operator!=(const CBigInt &cBigInt) const {
    return !(*this == cBigInt);
}

CBigInt operator+(int l, CBigInt &r) {
    return r + l;
}

CBigInt operator+(const char *l, CBigInt &r) {
    return r + l;
}

CBigInt operator*(int l, CBigInt &r) {
    return r * l;
}

CBigInt operator*(const char *l, CBigInt &r) {
    return r * l;
}

bool operator==(int l, const CBigInt &r) {
    return r == l;
}

bool operator==(const char *l, const CBigInt &r) {
    return r == l;
}

bool operator<=(int l, const CBigInt &r) {
    return r >= l;
}

bool operator<=(const char *l, const CBigInt &r) {
    return r >= l;
}

bool operator<(int l, const CBigInt &r) {
    return r > l;
}

bool operator<(const char *l, const CBigInt &r) {
    return r > l;
}

bool operator>=(int l, const CBigInt &r) {
    return r <= l;
}

bool operator>=(const char *l, const CBigInt &r) {
    return r <= l;
}

bool operator>(int l, const CBigInt &r) {
    return r < l;
}

bool operator>(const char *l, const CBigInt &r) {
    return r < l;
}

bool operator!=(int l, const CBigInt &r) {
    return r != l;
}

bool operator!=(const char *l, const CBigInt &r) {
    return r != l;
}

ostream &operator<<(ostream &os, const CBigInt &cBigInt) {      // todo
    cout << "helekamo";
    os << (cBigInt.positiveSign ? "" : "-") << cBigInt.bcd();

    return os;
}

istream &operator>>(istream &is,  CBigInt &cBigInt) {
    string clearNum;
    bool signPlace = true, skipWS = true, negativeSign = false, correctEntry = false;

    while(!is.eof()){
        char c = is.peek();

        if (skipWS && (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' )){
            is.get();
            continue;
        }

        if (signPlace && (c == '-')) {
            negativeSign = true;
            signPlace = false;
            skipWS = false;
            is.get();
            continue;
        }

        if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5'
            || c == '6' || c == '7' || c == '8' || c == '9'){
            signPlace = false;
            skipWS = false;
            correctEntry = true;
            clearNum += c;
            is.get();

            continue;
        }
        break;
    }

    if (!correctEntry)
        is.setstate(ios::failbit);

    if (negativeSign)
        clearNum = "-" + clearNum;

    cBigInt = clearNum.c_str();

    return is ;
}

string CBigInt::bcd() const{
    vector<int> decimalOrder;
    decimalOrder.push_back(0);

    for (auto rit = number.rbegin(); rit != number.rend(); rit++){
        int x = *rit;

        for (int i = 0; i < decimalOrder.size(); i++  ){
            if (decimalOrder[i] >= 5)
                decimalOrder[i] += 3;
        }

        if (decimalOrder.back() >= 8)
            decimalOrder.push_back(0);

        int shift = x;
        for (int i = 0; i < decimalOrder.size(); i++ ){
            decimalOrder[i] = (decimalOrder[i] << 1) + shift;
            shift = 0;

            if (decimalOrder[i] >= 16){
                decimalOrder[i] -= 16;
                shift = 1;
            }
        }

    }

    string res;

    for (auto rit = decimalOrder.rbegin(); rit != decimalOrder.rend(); rit++){
        res += to_string(*rit);
    }

    cout << res << "ss" << endl;

    return res;
}


#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char * val )
{
  ostringstream oss;
  oss << x;
  return oss . str () == val;
}
int main ( void )
{
    /* ============ MY TESTS ========== */
    vector <char> test;
    test.push_back(0);
    test.push_back(1);
    test.push_back(1);


    vector <char> test2;
    test2.push_back(1);
    test2.push_back(1);

//    assert(!(test2 > test));

    CBigInt c, d;
    c = 10;
    d = c;
    c += 10;

    assert((c > d));
    assert((d < c));
    assert((d <= c));
    assert((c >= d));
    assert(c == 20);
    assert(d == 10);

    c = 445;
    d = "445";
    assert(c == d);

    CBigInt e,f;
    istringstream myIs;

    myIs . clear ();
    myIs . str ( " 1234" );
    assert ( myIs >> e );
    f = "1234";
    assert(e == f);

    myIs . clear ();
    myIs . str ( " 12 34" );
    assert ( myIs >> e );
    f = "12";
    assert ( e == f );
    myIs . clear ();
    myIs . str ( "999z" );
    assert ( myIs >> e );
    f = "999";
    assert ( e == f);
    myIs . clear ();
    myIs . str ( "abcd" );
    assert ( ! ( myIs >> e ) );
    myIs . clear ();
    myIs . str ( "- 758" );
    assert ( ! ( myIs >> e ) );
    try
    {
        e = "-xyz";
        assert ( "missing an exception" == NULL );
    }
    catch ( const invalid_argument & ex )
    {
    }

    /* ============= GIVEN TESTS ============ */
  CBigInt a, b;
  istringstream is;
  a = 10;
  a += 20;
  assert ( equal ( a, "30" ) );
  a *= 5;
  assert ( equal ( a, "150" ) );
  b = a + "3";
  assert ( equal ( b, "153" ) );
  b = a * 7;
  assert ( equal ( b, "1050" ) );
  assert ( equal ( a, "150" ) );

  a = 10;
  a += -20;
  assert ( equal ( a, "-10" ) );
  a *= 5;
  assert ( equal ( a, "-50" ) );
  b = a + 73;
  assert ( equal ( b, "23" ) );
  b = a * -7;
  assert ( equal ( b, "350" ) );
  assert ( equal ( a, "-50" ) );

  a = "12345678901234567890";
  a += "-99999999999999999999";
  assert ( equal ( a, "-87654321098765432109" ) );
  a *= "54321987654321987654";
  assert ( equal ( a, "-4761556948575111126880627366067073182286" ) );
  a *= 0;
  assert ( equal ( a, "0" ) );
  a = 10;
  b = a + "400";
  assert ( equal ( b, "410" ) );
  b = a * "15";
  assert ( equal ( b, "150" ) );
  assert ( equal ( a, "10" ) );

  is . clear ();
  is . str ( " 1234" );
  assert ( is >> b );
  assert ( equal ( b, "1234" ) );
  is . clear ();
  is . str ( " 12 34" );
  assert ( is >> b );
  assert ( equal ( b, "12" ) );
  is . clear ();
  is . str ( "999z" );
  assert ( is >> b );
  assert ( equal ( b, "999" ) );
  is . clear ();
  is . str ( "abcd" );
  assert ( ! ( is >> b ) );
  is . clear ();
  is . str ( "- 758" );
  assert ( ! ( is >> b ) );
  try
  {
    a = "-xyz";
    assert ( "missing an exception" == NULL );
  }
  catch ( const invalid_argument & e )
  {
  }

    a = "73786976294838206464";
  assert ( a < "1361129467683753853853498429727072845824" );
  assert ( a <= "1361129467683753853853498429727072845824" );
  assert ( ! ( a > "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a >= "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a == "1361129467683753853853498429727072845824" ) );
  assert ( a != "1361129467683753853853498429727072845824" );
  assert ( ! ( a < "73786976294838206464" ) );
  assert ( a <= "73786976294838206464" );
  assert ( ! ( a > "73786976294838206464" ) );
  assert ( a >= "73786976294838206464" );
  assert ( a == "73786976294838206464" );
  assert ( ! ( a != "73786976294838206464" ) );
  assert ( a < "73786976294838206465" );
  assert ( a <= "73786976294838206465" );
  assert ( ! ( a > "73786976294838206465" ) );
  assert ( ! ( a >= "73786976294838206465" ) );
  assert ( ! ( a == "73786976294838206465" ) );
  assert ( a != "73786976294838206465" );
  a = "2147483648";
  assert ( ! ( a < -2147483648 ) );
  assert ( ! ( a <= -2147483648 ) );
  assert ( a > -2147483648 );
  assert ( a >= -2147483648 );
  assert ( ! ( a == -2147483648 ) );
  assert ( a != -2147483648 );

  return 0;
}
#endif /* __PROGTEST__ */
