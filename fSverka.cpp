//---------------------------------------------------------------------------

#include <vcl.h>
#include <algorithm>
#pragma hdrstop

#include "fSverka.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSverka *FormSverka;

//---------------------------------------------------------------------------
__fastcall TFormSverka::TFormSverka(TComponent* Owner)
   : TForm(Owner)
{
   Star = MainWindow->Star;
}
//---------------------------------------------------------------------------
void __fastcall TFormSverka::FormShow(TObject *Sender)
{
//   Update();
   SetGrid(vSverka);
}
//---------------------------------------------------------------------------
void __fastcall TFormSverka::Update()
{
  std::vector<stSverka> tmpSverka;
  tmpSverka.clear();
  vSverka.clear();
  std::map<AnsiString, unsigned hyper> bp;
  GetDBPay(bp);

  AnsiString pt;
// временный вектор, включая дублирующиеся типы платежей и нулевые суммы
  for(std::vector<stPayType>::iterator it = payType.begin(); it != payType.end(); ++it)
  {
      tmpSverka.push_back(stSverka(it->Type, Star->GetMoneyReg(it->code), bp[it->PayType]));
  }
// сортируем для группировки сумм
  std::sort(tmpSverka.begin(), tmpSverka.end(), &st_cmp);
// группируем по типу
  int t = tmpSverka.at(0).Type;  // тип первого элемента вектора
//  pt = tmpSverka.at(0).PayType;
  hyper db_sum = 0;
  hyper fr_sum = 0;
  for(std::vector<stSverka>::iterator i = tmpSverka.begin(); i != tmpSverka.end(); ++i)
  {
      if(t == i->Type)
      {  // если тип повторяется, то суммируем регистры ФР, а в базе данных уже просуммированые данные
         fr_sum += i->frSum * PayTypeSign[i->Type];
         db_sum = i->dbSum * PayTypeSign[i->Type];
      }
      else
      { // если идет переход типа на другой, сохраняем просуммированные данные в векторе
         vSverka.push_back(stSverka(t, fr_sum, db_sum));
         fr_sum = i->frSum * PayTypeSign[i->Type];
         db_sum = i->dbSum * PayTypeSign[i->Type];
         t = i->Type; // меняем тип на новый
      }
  }
//  std::sort(vSverka.begin(), vSverka.end(), &st_cmp);
}
//----------------------------------------------------------------------------
void __fastcall TFormSverka::GetDBPay(std::map<AnsiString, unsigned hyper> &bp)
{
   MainWindow->PriceQuery->Active = false;
   MainWindow->PriceQuery->SQL->Clear();
   MainWindow->PriceQuery->SQL->Add("SELECT PayType, SUM(Amount) AS summ FROM RetPay WHERE BillNumber IN (SELECT BillNumber FROM retail WHERE BillDateTime>(SELECT MAX(BillDateTime) FROM retail WHERE flag=100) and flag>100) GROUP BY PayType");
   MainWindow->PriceQuery->SQL->Add("UNION ALL");
   MainWindow->PriceQuery->SQL->Add("SELECT '88' as PayType, SUM(RoundPart) as summ FROM RetPay WHERE BillNumber IN (SELECT BillNumber FROM retail WHERE BillDateTime>(SELECT MAX(BillDateTime) FROM retail WHERE flag=100) and flag>100)");
   try
   {
      MainWindow->PriceQuery->Active = true;
   }
   catch (EOleException &eException)
   {
      MainWindow->PriceQuery->Active = false;
   }
   int count = MainWindow->PriceQuery->RecordCount;
   bp.clear();

   for(int i = 0; i < count; i++)
   {
      bp.insert(std::make_pair(MainWindow->PriceQuery->FieldByName("PayType")->AsString, (int)(MainWindow->PriceQuery->FieldByName("Summ")->AsFloat * 100 + 0.5)));
      MainWindow->PriceQuery->Next();
   }
   MainWindow->PriceQuery->Active = false;
   AnsiString q = bp[" 1"];
   AnsiString w = bp[" 3"];
   AnsiString e = bp["10"];

}
//----------------------------------------------------------------------------
void __fastcall TFormSverka::SetGrid(std::vector<stSverka> &vs)
{

   int i = 1;
   hyper sum = 0;
   for(std::vector<stSverka>::iterator it = vs.begin(); it != vs.end(); ++it)
   {
      if(it->frSum != 0 || it->dbSum != 0)
      {
         sgSverka->RowCount = i + 1;
         sgSverka->Cells[SV_PAYTYPE_COL][i] = PayTypeName[it->Type];
         sgSverka->Cells[SV_FR_SUM_COL][i] = MoneyAsString(it->frSum);
         sgSverka->Cells[SV_KASS_SUM_COL][i] = MoneyAsString(it->dbSum);
         sum = sum + (it->frSum - it->dbSum);
         sgSverka->Cells[SV_DIFF_SUM_COL][i] = MoneyAsString(it->frSum - it->dbSum);
         i++;

      }
   }
         // строка итога
      sgSverka->RowCount++;
      sgSverka->Cells[SV_PAYTYPE_COL][sgSverka->RowCount - 1] = "ИТОГ: ";
      sgSverka->Cells[SV_DIFF_SUM_COL][sgSverka->RowCount - 1] = MoneyAsString(sum);
}
//--------------------------------------------------------------------------
// функция для сортировки std::sort  1,2,3,11,12,13
bool __fastcall TFormSverka::st_cmp(const stSverka& a, const stSverka& b)
{
   bool ret;
   ret = a.Type < b.Type;
   return ret;
}
// сортировка 1,11,2,12,3,13
bool __fastcall TFormSverka::gr_cmp(const stSverka& a, const stSverka& b)
{
   int aa, bb;
   aa = a.Type;
   bb = b.Type;
   if(a.Type < 10) aa = a.Type + 10;
   if(b.Type < 10) bb = b.Type + 10;
   return aa < bb;
}
//----------------------------------------------------------------------------
void __fastcall TFormSverka::FormCreate(TObject *Sender)
{
   sgSverka->ColCount = 4;
   sgSverka->RowCount = 2;
   sgSverka->FixedCols = 1;
   sgSverka->FixedRows = 1;
   sgSverka->Cells[SV_PAYTYPE_COL][0] = "Тип платежа";
   sgSverka->Cells[SV_FR_SUM_COL][0] = "ФР";
   sgSverka->Cells[SV_KASS_SUM_COL][0] = "Касса";
   sgSverka->Cells[SV_DIFF_SUM_COL][0] = "Разница";
   sgSverka->ColWidths[SV_PAYTYPE_COL] = 150;


   //payType.insert(std::make_pair(1,"Наличные"));
   //payType.insert(std::make_pair(2,""));

   payType.push_back(stPayType(1, NAL_TYPE, NAL_PAYMENT, MR_NAL_REG));
   payType.push_back(stPayType(2, RETURN_NAL_TYPE, "", MR_RETURN_NAL_REG));
   payType.push_back(stPayType(3, ELECTRON_TYPE, INTERNATIONAL_CARD_PAYMENT, MR_BC_MIR_REG));
   payType.push_back(stPayType(4, RETURN_ELECTRON_TYPE, "", MR_RETURN_BC_MIR_REG));
   payType.push_back(stPayType(5, ELECTRON_TYPE, INTERNATIONAL_CARD_PAYMENT, MR_BC_VMC_REG));
   payType.push_back(stPayType(6, RETURN_ELECTRON_TYPE, "", MR_RETURN_BC_VMC_REG));
   payType.push_back(stPayType(7, ELECTRON_TYPE, INTERNATIONAL_CARD_PAYMENT, MR_BC_OTHER_REG));
   payType.push_back(stPayType(8, RETURN_ELECTRON_TYPE, "", MR_RETURN_BC_OTHER_REG));
   payType.push_back(stPayType(9, GIFTCARD_TYPE, GIFT_CARD_PAYMENT, MR_GIFTCARD_REG));
   payType.push_back(stPayType(10, RETURN_GIFTCARD_TYPE, "", MR_RETURN_GIFTCARD_REG));
   payType.push_back(stPayType(11, ROUND_PART_TYPE, "", 0));

   PayTypeName.insert(std::make_pair(NAL_TYPE, "Наличные"));
   PayTypeName.insert(std::make_pair(RETURN_NAL_TYPE, "Возврат наличные"));
   PayTypeName.insert(std::make_pair(ELECTRON_TYPE, "Банковские карты"));
   PayTypeName.insert(std::make_pair(RETURN_ELECTRON_TYPE, "Возврат БК"));
   PayTypeName.insert(std::make_pair(GIFTCARD_TYPE, "Подарочные карты"));
   PayTypeName.insert(std::make_pair(RETURN_GIFTCARD_TYPE, "Возврат ПК"));
   PayTypeName.insert(std::make_pair(ROUND_PART_TYPE, "Скидка"));

   PayTypeSign.insert(std::make_pair(NAL_TYPE, 1));
   PayTypeSign.insert(std::make_pair(RETURN_NAL_TYPE, -1));
   PayTypeSign.insert(std::make_pair(ELECTRON_TYPE, 1));
   PayTypeSign.insert(std::make_pair(RETURN_ELECTRON_TYPE, -1));
   PayTypeSign.insert(std::make_pair(GIFTCARD_TYPE, 1));
   PayTypeSign.insert(std::make_pair(RETURN_GIFTCARD_TYPE, -1));
   PayTypeSign.insert(std::make_pair(ROUND_PART_TYPE, -1));

   svLabel->Caption = "";
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSverka::MoneyAsString(hyper Money)
{
AnsiString str = "";
str = String(Money/100)+Format(".%2.2D",ARRAYOFCONST(((int)(abs(Money%100)))));
return str;
}
//---------------------------------------------------------------------------

int __fastcall TFormSverka::ShowModal(bool silent)
{
   ClearGrid(sgSverka);
   bool res = false;
   res = checkSverka();
   if(res)
   {
      if(silent) ModalResult = mrOk;
      else
      {
         svLabel->Font->Color = clGreen;
         svLabel->Caption = "Сверка корректна.";
         TCustomForm::ShowModal();
         ModalResult = mrOk;
      }
   }
   else
   {
      svLabel->Font->Color = clRed;
      svLabel->Caption = "Сверка не идет.";
      TCustomForm::ShowModal();
      ModalResult = mrCancel;
   }

  return ModalResult;
}
//----------------------------------------------------------------------------
bool __fastcall TFormSverka::checkSverka()
{
   Update();
   int sum = 0;
   for(std::vector<stSverka>::iterator it = vSverka.begin(); it != vSverka.end(); ++it)
   {
      sum += it->frSum;
      sum -= it->dbSum;
   }
   if(sum != 0) return false;
return true;
}
//----------------------------------------------------------------------------

void __fastcall TFormSverka::ClearGrid(TStringGrid *Grid)
{
   for(int i = 1; i< Grid->RowCount; i++)
   {
      Grid->Rows[i]->Clear();
   }
   Grid->RowCount = 2;
}
//---------------------------------------------------------------------
void __fastcall TFormSverka::svCloseBtnClick(TObject *Sender)
{
   if(MainWindow->MoneyAshyper(sgSverka->Cells[SV_DIFF_SUM_COL][sgSverka->RowCount - 1]) == 0) ModalResult = mrOk;
   else ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TFormSverka::FormKeyPress(TObject *Sender, char &Key)
{
   if(Key == 27) svCloseBtnClick(this);  
}
//---------------------------------------------------------------------------

