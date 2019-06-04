//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelectPayTypeFormUnit.h"
#include "mainform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSelectPayTypeForm *SelectPayTypeForm;
//---------------------------------------------------------------------------
__fastcall TSelectPayTypeForm::TSelectPayTypeForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSelectPayTypeForm::Button1Click(TObject *Sender)
{
MainWindow->log("Нажата кнопка 'Международные карты'");
MainWindow->PayType = INTERNATIONAL_CARD_PAYMENT;
this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TSelectPayTypeForm::Button2Click(TObject *Sender)
{
MainWindow->log("Нажата кнопка 'Сберкарта'");
MainWindow->PayType = SBERCARD_PAYMENT;
this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TSelectPayTypeForm::Button3Click(TObject *Sender)
{
MainWindow->log("Нажата кнопка 'Наличные'");
MainWindow->PayType = NAL_PAYMENT;
this->Close();
}

