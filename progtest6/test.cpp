#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */


class CCell {
public:
    int height, width;
    CCell ();
    virtual ~CCell();
    virtual string GetRow (int rowNum, int fill, int cellHeight);
};

CCell::CCell() : height (0), width(0) {}

CCell::~CCell() {}

string CCell::GetRow(int rowNum, int fill, int cellHeight) {
    ostringstream oss;
    oss << setw(fill) << setfill(' ') << "XXX";
    return oss.str();
}

/* ****************************** */

class CText : public CCell
{
public:
    enum ALIGN {ALIGN_LEFT, ALIGN_RIGHT};
    CText (string text, ALIGN align);
    void SetText (string text);
    string GetRow (int rowNum, int fill, int cellHeight);
private:
    vector<string> textRow;
    ALIGN textAlign;
};

CText::CText(string text, ALIGN align) : textAlign(align){
    int start = 0;
    for (int end = 0; end < text.length(); end++){
        if (text[end] == '\n'){
            textRow.push_back(text.substr(start, end - start));
            if (textRow.back().length() > width)
                width = textRow.back().length();
            height ++;
            start = end + 1;
        }
        else if ((end + 1 == text.length())){
            textRow.push_back(text.substr(start, end - start + 1));
            if (textRow.back().length() > width)
                width = textRow.back().length();
            height ++;
            start = end + 1;
        }
    }
}

void CText::SetText(string text) {
    textRow.clear();
    height = 0;
    width = 0;

    int start = 0;
    for (int end = 0; end < text.length(); end++){
        if (text[end] == '\n'){
            textRow.push_back(text.substr(start, end - start));
            if (textRow.back().length() > width)
                width = textRow.back().length();
            height ++;
            start = end + 1;
        }
        else if ((end + 1 == text.length())){
            textRow.push_back(text.substr(start, end - start + 1));
            if (textRow.back().length() > width)
                width = textRow.back().length();
            height ++;
            start = end + 1;
        }
    }
}

string CText::GetRow(int rowNum, int fill, int cellHeight) {
    ostringstream oss;
    if (rowNum >= height){
        oss << setw(fill) << setfill(' ') << "";
        return oss.str();
    }
    if (textAlign == ALIGN_LEFT)
        oss << left << setw(fill) << setfill(' ') << textRow[rowNum];
    else
        oss << right << setw(fill) << setfill(' ') << textRow[rowNum];

    return oss.str();
}

/* ****************************** */

class CEmpty : public CCell
{
public:
    CEmpty ();
    string GetRow (int rowNum, int fill, int cellHeight);
};

CEmpty::CEmpty() : CCell(){}

string CEmpty::GetRow(int rowNum, int fill, int cellHeight) {
    ostringstream oss;
    oss << setw(fill) << setfill(' ') << "";
    return oss.str();
};

/* ****************************** */

class CImage : public CCell
{
private:
    vector<string> image;
public:
    CImage ();
    CImage & AddRow (string row);
    string GetRow (int rowNum, int fill, int cellHeight);
};

CImage::CImage() : CCell() {}

CImage &CImage::AddRow(string row) {
    height ++;
    if (image.empty())
        width = row.length();
    image.emplace_back(row);
    return *this;
}

string CImage::GetRow(int rowNum, int fill, int cellHeight) {
    ostringstream oss;
    rowNum -= (cellHeight - height) / 2;
    if (rowNum >= height || rowNum < 0){
        oss << setw(fill) << setfill(' ') << "";
        return oss.str();
    }
    oss << right << setw((fill - width)/2) << setfill(' ') << "" << setw(0) << image[rowNum] << setw((fill - width)/2) << "";

    return oss.str();
}

/* ****************************** */

class CTable
{
  int rows, cols;
  vector< vector<CCell*> > cells;

public:
    CTable (int rows, int cols);
    CCell & SetCell (int row, int col, CText cell);
    CCell & SetCell (int row, int col, CImage cell);
    CCell & SetCell (int row, int col, CEmpty cell);
    CCell & SetCell (int row, int col, CCell & cell);
    CCell & GetCell (int row, int col);
    friend ostream & operator << (ostream & os, const CTable & table);

};

