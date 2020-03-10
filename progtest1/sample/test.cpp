#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>

using namespace std;
#endif /* __PROGTEST__ */
#include <istream>
#include <fstream>


class ibitstream {
private:
    int lengthBuff;
    unsigned char buffer;
    std::istream& stream;
public:
    explicit ibitstream(std::istream&);
    ibitstream& get(unsigned int, unsigned int&);
    unsigned int get(unsigned int);

    bool eof();
};

ibitstream::ibitstream(std::istream& in) : lengthBuff(0), buffer(0), stream(in){ }

ibitstream& ibitstream::get(unsigned int bits, unsigned int& data) {
    data = get(bits);
    return *this;
}

unsigned int ibitstream::get(unsigned int bits) {
    unsigned int res = 0;

    for (unsigned int i = 0; i < bits; i++) {
        if(lengthBuff == 0) {
            buffer = stream.get();
            if (stream.eof())
                return res;
            lengthBuff += 8;
        }
        if(lengthBuff > 0) {
            lengthBuff --;
            unsigned int b = buffer >> 7u;
            buffer <<= 1u;

            res <<= 1u;
            res += b;
        }
    }
    //cout << res << endl;
    return res;
}

bool ibitstream::eof() {
    return stream.eof();
}

class tableNode {
private:
    bool leaf;
    unsigned value = 0;
    ibitstream & stream;
    tableNode * left = nullptr;
    tableNode * right = nullptr;

    bool utfDecode (unsigned & res);
public:
    explicit tableNode (ibitstream & ibitstream, bool & ok);
    ~tableNode();
    bool find (unsigned & data);
};

tableNode::tableNode (ibitstream & ibitstream, bool & ok) : stream(ibitstream){
        if (ibitstream.get(1) == 1 ){
            // value = ibitstream.get(8);
            ok = utfDecode(value);                                   // TODO validace
            leaf = true;
            return;
        }
        leaf = false;
        left = new tableNode (ibitstream, ok);
        right = new tableNode (ibitstream, ok);
}

tableNode::~tableNode(){
    delete left;
    delete right;
}

bool tableNode::utfDecode (unsigned & res){     // TODO mozna predelat jen na 4 bajtovy UTF8
    int i = 0;
    while (true){
        unsigned u = 0;
        u = stream.get(1);
        if (u == 0)
            break;
        i++;
    }

    res = stream.get((i == 0 ? 7 : 8 - i - 1));
    for ( int j = 1; j < i; j++){
        res = res << 6u;
        if (stream.get(1) != 1 || stream.get(1) !=0)
            return false;
        res = res + stream.get(6);
    }
    return true;
}

bool tableNode::find (unsigned & data){
    if (leaf){
        data = value;
        return true;
    }

    unsigned c = stream.get(1);

    if ( c == 0 && left->find(data))
        return true;
    return c == 1 && right->find(data);
}

bool utfEncode (std::ofstream & stream, unsigned value){
    if (value <= 127){
        stream << (char) value;
    }
    else if (value > 127 && value <= 2047){
        stream << (char)((value >> 6u) + 192) << (char)((value & 63u) + 128);
    }
    else if (value > 2047 && value <= 65535){
        stream << (char)((value >> 12u) + 224) << (char)(((value >> 6u) & 63u) + 128) << (char)((value & 63u) + 128);
    }
    else if (value > 65535 && value <= 2097151){
        stream << (char)((value >> 18u) + 240) << (char)(((value >> 12u) & 63u) + 128) << (char)(((value >> 6u) & 63u) + 128) << (char)((value & 63u) + 128);
    }
    else if (value > 2097151 && value <= 67108863){
        stream << (char)((value >> 24u) + 248) << (char)(((value >> 18u) & 63u) + 128) << (char)(((value >> 12u) & 63u) + 128) << (char)(((value >> 6u) & 63u) + 128) << (char)((value & 63u) + 128);
    }
    else if (value > 67108863 && value <= 2147483647){
        stream << (char)((value >> 30u) + 252) << (char)(((value >> 24u) & 63u) + 128) << (char)(((value >> 18u) & 63u) + 128) << (char)(((value >> 12u) & 63u) + 128) << (char)(((value >> 6u) & 63u) + 128) << (char)((value & 63u) + 128);
    }
    else
        return false;
    return true;
}

bool decompressFile ( const char * inFileName, const char * outFileName )
{
    std::ifstream inFile (inFileName, std::ifstream::in | std::ifstream::binary);
    std::ofstream outFile (outFileName, std::ios::out | std::ios::binary);

    if(!inFile.is_open() || !outFile.is_open())
        return false;

    bool ok = true;
    ibitstream myStream(inFile);                                                // BUG?
    tableNode decodeTable(myStream, ok);

    if (! ok)
        return false;

    unsigned charCount;

    while(true){
        unsigned character, c = myStream.get(1);

        charCount = ( c == 1 ? 4096 : myStream.get(12));

        for (unsigned i = 0; i < charCount; i++){
            if( ! decodeTable.find(character) )                                 // BUG?
                return false;
            // cout << "|" << character ;
            // outFile << (char) character;
            utfEncode(outFile, character);                                          // BUG? -- spis ne ...
            if (myStream.eof())
                return false;
        }
        if (myStream.eof())
            return false;

        if (c == 0)
            break;
    }

    inFile.close();
    outFile.close();

  return true;
}

bool compressFile ( const char * inFileName, const char * outFileName )
{
  // keep this dummy implementation (no bonus) or implement the compression (bonus)
  return false;
}

#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
    std::ifstream rightFile(fileName2, std::ifstream::in | std::ifstream::binary);
    std::ifstream leftFile(fileName1, std::ifstream::in | std::ifstream::binary);

    if(!leftFile.is_open() || !rightFile.is_open())
        return false;

    const int BUFFER_SIZE = 1024;

    char *rBuff = new char[BUFFER_SIZE]();
    char *lBuff = new char[BUFFER_SIZE]();

    do {
        leftFile.read(lBuff, BUFFER_SIZE);
        rightFile.read(rBuff, BUFFER_SIZE);

        if (std::memcmp(lBuff, rBuff, BUFFER_SIZE) != 0)
        {
            delete[] lBuff;
            delete[] rBuff;
            return false;
        }
    } while (leftFile.good() || rightFile.good());

    delete[] lBuff;
    delete[] rBuff;
    return true;
}

int main ( void )
{
    //assert ( identicalFiles ( "bin.huf", "bin.huf" ) );
    //assert ( identicalFiles ( "origo.orig", "origo2.orig" ) );
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );

  assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );

  assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

  return 0;
}
#endif /* __PROGTEST__ */
