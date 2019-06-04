//---------------------------------------------------------------------------

#ifndef scannerH
#define scannerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TScanner : public TThread
{
private:
    void __fastcall Transmit();
    HANDLE hComm;          // the handle of the opened Comm Device.
    char szReturn[256];
protected:
    void __fastcall Execute();
public:
    int Port;
    int Baud;
    __fastcall TScanner(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
