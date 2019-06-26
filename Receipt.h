#ifndef receiptH
#define receiptH

#include <vector>
#include "struct.h"
#include "mstar.h"

class TMainWindow;

class Receipt
{
   public:
      TMStarF * Star;
      Receipt();
      __property AnsiString BillNumber={read=billNumber};
      __property AnsiString Sklad = {read=sklad};
      __property AnsiString CashierName = {read=cashierName};
      __property AnsiString CashboxSerial = {read=cashboxSerial};

      std::vector<AnsiString> ReceiptMidText;
      std::vector<BillItemLine> ProdItems;
      std::vector<ComboPay> vPay;
      std::vector<GiftCardData> GiftCardItems;
      std::vector<PresentsData> PresentItems;

      void SetBillNumber(AnsiString bn);
      AnsiString GetBillNumber();
      void SetSklad(AnsiString sk);
      AnsiString GetSklad();
      void SetCashierName(AnsiString cn);
      AnsiString GetCashierName();
      void SetCashboxSerial(AnsiString sn);
      AnsiString GetCashboxSerial();

   private:
      AnsiString billNumber;
      AnsiString sklad;
      AnsiString cashierName;
      AnsiString cashboxSerial;
};

#endif