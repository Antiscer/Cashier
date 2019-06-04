//---------------------------------------------------------------------------

#ifndef clockH
#define clockH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "struct.h"
#include "mstar.h"

//---------------------------------------------------------------------------
class TClockThread : public TThread
{
private:
    void __fastcall RefreshTime(void);
//    FRStatus clFRStatus;
protected:
    void __fastcall Execute();
public:
    __fastcall TClockThread(TMStarF *_Star, FRStatus *_clFRStatus, bool CreateSuspended);
    HANDLE evConnStatus;
    TMStarF *Star;
    FRStatus *clFRStatus;
};
//---------------------------------------------------------------------------
#endif
