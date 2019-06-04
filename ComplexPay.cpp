//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ComplexPay.h"
#include "mainform.h"
#include <map>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TComplexPayForm *ComplexPayForm;
// выражение типа ключ-значение "Название платежа"
static std::map<AnsiString, AnsiString> PayName;
std::map<AnsiString, AnsiString>::iterator it;  // названия платежей
AnsiString PayType = "";
BillData * cpBill = new BillData;
// шаблон для поиска в ассоциативном массиве по значению
template<class Iterator, class Value>
    Iterator find_value(Iterator begin, Iterator end, const Value& value)
{
    while (begin!=end) 
    {
        if ((*begin).second==value) return begin;
        ++begin;
    }
    return end;
}

//---------------------------------------------------------------------------
__fastcall TComplexPayForm::TComplexPayForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TComplexPayForm::FormActivate(TObject *Sender)
{
// регистрируем горячие клавиши
   RegisterHotKey(Handle, 0, 0, VK_F10);
   RegisterHotKey(Handle, 1, MOD_CONTROL, 'B');
   RegisterHotKey(Handle, 2, 0, VK_RETURN);
   RegisterHotKey(Handle, 3, 0, VK_ESCAPE);
   PayEdit->Text = MainWindow->TSum->Text;
   PayType = "";
   PayGroupBox->Caption = "Остаток";
   PayEdit->BorderStyle = bsNone;
   PayEdit->Color = clMenu;
   PayEdit->Text = RestPayment();
   PayEdit->ReadOnly = true;
   MainWindow->ClearGrid(ComplexPayGrid);
   ComplexPayGrid->SetFocus();

}
//---------------------------------------------------------------------------


