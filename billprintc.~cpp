//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "billprintc.h"
#include "mainform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBillPrint *BillPrint;
//---------------------------------------------------------------------------
__fastcall TBillPrint::TBillPrint(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TBillPrint::QuickRep1NeedData(TObject *Sender,
      bool &MoreData)
{
int i;// = 0;
i = QuickRep1->RecordNumber+1;
if(Itog == 0.0) i = 1;

if( i < MainWindow->Grid->RowCount)
  {
  MoreData = true;
  QRLabel6->Caption = MainWindow->Grid->Cells[2][i];
  QRLabel7->Caption = MainWindow->Grid->Cells[4][i];
  QRLabel8->Caption = MainWindow->Grid->Cells[5][i];
  QRLabel9->Caption = MainWindow->Grid->Cells[6][i];
  Itog += MainWindow->Grid->Cells[6][i].ToDouble();
  }

else if(i == MainWindow->Grid->RowCount)
  {
  MoreData = true;
  QRLabel6->Caption = "Итого:";
  QRLabel7->Caption = "";
  QRLabel8->Caption = "";
  QRLabel9->Caption = FormatFloat("#0.00",Itog);
  }
else MoreData = false;
}
//---------------------------------------------------------------------------

void __fastcall TBillPrint::TitleBand1BeforePrint(TQRCustomBand *Sender,
      bool &PrintBand)
{
for(int i; i < MainWindow->BillHeadCount; i++)
  QRMemo1->Lines->Strings[i] = MainWindow->BillHead[i];
}
//---------------------------------------------------------------------------

void __fastcall TBillPrint::QuickRep1BeforePrint(TCustomQuickRep *Sender,
      bool &PrintReport)
{
Itog = 0.0;
}
//---------------------------------------------------------------------------

