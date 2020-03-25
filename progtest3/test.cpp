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

const int BASE = 10;            /// can be value 10 or 2

/**
 * Represents integer with unlimited size.
 * Inside class is number represented as vector of single binary / decimal ciphers
 */
class CBigInt
{
private:
    bool positiveSign = true;       /// sign of number. Zero has always positive sign
    vector<char> number;            /// binary // decimal ciphers ...

    string bcd () const;
    bool gteAbs (const CBigInt & another) const;
    string bcdReverse (const string & decimal) const;
    void removeStartingZero ();
public:
    // default constructor
    CBigInt ();
    // int constructor
    CBigInt (const int & num);
    // string constructor
    CBigInt (const char * num);
    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    CBigInt operator + (const CBigInt & num) const;
    friend CBigInt operator + (int l, CBigInt & r);
    friend CBigInt operator + (const char * l, CBigInt & r);
    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    CBigInt operator * (const CBigInt & num) const;
    friend CBigInt operator * (int l, CBigInt & r);
    friend CBigInt operator * (const char * l, CBigInt & r);
    // operator +=, any of {CBigInt/int/string}
    CBigInt operator += (const CBigInt & num);
    // operator *=, any of {CBigInt/int/string}
    CBigInt operator *= (const CBigInt & num);
    // comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    bool operator == (const CBigInt & cBigInt) const;
    friend bool operator == (int l, const CBigInt & r);
    friend bool operator == (const char * l, const CBigInt & r);
    bool operator <= (const CBigInt & cBigInt) const;
    friend bool operator <= (int l, const CBigInt & r);
    friend bool operator <= (const char * l, const CBigInt & r);
    bool operator < (const CBigInt & cBigInt) const;
    friend bool operator < (int l, const CBigInt & r);
    friend bool operator < (const char * l, const CBigInt & r);
    bool operator >= (const CBigInt & cBigInt) const;
    friend bool operator >= (int l, const CBigInt & r);
    friend bool operator >= (const char * l, const CBigInt & r);
    bool operator > (const CBigInt & cBigInt) const;
    friend bool operator > (int l, const CBigInt & r);
    friend bool operator > (const char * l, const CBigInt & r);
    bool operator != (const CBigInt & cBigInt) const;
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

/**
 * Constructs CBigInt from int.
 * When storing as binary, needs to be converted from decimal to binary
 * @param num - decimal value in string
 */
CBigInt::CBigInt(const int & num) {
    int tmp = num;

    if (num == 0)
        number.push_back(0);

    if (num < 0) {
        if (num == -2147483648){
            *this = "-2147483648";
            return;
        }
        positiveSign = false;
        tmp = -1 * num;
    }

    while (tmp > 0){
        number.push_back(tmp % BASE);
        if (BASE == 2)
            tmp >>= 1;
        else
            tmp = tmp / BASE;
    }
}

/**
 * Constructs CBigInt from string.
 * If error occurs in string, throws an exception.
 * When storing as binary, needs to be converted from decimal to binary
 * @param num - decimal value in string
 */
CBigInt::CBigInt(const char *num) {
    string clearNum;
    bool signPlace = true, skipWS = true;

    for (int i = 0; num[i] != '\0'; i++){
        if (skipWS &&
                (num[i] == ' ' || num[i] == '\f' || num[i] == '\n' || num[i] == '\r' || num[i] == '\t' || num[i] == '\v' ))
            continue;

        if (signPlace && (num[i] == '-')) {
            positiveSign = false;
            skipWS = false;
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

    string binary = (BASE == 2 ? bcdReverse(clearNum) : clearNum);

    for (auto rit = binary.rbegin(); rit != binary.rend(); rit++){
        char tmp = *rit;
        string tmp2;
        tmp2 += tmp;
        int a = atoi(tmp2.c_str());
        this->number.push_back(a);
    }

    removeStartingZero();
    if (number.size() == 1 && number[0] == 0)
        positiveSign = true;
}

/**
 * Sums 2 numbers
 * @param num - 2. number
 * @return result of sum
 */
CBigInt CBigInt::operator+(const CBigInt &num) const {
    CBigInt result;
    result.number.pop_back();
    bool subtraction = false;

    const CBigInt * bigger =  (gteAbs(num) ? this : &num);
    const CBigInt * smaller =  (!gteAbs(num) ? this : &num);

    /// handles sign
    if (!this->positiveSign && !num.positiveSign) {
        result.positiveSign = false;
    }
    if (!this->positiveSign && num.positiveSign) {
        result.positiveSign = !gteAbs(num);
        subtraction = true;
    }
    if (this->positiveSign && !num.positiveSign) {
        result.positiveSign = gteAbs(num);
        subtraction = true;
    }

    auto biggerIt  = bigger->number.begin();
    auto smallerIt = smaller->number.begin();
    char add = 0;

    /// classical school addition / subtraction
    for (; biggerIt != bigger->number.end(); biggerIt++) {
        char sum;

        sum = add + *biggerIt;
        if (subtraction)
            sum -= (smallerIt == smaller->number.end() ? 0 : *smallerIt);
        else
            sum += (smallerIt == smaller->number.end() ? 0 : *smallerIt);

        add = ((add == 1) || (add == -1) ? 0 : add);

        /// overflow
        if (sum > BASE - 1) {
            sum -= BASE;
            add = 1;
        }
        if (sum < 0) {
            sum += BASE;
            add = -1;
        }

        result.number.push_back(sum);

        if (smallerIt != smaller->number.end())
            smallerIt++;
    }

    if (add > 0)
        result.number.push_back(add);

    result.removeStartingZero();
    if (result.number.size() == 1 && result.number[0] == 0)
        result.positiveSign = true;

    return result;
}

/**
 * Multiples 2 numbers. this number and another
 * @param num - 2. number for multiplication
 * @return result of multiplication
 */
CBigInt CBigInt::operator * (const CBigInt &num) const {
    CBigInt result;

    if ((this->positiveSign && !num.positiveSign) || (!this->positiveSign && num.positiveSign))
        result.positiveSign = false;

    CBigInt tmp = *this;
    if (!this->positiveSign && !num.positiveSign)
        tmp.positiveSign = true;

    for (auto it: num.number){
        for (int i = 0; i < it; i++){
            result += tmp;
        }

        tmp.number.insert(tmp.number.begin(), 0);
    }

    if (result.number.size() == 1 && result.number[0] == 0)
        result.positiveSign = true;

    return result;
}

/// BOOOORING
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
        return !gteAbs(cBigInt) || *this == cBigInt;
        /*if (this->number.size() == cBigInt.number.size()){
            auto itCB = cBigInt.number.rbegin();
            for (auto it = this->number.rbegin(); it != this->number.rend(); it++){
                if (*it > *itCB)
                    return false;
                if (*it < *itCB)
                    return true;
                itCB ++;
            }
            return true;
        }
        return this->number.size() < cBigInt.number.size();*/
    }

    if (!this->positiveSign && !cBigInt.positiveSign ){
        return gteAbs(cBigInt) || *this == cBigInt;
        /*if (this->number.size() == cBigInt.number.size()){
            auto itCB = cBigInt.number.rbegin();
            for (auto it = this->number.rbegin(); it != this->number.rend(); it++){
                if (*it < *itCB)
                    return false;
                if (*it > *itCB)
                    return true;
                itCB ++;
            }
            return true;
        }
        return this->number.size() > cBigInt.number.size();*/
    }

    return false;
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

/**
 * Writes number in decimal. If is stored inside class in binary, needs to be converted
 * @param os - stream, where number will be printed
 * @param cBigInt - number to be printed
 * @return os - original stream
 */
ostream &operator<<(ostream &os, const CBigInt &cBigInt) {
    if (BASE == 2)
        os << (cBigInt.positiveSign ? "" : "-") << cBigInt.bcd();
    else{
        os << (cBigInt.positiveSign ? "" : "-");
        for (auto it = cBigInt.number.rbegin(); it != cBigInt.number.rend(); it++) {
            string c;
            c += to_string(*it);
            os << c;
        }
    }
    return os;
}

/**
 * Loads decimal number as string from stream and stores to @class CBigInt.
 * Sets failbit when error occurs - number couldn't load (on input just "-" or "abc").
 * When bad character occurs, stops loading.
 * @param is -
 * @param cBigInt
 * @return is - original stream
 */
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

    if (!correctEntry) {
        is.setstate(ios::failbit);
        return is;
    }
    if (negativeSign)
        clearNum = "-" + clearNum;

    cBigInt = clearNum.c_str();

    return is ;
}

string CBigInt::bcd() const{            // fixme -- fucking slow
    vector<int> decimalOrder;
    decimalOrder.push_back(0);

    for (auto rit = number.rbegin(); rit != number.rend(); rit++){
        int x = *rit;

        for (int & i : decimalOrder){
            if (i >= 5)
                i += 3;
        }

        if (decimalOrder.back() >= 8)
            decimalOrder.push_back(0);

        int shift = x;
        for (int & i : decimalOrder){
            i = (i << 1) + shift;
            shift = 0;

            if (i >= 16){
                i -= 16;
                shift = 1;
            }
        }
    }

    string res;
    for (auto rit = decimalOrder.rbegin(); rit != decimalOrder.rend(); rit++){
        res += to_string(*rit);
    }

    return res;
}

/**
 * Decides, if is this absolute value of number greater then or equal.
 * @param another - 2. number to be compared
 * @return true, if bigger abs. val. Else false
 */
bool CBigInt::gteAbs(const CBigInt &another) const {
    if (this->number.size() == another.number.size()){
        auto itAnother = another.number.rbegin();
        for (auto it = this->number.rbegin(); it != this->number.rend(); it++){
            if (*it < *itAnother)
                return false;
            if (*it > *itAnother)
                return true;
            itAnother ++;
        }
        return true;
    }
    return this->number.size() > another.number.size();}

string CBigInt::bcdReverse(const string &decimal) const {       // fixme -- fucking slow
    vector<int> decimalOrder;
    string res;

    for (auto it : decimal) {
        char tmp = it;
        string tmp2;
        tmp2 += tmp;
        decimalOrder.push_back(atoi(tmp2.c_str()));
    }

    for (unsigned long _ = 0; _ < ((decimalOrder.size()*4)); _++){
        bool shiftIn = false;

        for (int & i : decimalOrder){
            bool shiftOut (i % 2 == 1);

            i = (i >> 1) + (shiftIn ? 8 : 0);
            shiftIn = shiftOut;
        }

        for (int & i : decimalOrder){
            if (i >= 8)
                i -= 3;
        }

        res += (shiftIn ? "1" : "0");
    }

    reverse(res.begin(), res.end());
    return res;
}

/**
 * Removes useless strating zeros in numbers abs. value
 */
void CBigInt::removeStartingZero() {
    for (auto it = this->number.rbegin(); it != this->number.rend(); it++){
        if (number.size() == 1)
            return;

        if (*it == 0){
            number.pop_back();
            continue;
        }
        break;
    }
}


#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char * val )
{
  ostringstream oss;
  oss << x;
  return oss . str () == val;
}
int main ( )
{
    /* ============= GIVEN TESTS ============ */

    CBigInt a, b;
    istringstream is;
    a = 10;
    a += 20;
    assert ( equal ( a, "30" ) );
    a *= 5;
    assert ( equal ( a, "150" ) );
    b = a + 3;
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
    a = 42;
    try
    {
        a = "-xyz";
        assert ( "missing an exception" == NULL );
    }
    catch ( const invalid_argument & e )
    {
        assert ( equal ( a, "42" ) );
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

    /* ============ MY TESTS ========== */

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

    c = "55" + d;
    assert(c == "500");

    //c.speedTest();
    // c = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    //c.speedTestBcd();
    //c = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    //c = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    //c = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    //c = c * c;
    //d = c + c;

  return 0;
}
#endif /* __PROGTEST__ */