void __fastcall TComplexPayForm::ApplicationEvents1Message(tagMSG &Msg,
      bool &Handled)
{
   if(Msg.message == WM_HOTKEY)
   {
      if(Msg.wParam == 0)
      {
         NalPayment();
      }
      else if(Msg.wParam == 1)
      {
         BeznalPayment();
      }
      else if(Msg.wParam == 2)
      {
         ClickReturn();
      }
      else if(Msg.wParam == 3)
      {
         ClickEscape();
      }
   }
}
//---------------------------------------------------------------------------
// при оплате наличными
void __fastcall TComplexPayForm::NalPayment()
{
   if(MainWindow->MoneyAshyper(PayEdit->Text) <= 0) return;
   PayType = NAL_PAYMENT;
   PayGroupBox->Caption = "Сумма наличными";
   PayEdit->BorderStyle = bsSingle;
   PayEdit->Color = clWindow;
   PayEdit->Text = RestPayment();
   PayEdit->ReadOnly = false;
   PayEdit->SelectAll();
   PayEdit->SetFocus();
}
//---------------------------------------------------------------------------
// при оплате безналичными
void __fastcall TComplexPayForm::BeznalPayment()
{
   if(MainWindow->MoneyAshyper(PayEdit->Text) <= 0) return;
   PayType = INTERNATIONAL_CARD_PAYMENT;
   PayGroupBox->Caption = "Сумма банковской картой";
   PayEdit->ReadOnly = true;
   PayEdit->Text = RestPayment();
   PayEdit->BorderStyle = bsSingle;
   PayEdit->Color = clWindow;
   PayEdit->ReadOnly = false;
   PayEdit->SelectAll();
   PayEdit->SetFocus();
}
//---------------------------------------------------------------------------
// Обработка нажатия Enter на форме
void __fastcall TComplexPayForm::ClickReturn()
{
   if(!(MainWindow->MoneyAshyper(PayEdit->Text) > MainWindow->MoneyAshyper(MainWindow->TSum->Text) - GridSum(ComplexPayGrid, 2)))
   if(!PayEdit->ReadOnly)
   {
         AddComplexPay(PayType, PayEdit->Text, ComplexPayGrid);
         PayType = "";
         PayGroupBox->Caption = "Остаток";
         PayEdit->BorderStyle = bsNone;
         PayEdit->Color = clMenu;
         PayEdit->Text = RestPayment();
         PayEdit->ReadOnly = true;
   }
}
//---------------------------------------------------------------------------
// обработка нажатия Escape на форме
void __fastcall TComplexPayForm::ClickEscape()
{
   MainWindow->ClearGrid(ComplexPayGrid);
   ComplexPayForm->Close();
}
//---------------------------------------------------------------------------
// чистка при закрытии формы
void __fastcall TComplexPayForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
// сбрасываем регистрацию горячих клавиш
   UnregisterHotKey(Handle,0);
   UnregisterHotKey(Handle,1);
   UnregisterHotKey(Handle,2);
   UnregisterHotKey(Handle,3);
}
//---------------------------------------------------------------------------
// добавляем запись в таблицу платежей
void __fastcall TComplexPayForm::AddComplexPay(AnsiString PayType, AnsiString Summ, TStringGrid *Grid)
{
   int Row = Grid->RowCount;
   if(!Grid->Cells[0][Grid->RowCount - 1].IsEmpty())Grid->RowCount = Grid->RowCount + 1;  // Если строка не пронумерована (пустая), то добавляем строку
   if(Grid->RowCount == 2 && Grid->Cells[0][1].IsEmpty()) Row = 1; // обходим вторую пустую строку на начальном этапе
   Grid->Cells[0][Row] = AnsiString(Row);
   Grid->Cells[1][Row] = PayName[PayType];
   Grid->Cells[2][Row] = Summ;
   Grid->Cells[3][Row] = "-";

}
//---------------------------------------------------------------------------
// разрешаем вводить только цифры и
void __fastcall TComplexPayForm::PayEditKeyPress(TObject *Sender,
      char &Key)
{
   // разрешаем только ввод цифр, десятичного разделителя и Backspace
   if((Key!=VK_BACK)&&(Key<'0'||Key>'9')&&(Key != DecimalSeparator))Key=0;
}
//---------------------------------------------------------------------------
// инициализация
void __fastcall TComplexPayForm::FormCreate(TObject *Sender)
{
// заполняем ячейки заголовков
   ComplexPayGrid->Cells[1][0] = "№";
   ComplexPayGrid->Cells[1][0] = "Вид расчета";
   ComplexPayGrid->Cells[2][0] = "Сумма";
   ComplexPayGrid->Cells[3][0] = "Удалить";
   MainWindow->ClearGrid(ComplexPayGrid);
//   Заполняем данные по типам платежей
   PayName.insert(std::make_pair(NAL_PAYMENT,"Наличные"));
   PayName.insert(std::make_pair(INTERNATIONAL_CARD_PAYMENT,"Банковская карта"));
   PayName.insert(std::make_pair(GIFT_CARD_PAYMENT,"Подарочная карта"));
}
//---------------------------------------------------------------------------

void __fastcall TComplexPayForm::ButtonNalPayClick(TObject *Sender)
{
   NalPayment();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TComplexPayForm::RestPayment()
{
   return MainWindow->MoneyAsString(MainWindow->MoneyAshyper(MainWindow->TSum->Text) - GridSum(ComplexPayGrid, 2));
}
//----------------------------------------------------------------------------
unsigned hyper __fastcall TComplexPayForm::GridSum(TStringGrid *Grid, int colNum)
{
   unsigned hyper sum = 0;
   for(int i = 1; i<=Grid->RowCount;i++)
   {
      sum += MainWindow->MoneyAshyper(Grid->Cells[colNum][i]);
   }
return sum;
}

//---------------------------------------------------------------------------
void __fastcall TComplexPayForm::ButtonBezPayClick(TObject *Sender)
{
   BeznalPayment();
}
//---------------------------------------------------------------------------


void __fastcall TComplexPayForm::PayButtonClick(TObject *Sender)
{
   ComboPay sPay;
   for(int i=1; i<= ComplexPayGrid->RowCount; i++)
   {
      it = find_value(PayName.begin(), PayName.end(), ComplexPayGrid->Cells[1][i]);
      sPay.PayType = it->first;
      sPay.Sum = MainWindow->MoneyAshyper(ComplexPayGrid->Cells[2][i]);
      MainWindow->mfBill.vPay.push_back(sPay);
   }
   MainWindow->CreateBillBody();
   ComplexPayForm->Close();
}
//---------------------------------------------------------------------------

