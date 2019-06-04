//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CopyRes.h"
#include "mainform.h"
#include "fstream.h"

#pragma package(smart_init)
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
__fastcall TCopyRes::TCopyRes(bool CreateSuspended)
    : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall TCopyRes::CheckTime()
{
  Word Hour, Min, Sec, MSec;
  TDateTime dtPresent = Now();
  DecodeTime(dtPresent, Hour, Min, Sec, MSec);
  if((Hour>=20) && !(MainWindow->Session))
    {
    MainWindow->SendFiles();
    }
}
//---------------------------------------------------------------------------
void __fastcall TCopyRes::Execute()
{
    while(!Terminated)
    {
        Synchronize(CheckTime);
        Sleep(300000);  //Раз в пять минут
        try
          {
          MainWindow->SynchronizeServer();
          }
        catch( ... ){}
    }
}
//---------------------------------------------------------------------------

