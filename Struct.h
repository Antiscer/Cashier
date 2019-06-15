#ifndef TObjectH
#define TObjectH

#include <vector>
#include "paramsln.h"

//std::vector<hyper> Sum;
// структура для платежей
struct ComboPay
{
 AnsiString PayType;
 hyper Sum;
 unsigned int RoundPart;
 unsigned int Change;
 unsigned int CType;
 unsigned int SberOwnCard;
 char RRN[MAX_REFNUM];
 char CardID [CARD_ID_LEN];
 char CardName [MAX_CARD_NAME_LEN];
 char Check[1000];
 char Hash [CARD_HASH_LEN];
 int ErrorCode;
 char FullErrorText [MAX_FULL_ERROR_TEXT];
 bool Complete;
 ComboPay()
 {
   PayType="";
   Sum = hyper();
   RoundPart = int();
   Change = int();
   CType = int();
   SberOwnCard = int();
   memset(&RRN,0, MAX_REFNUM);
   memset(&CardID, 0 , CARD_ID_LEN);
   memset(&CardName, 0, MAX_CARD_NAME_LEN);
   memset(&Check, 0, 1000);
   memset(&Hash, 0, CARD_HASH_LEN);
   ErrorCode = int();
   memset(&FullErrorText, 0, MAX_FULL_ERROR_TEXT);
 };
};


struct BillItemLine      // тип структуры для данных чека и данных печати чека
{
 AnsiString ScanCode;
 AnsiString Name;
 AnsiString Measure;
 unsigned hyper Price;
 unsigned hyper Quantity;
 unsigned hyper Sum;
 unsigned int NDS;
 // конструктор
 BillItemLine(){};
 BillItemLine(AnsiString ScanCode, AnsiString Name, AnsiString Measure,
   hyper Price, hyper Quantity, hyper Sum, int NDS)
   {
      this->ScanCode = ScanCode;
      this->Name = Name;
      this->Measure = Measure;
      this->Price = Price;
      this->Quantity = Quantity;
      this->Sum = Sum;
      this->NDS = NDS;
   }

};

/*struct ComInOut
{
   int nOut;
   bool fin;
   char SendCommand[1024];
   char Data[1024];
   bool DataReady;
   CRITICAL_SECTION CSCom;
}; */

struct BillData
{
   AnsiString BankCheck;
   std::vector<AnsiString> vBankCheck;
   AnsiString BillNumber;
   bool Sale;
   AnsiString CasName;
   int CasNumber;
   AnsiString PayType;
   AnsiString Recived;
   int Round;
   AnsiString MidText[6];
   int Status;
   std::vector<ComboPay> vPay;
};
struct GiftCardData  //тип данных для подарочных карт
{
  AnsiString Code;
  AnsiString Sum;
  AnsiString Residue;
  GiftCardData(AnsiString Code, AnsiString Sum, AnsiString Residue)
  {
      this->Code = Code;
      this->Sum = Sum;
      this->Residue = Residue;
  }
};
struct PresentsData  // тип данных для подарков
{
   AnsiString Code;
   AnsiString Name;
   PresentsData(AnsiString Code, AnsiString Name)
   {
      this->Code = Code;
      this->Name = Name;
   }
};
struct FRStatus
{
   char KKMStatus[2];
   char OperationResult;
   unsigned char OFDstatus;
   unsigned char OFDreadMsgStat;
   unsigned char OFDquantityMsg[2];
   unsigned char OFDdocNum[4];
   unsigned char OFDdocData[5];
};

   struct stSverka
   {
      int Type;
      hyper frSum;
      hyper dbSum;
      stSverka(){};
      stSverka(int Num, unsigned hyper frsum, unsigned hyper dbsum): Type(Num), frSum(frsum),dbSum(dbsum){}
   };
   struct Item
   {
      AnsiString ScanCode;
      AnsiString Name;
      AnsiString Measure;
      unsigned hyper Price;
      unsigned hyper Quantity;
      AnsiString IDNom;
      Item(AnsiString ScanCode, AnsiString Name, AnsiString Measure,
         hyper Quantity, hyper Price, AnsiString IDNom)
      {
         this->ScanCode = ScanCode;
         this->Name = Name;
         this->Measure = Measure;
         this->Price = Price;
         this->Quantity = Quantity;
         this->IDNom = IDNom;
      }
   };


namespace My
{
    class TCustomException // : public Exception
    {
    public:
//         __fastcall TCustomException(const String Msg, int AErrorCode)
//            :Exception(Msg),FErrorCode(AErrorCode)  // Список инициализации
//        {
//        }

//        __property int ErrorCode={read=FErrorCode}; // Примочка С++Builder(VCL) - "свойство"
         TCustomException(const AnsiString Msg, int AErrorCode): FMessage(Msg), FErrorCode(AErrorCode)
         {
         }
         __property int ErrorCode = {read=FErrorCode};
         __property AnsiString Message = {read=FMessage};


    private:
        int FErrorCode;
        AnsiString FMessage;
//        int ErrorCode;

    };


}

struct DeliveryItems
{
   AnsiString IDNom;
   AnsiString ItemScanCode;
   AnsiString Name;
   unsigned hyper Quantity;
   unsigned hyper Price;
   AnsiString Measure;
   DeliveryItems()
   {
      this->Quantity = hyper();
      this->Price = hyper();
   }
   DeliveryItems(AnsiString idnom, AnsiString name, AnsiString sc, unsigned hyper qnty, unsigned hyper price, AnsiString meas)
   {
      this->IDNom = idnom;
      this->Name = name;
      this->ItemScanCode = sc;
      this->Quantity = qnty;
      this->Price = price;
      this->Measure = meas;
   }
};

struct Delivery
{
   AnsiString ScanCode;
   AnsiString BillNumber;
   TDateTime DateTime;
   unsigned int DocID;
   unsigned int Type;
   unsigned int Status;
   TDateTime  StatusDate;
   AnsiString CashBox;
   AnsiString Operator;
   std::vector<DeliveryItems> Items;
   Delivery()
   {
      this->ScanCode = "";
      this->BillNumber = "";
      this->DateTime = TDateTime();
      this->DocID = hyper();
      this->Type = int();
      this->Status = int();
      this->StatusDate = TDateTime();
      this->CashBox = "";
      this->Operator = "";
      this->Items.clear();
   };
   Delivery(AnsiString sc, AnsiString bn, TDateTime dt,
      unsigned hyper did, unsigned int type, unsigned int status, TDateTime sd,
      AnsiString cb, AnsiString op)
   {
      this->ScanCode = sc;
      this->BillNumber = bn;
      this->DateTime = dt;
      this->DocID = did;
      this->Type = type;
      this->Status = status;
      this->StatusDate = sd;
      this->CashBox = cb;
      this->Operator = op;
   }
   void clear()
   {
      this->ScanCode = "";
      this->BillNumber = "";
      this->DateTime = TDateTime();
      this->DocID = hyper();
      this->Type = int();
      this->Status = int();
      this->StatusDate = TDateTime();
      this->CashBox = "";
      this->Operator = "";
      this->Items.clear();
   }
};



#endif
