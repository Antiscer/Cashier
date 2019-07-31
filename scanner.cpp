//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "scanner.h"
#include "mainform.h"

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

__fastcall TScanner::TScanner(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    Port = 0;
    Baud = 9600;
    hComm = NULL;
    Priority = tpLowest;
}
//---------------------------------------------------------------------------
void __fastcall TScanner::Transmit()
{
    MainWindow->ScannerAnswer((AnsiString)szReturn);
}
//---------------------------------------------------------------------------
void __fastcall TScanner::Execute()
{
    static char *szComPort[] = {"COM1", "COM2", "COM3", "COM4", "COM5", "COM6"};
    static DCB    dcb;
    DWORD dwEvMask, dwError;
    COMSTAT csStat;
    static char szRead[256];
    unsigned long nChar, Length;

    if ((hComm = CreateFile (szComPort [Port-1], GENERIC_READ,
                  0, NULL, OPEN_EXISTING, NULL , NULL)) == INVALID_HANDLE_VALUE)
    {
        return;
    }

    SetupComm (hComm, 256, 256);       // allocate transmit & receive buffer
    dcb.BaudRate = Baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (SetCommState (hComm, &dcb) < 0)
    {
        CloseHandle (hComm);                    // close the COM port.
        return;
    }
    if (!SetCommMask(hComm, EV_RXCHAR | EV_BREAK))
    {
        CloseHandle (hComm);                    // close the COM port.
        return;
    }
    while(!Terminated)
    {
        //WaitCommEvent(hComm, &dwEvMask, NULL);
        Sleep(50);
        if (dwEvMask & EV_RXCHAR)
        {
            szReturn[0] = 0;
            Length = 0;
            ClearCommError(hComm, &dwError, &csStat);
            while(csStat.cbInQue > 0)
            {
                if(ReadFile(hComm, szRead, csStat.cbInQue, &nChar, NULL))
                {
                    strncat(szReturn, szRead, nChar);
                    Length += nChar;
                }
                else break;
                Sleep(50);
                ClearCommError(hComm, &dwError, &csStat);
            }
            if(Length)
            {
                szReturn[Length-1] = 0;
                Synchronize(Transmit);
            }
        }
    }
}
//---------------------------------------------------------------------------
