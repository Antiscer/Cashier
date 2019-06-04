//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "smartcardreader.h"
#include <winscard.h>
#include "mainform.h"

#define IOCTL_SMARTCARD_SET_CARD_TYPE SCARD_CTL_CODE(2060)

#pragma package(smart_init)
#pragma comment(lib,"winscard.lib")
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
SCARDCONTEXT SCardContext;
SCARDHANDLE  SCardHandle;
SCARD_READERSTATE SCardReaderState;
LPTSTR       ReaderName;
hyper Old_Result = 1;
//---------------------------------------------------------------------------
__fastcall TSmartCardReader::TSmartCardReader(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    Priority = tpIdle;
}
//---------------------------------------------------------------------------
void __fastcall TSmartCardReader::CardType(void)
{
    switch(Result)
    {
        case SCARD_S_SUCCESS:
            break;
        case SCARD_ABSENT:
            strcpy(Data, "В аппарате нет карты");
            break;
        default:
            strcpy(Data, "Неизвестная ошибка");
    }
    MainWindow->ReaderAnswer(Result);
}
//---------------------------------------------------------------------------
void __fastcall TSmartCardReader::TerminateCancel()
{
    SCardCancel(SCardContext);
    SCardDisconnect(SCardHandle, SCARD_LEAVE_CARD);
}
//---------------------------------------------------------------------------
void __fastcall TSmartCardReader::Execute()
{
    DWORD SCardProtocol;
    LPCSCARD_IO_REQUEST lpIORequest = SCARD_PCI_T0;
    BYTE SendBuffer[] = { 0xff, 0xb2, 0x01, 0x00, 0x20 };
    DWORD dwSize = 256;
    DWORD ReaderNameLength;

    AnsiString Test;

    int nCardType1 = 16; //SLE4428
    BYTE cbOutBuffer[10];
    DWORD dwBytesRet;
    BYTE EZ100PRinBuf1[] = {0xCA,0x28,0x01,0x00,0xFF};
//    BYTE CardType2[] = { 0xff, 0xa4, 0x00, 0x00, 0x01, 0x05 };//SLE4428
//    Byte SendBuffer2[] = {0xff, 0xb0, 0x01, 0x00, 0x20}

    Result = SCardEstablishContext(SCARD_SCOPE_USER,
        NULL,
        NULL,
        &SCardContext);
    if(Result != SCARD_S_SUCCESS)
    {
        Synchronize(CardType);
        TerminateCancel();
        return;
    }
    Result = SCardListReaders(SCardContext, NULL, NULL, &ReaderNameLength);
    if(Result != SCARD_S_SUCCESS)
    {
        Synchronize(CardType);
        TerminateCancel();
        return;
    }
    ReaderName = (LPTSTR) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ReaderNameLength);
    Result = SCardListReaders(SCardContext,
        NULL,
        ReaderName,
        &ReaderNameLength);
    if(Result != SCARD_S_SUCCESS)
    {
        Synchronize(CardType);
        TerminateCancel();
        return;
    }
    SCardReaderState.szReader = (LPCTSTR)ReaderName;
    SCardReaderState.dwCurrentState = SCARD_STATE_UNAWARE;

    if((AnsiString)ReaderName == "ACS CCID USB Reader 0")
       SendBuffer[1] = 0xb0;

    while(!Terminated)
    {
        Result = SCardGetStatusChange(SCardContext,
            INFINITE,
            &SCardReaderState,
            1);
        if((SCardReaderState.dwCurrentState & 0xFF)
            == (SCardReaderState.dwEventState & 0xFF))
        {
            SCardReaderState.dwCurrentState = SCardReaderState.dwEventState;
            continue;
        }
        SCardReaderState.dwCurrentState = SCardReaderState.dwEventState;
        if(SCardReaderState.dwEventState & SCARD_STATE_PRESENT)
        {
            Result = SCardConnect(SCardContext,
                SCardReaderState.szReader,
                SCARD_SHARE_SHARED,
                SCARD_PROTOCOL_T0,
                &SCardHandle,
                &SCardProtocol);
    	    if(Result != SCARD_S_SUCCESS)
            {
                Synchronize(CardType);
                continue;
            }
            Result = SCardBeginTransaction( SCardHandle );
    	    if(Result != SCARD_S_SUCCESS)
            {
                Synchronize(CardType);
                continue;
            }
            //Установка типа карты
            if((AnsiString)ReaderName == "ACS ACR38U 0")
              {
              Result = SCardControl(SCardHandle,IOCTL_SMARTCARD_SET_CARD_TYPE,
              &nCardType1,sizeof(nCardType1),cbOutBuffer,10,&dwBytesRet);
    	      if(Result != SCARD_S_SUCCESS) {Synchronize(CardType); continue; }
              }
//
            if((AnsiString)ReaderName == "CASTLES EZ100PR 0" || (AnsiString)ReaderName == "CASTLES EZ100PU 0" )
            {
            //Чтение Ez через Control
            Result = SCardControl(SCardHandle,
            0x00312000,
            EZ100PRinBuf1,
            5,
            Data,
            256,
            &dwBytesRet);
            if(Result != SCARD_S_SUCCESS) {Synchronize(CardType); continue; }
            Test = String(&Data[6]);
            Result = SCardControl(SCardHandle,
            0x00312000,
            EZ100PRinBuf1,
            5,
            Data,
            256,
            &dwBytesRet);
            for(int i=0;i<255;i++) Data[i]=Data[i+1];
            if(Test != String(&Data[5])) Result = ILLEGAL_CARD;
            }
            else
            {
            Result = SCardTransmit(
              SCardHandle,
              lpIORequest,
              SendBuffer, //sizeof(pbSendBuffer),
              5, NULL,
              (LPBYTE)Data,
              &dwSize);
            Test = String(&Data[5]);
            Result = SCardTransmit(
              SCardHandle,
              lpIORequest,
              SendBuffer, //sizeof(pbSendBuffer),
              5, NULL,
              (LPBYTE)Data,
              &dwSize);
            if(Test != String(&Data[5])) Result = ILLEGAL_CARD;
            }
    	    if(Result != SCARD_S_SUCCESS)
            {
                Synchronize(CardType);
                continue;
            }
            SCardDisconnect(SCardHandle, SCARD_LEAVE_CARD);
        }
        else Result = SCARD_ABSENT;
        Synchronize(CardType);
    }
}
//---------------------------------------------------------------------------
