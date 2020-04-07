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
#include <set>
#include <list>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST */
using namespace std;

#ifndef __PROGTEST__
class CTimeStamp
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int sec;
  public:
                   CTimeStamp                              ( int               year,
                                                             int               month,
                                                             int               day,
                                                             int               hour,
                                                             int               minute,
                                                             int               sec );
    int            Compare                                 ( const CTimeStamp & x ) const;
    friend ostream & operator <<                           ( ostream          & os,
                                                             const CTimeStamp & x );
};

CTimeStamp::CTimeStamp(int year, int month, int day, int hour, int minute, int sec) :
    year(year), month(month), day(day), hour(hour), minute(minute), sec(sec) {}

int CTimeStamp::Compare(const CTimeStamp &x) const {
    if (tie(year, month, day, hour, minute, sec) < tie(x.year, x.month, x.day, x.hour, x.minute, x.sec))
        return -1;
    if (tie(year, month, day, hour, minute, sec) > tie(x.year, x.month, x.day, x.hour, x.minute, x.sec))
        return 1;
    return 0;
}

ostream &operator<<(ostream &os, const CTimeStamp &x) {
    os << setw(4) << setfill('0') << x.year << "-" << setw(2) << x.month << "-" << x.day << " " << x.hour << ":" << x.minute << ":" << x.sec;

    return os;
}

//=================================================================================================
class CMailBody
{
  public:
                   CMailBody                               ( int               size,
                                                             const char      * data );
     // copy cons/op=/destructor is correctly implemented in the testing environment
    friend ostream & operator <<                           ( ostream         & os,
                                                             const CMailBody & x )
    {
      return os << "mail body: " << x . m_Size << " B";
    }
  private:
    int            m_Size;
    string         m_Data;
};

CMailBody::CMailBody(int size, const char *data) : m_Size(size) {
    m_Data = "";

    for (int i = 0; i < size; i++){
        m_Data += data[i];
    }
}

//=================================================================================================
class CAttach
{
  public:
                   CAttach                                 ( int               x )
      : m_X (x),
        m_RefCnt ( 1 )
    {
    }
    void           AddRef                                  ( void ) const 
    { 
      m_RefCnt ++; 
    }
    void           Release                                 ( void ) const 
    { 
      if ( !--m_RefCnt ) 
        delete this; 
    }
  private:
    int            m_X;
    mutable int    m_RefCnt;
                   CAttach                                 ( const CAttach   & x );
    CAttach      & operator =                              ( const CAttach   & x );
                  ~CAttach                                 ( void ) = default;
    friend ostream & operator <<                           ( ostream         & os,
                                                             const CAttach   & x )
    {
      return os << "attachment: " << x . m_X << " B";
    }
};
//=================================================================================================
#endif /* __PROGTEST__, DO NOT remove */


class CMail
{
  public:
                   CMail                                   ( const CTimeStamp & timeStamp,
                                                             string  from,
                                                             CMailBody  body,
                                                             const CAttach    * attach );
    const string     & From                                ( void ) const;
    const CMailBody  & Body                                ( void ) const;
    const CTimeStamp & TimeStamp                           ( void ) const;
    const CAttach* Attachment                              ( void ) const;

    friend ostream & operator <<                           ( ostream          & os,
                                                             const CMail      & x );
  private:
    string from;
    CMailBody body;
    CTimeStamp timeStamp;
    const CAttach * cAttach;
};

CMail::CMail(const CTimeStamp &timeStamp, string from, CMailBody body, const CAttach *attach) :
    from(std::move(from)), body(std::move(body)), timeStamp(timeStamp){
    cAttach = attach;
    if (attach)
        cAttach->AddRef();
}

const string &CMail::From(void) const {
    return from;
}

const CMailBody &CMail::Body(void) const {
    return body;
}

const CTimeStamp &CMail::TimeStamp(void) const {
    return timeStamp;
}

const CAttach *CMail::Attachment(void) const {
    return cAttach;
}

ostream &operator<<(ostream &os, const CMail &x) {
    os << x.timeStamp << " " << x.from << " " <<  x.body;
    if (x.cAttach)
        os << " + " << *x.cAttach;

    return os;
}

