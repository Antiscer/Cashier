//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mainform.h"

#include "frReturn.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfrReturnForm *frReturnForm;

//---------------------------------------------------------------------------
__fastcall TfrReturnForm::TfrReturnForm(TComponent* Owner)
   : TForm(Owner)
{
   sgCheckList->Cells[CL_NUMBER_COL][0] = "?";
   sgCheckList->Cells[CL_DATE_COL][0] = "?????";
   sgCheckList->Cells[CL_BILLNUM_COL][0] = "????? ????";
   sgCheckList->Cells[CL_SUM_COL][0] = "?????";
   sgCheckList->Cells[CL_PAYTYPE_COL][0] = "???";
   sgCheckList->RowCount = 2;
   sgCheckList->Visible = true;

   sgOneCheck->Cells[OC_NUMBER_COL][0] = "?";
   sgOneCheck->Cells[OC_NAME_COL][0] = "????????";
   sgOneCheck->Cells[OC_QUANTITY_COL][0] = "???-??";
   sgOneCheck->Cells[OC_PRICE_COL][0] = "????";
   sgOneCheck->Cells[OC_SUM_COL][0] = "?????";
   sgOneCheck->RowCount = 2;
   sgOneCheck->Visible = false;
   Star = MainWindow->Star;
   frRetBill = &MainWindow->mfBill;
//    asdasd;
}

