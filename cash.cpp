//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "cash.h"
#include "mainform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCashForm *CashForm;

//---------------------------------------------------------------------------
__fastcall TCashForm::TCashForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
// ????????? ?????? ?? ?????
void __fastcall TCashForm::FormKeyPress(TObject *Sender, char &Key)
{

    if(Key == 13)
    {
      unsigned hyper db = MainWindow->MoneyAshyper(Cash->Text);
        if(db > 200000000) // ???????????? ???? ????? 2 ???. ???
        {
            Cash->Text = MainWindow->MoneyAsString(MainWindow->RestPayment());
            Cash->SelectAll();
            Cash->SetFocus();
            return;
        }
        MainWindow->log("???????????? ????? " + Cash->Text + ", ??? ??????? " + MainWindow->PayType);
        if(MainWindow->PayType == INTERNATIONAL_CARD_PAYMENT
            && (db + MainWindow->GridSum(MainWindow->ComboPayGrid, CP_SUM_COL)) > MainWindow->MoneyAshyper(MainWindow->TSum->Text))
        { // ???? ?????? ?? ??, ?? ?? ???? ????? ????? ?????? ???????
            Cash->Text = MainWindow->MoneyAsString(MainWindow->RestPayment());
            Cash->SelectAll();
            Cash->SetFocus();
            return;
        }
        MainWindow->ComboPayGrid->Cells[CP_SUM_COL][MainWindow->ComboPayGrid->RowCount -1] = MainWindow->MoneyAsString(db); // ????????? ?????
        MainWindow->ComboPayGrid->Invalidate();
        MainWindow->ComboPayGrid->OnDrawCell;
        MainWindow->Recived->Text = MainWindow->MoneyAsString(MainWindow->GridSum(MainWindow->ComboPayGrid, CP_SUM_COL)); // ??????? ????????
        if(MainWindow->PayType == NAL_PAYMENT)
            MainWindow->SetPayStatus(MainWindow->ComboPayGrid, MainWindow->ComboPayGrid->RowCount - 1, CP_COMPLETE_STATUS); // ?????? ?????? ??????? ?????????
        ModalResult = mrOk;

    }
    if(Key == 27)
    {
      ModalResult = mrCancel;
      MainWindow->DeleteLastRow(MainWindow->ComboPayGrid);
    }
}
//---------------------------------------------------------------------------
// ?????????? ?????
void __fastcall TCashForm::FormShow(TObject *Sender)
{
//    unsigned hyper db;

   hyper Sum = MainWindow->MoneyAshyper(MainWindow->TSum->Text) - MainWindow->MoneyAshyper(MainWindow->Recived->Text);
   if(MainWindow->PayType == NAL_PAYMENT)
   {
      CashForm->Caption = "???????? ????????";
      Sum = Sum - Sum%MainWindow->Round;
   }
   else if(MainWindow->PayType == INTERNATIONAL_CARD_PAYMENT)
   {
      CashForm->Caption = "????? ?? ?????????? ?????";
   }
//    db = MainWindow->MoneyAshyper(Cash->Text);
   MainWindow->AddComboPay(MainWindow->PayType, 0, MainWindow->ComboPayGrid);
   MainWindow->SetPayStatus(MainWindow->ComboPayGrid, MainWindow->ComboPayGrid->RowCount - 1, 1);

   Cash->Text = MainWindow->MoneyAsString(Sum);
   Cash->SelectAll();
   Cash->SetFocus();
}
//---------------------------------------------------------------------------
// ?????? ????? ????? ????? ???? ? ???????????
void __fastcall TCashForm::CashKeyPress(TObject *Sender, char &Key)
{
   // ????????? ???? ?????? ? ???? ? ????????? ?????? ????? ? ???????????, ?????? ??????????? ? ????????? ???? ?????????
   if((Key!=VK_BACK)&&(Key<'0'||Key>'9')&&
      !((Key == DecimalSeparator || Key == '.' || Key == ',') && MainWindow->PayType != NAL_PAYMENT))Key=0;
}
//---------------------------------------------------------------------------