CTable::CTable(int rows, int cols) : rows(rows), cols(cols) {
    for (int i = 0; i < rows; i++){
        vector<CCell *> row;
        for (int j = 0; j < cols; j++){
            row.push_back(new CEmpty());
        }
        cells.push_back(row);
    }
}

CCell &CTable::SetCell(int row, int col, CText cell) {
    delete cells[row][col];
    cells[row][col] = new CText(std::move(cell));

    return *cells[row][col];
}

CCell &CTable::SetCell(int row, int col, CImage cell) {
    delete cells[row][col];
    cells[row][col] = new CImage(std::move(cell));

    return *cells[row][col];
}

CCell &CTable::SetCell(int row, int col, CEmpty cell) {
    return *cells[row][col];
}

CCell &CTable::SetCell(int row, int col, CCell& cell) {
    delete cells[row][col];
    cells[row][col] = &cell;

    return *cells[row][col];
}

CCell &CTable::GetCell(int row, int col) {
    return *cells[row][col];
}

/* ****************************** */

ostream & operator << (ostream & os, const CTable & table){
    vector <int> rowLen;
    vector <int> colWidth;

    rowLen.resize(table.rows, 0);
    colWidth.resize(table.cols, 0);
    for (int i = 0; i < table.rows; i++){
        for (int j = 0; j < table.cols; j++){
            if (table.cells[i][j]->height > rowLen[i])
                rowLen[i] = table.cells[i][j]->height;
            if (table.cells[i][j]->width > colWidth[j])
                colWidth[j] = table.cells[i][j]->width;
        }
    }

    for (auto & i : rowLen)
        cout << i << " ";
    cout << endl;
    for (auto & i : colWidth)
        cout << i << " ";
    cout << endl;

    os << "+";
    for (auto & col : colWidth)
        os << setw(col) << setfill('-') << "" << setw(0) << "+";
    os << endl;

    for (int row = 0; row < table.rows; row ++ ){
        for (int h = 0; h < rowLen[row]; h ++) {
            os << "|";
            for (int col = 0; col < table.cols; col++) {
                os << table.cells[row][col]->GetRow(h, colWidth[col], rowLen[row]) << setw(0) << "|";
            }
            os << endl;
        }

        os << "+";
        for (auto & col : colWidth)
            os << setw(col) << setfill('-') << "" << setw(0) << "+";
        os << endl;
    }

    return os;
}

/* -------------------- */


