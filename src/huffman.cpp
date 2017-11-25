// Huffman.cpp: implementation of the Huffman class.
//
//////////////////////////////////////////////////////////////////////

#include "huffman.h"


static int compare(const void *e1, const void *e2 )
{
    return ((SFreq*)e2)->nfreq > ((SFreq*)e1)->nfreq;
}

Node* Hufmann::_getNode(int c, int fa)
{
    vector<Node>::iterator b = _tree.begin();
    vector<Node>::iterator e = _tree.end();
    for(;b!=e;b++)
    {
        Node& n = *b; 
        if(n.c_char==c && n.n_freq==fa)
            return &n;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////
#define TREESZ 96
Hufmann::Hufmann()
{
    int     K = 256;
    Node    nnA,nnB,nnP;
    XCHAR    charset[]  = "eairtonslcupmdhgbyfvwkxzqjEAIRTONSLCUPMDHGBYFVWKXZQJ12346567890';:(){}[]!@#$%^&*~`\\/.,+-_=?\0\0";
    int     nlength    = TREESZ;//(int)str_len(charset);
    SFreq   freqs[TREESZ];

	memset(freqs,0,sizeof(freqs));
    for(int i=0; i < nlength; i++)
    {
        freqs[i].nfreq = nlength-i + 1;
        freqs[i].cchar = charset[i];
    }

    qsort(freqs, TREESZ, sizeof(SFreq), compare);
    int nfreqs = TREESZ;
    do{

        int   a   = freqs[nfreqs-1].cchar;
        int   fa  = freqs[nfreqs-1].nfreq;
        int   b   = freqs[nfreqs-2].cchar;
        int   fb  = freqs[nfreqs-2].nfreq;

        nnP.c_char  = K++;
        nnP.n_index = (int)_tree.size();
        _tree.push_back(nnP);
        
        _root = nnP.n_index;        

        Node* pNA = _getNode(a, fa);
        if(!pNA)
        {
            pNA           = &nnA;
            pNA->n_freq   = fa;
            pNA->c_char   = a;
            pNA->n_index = (int)_tree.size();
            _tree.push_back(*pNA);

            pNA = &_tree[pNA->n_index];
        }
        else
        {
            fa = pNA->n_freq;
        }
        _tree[pNA->n_index].n_Parent = nnP.n_index;
        _tree[nnP.n_index].n_Right   = pNA->n_index;

        Node* pNB = _getNode(b, fb);
        if(!pNB)
        {
            pNB           = &nnB;
            pNB->n_freq   = fb;
            pNB->c_char   = b;
            pNB->n_index  = (int)_tree.size();
            _tree.push_back(*pNB);

            pNB = &_tree[pNB->n_index];
        }
        else
        {
            fb = pNB->n_freq;
        }
        _tree[pNB->n_index].n_Parent = nnP.n_index;
        _tree[nnP.n_index].n_Left    = pNB->n_index;
        

        _tree[nnP.n_index].n_freq  = fa + fb;

        freqs[nfreqs-2].cchar=nnP.c_char;
        freqs[nfreqs-2].nfreq=fa + fb;


        qsort(freqs, nfreqs-1, sizeof(SFreq), compare);

    }while(--nfreqs>1);


    for(vector<Node>::iterator b = _tree.begin(); 
                               b != _tree.end(); b++)
    {
        int ch = b->c_char;
        if(ch < 256 )
        {
            Encoded e;
            
            Node*   n = (Node*)&(*b);
            do{
                Node* np = &_tree[n->n_Parent];
                e.value<<=1;
                if(np->n_Right == n->n_index)
                    e.value|=0x1;
                ++e.bits;
                n = np;
            }while(n->n_Parent != -1);

            _encodes[ch] = e;
        }
    }
}


BYTE* Hufmann::encode(KXCHAR* ps, int& length)
{
    _byoffset    = 0;
    _bioffset    = 0;
    int          bytes = (int)str_len(ps);
    BYTE* out    = new BYTE[bytes];
    memset(out, 0, bytes);
    (*(WORD*)out) = (unsigned short)str_len(ps);
    int   idx     = 16;
    while(*ps)
    {
        int bits = _encodes[*ps].bits;
        DWORD value = _encodes[*ps].value;
        value <<= (idx & 7);
        *(DWORD*)(out+(idx>>3)) |= value ;
        idx += bits;
        ++ps;
    }
    length = ((idx-16)+8)/8;
    return out;
}


XCHAR* Hufmann::decode(const BYTE* pby, int length)
{
    int   dst  = 0;
    short lsize = *((unsigned short*)pby);
    lsize &= 0xFFFF;
    XCHAR* dest =  new XCHAR[lsize];
    memset(dest,0,lsize);
    
    int idx = 16;
    while(1)
    {
        DWORD cur = *(DWORD*)(pby+(idx>>3));
        cur >>= (idx & 0x7);
        
        Node* n = &_tree[_root];
        while(1)
        {
            if(n->c_char < 256)
            {
                dest[dst++]=n->c_char;
                break;
            }
            if(cur & 0x1)
                n = &_tree[n->n_Right];
            else
                n = &_tree[n->n_Left];
            ++idx;
            cur >>= 1;
        }
        if(lsize==dst)
        {
             dest[lsize]=0;
            break;
        }
    }
    return 0;
}
