//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "clock.h"
#include "mainform.h"
#pragma package(smart_init)

#define CONNSTATUS_FREQ 120
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
// экземпляры объектов из MainForm и TMStarF
   clFRStatus = _clFRStatus;
   Star = _Star;
   evConnStatus = OpenEvent(EVENT_ALL_ACCESS,false, "ConnectionStatus");
}
//---------------------------------------------------------------------------
void __fastcall TClockThread::RefreshTime(void)
{
    MainWindow->StatusBar->Panels->Items[3]->Text = DateTimeToStr(Now());
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
    // количество документов на отправку в ОФД
    MainWindow->StatusBar->Panels->Items[4]->Text = "ОФД док: " + AnsiString(*clFRStatus->OFDquantityMsg);
}
//---------------------------------------------------------------------------
void __fastcall TClockThread::Execute()
{
   int s = 0; 
    while(!Terminated)
    {
        Synchronize(RefreshTime);
        Sleep(1000);
        if((s--) <=0)
        {
          SetEvent(evConnStatus);
          s = CONNSTATUS_FREQ;
        }
    }
}
//---------------------------------------------------------------------------