struct compare {
    bool operator()(const CMail &lhs,
                    const CMail &rhs) const {
        int res = lhs.TimeStamp().Compare(rhs.TimeStamp());
        return res == -1 || res == 0;
    }
};

//=================================================================================================
class CMailBox
{
  public:
                   CMailBox                                ( void );
    bool           Delivery                                ( const CMail      & mail );
    bool           NewFolder                               ( const string     & folderName );
    bool           MoveMail                                ( const string     & fromFolder,
                                                             const string     & toFolder );
    list<CMail>    ListMail                                ( const string     & folderName,
                                                             const CTimeStamp & from,
                                                             const CTimeStamp & to ) const;
    set<string>    ListAddr                                ( const CTimeStamp & from,
                                                             const CTimeStamp & to ) const;
  private:
    // todo
    //map <string, vector<CMail> > folders;
    map <string, multiset<CMail, compare> > folders;
};

CMailBox::CMailBox(void) {
    multiset<CMail, compare> a;
    folders.insert({"inbox", a});
}

bool CMailBox::Delivery(const CMail &mail) {
    folders["inbox"].insert(mail);

    return true;
}

bool CMailBox::NewFolder(const string &folderName) {
    multiset<CMail, compare> a;
    folders.insert({folderName, a});

    return true;
}

bool CMailBox::MoveMail(const string &fromFolder, const string &toFolder) {
    return false;
}

list<CMail> CMailBox::ListMail(const string &folderName, const CTimeStamp &from, const CTimeStamp &to) const {
    list<CMail> res;

    auto begin = folders.find(folderName)->second.lower_bound(CMail(from, "", CMailBody(5, "nullptr"), nullptr));
    auto end = folders.find(folderName)->second.lower_bound(CMail(to, "", CMailBody(5, "nullptr"), nullptr));


    for (; begin != end; begin++){
        //cout << *begin << endl;
        CMail tmp = *begin;
        res.push_back(tmp);
    }

    return res;
}

set<string> CMailBox::ListAddr(const CTimeStamp &from, const CTimeStamp &to) const {
    return set<string>();
}

//=================================================================================================
#ifndef __PROGTEST__
static string showMail ( const list<CMail> & l )
{
  ostringstream oss;
  for ( const auto & x : l ) {
      oss << x << endl;
      cout << x << endl;
  }
  return oss . str ();
}
static string showUsers ( const set<string> & s )
{
  ostringstream oss;
  for ( const auto & x : s )
    oss << x << endl;
  return oss . str ();
}
int main ( void )
{
  list<CMail> mailList;
  set<string> users;
  CAttach   * att;

  CMailBox m0;
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), nullptr ) ) );
  att = new CAttach ( 200 );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), att ) ) );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> Release ();
  att = new CAttach ( 97 );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), att ) ) );
  att -> Release ();
  assert ( showMail ( m0 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n" );
  assert ( showMail ( m0 . ListMail ( "inbox",
                      CTimeStamp ( 2014, 3, 31, 15, 26, 23 ),
                      CTimeStamp ( 2014, 3, 31, 16, 12, 48 ) ) ) == "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n" );
  /*assert ( showUsers ( m0 . ListAddr ( CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                       CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );
  assert ( showUsers ( m0 . ListAddr ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ),
                       CTimeStamp ( 2014, 3, 31, 16, 12, 48 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );

  CMailBox m1;
  assert ( m1 . NewFolder ( "work" ) );
  assert ( m1 . NewFolder ( "spam" ) );
  assert ( !m1 . NewFolder ( "spam" ) );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  att = new CAttach ( 500 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), att ) ) );
  att -> Release ();
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), nullptr ) ) );
  att = new CAttach ( 468 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> Release ();
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), nullptr ) ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( m1 . MoveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 19, 24, 13 ), "user2@fit.cvut.cz", CMailBody ( 14, "mail content 4" ), nullptr ) ) );
  att = new CAttach ( 234 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 13, 26, 23 ), "user3@fit.cvut.cz", CMailBody ( 9, "complains" ), att ) ) );
  att -> Release ();
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . MoveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n"
                        "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );
/**/
  return 0;
}
#endif /* __PROGTEST__ */
