//---------------------------------------------------------------------------

#ifndef smartcardreaderH
#define smartcardreaderH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TSmartCardReader : public TThread
{
private:
    void __fastcall CardType(void);
    long Result;
protected:
    void __fastcall Execute();
public:
    __fastcall TSmartCardReader(bool CreateSuspended);
    void __fastcall TerminateCancel();
    char Data[256];
};
//---------------------------------------------------------------------------
#endif
