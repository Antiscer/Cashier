
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("cashier.res");
USEFORM("mainform.cpp", MainWindow);
USEFORM("SelectPayTypeFormUnit.cpp", SelectPayTypeForm);
USEUNIT("clock.cpp");
USEUNIT("smartcardreader.cpp");
USELIB("CASMCARD.LIB");
USEFORM("screensaver.cpp", Saver);
USEUNIT("scanner.cpp");
USEFORM("cash.cpp", CashForm);
USEUNIT("mstar.cpp");
USELIB("pilot_nt.lib");
USEUNIT("CopyRes.cpp");
USE("CopyRes.h", File);
USEFORM("FreeCheck.cpp", FreeCheckForm);
USE("Struct.h", File);
USEFORM("billprintc.cpp", BillPrint);
USEFORM("frReturn.cpp", frReturnForm);
USEFORM("fSverka.cpp", FormSverka);
USEUNIT("Receipt.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "Кассир";
         Application->CreateForm(__classid(TMainWindow), &MainWindow);
       Application->CreateForm(__classid(TSaver), &Saver);
       Application->CreateForm(__classid(TCashForm), &CashForm);
       Application->CreateForm(__classid(TFreeCheckForm), &FreeCheckForm);
       Application->CreateForm(__classid(TBillPrint), &BillPrint);
       Application->CreateForm(__classid(TfrReturnForm), &frReturnForm);
       Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------





