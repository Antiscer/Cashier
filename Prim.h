//---------------------------------------------------------------------------

#ifndef PrimH
#define PrimH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TPrimF : public TThread
{
private:
//    char* Command;
    bool DataReady;
    bool Printing;
    bool Wait;  //true - нет работы, false - идет работа с портом
    bool TimeOut;
    int ErrorCount;
    int Slip;
    char Data[1024];
    unsigned long Length;
    HANDLE hComm;          // the handle of the opened Comm Device.
    bool __fastcall SendData(char* Command);
    char __fastcall WinToDos(char c);
    void __fastcall StrWinToDos(char* str);
protected:
    void __fastcall Execute();
public:
    int Port;
    long Baud;
    AnsiString Psw;
    AnsiString KKMResult;
    AnsiString KKMStatus;
    AnsiString PrinterStatus;
    AnsiString Sum;
    AnsiString Serial;
    void __fastcall DeleteDoc();
    void __fastcall CRLF();
    void __fastcall BillLine(char* Name, char* Price, char* Qnty, int Num);
    void __fastcall BillClose(AnsiString Cash,  int OperatorNumber);
    void __fastcall BillZK(int OperatorNumber, char* Card);
    void __fastcall BillOpen(bool Sale);
    void __fastcall OpenBox();
    void __fastcall OpenSession(AnsiString OperatorName, int OperatorNumber);
    void __fastcall ZReport();
    void __fastcall XReport();
    void __fastcall GetVersion();
    void __fastcall Print(char* Text, bool WinText); 
    void __fastcall PrintCopyCheck();
    __fastcall TPrimF(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
 