 //---------------------------------------------------------------------------

#ifndef CopyResH
#define CopyResH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TCopyRes : public TThread
{
private:
    void __fastcall CheckTime();
protected:
    void __fastcall Execute();
public:
    __fastcall TCopyRes(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
