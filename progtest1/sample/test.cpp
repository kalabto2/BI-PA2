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

class ibitstream {
private:
    int length;
    unsigned char buffer;
    std::istream& stream;

public:
    explicit ibitstream(std::istream&);
    ibitstream& get(unsigned int, unsigned int&);
    unsigned int get(unsigned int);
};

ibitstream::ibitstream(std::istream& in) : length(0), buffer(0), stream(in){ }

ibitstream& ibitstream::get(unsigned int bits, unsigned int& data) {
    data = get(bits);
    return *this;
}

unsigned int ibitstream::get(unsigned int bits) {
    unsigned int acc = 0;

    for (unsigned int i = 0; i < bits; i++) {
        if(length == 0) {
            buffer = stream.get();
            length += 8;
        }
        if(length > 0) {
            length --;
            unsigned int b = buffer >> 7u;
            buffer <<= 1u;

            acc <<= 1u;
            acc += b;
        }
    }
    cout << acc << endl;
    return acc;
}

class tableNode {
private:
    bool leaf;
    unsigned value = 0;
    ibitstream & stream;
    tableNode * left = nullptr;
    tableNode * right = nullptr;

    bool utfDecode (unsigned & res){
        int i = 0;
        while (true){
            unsigned u = stream.get(1);
            if (u == 0)
                break;
            i++;
        }

        switch(i){
            case 0:{
                res = stream.get(7);
                break;
            }
            case 2:{
                unsigned a = stream.get(5);
                a = a << 6u;
                if (stream.get(1) != 1 || stream.get(1) != 0)
                    return false;
                res = a + stream.get(6);
                break;
            }
            case 3:{
                unsigned a = stream.get(4);
                a = a << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                res = a + stream.get(6);
                break;
            }
            case 4:{
                unsigned a = stream.get(3);
                a = a << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                res = a + stream.get(6);
                break;
            }
            case 5:{
                unsigned a = stream.get(2);
                a = a << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                res = a + stream.get(6);
                break;
            }
            case 6:{
                unsigned a = stream.get(1);
                a = a << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                a = (a + stream.get(6)) << 6u;
                if (stream.get(1) != 1 || stream.get(1) !=0)
                    return false;
                res = a + stream.get(6);
                break;
            }
        }
        return true;
    }

public:
    explicit tableNode (ibitstream & ibitstream) : stream(ibitstream){
        if (ibitstream.get(1) == 1 ){
            //
            //
            // value = ibitstream.get(8);  // TODO predelat na UTF-8
            utfDecode(value);
            //cout << "|" << (char)value <<";";
            leaf = true;
            return;
        }
        leaf = false;
        left = new tableNode (ibitstream);
        right = new tableNode (ibitstream);
    }

    ~tableNode(){
        // cout << "zalez!" << endl;
        if (left != nullptr) {
            left->~tableNode();
            delete left;
            left = nullptr;
        }
        if (right != nullptr) {
            right->~tableNode();
            delete right;
            right = nullptr;
        }
    }

    bool find (unsigned & data){
        if (leaf){
            data = value;
            return true;
        }

        unsigned c = stream.get(1);

        if ( c == 0 && left->find(data))
                return true;
        return c == 1 && right->find(data);
    }
};

bool utfEncode (ofstream & stream, unsigned value){
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
        stream << (char)((value >> 24u) + 248) << (char)((value & 63u) + 128) << (char)((value & 63u) + 128) << (char)((value & 63u) + 128) << (char)((value & 63) + 128);
    }
    else if (value > 67108863 && value <= 2147483647){
        stream << (char)((value >> 30u) + 252) << (char)((value & 63) + 128) << (char)((value & 63) + 128) << (char)((value & 63) + 128) << (char)((value & 63) + 128) << (char)((value & 63) + 128);
    }
    return true;
}

bool decompressFile ( const char * inFileName, const char * outFileName )
{
    std::ifstream inFile (inFileName, std::ifstream::in | std::ifstream::binary);
    ofstream outFile (outFileName, ios::out | ios::binary);

    if(!inFile.is_open() || !outFile.is_open())
        return false;

    ibitstream myStream(inFile);
    tableNode decodeTable(myStream);

    unsigned charCount = 0;

    while(true){
        unsigned character, c = myStream.get(1);

        charCount = ( c == 1 ? 4096 : myStream.get(12));

        for (unsigned i = 0; i < charCount; i++){
            decodeTable.find(character);
            // cout << "|" << character ;
            // outFile << (char) character;
            utfEncode(outFile, character);
        }

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
    const int BUFFER_SIZE = 1024;

    std::ifstream lFile(fileName1, std::ifstream::in | std::ifstream::binary);
    std::ifstream rFile(fileName2, std::ifstream::in | std::ifstream::binary);

    if(!lFile.is_open() || !rFile.is_open())
    {
        return false;
    }

    char *lBuffer = new char[BUFFER_SIZE]();
    char *rBuffer = new char[BUFFER_SIZE]();

    do {
        lFile.read(lBuffer, BUFFER_SIZE);
        rFile.read(rBuffer, BUFFER_SIZE);

        if (std::memcmp(lBuffer, rBuffer, BUFFER_SIZE) != 0)
        {
            delete[] lBuffer;
            delete[] rBuffer;
            return false;
        }
    } while (lFile.good() || rFile.good());

    delete[] lBuffer;
    delete[] rBuffer;
    return true;
}

int main ( void )
{

    /*assert ( identicalFiles ( "bin.huf", "bin.huf" ) );
    assert ( identicalFiles ( "origo.orig", "origo2.orig" ) );
  assert ( decompressFile ( "tests/test0.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test0.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test1.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test1.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test2.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test2.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test3.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test3.orig", "tempfile" ) );

  assert ( decompressFile ( "tests/test4.huf", "tempfile" ) );
  assert ( identicalFiles ( "tests/test4.orig", "tempfile" ) );*/

  //assert ( ! decompressFile ( "tests/test5.huf", "tempfile" ) );


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