//---------------------------------------------------------------------------
void __fastcall TfrReturnForm::ReturnBtnClick(TObject *Sender)
{
   if(sgCheckList->Cells[0][1].IsEmpty()) return;
   AnsiString bn = sgCheckList->Cells[CL_BILLNUM_COL][sgCheckList->Row];
   edBillNumber->Text = bn.SubString(1,2) + "-" + bn.SubString(3,4) + "-" + bn.SubString(7,4) + "-" + bn.SubString(11,4) + "-" + bn.SubString(15,4);
   std::vector<BillItemLine> biLine;
   std::vector<ComboPay> cPay;

   PanelEnable(true);
   int i = 0;
   GetBillItem(biLine);
   for(std::vector<BillItemLine>::iterator it = biLine.begin(); it < biLine.end(); ++it)
   {
      sgThingsList->Cells[0][i] = it->ScanCode + " " + it->Name.Trim() + " (" + it->Measure.Trim() + ")";
      sgThingsList->Cells[2][i] = MainWindow->QuantityAsString(it->Quantity) + " X " + MainWindow->MoneyAsString(it->Price);
      sgThingsList->Cells[2][i+1] = "=" + MainWindow->MoneyAsString(it->Sum);
      i+=2;
      sgThingsList->RowCount = i;
   }
   i = 0;
   GetPay(cPay);
   unsigned hyper sum = 0;
   for(std::vector<ComboPay>::iterator it = cPay.begin(); it != cPay.end(); ++it)
   {
      AnsiString PaymentStr = "";
      if(it->PayType == NAL_PAYMENT) PaymentStr = "?????????";
      else if(it->PayType == INTERNATIONAL_CARD_PAYMENT) PaymentStr = "????????????";
      else if(it->PayType == GIFT_CARD_PAYMENT) PaymentStr = "??????????? ???????";
      sgPays->Cells[0][i] = PaymentStr;
      sgPays->Cells[1][i] = MainWindow->MoneyAsString(it->Sum);
      sum += it->Sum;
      i++;
      sgPays->RowCount = i;
   }
   edSum->Text = MainWindow->MoneyAsString(sum);
}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::CancelBtnClick(TObject *Sender)
{

   if(sgCheckList->Enabled)  //?? ? ??????? ?????
   {
      ModalResult = mrCancel;
      frReturnForm->Close();
   }
   else  // ?? ? ??????????? ????
   {
      sgCheckList->Enabled = true;
      sgCheckList->Visible = true;
      sgOneCheck->Visible = false;
      CancelBtn->Caption = "???????";
   }


}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::sgCheckListDblClick(TObject *Sender)
{
   if(!sgCheckList->Enabled) return;
   AnsiString BillNumber;
   BillNumber = sgCheckList->Cells[CL_BILLNUM_COL][sgCheckList->Row];
   MainWindow->PriceQuery->SQL->Text = "SELECT Name, Quantity, price, Quantity*price as Summ FROM retail rt cross apply (SELECT TOP 1 name FROM price WHERE rt.IDnom = price.IDnom) pr WHERE  BillNumber='" + BillNumber + "'";
   try
   {
      MainWindow->PriceQuery->Active = true;
   }
   catch (...)
   {

   }
   unsigned hyper count = MainWindow->PriceQuery->RecordCount;
   if(count == 0)
   {
      MainWindow->PriceQuery->Active = false;
      return;
   }
   for(hyper i = 1; i <= count; i++)
   {
      sgOneCheck->RowCount = count + 1;
      sgOneCheck->Cells[OC_NUMBER_COL][i] = i;
      sgOneCheck->Cells[OC_NAME_COL][i] = MainWindow->PriceQuery->FieldByName("Name")->AsString;
      sgOneCheck->Cells[OC_QUANTITY_COL][i] = MainWindow->PriceQuery->FieldByName("Quantity")->AsString;
      sgOneCheck->Cells[OC_PRICE_COL][i] = MainWindow->PriceQuery->FieldByName("Price")->AsString;
      sgOneCheck->Cells[OC_SUM_COL][i] = MainWindow->PriceQuery->FieldByName("Summ")->AsString;
      MainWindow->PriceQuery->Next();
   }
   MainWindow->PriceQuery->Active = false;
   sgCheckList->Enabled = false;
   sgCheckList->Visible = false;
   sgOneCheck->Visible = true;
   CancelBtn->Caption = "?????";
}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::sgCheckListDrawCell(TObject *Sender,
      int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
   TStringGrid *Grid = (TStringGrid *) Sender;
   TCanvas *Canvas = Grid->Canvas;
   Canvas->Font->Color = clWindowText;
   if(State.Contains(gdSelected))
   {
      Canvas->Brush->Color = RGB(150,150,255);
   }
   Canvas->FillRect(Rect);
   TRect r = Rect;
       r.left +=2;
       r.top+=2;
   TGridRect R;
   R.Left = Grid->FixedCols;
   R.Top = Grid->Row;
   R.Right = Grid->ColCount;
   R.Bottom = Grid->Row;
   Grid->Selection = R;
   DrawText(Canvas->Handle,this->sgCheckList->Cells[ACol][ARow].c_str(),-1,(TRect*)&r,DT_LEFT);
}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::FormShow(TObject *Sender)
{
//   MainWindow->PriceQuery->SQL->Text = "SELECT BillDateTime, BillNumber, SUM(Quantity*Price) as Summ FROM retail WHERE BillDateTime > CONVERT (date, SYSDATETIME()) and flag>100 GROUP BY BillDateTime, BillNumber ORDER BY BillDateTime DESC";
   DateTimePicker->Date = Date();
   GetBillList(DateTimePicker->Date);
   MainWindow->PriceQuery->Active = false;
   MainWindow->CentralQuery->Active = false;
   }
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::FormCreate(TObject *Sender)
{

   sgCheckList->Row = 1;
   sgCheckList->Col = 1;
   sgOneCheck->Row = 1;
   sgOneCheck->Col = 1;
   sgThingsList->ColWidths[1] = 5;
   Multiline(btnFRandDB);
   PanelEnable(false);
}
//---------------------------------------------------------------------------

