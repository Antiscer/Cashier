//---------------------------------------------------------------------------

#ifndef clockH
#define clockH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "struct.h"
#include "mstar.h"
#include <adodb.hpp>

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
    HANDLE evConnStatus, evSQLConnStatusOK;
    TMStarF *Star;
    FRStatus *clFRStatus;
    bool __fastcall TClockThread::CheckSQLConnection();
    TADOConnection *Conn;
    TADOQuery *Query;
};
//---------------------------------------------------------------------------
#endif
