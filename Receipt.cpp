#include "receipt.h"
#include "mainform.h"

//реализация методов класса Receipt

Receipt::Receipt()
{
   Star = MainWindow->Star;
}

void Receipt::SetBillNumber(AnsiString bn)
{
   billNumber = bn.SubString(1,18);
}
//---------------------------------------------
AnsiString Receipt::GetBillNumber()
{
   billNumber = Star->GetBillNumber();
   return billNumber;
}
//------------------------------------------------------------------------
void Receipt::SetSklad(AnsiString sk)
{
   sklad = sk.SubString(1,32);
}
//-------------------------------------------------------------------------
AnsiString Receipt::GetSklad()
{
   return sklad;
}
//--------------------------------------------------------------------------
void Receipt::SetCashierName(AnsiString cn)
{
   cashierName = cn.SubString(1,15);
}
//---------------------------------------------------------------------------
AnsiString Receipt::GetCashierName()
{
   return cashierName;
}
//-------------------------------------------------------------------------
void Receipt::SetCashboxSerial(AnsiString sn)
{
   cashboxSerial = sn;
}
//---------------------------------------------------------------------------
AnsiString Receipt::GetCashboxSerial()
{
   if(Star->Serial == "") Star->GetVersion();
   cashboxSerial = Star->Serial;
   return cashboxSerial;
}
//-----------------------------------------------------------------------------