bool __fastcall TfrReturnForm::retBillDelete(AnsiString BillNumber)
{
   MainWindow->PriceQuery->SQL->Clear();
   MainWindow->PriceQuery->SQL->Add("BEGIN TRY BEGIN TRAN");
   MainWindow->PriceQuery->SQL->Add("DELETE FROM retcarddata WHERE retpayid in (SELECT id FROM RetPay WHERE BillNumber='" + BillNumber + "')");
   MainWindow->PriceQuery->SQL->Add("DELETE FROM RetPay WHERE BillNumber='" + BillNumber + "'");
   MainWindow->PriceQuery->SQL->Add("DELETE FROM retail WHERE BillNumber='" + BillNumber + "'");
   MainWindow->PriceQuery->SQL->Add("COMMIT TRAN END TRY");
   MainWindow->PriceQuery->SQL->Add("BEGIN CATCH");
   MainWindow->PriceQuery->SQL->Add("IF @@TRANCOUNT > 0 ROLLBACK TRAN;");
   MainWindow->PriceQuery->SQL->Add("DECLARE @ErrorMessage NVARCHAR(4000);");
   MainWindow->PriceQuery->SQL->Add("DECLARE @ErrorSeverity INT;");
   MainWindow->PriceQuery->SQL->Add("DECLARE @ErrorState INT;");
   MainWindow->PriceQuery->SQL->Add("SELECT @ErrorMessage = ERROR_MESSAGE(), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();");
   MainWindow->PriceQuery->SQL->Add("RAISERROR(@ErrorMessage, @ErrorSeverity, @ErrorState);");
   MainWindow->PriceQuery->SQL->Add("END CATCH");
   _di_Errors       errCollection;
   _di_Error        errSingle;
   int iCount = 0;
   int SQL_RAISERROR_CODE =0;
   try
   {
      MainWindow->PriceQuery->ExecSQL();
      errCollection = MainWindow->CashConnection->Errors;
      iCount = errCollection->Count;
      for(int i = 0; i < iCount; i++)
      {
         errCollection->Get_Item(i,errSingle);
         errSingle->Get_NativeError(SQL_RAISERROR_CODE);
      }
      if(SQL_RAISERROR_CODE)
      {
         throw errCollection;
      }
   }
 catch (EOleException &eException)
   {
      MainWindow->Name->Caption = "?????? SQL. ?????????? ?????????? ??????!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + MainWindow->PriceQuery->SQL->Text;
      MainWindow->log(errormsg);
      return false;
   }
   catch (_di_Errors &errCollection)
   {
      MainWindow->Name->Caption = "?????? SQL. ?????????? ?????????? ??????!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg, Desc;
      int errorCode = 0;
      int iCount = errCollection->Count;
      _di_Error        errSingle;
      for(int i = 0; i < iCount; i++)
      {
         errCollection->Get_Item(i,errSingle);
         errSingle->Get_NativeError(errorCode);
         errormsg = " ?????? SQL: Code = " + IntToStr(errSingle->NativeError) + " Description: " + errSingle->Description + " " + MainWindow->PriceQuery->SQL->Text;
         MainWindow->log(errormsg);
      }
      return false;
   }
   MainWindow->CentralQuery->SQL->Text = MainWindow->PriceQuery->SQL->Text;
   try
   {
      MainWindow->CentralQuery->ExecSQL();
      errCollection = MainWindow->CashConnection->Errors;
      iCount = errCollection->Count;
      for(int i = 0; i < iCount; i++)
      {
         errCollection->Get_Item(i,errSingle);
         errSingle->Get_NativeError(SQL_RAISERROR_CODE);
      }
      if(SQL_RAISERROR_CODE)
      {
         throw errCollection;
      }
   }
   catch (EOleException &eException)
   {
    MainWindow->Name->Caption = "?????? SQL. ?????????? ?????????? ??????!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + MainWindow->CentralQuery->SQL->Text;
      MainWindow->log(errormsg);
      return false;
   }
   catch (_di_Errors &errCollection)
   {
      MainWindow->Name->Caption = "?????? SQL. ?????????? ?????????? ??????!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg, Desc;
      int errorCode = 0;
      int iCount = errCollection->Count;
      _di_Error        errSingle;
      for(int i = 0; i < iCount; i++)
      {
         errCollection->Get_Item(i,errSingle);
         errSingle->Get_NativeError(errorCode);
         errormsg = " ?????? SQL: Code = " + IntToStr(errSingle->NativeError) + " Description: " + errSingle->Description + " " + MainWindow->CentralQuery->SQL->Text;
         MainWindow->log(errormsg);
      }
      return false;
   }
 return true;
}
//-------------------------------------------------------------------------

