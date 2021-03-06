//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#ifndef mstarH
#define mstarH
//---------------------------------------------------------------------------
#include "Struct.h"
#include "cardtype.h"
#include <Classes.hpp>

#define NUM_TYPE_PAYMENTS     16
#define SUM_NAL               0
#define SUM_MIR_CARD          1
#define SUM_EC_VISA           2
#define SUM_OTHER             3
#define SUM_PREPAY            13

#define OT_RECEIVE            1
#define OT_REFUND_REC         2
#define OT_CONSUMTION         3
#define OT_REFUND_CONS        4

#define FR_LINE_LEN           40


//---------------------------------------------------------------------------
// ??????? ????? ?????
class TMStarF : public TThread
{
private:
//    char* Command;
   CRITICAL_SECTION starCS;
    bool DataReady;
    bool PrintingFR;
    bool TimeOut;
    int ErrorCount;
    int Slip;
    int SSec; // 1 - ????? ? ?????????? ?????????, 0 - ?? ????????, -1 - ???? ??????????? ??????
    long SellSumm; // ????????? ??? ????? - ???????
    char Data[1024];
    unsigned long Length;
    char SendCommand[1024];
    unsigned long nOut;
    HANDLE hComm;          // the handle of the opened Comm Device.
    char __fastcall WinToDos(char c);
    void __fastcall SetDateTimeStr(char* str);
    void __fastcall SetupPrime(void);
    void __fastcall GetupPrime(void);
    void __fastcall StartPrime(void);
    char __fastcall NextSbyte(void);
    bool __fastcall WaitFR(int seconds);
    TDateTime dt;
    long SerialNumber;
    int __fastcall Explode(String sContent, String sSeparator, TStringList *slTokens, int iLength, bool bTrim);

protected:  // ?????????????? ????? Execute()
    void __fastcall Execute();
public:
    bool __fastcall SendData(char* Command);
    int Port;
    int FRType;
    int flag21; //???? ?????? ????? ?1 ?1 ?21
    long Baud;
    AnsiString BillNumber; //????? ?????????? ????
    bool Wait;  //true - ??? ??????, false - ???? ?????? ? ??????
    AnsiString Psw;
    AnsiString KKMResult;
    AnsiString KKMStatus;
    AnsiString PrinterStatus;
    AnsiString Sum;
    int DiscountType;  // ??? ?????? 0 - ?????? ?? ???, ?? ???????, 1 - ???????? ???????????? ?????? ?? ???????
    AnsiString Serial; //???????? ????? ??
    AnsiString SerialID; //????????????? ?? ? 1?
    void __fastcall DeleteDoc();
    void __fastcall CRLF();
    void __fastcall StrWinToDos(char* str, unsigned int maxlen);
    void __fastcall BillLine(char* Name, char* Price, char* Qnty, int Num, char* Meas, char* Code, char* NDS);
    bool __fastcall BillShadowLine(AnsiString Name, hyper Price, hyper Qnty, AnsiString Meas, AnsiString Code, int NDS);
    void __fastcall PrintLine(AnsiString Name, AnsiString Price, AnsiString Qnty, AnsiString Summ, AnsiString Meas, AnsiString Code, AnsiString NDS);
    bool __fastcall BillClose(int OperatorNumber);
    void __fastcall BillPayment(AnsiString Cash1,AnsiString Cash2,AnsiString Cash3,AnsiString Cash4);
    void __fastcall BillOff(AnsiString Cash);
    bool __fastcall BillZK(int OperatorNumber, char* Card);
    bool __fastcall BillOpen(bool Sale, AnsiString OperatorName, int OperatorNumber);
    void __fastcall BillReturn(bool Sale, AnsiString OperatorName, int OperatorNumber);
    void __fastcall OpenBox();
    void __fastcall OpenSession(AnsiString OperatorName, int OperatorNumber);
    void __fastcall ZReport();
    void __fastcall XReport();
    void __fastcall GetVersion();
    void __fastcall Print(char* Text, bool WinText);
    void __fastcall PrintBold(char* Text, bool WinText);
    void __fastcall PrintCopyCheck();
    bool __fastcall PrintEAN();
    void __fastcall PrintOldSession(AnsiString SessionNumber);
    AnsiString __fastcall GetBillNumber();
    long __fastcall GetSellSumm();
    void __fastcall ResumePrint();
    void __fastcall StrihWriteTable(char Table,int Row,char Field,char * Value,int Len);
    void __fastcall SetClock(void);
    __fastcall TMStarF(bool CreateSuspended);
    bool __fastcall GetStatus();
   int __fastcall GetMode();
   int __fastcall GetSubMode();
   TDateTime __fastcall GetDateTime();
   long __fastcall SubTotal();
    AnsiString Version;
    AnsiString Build;
    AnsiString BuildDate;
    int DocumentNum;
    int Mode;
    int SubMode;
    void __fastcall GetNumItems();
    void __fastcall GetNumGiftCard();
    int NumOfItems; // ?????????? ??????? ? ??????? ????
    int NumOfGiftCard;
    bool __fastcall BillShadowMass();
    bool __fastcall PrintLineMass();
    AnsiString __fastcall QuantityAsString(unsigned hyper Quantity);
    AnsiString __fastcall MoneyAsString(unsigned hyper Money);
    bool __fastcall BillComplete();
    void __fastcall log(AnsiString s);
    BillData *stBill;
    bool __fastcall GiftCardPrintPayments();
    bool __fastcall PresentPrintBill();
    int NumOfPresents;
    void __fastcall GetNumPresents();
    HANDLE hEvent1, evConnStatus;
    HANDLE hPrnEvent;
    bool __fastcall FRInit();
    hyper __fastcall MoneyAshyper(AnsiString String);
    void __fastcall GetConnectionStatus();
    FRStatus *frStatus;
    std::vector<BillItemLine> *ShadowLineData;
    std::vector<BillItemLine> *PrintLineData;
    std::vector<GiftCardData> *GiftItemData;
    std::vector<PresentsData> *PresentItemData;
    AnsiString *BankCheck;
    bool __fastcall FNOperation(int OpType, hyper Price, hyper Quantity, int NDS, int AttMethodCalc, int AttSubjectCalc, AnsiString Name, AnsiString Meas);
    bool __fastcall TMStarF::FNCloseCheckEx(std::vector<ComboPay> &vPay);
    hyper __fastcall GetMoneyReg(unsigned short int Register);
    void __fastcall TMStarF::PrintF(AnsiString Text, int FontType);
    bool __fastcall TMStarF::PrintEAN(unsigned __int64 HEAN);
    void __fastcall TMStarF::Feed(short int line);

};

//---------------------------------------------------------------------------
#endif