#ifndef __PROGTEST__
int main ( void )
{
  ostringstream oss;
  CTable t0 ( 3, 2 );
  t0 . SetCell ( 0, 0, CText ( "Hello,\n"
        "Hello Kitty", CText::ALIGN_LEFT ) );
  t0 . SetCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
  t0 . SetCell ( 2, 0, CText ( "Bye,\n"
        "Hello Kitty", CText::ALIGN_RIGHT ) );
  t0 . SetCell ( 1, 1, CImage ()
        . AddRow ( "###                   " )
        . AddRow ( "#  #                  " )
        . AddRow ( "#  # # ##   ###    ###" )
        . AddRow ( "###  ##    #   #  #  #" )
        . AddRow ( "#    #     #   #  #  #" )
        . AddRow ( "#    #     #   #  #  #" )
        . AddRow ( "#    #      ###    ###" )
        . AddRow ( "                     #" )
        . AddRow ( "                   ## " )
        . AddRow ( "                      " )
        . AddRow ( " #    ###   ###   #   " )
        . AddRow ( "###  #   # #     ###  " )
        . AddRow ( " #   #####  ###   #   " )
        . AddRow ( " #   #         #  #   " )
        . AddRow ( "  ##  ###   ###    ## " ) );
  t0 . SetCell ( 2, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
    cout << oss.str() << endl;
  assert ( oss . str () ==
        "+--------------------------+----------------------+\n"
        "|Hello,                    |                      |\n"
        "|Hello Kitty               |                      |\n"
        "+--------------------------+----------------------+\n"
        "|Lorem ipsum dolor sit amet|###                   |\n"
        "|                          |#  #                  |\n"
        "|                          |#  # # ##   ###    ###|\n"
        "|                          |###  ##    #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #      ###    ###|\n"
        "|                          |                     #|\n"
        "|                          |                   ## |\n"
        "|                          |                      |\n"
        "|                          | #    ###   ###   #   |\n"
        "|                          |###  #   # #     ###  |\n"
        "|                          | #   #####  ###   #   |\n"
        "|                          | #   #         #  #   |\n"
        "|                          |  ##  ###   ###    ## |\n"
        "+--------------------------+----------------------+\n"
        "|                      Bye,|                      |\n"
        "|               Hello Kitty|                      |\n"
        "+--------------------------+----------------------+\n" );
  t0 . SetCell ( 0, 1, t0 . GetCell ( 1, 1 ) );
  t0 . SetCell ( 2, 1, CImage ()
        . AddRow ( "*****   *      *  *      ******* ******  *" )
        . AddRow ( "*    *  *      *  *      *            *  *" )
        . AddRow ( "*    *  *      *  *      *           *   *" )
        . AddRow ( "*    *  *      *  *      *****      *    *" )
        . AddRow ( "****    *      *  *      *         *     *" )
        . AddRow ( "*  *    *      *  *      *        *       " )
        . AddRow ( "*   *   *      *  *      *       *       *" )
        . AddRow ( "*    *    *****   ****** ******* ******  *" ) );
  dynamic_cast<CText &> ( t0 . GetCell ( 1, 0 ) ) . SetText ( "Lorem ipsum dolor sit amet,\n"
        "consectetur adipiscing\n"
        "elit. Curabitur scelerisque\n"
        "lorem vitae lectus cursus,\n"
        "vitae porta ante placerat. Class aptent taciti\n"
        "sociosqu ad litora\n"
        "torquent per\n"
        "conubia nostra,\n"
        "per inceptos himenaeos.\n"
        "\n"
        "Donec tincidunt augue\n"
        "sit amet metus\n"
        "pretium volutpat.\n"
        "Donec faucibus,\n"
        "ante sit amet\n"
        "luctus posuere,\n"
        "mauris tellus" );
  oss . str ("");
  oss . clear ();
  oss << t0;
    cout << oss.str() << endl;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  /*CTable t1 ( t0 );
  t1 . SetCell ( 1, 0, CEmpty () );
  t1 . SetCell ( 1, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+-----------+------------------------------------------+\n"
        "|Hello,     |          ###                             |\n"
        "|Hello Kitty|          #  #                            |\n"
        "|           |          #  # # ##   ###    ###          |\n"
        "|           |          ###  ##    #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #      ###    ###          |\n"
        "|           |                               #          |\n"
        "|           |                             ##           |\n"
        "|           |                                          |\n"
        "|           |           #    ###   ###   #             |\n"
        "|           |          ###  #   # #     ###            |\n"
        "|           |           #   #####  ###   #             |\n"
        "|           |           #   #         #  #             |\n"
        "|           |            ##  ###   ###    ##           |\n"
        "+-----------+------------------------------------------+\n"
        "+-----------+------------------------------------------+\n"
        "|       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|           |*    *  *      *  *      *           *   *|\n"
        "|           |*    *  *      *  *      *****      *    *|\n"
        "|           |****    *      *  *      *         *     *|\n"
        "|           |*  *    *      *  *      *        *       |\n"
        "|           |*   *   *      *  *      *       *       *|\n"
        "|           |*    *    *****   ****** ******* ******  *|\n"
        "+-----------+------------------------------------------+\n" );
  t1 = t0;
  t1 . SetCell ( 0, 0, CEmpty () );
  t1 . SetCell ( 1, 1, CImage ()
        . AddRow ( "  ********                    " )
        . AddRow ( " **********                   " )
        . AddRow ( "**        **                  " )
        . AddRow ( "**             **        **   " )
        . AddRow ( "**             **        **   " )
        . AddRow ( "***         ********  ********" )
        . AddRow ( "****        ********  ********" )
        . AddRow ( "****           **        **   " )
        . AddRow ( "****           **        **   " )
        . AddRow ( "****      **                  " )
        . AddRow ( " **********                   " )
        . AddRow ( "  ********                    " ) );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                              |          ###                             |\n"
        "|                                              |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
    /**/
  return 0;
}
#endif /* __PROGTEST__ */