void TfrReturnForm::Multiline(TWinControl *control)
{
    HWND h = control->Handle;
    SetWindowLong(h, GWL_STYLE, GetWindowLong(h, GWL_STYLE) | BS_MULTILINE);
    control->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::btnCancelPClick(TObject *Sender)
{
   PanelEnable(false);
}
//---------------------------------------------------------------------------
// ?????????? ??????? ??????? ??????? ? ??????????? ???????
void __fastcall TfrReturnForm::GetBillItem(std::vector<BillItemLine> &biLine)
{
   AnsiString BillNumber;
   biLine.clear();
   BillNumber = sgCheckList->Cells[CL_BILLNUM_COL][sgCheckList->Row];
   MainWindow->CentralQuery->SQL->Clear();
   MainWindow->CentralQuery->SQL->Add("SELECT pr.ScanCode, Name, Meas, Quantity, price, Quantity*price as Summ, NDS FROM retail rt ");
   MainWindow->CentralQuery->SQL->Add("CROSS APPLY (SELECT TOP 1 price.ScanCode, Name, Meas, NDS FROM price WHERE rt.IDnom = price.IDnom) pr");
   MainWindow->CentralQuery->SQL->Add("WHERE rt.BillNumber='" + BillNumber + "' AND rt.IDnom NOT IN (SELECT IDnom FROM GiftCard GROUP BY IDnom) AND rt.flag > 100");
   MainWindow->CentralQuery->SQL->Add("UNION ALL");
   MainWindow->CentralQuery->SQL->Add("SELECT gc.ScanCode, pr.Name, pr.Meas, 1 as Quantity, gc.Nominal as price, gc.Nominal as Summ, pr.NDS FROM GiftCard gc");
   MainWindow->CentralQuery->SQL->Add("CROSS APPLY (SELECT TOP 1 Name, Meas, NDS FROM Price WHERE gc.IDnom = Price.IDnom) pr");
   MainWindow->CentralQuery->SQL->Add("WHERE BillNumber='" + BillNumber + "' and Balance = Nominal");
   try
   {
      MainWindow->CentralQuery->Active = true;
   }
   catch (...)
   {

   }
   unsigned hyper count = MainWindow->CentralQuery->RecordCount;
   if(count == 0)
   {
      MainWindow->CentralQuery->Active = false;
      return;
   }
   BillItemLine bi;
   memset(&bi, 0, sizeof(bi));
   for(hyper i = 1; i <= count; i++)
   {
      bi.ScanCode = MainWindow->CentralQuery->FieldByName("ScanCode")->AsString;
      bi.Name = MainWindow->CentralQuery->FieldByName("Name")->AsString.Trim();
      bi.Measure = MainWindow->CentralQuery->FieldByName("Meas")->AsString;
      bi.Price = MainWindow->CentralQuery->FieldByName("Price")->AsFloat * 100 + 0.5;
      bi.Quantity = MainWindow->CentralQuery->FieldByName("Quantity")->AsFloat * 1000 + 0.5;
      bi.Sum = MainWindow->CentralQuery->FieldByName("Summ")->AsFloat * 100 + 0.5;
      bi.NDS = MainWindow->CentralQuery->FieldByName("NDS")->AsInteger;
      biLine.push_back(bi);
      MainWindow->CentralQuery->Next();
   }
   MainWindow->CentralQuery->Active = false;
}
//--------------------------------------------------------------------------
void __fastcall TfrReturnForm::GetPay(std::vector<ComboPay> &cPay)
{
   AnsiString BillNumber;
   BillNumber = sgCheckList->Cells[CL_BILLNUM_COL][sgCheckList->Row];
   MainWindow->CentralQuery->SQL->Clear();
   MainWindow->CentralQuery->SQL->Add("SELECT PayType, Amount, RoundPart, rcd.CardID, ISNULL(CType, 0) AS CType FROM RetPay rp");
   MainWindow->CentralQuery->SQL->Add("LEFT JOIN RetCardData rcd ON rp.Id = rcd.RetPayID");
   MainWindow->CentralQuery->SQL->Add(" WHERE  BillNumber='" + BillNumber + "' ORDER BY PayNum");
   try
   {
      MainWindow->CentralQuery->Active = true;
   }
   catch (...)
   {
      MainWindow->CentralQuery->Active = false;
   }
   unsigned hyper count = MainWindow->CentralQuery->RecordCount;
   if(count == 0)
   {
      MainWindow->CentralQuery->Active = false;
      return;
   }
   ComboPay cp;
   memset(&cp, 0, sizeof(cp));
   for(hyper i = 1; i <= count; i++)
   {
      if(MainWindow->CentralQuery->FieldByName("PayType")->AsString == "1 " || MainWindow->CentralQuery->FieldByName("PayType")->AsString == "3 ")
         cp.PayType = " " + MainWindow->CentralQuery->FieldByName("PayType")->AsString.Trim();
      else
         cp.PayType = MainWindow->CentralQuery->FieldByName("PayType")->AsString;
         double A = MainWindow->CentralQuery->FieldByName("Amount")->AsFloat * 100 + 0.5;
      cp.Sum =  A;
      cp.RoundPart = MainWindow->CentralQuery->FieldByName("RoundPart")->AsFloat * 100 +0.5;
      cp.CType = MainWindow->CentralQuery->FieldByName("CType")->AsInteger;
      strcpy(cp.CardID,MainWindow->CentralQuery->FieldByName("CardId")->AsString.c_str());
      cPay.push_back(cp);
      MainWindow->CentralQuery->Next();
   }
   MainWindow->CentralQuery->Active = false;
}
//---------------------------------------------------------------------------
// ???????? ??????? ????????
void __fastcall TfrReturnForm::BillReturn(bool delBase)
{
   bool pkBillPresent = false;  // ???? ??????? ?????? ?? ? ???????????? ????
   bool pkPayPresent = false; // ???? ??????? ???????? ?? ?? ? ???????????? ????
   AnsiString BillNumber = sgCheckList->Cells[CL_BILLNUM_COL][sgCheckList->Row];
// ????????? ??? ????????. ??? ????????? ??????????????? ? ???????????? ? ?????? ???????
   std::vector<BillItemLine> bi;
   GetBillItem(bi);
   for(std::vector<BillItemLine>::iterator it = bi.begin(); it != bi.end(); ++it)
   {
      // ?????????, ???? ??????? ????? 10 ????????, ?? ??? ?? - ??? ??? ????????? ?????????
      int AttMethodCalc(4), AttSubjectCalc(1); // ?????? ??????, ?????
      if(it->ScanCode.Length() == 10)
      {
         pkBillPresent = true;
         AttMethodCalc = 3;  // ?????
         AttSubjectCalc = 10;  // ??????
      }
      Star->FNOperation(OT_REFUND_REC,
         it->Price,
         it->Quantity*1000,
         it->NDS,
         AttMethodCalc, AttSubjectCalc,
         it->ScanCode +" "+ it->Name,
         it->Measure);
   }
 // ????????? ??????? ???????? ?? ?????
   std::vector<ComboPay> Pay;
   GetPay(Pay);
   for(std::vector<ComboPay>::iterator it = Pay.begin(); it != Pay.end(); ++it)
   {
      if(it->PayType == GIFT_CARD_PAYMENT) pkPayPresent = true;
   }
   if(!Star->FNCloseCheckEx(Pay))
   {
      MainWindow->log("?????? ???????? ???? ????????!");
      ModalResult = mrCancel;
      return;
   }
   if(delBase)   // ??????? ??? ?? ????, ???? ??? ?????
   {
      if(pkBillPresent)
      {
         for(std::vector<BillItemLine>::iterator it = bi.begin(); it != bi.end(); ++it)
         {
            if(it->ScanCode.Length() == 10) pkDeactivate(it->ScanCode);
         }
      }
      if(pkPayPresent)
      {
         for(std::vector<ComboPay>::iterator it = Pay.begin(); it != Pay.end(); ++it)
         {
            if(it->CType == 99) pkReturn(it->CardID, it->Sum);
         }
      }
      if(!retBillDelete(BillNumber))
      {
         MainWindow->log("?????? ???????? ???? ???????? ?? ??!");
         ModalResult = mrCancel;
         return;
       }
   }
   ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfrReturnForm::btnFRandDBClick(TObject *Sender)
{
   BillReturn(true);
   PanelEnable(false);
}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::btnFRonlyClick(TObject *Sender)
{
   BillReturn(false);
   PanelEnable(false);
}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::PanelEnable(bool enable)
{
   TDateTime dt1 = DateTimePicker->Date;
   TDate dt2 = Date();
   bool dateIdentity = (int(dt1) == int(dt2));
   btnFRandDB->Enabled = dateIdentity;
   if(enable)
   {
      gbReturnCheck->Enabled = false;
      gbReturnFree->Enabled = false;
      ReturnBtn->Enabled = false;
      CancelBtn->Enabled = false;
      rbReturnCheck->Enabled = false;
      rbReturnFree->Enabled = false;
      plConfirm->Enabled = true;
      plConfirm->Visible = true;
      DateTimePicker->Enabled = false;
   }
   else
   {
      plConfirm->Enabled = false;
      plConfirm->Visible = false;
      gbReturnCheck->Enabled = true;
      gbReturnFree->Enabled = true;
      ReturnBtn->Enabled = true;
      CancelBtn->Enabled = true;
      rbReturnCheck->Enabled = true;
      rbReturnFree->Enabled = true;
      DateTimePicker->Enabled = true;

   }
}
//--------------------------------------------------------------------------
void __fastcall TfrReturnForm::FormResize(TObject *Sender)
{
   int margin = 15;
   int btnHeight = 80;
   int btnWidth = 140;
   rbReturnCheck->Top = margin;
   rbReturnCheck->Left = margin;
   gbReturnCheck->Top = rbReturnCheck->Top + margin;
   gbReturnCheck->Left = margin;
   gbReturnCheck->Width = frReturnForm->ClientWidth - gbReturnCheck->Left*2;
   gbReturnCheck->Height = frReturnForm->ClientHeight/2 - gbReturnCheck->Top/2;
   sgCheckList->Top = margin*3;
   sgCheckList->Left = 0;
   sgCheckList->Width = gbReturnCheck->Width;
   sgCheckList->Height = gbReturnCheck->Height - sgCheckList->Top;
   sgCheckList->ColWidths[0] = 50;
   sgCheckList->ColWidths[1] = 150;
   sgCheckList->ColWidths[2] = 150;
   sgCheckList->ColWidths[3] = 150;
   sgCheckList->ColWidths[4] = 100;

   sgOneCheck->Top = margin*3;
   sgOneCheck->Left = 0;
   sgOneCheck->Width = gbReturnCheck->Width;
   sgOneCheck->Height = gbReturnCheck->Height - sgOneCheck->Top;
   sgOneCheck->ColWidths[4] = 100;
   sgOneCheck->ColWidths[3] = 100;
   sgOneCheck->ColWidths[2] = 80;
   sgOneCheck->ColWidths[0] = 50;
   sgOneCheck->ColWidths[1] = sgOneCheck->Width - sgOneCheck->ColWidths[0] - sgOneCheck->ColWidths[2] - sgOneCheck->ColWidths[3] - sgOneCheck->ColWidths[4];

   ReturnBtn->Height = btnHeight;
   ReturnBtn->Width = btnWidth;
   ReturnBtn->Top = frReturnForm->ClientHeight - ReturnBtn->Height - margin;
   ReturnBtn->Left = margin;

   CancelBtn->Top = ReturnBtn->Top;
   CancelBtn->Width = btnWidth;
   CancelBtn->Left = frReturnForm->ClientWidth - CancelBtn->Width - margin;
   CancelBtn->Height = btnHeight;

   rbReturnFree->Top = gbReturnCheck->Top + gbReturnCheck->Height + margin;
   rbReturnFree->Left = margin;
   gbReturnFree->Top = rbReturnFree->Top + margin;
   gbReturnFree->Left = margin;
   gbReturnFree->Width = frReturnForm->ClientWidth - margin*2;
   gbReturnFree->Height = ReturnBtn->Top - gbReturnFree->Top - margin;

   float plW = 0.8;
   plConfirm->Top = margin/3;
   plConfirm->Height = frReturnForm->ClientHeight - margin*2/3;
   plConfirm->Width = frReturnForm->Width * plW;
   plConfirm->Left = (frReturnForm->ClientWidth - plConfirm->Width)/2;

   float tlW = 0.6;
   edBillNumber->Top = margin;
   edBillNumber->Left = (plConfirm->Width - edBillNumber->Width)/2;
   sgThingsList->Top = edBillNumber->Top + edBillNumber->Height + margin/3;
   sgThingsList->Left = margin;
   sgThingsList->Height = plConfirm->Height * tlW - sgThingsList->Top;
   sgThingsList->Width = plConfirm->Width - margin*2;
   sgThingsList->ColWidths[2] = 100;
   sgThingsList->ColWidths[1] = 10;
   sgThingsList->ColWidths[0] = sgThingsList->Width - sgThingsList->ColWidths[2] - sgThingsList->ColWidths[1];

   lbSum->Top = sgThingsList->Top + sgThingsList->Height + margin/5;
   lbSum->Left = margin * 6;
   edSum->Top = lbSum->Top;
   edSum->Left = plConfirm->Width - edSum->Width - margin * 6;
   edSum->Height = lbSum->Height;

   int btnH = 50;
   btnFRandDB->Height = btnH;
   btnFRandDB->Top = plConfirm->Height - btnFRandDB->Height - margin;
   btnFRandDB->Width = plConfirm->Width/3 - margin*2;
   btnFRandDB->Left = margin;
   btnFRonly->Top = btnFRandDB->Top;
   btnFRonly->Height = btnH;
   btnFRonly->Width = btnFRandDB->Width;
   btnFRonly->Left = btnFRandDB->Left + btnFRandDB->Width + margin*2;
   btnCancelP->Top = btnFRandDB->Top;
   btnCancelP->Height = btnH;
   btnCancelP->Width = btnFRandDB->Width;
   btnCancelP->Left = btnFRonly->Left + btnFRonly->Width + margin*2;

   sgPays->Top = lbSum->Top + lbSum->Height + margin/5;
   sgPays->Left = margin;
   sgPays->Width = plConfirm->Width * 0.8;
   sgPays->Height = btnFRandDB->Top - margin/2 - sgPays->Top;
   sgPays->ColWidths[1] = 70;
   sgPays->ColWidths[0] = sgPays->Width - sgPays->ColWidths[1];
}
//---------------------------------------------------------------------------
void __fastcall TfrReturnForm::pkReturn(AnsiString ScanCode, hyper Sum)
{
   MainWindow->CentralQuery->SQL->Clear();
   MainWindow->CentralQuery->SQL->Add("UPDATE GiftCard SET Balance+=" + MainWindow->MoneyAsString(Sum) + " WHERE ScanCode = '" + ScanCode + "'");
   MainWindow->CentralQuery->SQL->Add("DELETE FROM GiftCardPayment WHERE ScanCode='"+ScanCode+"' AND Amount = " + MainWindow->MoneyAsString(Sum));
   try
   {
      MainWindow->CentralQuery->ExecSQL();
   }
   catch(EOleException &e)
   {
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+e.Source+"\" ErrorCode="+IntToStr(e.ErrorCode)+" Message=\""+e.Message+"\"" + MainWindow->PriceQuery->SQL->Text;
      MainWindow->log(errormsg);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrReturnForm::pkDeactivate(AnsiString ScanCode)
{
   MainWindow->CentralQuery->SQL->Clear();
   MainWindow->CentralQuery->SQL->Add("UPDATE GiftCard SET Balance=0,ActivationDate=NULL, StartDate=NULL, Seller='', SCash='',BillNumber='',flag=1 WHERE ScanCode='" + ScanCode + "'");
   try
   {
      MainWindow->CentralQuery->ExecSQL();
   }
   catch(EOleException &e)
   {
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+e.Source+"\" ErrorCode="+IntToStr(e.ErrorCode)+" Message=\""+e.Message+"\"" + MainWindow->PriceQuery->SQL->Text;
      MainWindow->log(errormsg);
   }
}
//---------------------------------------------------------------------------
void __fastcall TfrReturnForm::GetBillList(TDate dt)
{
   AnsiString today = dt.FormatString("yyyy.MM.dd");
   AnsiString tomorrow = (dt + 1.0).FormatString("yyyy.MM.dd");
   MainWindow->CentralQuery->SQL->Clear();
   MainWindow->CentralQuery->SQL->Add("SELECT BillDateTime, rt.BillNumber, rp.Summ, rp.PayType FROM retail rt");
   MainWindow->CentralQuery->SQL->Add("CROSS APPLY (SELECT SUM(Amount) as Summ, t1.BillNumber,");
   MainWindow->CentralQuery->SQL->Add("STUFF((select N',' + CASE RTRIM(LTRIM(t2.PayType)) WHEN '1' THEN '?' WHEN '3' THEN '??' WHEN '10' THEN '??' END from RetPay t2 where t1.BillNumber = t2.BillNumber for xml path(''), type).value('.','nvarchar(max)'),1,1,'') as PayType");
   MainWindow->CentralQuery->SQL->Add("FROM RetPay t1 WHERE t1.BillNumber = rt.BillNumber group by t1.BillNumber) rp");
   MainWindow->CentralQuery->SQL->Add("WHERE BillDateTime > '" + today + "' and BillDateTime<'" + tomorrow + "' and flag>100 and SCash=" + Star->Serial + " GROUP BY BillDateTime, rt.BillNumber, rp.Summ, rp.PayType ORDER BY BillDateTime DESC");
   try
   {
      MainWindow->CentralQuery->Active = true;
   }
   catch (...)
   {

   }
   MainWindow->ClearGrid(sgCheckList);
   unsigned hyper count = MainWindow->CentralQuery->RecordCount;
   if(count == 0)
   {
      MainWindow->CentralQuery->Active = false;
      return;
   }
   for(hyper i = 1; i <= count; i++)
   {
      sgCheckList->RowCount = count + 1;
      sgCheckList->Cells[CL_NUMBER_COL][i] = i;
      sgCheckList->Cells[CL_DATE_COL][i] = MainWindow->CentralQuery->FieldByName("BillDateTime")->AsString;
      sgCheckList->Cells[CL_BILLNUM_COL][i] = MainWindow->CentralQuery->FieldByName("BillNumber")->AsString;
      sgCheckList->Cells[CL_SUM_COL][i] = MainWindow->CentralQuery->FieldByName("Summ")->AsString;
      sgCheckList->Cells[CL_PAYTYPE_COL][i] = MainWindow->CentralQuery->FieldByName("PayType")->AsString;
      MainWindow->CentralQuery->Next();
   }
   MainWindow->CentralQuery->Active = false;
   sgOneCheck->Row = 1;

}
//---------------------------------------------------------------------------

void __fastcall TfrReturnForm::DateTimePickerChange(TObject *Sender)
{
   GetBillList(DateTimePicker->Date);
}
//---------------------------------------------------------------------------
void __fastcall TfrReturnForm::ClearGrid(TStringGrid *Grid)
{
   for(int i = 0; i< Grid->RowCount; i++)
   {
      Grid->Rows[i]->Clear();
   }
   Grid->RowCount = 1;
}
