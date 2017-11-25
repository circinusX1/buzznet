

#include "buzznet.h"
#include "watchdog.h"


//-----------------------------------------------------------------------------
void WatchDog::ThreadFoo()
{
    int   fromt2t = 0;
    int   c =  _peng->GetCounter();
    int   s =  _peng->GetInSelect();

    while(!BStop())
    {
        os::ge_sleep(3000);             //better like this coe in limux events suck
        ++fromt2t;
        if((fromt2t & 0x3F) == 0x3F)              // each 63 seconds
        {
            if(_peng->GetCounter() == c)
            {
                if(_peng->GetInSelect())
                {
                    _peng->CloseSockets();
                }
            }
            c = _peng->GetCounter();
        }
    }
}