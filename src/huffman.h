#ifndef __HUFFMAN_CODE_H__
#define __HUFFMAN_CODE_H__

#include "osg.h"
#include <vector>
#include <map>
using namespace std;
/*
    common words frequency
    e a i r t o n s l c u p m d h g b y f v w k x z q j 1 2 3 4 5 6 7 8 9 0 . - _ 
*/

struct SFreq
{
    int nfreq;
    int cchar;
};

struct Encoded
{
    Encoded():value(), bits(0){}
    DWORD value;
    int   bits;
};

class Node
{
public:
	Node() {
        n_Parent = n_Left = n_Right = -1;
        n_freq = 0;
        c_char = 0;
    }
    ~Node(){
    }
	int     c_char;
	int     n_Parent, n_Left, n_Right;
    int     n_freq;
    int     n_index;
};



class Hufmann
{
public:
    Hufmann();
    ~Hufmann(){
        
    }
    BYTE* encode(KXCHAR*, int& length);
    XCHAR* decode(const BYTE*, int length);
private:
    void    Append(BYTE* ptr, BYTE value, int length);

    Node* _getNode(int c, int fa);

    vector<Node>        _tree;
    map<XCHAR, Encoded>  _encodes;
    int                 _root;
    int                 _byoffset;
    int                 _bioffset;
};


#endif //

