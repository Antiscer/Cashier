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
static double Itog = 0;

if(QuickRep1->RecordNumber+1 < MainWindow->Grid->RowCount)
  {
  MoreData = true;
  QRLabel6->Caption = MainWindow->Grid->Cells[2][QuickRep1->RecordNumber+1];
  QRLabel7->Caption = MainWindow->Grid->Cells[4][QuickRep1->RecordNumber+1];
  QRLabel8->Caption = MainWindow->Grid->Cells[5][QuickRep1->RecordNumber+1];
  QRLabel9->Caption = MainWindow->Grid->Cells[6][QuickRep1->RecordNumber+1];
  Itog += MainWindow->Grid->Cells[6][QuickRep1->RecordNumber+1].ToDouble();
  }

else if(QuickRep1->RecordNumber+1 == MainWindow->Grid->RowCount)
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

