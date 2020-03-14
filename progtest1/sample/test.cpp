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

/**
 * represents bitstream library
 */
class ibitstream {
private:
    int lengthBuff;                                                         /// number of yet unloaded bits of byte
    unsigned char buffer;                                                   /// 1 loaded byte
    std::istream& stream;                                                   /// stream, from which we're loading
public:
    explicit ibitstream(std::istream&);
    unsigned int get(unsigned int);
    bool eof();
};

/**
 * Constructor of class ibitstream
 * @param in Stream, from which will be bits loaded
 */
ibitstream::ibitstream(std::istream& in) : lengthBuff(0), buffer(0), stream(in){ }

/**
 * Method loads required amount of bits from stream. If there is EOF, method returns so far loaded bits and ends.
 * @param bits Number of bits wanted to load from stream.
 * @return 10-based number loaded from stream by bits.
 */
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
    // cout << res << endl;
    return res;
}

/**
 * Method returns, if there is eof
 * @return true, if there is end of file, else false.
 */
bool ibitstream::eof() {
    return stream.eof();
}

/**
 * class represents 1 node of Huffmann tree
 */
class tableNode {
private:
    bool leaf;                                                                  /// true, if node is a leaf
    unsigned value = 0;                                                         /// value of utf8 character
    ibitstream & stream;                                                        /// stream, from which bits are loaded
    tableNode * left = nullptr;                                                 /// left subtree, default nullptr
    tableNode * right = nullptr;                                                /// right subtree, default nullptr

    bool utfDecode (unsigned & res);
public:
    explicit tableNode (ibitstream & ibitstream, bool & ok);
    ~tableNode();
    bool huffmanDecode (unsigned & data);
};

/**
 * Constructor recursively creates another nodes according to ibitstream.
 * When loaded '0' -> not leaf, has subtrees and continue creating new nodes. When loaded '1' -> leaf, no subtrees.
 *      Loads utf8 value and stores it to 'value'
 * If any fail from utf8 load or stream, then sets control bit 'ok' to false and ends.
 * @param ibitstream Stream, from which are bits loaded
 * @param ok Control bit. Sets true, if everything is ok, else false.
 */
tableNode::tableNode (ibitstream & ibitstream, bool & ok) : stream(ibitstream){
        if (ibitstream.eof() || !ok){
            ok = false;
            return;
        }

        if (ibitstream.get(1) == 1 ){
            ok = utfDecode(value);
            if (stream.eof() || !ok) return;
            leaf = true;
            return;
        }

        leaf = false;
        left = new tableNode (ibitstream, ok);
        right = new tableNode (ibitstream, ok);
}

/**
 * Destructor. Deletes recursively subtrees.
 */
tableNode::~tableNode(){
    delete left;
    delete right;
}

/**
 * Method try to load 1 utf8 character from 'stream'. If error occurs, returns false.
 * Accepts only utf8 4-byte standard.
 * @param res Final value of decoded utf8 bytes are stored here.
 * @return false, load from stream fails or utf format is wrong. Else returns true.
 */
bool tableNode::utfDecode (unsigned & res){
    int onBits = 0;
    if (stream.eof()) return false;

    while (true){
        unsigned bit = stream.get(1);
        if (stream.eof()) return false;
        if (bit == 0) break;
        onBits++;
    }
    if (onBits > 4 || onBits == 1) return false;

    res = stream.get((onBits == 0 ? 7 : 8 - onBits - 1));
    if (stream.eof()) return false;

    for (int j = 1; j < onBits; j++){
        res = res << 6u;
        unsigned bit1 = stream.get(1);
        if (stream.eof()) return false;

        unsigned bit2 = stream.get(1);
        if (stream.eof() || bit1 != 1 || bit2 !=0) return false;

        res = res + stream.get(6);
        if (stream.eof()) return false;
    }
    return true;
}

/**
 * Method finds value stored in Huffman tree according to Huffman code from 'stream'.
 * Loads bit from 'stream'. When '0'/'1', goes to left/right subtree respectively.
 *      If node is leaf, stores value to @param data and returns.
 * @param data Stores decoded Huffman value here.
 * @return false, if error occurs (code not found, input error). Else returns true.
 */
bool tableNode::huffmanDecode (unsigned & data){
    if (leaf){
        data = value;
        return true;
    }
    if (stream.eof()) return false;

    unsigned leafBit = stream.get(1);
    if (stream.eof()) return false;

    if (leafBit == 0 && left->huffmanDecode(data))
        return true;
    return leafBit == 1 && right->huffmanDecode(data);
}

/**
 * Function encodes @param value to utf8 4-byte standard and writes it to @param stream.
 * @param stream stream, where encoded value will be written.
 * @param value Value to encode
 * @return false, if error while loading from stream. Else true.
 */
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

    return !stream.fail();

}

/**
 * Function decodes text from huffman encoded file and writes it into 2. file in utf8 encoding.
 * If files are successfully opened, loads huffman tree from @param inFileName. Then reads code from chunks.
 *      Each chunk is prefixed by its length. Bit '1' means 4096 encoded characters.
 *      Bit '0' followed by 12-bit stored length (0 - 4095)
 * @param inFileName Name of file with Huffman encode message.
 * @param outFileName Name of file, where message from @param inFileName will be written in utf8.
 * @return fail, if error occurs (I/O error, utf8 error, huffman error, ...). Else true.
 */
bool decompressFile ( const char * inFileName, const char * outFileName )
{
    ifstream inFile(inFileName,ios::binary|ios::in);
    ofstream outFile(outFileName,ios::binary|ios::out);

    if (!outFile.is_open() || outFile.fail()) return false;
    if (!inFile.is_open() || inFile.fail()) return false;

    bool ok = true;
    ibitstream myStream(inFile);
    tableNode decodeTable(myStream, ok);

    if (! ok)
        return false;

    unsigned charCount;
    const int CHUNK_SIZE = 4096;

    while(true){
        unsigned character, chunkBit = myStream.get(1);
        if (myStream.eof()) return false;

        charCount = (chunkBit == 1 ? CHUNK_SIZE : myStream.get(12));
        if (myStream.eof()) return false;

        for (unsigned i = 0; i < charCount; i++){
            if(!decodeTable.huffmanDecode(character)) return false;

            ok = utfEncode(outFile, character);
            if (!ok || myStream.eof()) return false;
        }

        if (chunkBit == 0)
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

    /* =================== VLASTNI TESTY ====================*/

    assert ( ! decompressFile ( "tests/mytest1", "tempfile" ) );                  // Prazdny file
    assert ( ! decompressFile ( "tests/mytest2", "tempfile2" ) );                 // validni soubor + tempfile bez prava na zapisovani
    assert ( identicalFiles ( "bin.huf", "bin.huf" ) );                           // testovani identical file fce
    assert ( identicalFiles ( "origo.orig", "origo2.orig" ) );                    // testovani identical file fce
    assert ( ! decompressFile ( "tests/in_3502985.bin", "tempfile" ) );           // File s neplatnym utf8 -- 1. bajt zacina na '10'

    return 0;
}
#endif /* __PROGTEST__ */
