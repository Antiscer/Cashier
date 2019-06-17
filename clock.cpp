//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "clock.h"
#include "mainform.h"
#pragma package(smart_init)

#define FR_CONNSTATUS_FREQ 120
#define SQL_CONNSTATUS_FREQ 60
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TClockThread::TClockThread(TMStarF *_Star, FRStatus *_clFRStatus, bool CreateSuspended)
    : TThread(CreateSuspended)
{
// ���������� �������� �� MainForm � TMStarF
   clFRStatus = _clFRStatus;
   Star = _Star;
   evConnStatus = OpenEvent(EVENT_ALL_ACCESS,false, "ConnectionStatus");
   evSQLConnStatusOK = OpenEvent(EVENT_ALL_ACCESS,false, "SQLConnectionStatusOK");
//   ::CoInitializeEx(0, COINIT_MULTITHREADED);
   Conn = new TADOConnection(NULL);
   Conn->ConnectionString = MainWindow->SQLServerCentral;
   Conn->LoginPrompt = false;
   Query = new TADOQuery(NULL);
   Query->Connection = Conn;
  // Query

}
//---------------------------------------------------------------------------
void __fastcall TClockThread::RefreshTime(void)
{
    MainWindow->StatusBar->Panels->Items[4]->Text = DateTimeToStr(Now());
    DWORD dwConnStatus = WaitForSingleObject(evConnStatus, 1);
    switch (dwConnStatus)
    {
        case WAIT_FAILED:
           break;
        case WAIT_TIMEOUT:
           break;
        case WAIT_OBJECT_0:
           Star->GetConnectionStatus();
           break;
    }
    DWORD dwSQLConnStatus = WaitForSingleObject(evSQLConnStatusOK, 1);
    switch (dwSQLConnStatus)
    {
       case WAIT_FAILED:
          break;
       case WAIT_TIMEOUT:
          break;
       case WAIT_OBJECT_0:
          MainWindow->SetConnStatus(CheckSQLConnection());
          break;
          }
    // ���������� ���������� �� �������� � ���
    MainWindow->StatusBar->Panels->Items[5]->Text = "��� ���: " + AnsiString(*clFRStatus->OFDquantityMsg);
}
//---------------------------------------------------------------------------
void __fastcall TClockThread::Execute()
{
   int s = 0;
   int conn = 0;
    while(!Terminated)
    {
        Synchronize(RefreshTime);
        Sleep(1000);
        if((s--) <=0)
        {
          SetEvent(evConnStatus);
          s = FR_CONNSTATUS_FREQ;
        }
        if((conn--) <= 0)
        {
            SetEvent(evSQLConnStatusOK);
            conn = SQL_CONNSTATUS_FREQ;
        }

    }
}
//---------------------------------------------------------------------------
bool __fastcall TClockThread::CheckSQLConnection()
{

   Query->SQL->Text = "SELECT 1";
   try
   {
      Conn->Connected = true;
      Query->Active = true;
   }
   catch (EOleException &eException)
    {
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + Query->SQL->Text;
      Query->Active = false;
      Conn->Connected = false;
      return false;
    }
    if(Query->RecordCount > 0)
    {
      Conn->Connected = false;
      return true;
    }
    else
    {
      Conn->Connected = false;
      return false;
    }
}
