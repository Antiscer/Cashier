//---------------------------------------------------------------------------

#include <vcl.h>
#include "fstream.h"
#pragma hdrstop

#include "mstar.h"
#include "mainform.h"
#pragma package(smart_init)

#define CHLEN 20

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
// создание и инициализация экземпляра класса потока (Конструктор)
// CreateSuspended = True - метод Execute() выполняется только после метода Resume()
// CreateSuspended = False - метод Execute() выполняется немедленно
__fastcall TMStarF::TMStarF(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    log("Запуск потока обмена COM-порта");
    Port = 0;
    Baud = 57600;
    ErrorCount = 0;
    hComm = NULL;
    Priority = tpTimeCritical;
    KKMResult = "FFFF";
    KKMStatus = "0000";
    PrinterStatus = "00";
    Slip = 0;
    BillNumber = "-1";
    flag21 = false;
    Serial = "";
    SerialID = "";
    Wait = true;
//    stCom->nOut=0;
    Mode = 255;
    hEvent1 = OpenEvent(EVENT_ALL_ACCESS,false, "evBillSuccess");
    hPrnEvent = OpenEvent(EVENT_ALL_ACCESS,false, "evPrintSuccess");
    evConnStatus = OpenEvent(EVENT_ALL_ACCESS,false, "ConnectionStatus");
    InitializeCriticalSection(&starCS);
}
//----------------------------------------------------------------------------
// открытие чека. Sale - распродажа
bool __fastcall TMStarF::BillOpen(bool Sale, AnsiString OperatorName, int OperatorNumber)
{
    AnsiString str;
    int p;
    int ypos = 0;
    AnsiString BillString;

    struct tagReq
    {
        char type[3];
        char flag[3];
        char x[3];
        char y[4];
        char str[41];
    };
    struct
    {
        char stx;
        char code;
        char psw[5];
        char operation;
        char a;
        char docflag[3];
        char number[4];
        tagReq Req[10];
        char bcc[2];
        char etx;
    } Cmd = {0x02, 0x53, "", 0x30, 0, "00", "010"};

    struct
    {
        char stx;
        char psw[4];
        char sbyte;
        char code[3];
        char date[7];
        char time[5];
        char doctype[3];
        char OperName[12];
        char OperNumber[3];
        char table;
        char place;
        char Ncopy[3];
        char Naccount;
        char etx;
        char bcc[4];
    }CmdP = {0x02, "",0,"10\x1c","","","00\x1c","","",0x1c,0x1c,"01\x1c",0x1c,0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char BillNumber[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;
// запрос-ответ для Штриха
    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char type;
        char bcc;    // BCC	B	2
    } CmdS = {0x02,6,0x8d,"",0,0};
   // Штрих-Онлайн
    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    // запрос-ответ для Штриха-онлайн
    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char type;
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,6,0x8d,"",0,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;
  switch(FRType)
  {
  case 0:
  default:
    BillNumber = GetBillNumber();
    BillString = "       ЧЕК "+BillNumber.SubString(1,2)+"-"+BillNumber.SubString(3,4)+"-"+BillNumber.SubString(7,4)+"-"+BillNumber.SubString(11,4)+"-"+BillNumber.SubString(15,4);
    for(int j = 0; j <10; j++)
    {
        for (int i = 0; i < 41; i++) Cmd.Req[j].str[i] = 0;
    }
// Наименование учреждения
    strcpy(Cmd.Req[0].type, "01");
    strcpy(Cmd.Req[0].flag, "00");
    strcpy(Cmd.Req[0].x, "00");
    strcpy(Cmd.Req[0].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
// ИНН
    strcpy(Cmd.Req[1].type, "10");
    strcpy(Cmd.Req[1].flag, "00");
    strcpy(Cmd.Req[1].x, "12");
    strcpy(Cmd.Req[1].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
    ypos++;
// Номер документа
    strcpy(Cmd.Req[2].type, "07");
    strcpy(Cmd.Req[2].flag, "00");
    strcpy(Cmd.Req[2].x, "04");
    strcpy(Cmd.Req[2].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
// Номер ФР
    strcpy(Cmd.Req[3].type, "00");
    strcpy(Cmd.Req[3].flag, "00");
    strcpy(Cmd.Req[3].x, "24");
    strcpy(Cmd.Req[3].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
// ===
    strcpy(Cmd.Req[4].type, "99");
    strcpy(Cmd.Req[4].flag, "00");
    strcpy(Cmd.Req[4].x, "00");
    strcpy(Cmd.Req[4].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[4].str, "========================================");
    ypos++;

// ===
    strcpy(Cmd.Req[5].type, "99");
    strcpy(Cmd.Req[5].flag, "00");
    strcpy(Cmd.Req[5].x, "00");
    strcpy(Cmd.Req[5].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[5].str, BillString.c_str());
    StrWinToDos(Cmd.Req[5].str,100);
    ypos++;

// ===
    strcpy(Cmd.Req[6].type, "99");
    strcpy(Cmd.Req[6].flag, "00");
    strcpy(Cmd.Req[6].x, "00");
    strcpy(Cmd.Req[6].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[6].str, "========================================");
    ypos++;
// ===
    if(Sale) ypos++;
    strcpy(Cmd.Req[7].type, "99");
    strcpy(Cmd.Req[7].flag, "00");
    strcpy(Cmd.Req[7].x, "00");
    strcpy(Cmd.Req[7].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    if(Sale) strcpy(Cmd.Req[8].str, "      Внимание! Уцененный товар");
    else strcpy(Cmd.Req[7].str, "");
    StrWinToDos(Cmd.Req[7].str,100);
// ===
    if(Sale) ypos++;
    strcpy(Cmd.Req[8].type, "99");
    strcpy(Cmd.Req[8].flag, "00");
    strcpy(Cmd.Req[8].x, "00");
    strcpy(Cmd.Req[8].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    if(Sale) strcpy(Cmd.Req[9].str, "     возврату и обмену НЕ подлежит");
    else strcpy(Cmd.Req[8].str, "");
    StrWinToDos(Cmd.Req[8].str,100);

    // ===
    if(Sale) ypos++;
    strcpy(Cmd.Req[9].type, "99");
    strcpy(Cmd.Req[9].flag, "00");
    strcpy(Cmd.Req[9].x, "00");
    strcpy(Cmd.Req[9].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    if(Sale) strcpy(Cmd.Req[9].str, "========================================");
    else strcpy(Cmd.Req[9].str, "");

    Cmd.etx = 0x03;
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.sbyte = NextSbyte();
    SetDateTimeStr(CmdP.date);
    str = OperatorName;
    if((p = str.Pos(" "))>0) str = str.SubString(1,p-1);
    p = str.Length();
    for(;p<11;p++) str += " ";
    str = str.SubString(1,11);
    str += "|";
    strncpy(CmdP.OperName, str.c_str(), 12);
    CmdP.OperNumber[0] = 0;
    StrWinToDos(CmdP.OperName,12);
    strncpy(CmdP.OperNumber, Format("%2.2d", ARRAYOFCONST((OperatorNumber))).c_str(),2);
    CmdP.OperNumber[2] = 0x1c;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 4:  // ШТРИХ
    BillNumber = GetBillNumber();
//    if(!PrintEAN()) // if(!PrintEAN()) if(!PrintEAN())
//      {
//        KKMResult = "00FF";
//        KKMStatus = "0000";
//        PrinterStatus = "00";
//      }
    BillString = "       ЧЕК "+BillNumber.SubString(1,2)+"-"+BillNumber.SubString(3,4)+"-"+BillNumber.SubString(7,4)+"-"+BillNumber.SubString(11,4)+"-"+BillNumber.SubString(15,4);
//    SellSumm = GetSellSumm();
    if( MainWindow->Grid->RowCount > 100 )
      {
      StrihWriteTable(1,1,21,"\x01",1);
      flag21 = true;
      }

    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
        if(KKMResult == "0000")
          {
          Print("========================================",true);
          Print(BillString.c_str(),true);
          Print("========================================",true);
          }
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }

    //PrintEAN();

  break;
    case 8:  // ШТРИХ-онлайн
    BillNumber = GetBillNumber();
    stBill->BillNumber = BillNumber;
//    if(!PrintEAN()) // if(!PrintEAN()) if(!PrintEAN())
//      {
//        KKMResult = "00FF";
//        KKMStatus = "0000";
//        PrinterStatus = "00";
//      }
    BillString = "       ЧЕК "+BillNumber.SubString(1,2)+"-"+BillNumber.SubString(3,4)+"-"+BillNumber.SubString(7,4)+"-"+BillNumber.SubString(11,4)+"-"+BillNumber.SubString(15,4);
//    SellSumm = GetSellSumm();
    if( MainWindow->Grid->RowCount > 100 )
      {
      StrihWriteTable(1,1,21,"\x01",1);
      flag21 = true;
      }

    strncpy(CmdSo.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
        if(KKMResult == "0000")
          {
          PrintEAN();
          Print("========================================",true);
          Print(BillString.c_str(),true);
          Print("========================================",true);
          }
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
        return false;
    }
  }
return true;
}
//---------------------------------------------------------------------------
// возврат, в ШТРИХ не используется!!!
void __fastcall TMStarF::BillReturn(bool Sale, AnsiString OperatorName, int OperatorNumber)
{
    AnsiString str;
    int p;

    struct
    {
        char stx;
        char psw[4];
        char sbyte;
        char code[3];
        char date[7];
        char time[5];
        char doctype[3];
        char OperName[12];
        char OperNumber[3];
        char table;
        char place;
        char Ncopy[3];
        char Naccount;
        char etx;
        char bcc[4];
    }CmdP = {0x02, "",0,"10\x1c","","","05\x1c","","",0x1c,0x1c,"01\x1c",0x1c,0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char BillNumber[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

  switch(FRType)
  {
  case 0:
  default:
    break;
  case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.sbyte = NextSbyte();
    SetDateTimeStr(CmdP.date);
    str = OperatorName;
    if((p = str.Pos(" "))>0) str = str.SubString(1,p-1);
    p = str.Length();
    for(;p<11;p++) str += " ";
    str = str.SubString(1,11);
    str += "|";
    strncpy(CmdP.OperName, str.c_str(), 12);
    CmdP.OperNumber[0] = 0;
    StrWinToDos(CmdP.OperName,12);
    strncpy(CmdP.OperNumber, Format("%2.2d", ARRAYOFCONST((OperatorNumber))).c_str(),2);
    CmdP.OperNumber[2] = 0x1c;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}

//---------------------------------------------------------------------------
bool __fastcall TMStarF::BillZK(int OperatorNumber, char* Card)
{
int k,p;
AnsiString TmpStr;
long TmpLong,bn,ss;

// ШТРИХ-онлайн
        struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char nal[5];
        unsigned char cred[5];
        unsigned char tara[5];
        unsigned char card[5];
        char discount[2];
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,71,0x85,"","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0",0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char sdacha[5];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

switch(FRType)
{
  case 8:
    Print("========================================",true);
    Print(" ",true);
    for(int i=0;i<6;i++)
    {
      if(stBill->MidText[i] != "") {Print((stBill->MidText[i]).c_str(),true);}
      else {break;}
    }
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    memcpy(CmdSo.text," ",2);
// обращаемся к компоненту VCL, нужно делать syncronyze?
    TmpStr = MainWindow->TSum->Text;
    p = TmpStr.Pos(".");

    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return false;

    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdSo.card),&TmpLong,4);
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }

    if(frStatus->OperationResult != 0) return false;
//    Sleep(1000);
//    GetVersion(); //Сюда заложено ожидание окончания печати

  }
return true;
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::BillLine(char* Name, char* Price, char* Qnty, int Num, char * Meas, char * Code, char * NDS)
{
     AnsiString str,TmpStr;
     long TmpLong;
     int m;
     char * p;
     int ypos = 0;
     char* point;
     int ks, bs;
     char buf[41];
     int inbuf;
     int i,j,k;

    struct tagReq
    {
        char type[3];
        char flag[3];
        char x[3];
        char y[4];
        char str[41];
    };
    struct tagPrice
    {
        tagReq Req[3];
        char type[3];
        char flag[3];
        char x[3];
        char y[4];
        char sec[3];
        char code[5];
        char disc[6];
        char qnty[12];
        char price[12];
        char unit[3];
    };
    struct
    {
        char stx;
        char code;
        char psw[5];
        char operation;
        char a;
        char docflag[3];
        char number[4];
        tagPrice Price[CHLEN];
        char bcc[2];
        char etx;
    } Cmd = {0x02, 0x53, "", 0x30, 0, "00"};

    struct
    {
        char stx;
        char sbyte;
        char code[3];
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char SellSumm[15];      //Сумма строки
        char DocSumm[15];       //Сумма текущая по чеку
        char etx;
        char bcc[4];
    }Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char qnty[5];
        char price[5];
        char dep;
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdS = {0x02,60,0x80,"","","",0,0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

// ШТРИХ-онлайн

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char qnty[5];
        char price[5];
        char dep;
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,60,0x80,"","","",0,0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    switch(FRType)
    {
    default:
    case 0:
    Cmd.etx = 0x03;
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;

    point = (char*) &Cmd.Price[0].Req[0].type[0];
    for(int j = 0; j < sizeof(tagPrice)*CHLEN; j++) *(point+j) = 0;

    ks = 0;
    bs = 0;
    while(true)
    {

// NNum + Name + Unit 3 строки
        AnsiString str = AnsiString(&Name[ks*131]);
        strcpy(Cmd.Price[bs].Req[0].type, "99");
        strcpy(Cmd.Price[bs].Req[0].flag, "00");
        strcpy(Cmd.Price[bs].Req[0].x, "00");
        strcpy(Cmd.Price[bs].Req[0].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        strncpy(Cmd.Price[bs].Req[0].str, str.c_str(), 40);
        StrWinToDos(Cmd.Price[bs].Req[0].str,100);
        ypos++;
        strcpy(Cmd.Price[bs].Req[1].type, "99");
        strcpy(Cmd.Price[bs].Req[1].flag, "00");
        strcpy(Cmd.Price[bs].Req[1].x, "00");
        strcpy(Cmd.Price[bs].Req[1].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        if(strlen(str.c_str()) > 40) strncpy(Cmd.Price[bs].Req[1].str, &(str[41]), 40);
        else Cmd.Price[bs].Req[1].str[0] = 0;
        StrWinToDos(Cmd.Price[bs].Req[1].str,100);
        if(strlen(Cmd.Price[bs].Req[1].str) > 0) ypos++;
        strcpy(Cmd.Price[bs].Req[2].type, "99");
        strcpy(Cmd.Price[bs].Req[2].flag, "00");
        strcpy(Cmd.Price[bs].Req[2].x, "00");
        strcpy(Cmd.Price[bs].Req[2].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        if(strlen(str.c_str()) > 80) strncpy(Cmd.Price[bs].Req[2].str, &(str[81]), 40);
        else Cmd.Price[bs].Req[2].str[0] = 0;
        StrWinToDos(Cmd.Price[bs].Req[2].str,100);
        if(strlen(Cmd.Price[bs].Req[2].str) > 0) ypos++;
// Цена услуги
        strcpy(Cmd.Price[bs].type, "11");
        strcpy(Cmd.Price[bs].flag, "02");
        strcpy(Cmd.Price[bs].x, "00");
        strcpy(Cmd.Price[bs].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        strcpy(Cmd.Price[bs].qnty, &Qnty[ks*15]);
        strcpy(Cmd.Price[bs].price, &Price[ks*15]);
        strcpy(Cmd.Price[bs].sec, "01");
        strcpy(Cmd.Price[bs].code, Format("%4.4d", ARRAYOFCONST((ks+1))).c_str());
        strcpy(Cmd.Price[bs].unit, "00");
        ypos++;
        bs++;
        ks++;

        if(bs == CHLEN || ks == Num)
        {
            strcpy(Cmd.number, Format("%3.3d", ARRAYOFCONST((bs*4))).c_str());
            char* cb;
            cb =(char*) &Cmd.stx;
            int cn = sizeof(Cmd) - sizeof(tagPrice)*CHLEN + sizeof(tagPrice)*bs;
            cb[cn-1] = 0x03;

            bs = 0;
            ypos = 0;

            if(SendData((char*) &Cmd))
            {
                KKMResult = String(&Data[8]);
                KKMStatus = String(&Data[3]);
                PrinterStatus = String(&Data[13]);
                if(KKMResult == "0000" && ks < Num) continue;
                else return;
            }
            else
            {
//                KKMResult = "FF01";
                KKMStatus = "0000";
                PrinterStatus = "00";
                return;
            }
        }
    }
    case 4:
     strncpy(CmdS.psw, Psw.c_str(), 4);
     for(i=0;i<Num;i++)
       {
       inbuf = 0;
       //Код товара
       p = Code+21*i;
       for(j=0;j<20 && p[j];j++) buf[inbuf++] = p[j];
       buf[inbuf++] = ' ';
       //Наименование товара
       p = Name+131*i;
       for(j=0;j<130 && p[j];j++)
         {
         buf[inbuf++] = p[j];
         if( inbuf >= 40 ) //переполнение
           {
           for(k = 40;k>0 && buf[k]!=' ';k--);
           if(k) buf[k] = 0;
           else buf[40] = 0;
           Print(buf,true);
           if(KKMResult != "0000") return;
           inbuf = 0;
           if(k)
             {
             k++;
             for(;(k<40);k++) buf[inbuf++] = buf[k];
             }
           }
         }
       //Единица измерения
       p = Meas+26*i;
       for(j=0;p[j] && j<25;j++);
       if(inbuf+3+j > 40)
         {
         buf[inbuf] = 0;
         //inbuf = 0;
         Print(buf,true);
         for(k=0;k<38-j;k++) CmdS.text[k] = ' ';
         CmdS.text[k++] = '(';
         for(int l=0;l<j;l++) CmdS.text[k++] = p[l];
         CmdS.text[k] = ')';
         }
       else
         {
         k=0;
         while(k<inbuf) CmdS.text[k] = buf[k++];
         while(k<38-j) CmdS.text[k++] = ' ';
         CmdS.text[k++] = '(';
         for(int l=0;l<j;l++) CmdS.text[k++] = p[l];
         CmdS.text[k] = ')';
         }
       //Цена
       p = Price+16*i;
       TmpStr = String(p);
       if(TmpStr.SubString(TmpStr.Length()-2,1)!=".") return;
       TmpStr = TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
       TmpLong = atol(TmpStr.c_str());
       memcpy(&(CmdS.price),&TmpLong,4);
       CmdS.price[4] = 0;
       //Количество
       p = Qnty+16*i;
       TmpStr = String(p);
       k = TmpStr.Pos(".");
       if(k == 0) TmpStr += ".000";
       else if(k == TmpStr.Length()) TmpStr += "000";
       else if(k == TmpStr.Length()-1) TmpStr += "00";
       else if(k == TmpStr.Length()-2) TmpStr += "0";
       else if(k != TmpStr.Length()-3) return;

       TmpStr = TmpStr.SubString(1,TmpStr.Length()-4)+ TmpStr.SubString(TmpStr.Length()-2,3);
       TmpLong = atol(TmpStr.c_str());
       memcpy(&(CmdS.qnty),&TmpLong,4);
       CmdS.qnty[4] = 0;

       CmdS.nalog1 = NDS[i*2]-'0';

       if(SendData((char*) &CmdS))
       {
           memcpy(&AnsS,Data,sizeof(AnsS));
           KKMResult = IntToHex(AnsS.error,4);
           KKMStatus = "0000";
           PrinterStatus = "00";
       }
       else
       {
           KKMResult = "00FF";
           KKMStatus = "0000";
           PrinterStatus = "00";
           return;
       }
       if(KKMResult != "0000") return;
    }
    break;

        case 8:
     strncpy(CmdSo.psw, Psw.c_str(), 4);
     for(i=0;i<Num;i++)
       {
       inbuf = 0;
       //Код товара
       p = Code+21*i;
       for(j=0;j<20 && p[j];j++) buf[inbuf++] = p[j];
       buf[inbuf++] = ' ';
       //Наименование товара
       p = Name+131*i;
       for(j=0;j<130 && p[j];j++)
         {
         buf[inbuf++] = p[j];
         if( inbuf >= 40 ) //переполнение
           {
           for(k = 40;k>0 && buf[k]!=' ';k--);
           if(k) buf[k] = 0;
           else buf[40] = 0;
           Print(buf,true);
           if(KKMResult != "0000") return;
           inbuf = 0;
           if(k)
             {
             k++;
             for(;(k<40);k++) buf[inbuf++] = buf[k];
             }
           }
         }
       //Единица измерения
       p = Meas+26*i;
       for(j=0;p[j] && j<25;j++);
       if(inbuf+3+j > 40)
         {
         buf[inbuf] = 0;
         //inbuf = 0;
         Print(buf,true);
         for(k=0;k<38-j;k++) CmdSo.text[k] = ' ';
         CmdSo.text[k++] = '(';
         for(int l=0;l<j;l++) CmdSo.text[k++] = p[l];
         CmdSo.text[k] = ')';
         }
       else
         {
         k=0;
         while(k<inbuf) CmdSo.text[k] = buf[k++];
         while(k<38-j) CmdSo.text[k++] = ' ';
         CmdSo.text[k++] = '(';
         for(int l=0;l<j;l++) CmdSo.text[k++] = p[l];
         CmdSo.text[k] = ')';
         }
       //Цена
       p = Price+16*i;
       TmpStr = String(p);
       if(TmpStr.SubString(TmpStr.Length()-2,1)!=".") return;
       TmpStr = TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
       TmpLong = atol(TmpStr.c_str());
       memcpy(&(CmdSo.price),&TmpLong,4);
       CmdSo.price[4] = 0;
       //Количество
       p = Qnty+16*i;
       TmpStr = String(p);
       k = TmpStr.Pos(".");
       if(k == 0) TmpStr += ".000";
       else if(k == TmpStr.Length()) TmpStr += "000";
       else if(k == TmpStr.Length()-1) TmpStr += "00";
       else if(k == TmpStr.Length()-2) TmpStr += "0";
       else if(k != TmpStr.Length()-3) return;

       TmpStr = TmpStr.SubString(1,TmpStr.Length()-4)+ TmpStr.SubString(TmpStr.Length()-2,3);
       TmpLong = atol(TmpStr.c_str());
       memcpy(&(CmdSo.qnty),&TmpLong,4);
       CmdSo.qnty[4] = 0;

       CmdSo.nalog1 = NDS[i*2]-'0';

       if(SendData((char*) &CmdSo))
       {
           memcpy(&AnsSo,Data,sizeof(AnsSo));
           frStatus->OperationResult = AnsSo.error;
           KKMResult = IntToHex(AnsSo.error,4);
           KKMStatus = "0000";
           PrinterStatus = "00";
       }
       else
       {
           frStatus->OperationResult = 0xFF;
           KKMResult = "00FF";
           KKMStatus = "0000";
           PrinterStatus = "00";
           return;
       }
      if(frStatus->OperationResult != 0) return;
    }
    break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::OpenSession(AnsiString OperatorName, int OperatorNumber)
{
unsigned int m;
AnsiString Recv;

    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char number[3]; // Номер оператора	N	2
        char name[41];  // Фамилия оператора	S	40
        char bcc[2];    // BCC	B	2
        char etx;        // ETX		1
    } Cmd = {0x02, 0x31, "", "", "", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char date[7];   //
        char time[5];
        char recvisites[256];
        char etx;        // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'3',"02","","","",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //E0h
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,5,0xE0,"",0};

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //E0h
        char bcc;    // BCC	B	2
    } AnsSo;

  switch(FRType)
    {
    default:
    case 0:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    strncpy(Cmd.number,Format("%2.2d", ARRAYOFCONST((OperatorNumber))).c_str(),2);
    for(int i = 0; i < 40; i++) Cmd.name[i] = 0;
    for(int i = 0; i < (int) OperatorName.Length(); i++) Cmd.name[i] = WinToDos(OperatorName[i+1]);
    Cmd.name[40] = 0;
    Cmd.psw[4] = 0;
    Cmd.number[2] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
//        if(KKMResult == "0000") Serial = String(&Data[16]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    break;
    case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.sbyte = NextSbyte();
    CmdP.code[2] = 0x1c;
    SetDateTimeStr(CmdP.date);
    Recv = String(OperatorNumber)+" "+OperatorName;
    StrWinToDos(Recv.c_str(),Recv.Length());
    m=250;
    m = min(strlen(Recv.c_str()),m);
    strncpy(CmdP.recvisites, Recv.c_str(), m);
    CmdP.recvisites[m++] = 0x1c;
    CmdP.recvisites[m++] = 0x03;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    break;
    case 4:
      Print("      Открытие смены\n \n \n",true);
    break;
    case 8:
       GetStatus();
      switch (Mode)
      {
         case 2:
//            MainWindow->Name->Caption = "Смена уже открыта!";
            KKMResult = IntToHex(115,4);
            KKMStatus = "0200";
            PrinterStatus = "00";
            break;
         case 3:
//            MainWindow->Name->Caption = "Смена открыта, 24 часа кончились! Снимите Z-отчет.";
            KKMResult = IntToHex(115,4);
            KKMStatus = "0300";
            PrinterStatus = "00";
            break;
         case 4:
            strncpy(CmdSo.psw, Psw.c_str(), 4);
            if (SendData((char*) &CmdSo))
            {
               memcpy(&AnsSo,Data,sizeof(AnsSo));
               KKMResult = IntToHex(0,4);
               KKMStatus = "0000";
               PrinterStatus = "00";
            }
            break;
      }
    break;
  }
}
//---------------------------------------------------------------------------
// по сути функция получает статус ККМ и обрабатывает их
void __fastcall TMStarF::GetVersion()
{
int Cycle = true;

    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char a;  // Не используется	N	2
        char b;
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x45, "", 0, 0, "", 0x03};

    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];      // Код сообщения  3
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'(',"96",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char Serial[12];        //Серийный номер ККМ
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //11h
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdS = {0x02,5,0x11,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned short FRPOVersion;
        unsigned short FRPOSubVersion;
        unsigned char FRPODate[3];
        unsigned char FRNumber;
        unsigned short BillNumber;
        unsigned short FRFlags;
        unsigned char FRRegime;
        unsigned char FRSubRegime;
        unsigned char FRPort;
        unsigned short FPPOVersion;
        unsigned short FPPOSubVersion;
        unsigned char FPPODate[3];
        unsigned char date[3];
        unsigned char time[3];
        unsigned char FPFlags;
        unsigned long SN;
        unsigned short LastSession;
        unsigned short FPRest;
        unsigned char ReRegistration;
        unsigned char ReRegistrationRemain;
        unsigned char INN[6];
        unsigned char bcc;    // BCC	B	2
    } AnsS;


switch(FRType)
  {
  default:
  case 0:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
        if(KKMResult == "0000" && Serial == "") Serial = String(&Data[16]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.sbyte = NextSbyte();
    CmdP.code[2] = 0x1c;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        Ans.Serial[11]=0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
        if(KKMResult == "0000" && Serial == "") Serial = String(Ans.Serial);
        StartPrime();
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    while(Cycle)
    {
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = IntToHex(AnsS.FRRegime,2)+IntToHex(AnsS.FRSubRegime,2);
        PrinterStatus = IntToHex(AnsS.FRFlags,4);
        if(KKMResult == "0000" && Serial == "") Serial = IntToStr(AnsS.SN);
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }

        //DumpLog
/*        if(MainWindow->Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<" KKMStatus ";
          errlog << KKMStatus.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
        log("KKMStatus " + KKMStatus);
// обработка кодов ошибок
    if(KKMStatus == "0300") //
    {
      MainWindow->Name->Caption = "Смена открыта более 24 часов. Снимите Z-отчет";
      return;
    }
    else if(KKMStatus == "0205")  // смена открыта, идет печать
      {
      MainWindow->Name->Caption = "Идет печать";
      Sleep(5000); //Печатает - подождем
      }
    else if(KKMStatus == "0805")  // чек открыт, идет печать
      {
      MainWindow->Name->Caption = "Идет печать";
      Sleep(5000); //Печатает - подождем
      }
    else if(KKMStatus == "0202") // нет бумаги
      MessageBox(GetActiveWindow(),"Замените бумагу, потом нажмите Enter","Нет бумаги",MB_OK);
    else if(KKMStatus == "0203") // после замены бумаги
      ResumePrint(); //После замены продолжим печать
    else
      {
      Cycle = false;
      }
    }
  if( flag21 )
    {
    StrihWriteTable(1,1,21,"\x01",1); // Промотка перед отрезкой
    flag21 = false;
    }
  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::ZReport()
{
    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char flag[2];   // Флаги	B	1
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x35, "", "6", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char date[7];
        char time[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'1',"31","","",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //41h
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdS = {0x02,5,0x41,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

   switch(FRType)
     {
     case 0:
     default:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
        if(KKMResult == "0000") Sum = String(&Data[30]);
        else Sum = "";
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
        Sum = "";
    }
    break;
    case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.sbyte = NextSbyte();
    CmdP.code[2] = 0x1c;
    SetDateTimeStr(CmdP.date);
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::XReport()
{
    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char flag[2];   // Флаги	B	1
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x35, "", "4", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char date[7];
        char time[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'2',"30","","",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //41h
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdS = {0x02,5,0x40,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

    switch(FRType)
    {
    default:
    case 0:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
        if(KKMResult == "0000") Sum = String(&Data[30]);
        else Sum = "";
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
        Sum = "";
    }
    break;
    case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.code[2] = 0x1c;
    SetDateTimeStr(CmdP.date);
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::DeleteDoc()
{

    struct
    {
        char stx;
        char code;
        char psw[5];
        char operation;
        char a;
        char docflag[3];
        char number[4];
        char bcc[2];
        char etx;
    } Cmd = {0x02, 0x53, "", 0x30, 0, "08", "000", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char date[7];
        char time[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'2',"17","","",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdS = {0x02,5,0x88,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

    switch(FRType)
    {
    default:
    case 0:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    break;
    case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.code[2] = 0x1c;
    SetDateTimeStr(CmdP.date);
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        frStatus->OperationResult = AnsS.error;
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}
//---------------------------------------------------------------------------
// отрезка чека
void __fastcall TMStarF::CRLF()
{
if(FRType==2) return;

    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char qlf[3];   // Кол-во строк
        char cut[3];
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x52, "", "09", "", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //29h
        char psw[4];    // Пароль на связь	S4
        char flag;
        char string;
        char bcc;    // BCC	B	2
    } CmdSM = {0x02,7,0x29,"",3,7,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSM;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //25h
        char psw[4];    // Пароль на связь	S4
        char type;      // 0 - отрезать напрочь, 1 - немножко порезать
        char bcc;    // BCC	B	2
    } CmdS = {0x02,6,0x25,"",1,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;      //17h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  case 0:
  default:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 4:
  case 8:
    strncpy(CmdSM.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdSM))
    {
        memcpy(&AnsSM,Data,sizeof(AnsSM));
        KKMResult = IntToHex(AnsSM.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::Print(char* Text, bool WinText)
{
int j, k;
unsigned char ending[12] = {0x1b,0x64,0x04,0x1b,0x69,0x1b,0x1b,0x03,0,0,0,0};

ResetEvent(hPrnEvent);

    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x36, "", "", 0x03};
    char End = 0x1B;
    char c;

    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];      // Код сообщения  3
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdRegime = {0x02,"",'d',"70",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } AnsRegime;

unsigned long nChar;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //17h
        char psw[4];    // Пароль на связь	S4
        char flag;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdS = {0x02,46,0x17,"\x1e\x0\x0\x0",3,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;
// ШТРИХ-онлайн
        struct
    {
        char stx;       // STX		1
        char length;
        char code;      //17h
        char psw[4];    // Пароль на связь	S4
        char flag;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,46,0x17,"\x1e\x0\x0\x0",3,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //17h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

switch(FRType)
  {
  default:
  case 0:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
        if(KKMResult == "0000")
        {
            int index = 0;
            while(Text[index] != 0)
            {
                Slip = 1;
                c = Text[index];
                if(WinText) c = WinToDos(c);
                if(SendData(&c))
                {
                    if(Slip < 0)
                    {
                        index++;
                        continue;
                    }
                    KKMResult = String(&Data[8]);
                    KKMStatus = String(&Data[3]);
                    PrinterStatus = String(&Data[13]);
                    Slip = 0;
                    return;
                }
                else
                {
                    KKMStatus = "0000";
                    PrinterStatus = "00";
                    Slip = 0;
                    return;
                }
            }
            Slip = 1;
            if(!SendData(&End))
            {
                KKMStatus = "0000";
                PrinterStatus = "00";
                Slip = 0;
                return;
            }
            Slip = 1;
            if(!SendData(&End))
            {
                KKMStatus = "0000";
                PrinterStatus = "00";
                Slip = 0;
                return;
            }
            KKMResult = String(&Data[8]);
            KKMStatus = String(&Data[3]);
            PrinterStatus = String(&Data[13]);
            Slip = 0;
        }
    }
    else
    {
        KKMStatus = "0000";
        PrinterStatus = "00";
        Slip = 0;
    }
  break;
  case 2:
    strncpy(CmdRegime.psw, Psw.c_str(), 4);
    CmdRegime.sbyte = NextSbyte();
    CmdRegime.code[2] = 0x1c;
    if(SendData((char*) &CmdRegime))
    {
    strncpy((char *)&AnsRegime,Data,sizeof(AnsRegime));
    AnsRegime.KKMStatus[4] = 0; //К ANSIstring преобразуем
    AnsRegime.KKMResult[4] = 0;
    AnsRegime.PrnStatus[10] = 0;
    KKMResult = String(AnsRegime.KKMResult);
    KKMStatus = String(AnsRegime.KKMStatus);
    PrinterStatus = String(AnsRegime.PrnStatus);
    if(KKMResult == "0000")
      {
      StrWinToDos(Text,strlen(Text));
      //Теперь в режиме принтера
      strcpy(SendCommand,Text);
      nOut = strlen(Text);
      Resume();
//      WriteFile (hComm, Text, strlen(Text), &nChar, NULL);
      if(SendData(ending))
        {
        strncpy((char *)&AnsRegime,Data,sizeof(AnsRegime));
        AnsRegime.KKMStatus[4] = 0; //К ANSIstring преобразуем
        AnsRegime.KKMResult[4] = 0;
        AnsRegime.PrnStatus[10] = 0;
        KKMResult = String(AnsRegime.KKMResult);
        KKMStatus = String(AnsRegime.KKMStatus);
        PrinterStatus = String(AnsRegime.PrnStatus);
        }
      }
    }
  break;
  case 4:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    k = 0; //Указатель в печатаемой строке
    j = 0; //Указатель в исходном тексте
    while(true)
    {
    if(Text[j]=='\n' || k >= 40 || !Text[j] )
      {
      if(Text[j] == '\n') j++;
      for(;k<40;k++) CmdS.text[k] = 0;
      k = 0;
      if(SendData((char*) &CmdS))
          {
          memcpy(&AnsS,Data,sizeof(AnsS));
          KKMResult = IntToHex(AnsS.error,4);
          if(AnsS.error) return;
          KKMStatus = "0000";
          PrinterStatus = "00";
          }
      else
          {
          KKMResult = "00FF";
          KKMStatus = "0000";
          PrinterStatus = "00";
          return;
          }
      if(!Text[j]) return;
      }
    else CmdS.text[k++] = Text[j++];
    break;
    }

   case 8:
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    k = 0; //Указатель в печатаемой строке
    j = 0; //Указатель в исходном тексте
    while(true)
    {
    if(Text[j]=='\n' || k >= 40 || !Text[j] || Text[j]=='\r') // попадает переполнение, конец строки или ноль
      {
      while(Text[j] == '\n' || Text[j]=='\r') j++;
      for(;k<=40;k++) CmdSo.text[k] = 0;
      k = 0;
      if(SendData((char*) &CmdSo))
          {
          memcpy(&AnsSo,Data,sizeof(AnsSo));
          frStatus->OperationResult = AnsSo.error;
          KKMResult = IntToHex(AnsSo.error,4);
          if(AnsSo.error) {SetEvent(hPrnEvent);  return;}   // устанавливаем сообщение окончания печати
          KKMStatus = "0000";
          PrinterStatus = "00";
          }
      else
          {
          frStatus->OperationResult = 0xFF;
          KKMResult = "00FF";
          KKMStatus = "0000";
          PrinterStatus = "00";
          return;
          }
      if(!Text[j]){SetEvent(hPrnEvent);  return;}      // устанавливаем сообщение окончания печати

      }
    else CmdSo.text[k++] = Text[j++];
//    break;
    }
  }
  SetEvent(hPrnEvent);  // устанавливаем сообщение окончания печати
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::PrintBold(char* Text, bool WinText)
{
int j, k;
//unsigned char ending[12] = {0x1b,0x64,0x04,0x1b,0x69,0x1b,0x1b,0x03,0,0,0,0};

unsigned long nChar;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //17h
        char psw[4];    // Пароль на связь	S4
        char flag;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdS = {0x02,46,0x12,"\x1e\x0\x0\x0",3,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

        struct
    {
        char stx;       // STX		1
        char length;
        char code;      //17h
        char psw[4];    // Пароль на связь	S4
        char flag;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,46,0x12,"\x1e\x0\x0\x0",3,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

switch(FRType)
  {
  default:
    break;
  case 4:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    k = 0; //Указатель в печатаемой строке
    j = 0; //Указатель в исходном тексте
    while(true)
    {
    if(Text[j]=='\n' || k >= 40 || !Text[j] )
      {
      if(Text[j] == '\n') j++;
      for(;k<40;k++) CmdS.text[k] = 0;
      k = 0;
      if(SendData((char*) &CmdS))
          {
          memcpy(&AnsS,Data,sizeof(AnsS));
          KKMResult = IntToHex(AnsS.error,4);
          if(AnsS.error) return;
          KKMStatus = "0000";
          PrinterStatus = "00";
          }
      else
          {
          KKMResult = "00FF";
          KKMStatus = "0000";
          PrinterStatus = "00";
          return;
          }
      if(!Text[j]) return;
      }
    else CmdS.text[k++] = Text[j++];
    break;
    }
    case 8:
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    k = 0; //Указатель в печатаемой строке
    j = 0; //Указатель в исходном тексте
    while(true)
    {
    if(Text[j]=='\n' || k >= 40 || !Text[j] )
      {
      if(Text[j] == '\n') j++;
      for(;k<40;k++) CmdSo.text[k] = 0;
      k = 0;
      if(SendData((char*) &CmdSo))
          {
          memcpy(&AnsSo,Data,sizeof(AnsSo));
          frStatus->OperationResult = AnsSo.error;
          KKMResult = IntToHex(AnsSo.error,4);
          if(AnsSo.error) return;
          KKMStatus = "0000";
          PrinterStatus = "00";
          }
      else
          {
          frStatus->OperationResult = 0xFF;
          KKMResult = "00FF";
          KKMStatus = "0000";
          PrinterStatus = "00";
          return;
          }
      if(!Text[j]) return;
      }
    else CmdSo.text[k++] = Text[j++];
    break;
    }
  }
}
//---------------------------------------------------------------------------
// открыть ящик
void __fastcall TMStarF::OpenBox()
{
    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char chanel[2]; // Канал.	B	1
        char t1[3];     // Длительность импульса t1 ON (только для канала 1)	B	2
        char t2[3];     // Длительность импульса t2 OFF (только для канала 1)	B	2
        char num[3];    // Количество импульсов	B	2
        char bcc[2];       // BCC	B	2
        char etx;       //ETX		1
    } Cmd = {0x02, 0x38, "", "0", "01", "01", "01", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //28h
        char psw[4];    // Пароль на связь	S4
        char box;
        char bcc;    // BCC	B	2
    } CmdS = {0x02,6,0x28,"",0,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  default:
  case 0:

    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    break;
  case 2:
    unsigned long nChar;
    WriteFile (hComm, "\x05", 1, &nChar, NULL);
    break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}
//-------------------------------------------------------------------------//
char __fastcall TMStarF::WinToDos(char c)
{
    if(c >= 'А' && c <= 'п') return (c - 'А' + 128);
    if(c >= 'р' && c <= 'я') return (c - 'р' + 224);
    if(c == 'Ё') return 240;
    if(c == 'ё') return 241;
    return c;
}
//-------------------------------------------------------------------------//
void __fastcall TMStarF::StrWinToDos(char* str,unsigned int maxlen)
{
    for(unsigned int i = 0; i < strlen(str) && i < maxlen; i++)
        str[i] = WinToDos(str[i]);
    return;
}
//---------------------------------------------------------------------------
// печать копии чека
void __fastcall TMStarF::PrintCopyCheck()
{
    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char bcc[2];       // BCC	B	2
        char etx;       //ETX		1
    } Cmd = {0x02, 0x54, "", "", 0x03};

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdS = {0x02,5,0x8c,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  case 0:
  default:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  case 2:
  break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);

    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        frStatus->OperationResult = AnsS.error;
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  Print("========================================",true);
  Print("              КОПИЯ ЧЕКА",true);
  Print("========================================",true);
  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::SetDateTimeStr(char* str)
{
  AnsiString All;
  Word Year, Month, Day, Hour, Min, Sec, MSec;
  TDateTime dtPresent = Now();
  DecodeDate(dtPresent, Year, Month, Day);
  DecodeTime(dtPresent, Hour, Min, Sec, MSec);

  All  = Format("%2.2D",ARRAYOFCONST(((int)Day)));
  All += Format("%2.2D",ARRAYOFCONST(((int)Month)));
  All += AnsiString(Year).SubString(3,2);
  strncpy(str, All.c_str(), 6);
  str[6] = 0x1c;
  All  = Format("%2.2D",ARRAYOFCONST(((int)Hour)));
  All += Format("%2.2D",ARRAYOFCONST(((int)Min)));
  strncpy(str+7, All.c_str(), 4);
  str[11] = 0x1c;
}
//-------------------------------------------------------------------------//
// не нужная функция
void __fastcall TMStarF::SetupPrime(void)
{
    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char speed[5];  // 9600
        char line[3];   //  01 - 5-проводная
        char res1[3];   //Не менять! Задано производителем
        char res2[2];   //Не менять!  для совместимости
        char res3[3];   //Не менять!
        char shortanswer[3]; //"00" - нет, "01" - да
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'(',"94","9600","00","00","","00","00",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.code[2] = 0x1c;
    CmdP.speed[4] = 0x1c;
    CmdP.line[2] = 0x1c;
    CmdP.res1[2] = 0x1c;
    CmdP.res2[0] = 0x20;
    CmdP.res2[1] = 0x1c;
    CmdP.res3[2] = 0x1c;
    CmdP.shortanswer[2] = 0x1c;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
}

//-------------------------------------------------------------------------//
// под старый тип ККТ
void __fastcall TMStarF::GetupPrime(void)
{
    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'(',"95",0x03,""};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char Version[7];        //Версия ПО ККМ
        char speed[5];  // 9600
        char line[3];   //  01 - 5-проводная
        char res1[3];   //Не менять! Задано производителем
        char res2[2];   //Не менять!  для совместимости
        char res3[3];   //Не менять!
        char shortanswer[3]; //"00" - нет, "01" - да
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.code[2] = 0x1c;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
}
//-------------------------------------------------------------------------//
// под старый тип ККТ
void __fastcall TMStarF::StartPrime(void)
{
    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char date[7];
        char time[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'d',"01","","",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.code[2] = 0x1c;
    SetDateTimeStr(CmdP.date);
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
        KKMStatus = "0000";
        PrinterStatus = "00";
//        Serial = "";
    }
}

//-------------------------------------------------------------------------//
char __fastcall TMStarF::NextSbyte(void)
{
static char Sbyte = '0';

if(++Sbyte > '9') Sbyte = '1';
return Sbyte;
}
//-------------------------------------------------------------------------//
// не под штрих
void __fastcall TMStarF::PrintOldSession(AnsiString SessionNumber)
{

AnsiString SN = SessionNumber;
int SNH;
    struct
    {
        char stx;       // STX		1
        char psw[4];    // Пароль на связь	S4
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char Session[5];
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } CmdP = {0x02,"",'2',"8C","",0x03};

    struct
    {
        char stx;       // STX		1
        char sbyte;     // Отличительный байт
        char code[3];   // Код сообщения  3
        char KKMConstStatus[3]; //Постоянный статус ККМ
        char KKMStatus[5];      //Текущий статус ККМ
        char KKMResult[5];      //Результат выполнения команды
        char PrnStatus[11];     //Состояние принтера
        char etx;       // ETX		1
        char bcc[4];    // BCC	B	2
    } Ans;

    switch(FRType)
    {
    default:
    case 0:
      break;
    case 2:
    strncpy(CmdP.psw, Psw.c_str(), 4);
    CmdP.code[2] = 0x1c;
    SNH = SN.ToInt();
    SN = IntToHex(SNH,4);
    SN = SN.SubString(3,2)+SN.SubString(1,2);

    strncpy(CmdP.Session,SN.c_str(), 4);

    CmdP.Session[4] = 0x1c;
    if(SendData((char*) &CmdP))
    {
        strncpy((char *)&Ans,Data,sizeof(Ans));
        Ans.KKMStatus[4] = 0; //К ANSIstring преобразуем
        Ans.KKMResult[4] = 0;
        Ans.PrnStatus[10] = 0;
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
    }
    else
    {
//      KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
//        Serial = "";
    }
  break;
    }

}
//-------------------------------------------------------------------------//
void __fastcall TMStarF::ResumePrint(void)
{
    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdN = {0x02,5,0xb0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsN;

    switch(FRType)
    {
    default:
    case 0:
      break;
    case 2:
      break;
    case 4:
    case 8:
    strncpy(CmdN.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdN))
    {
        memcpy(&AnsN,Data,sizeof(AnsN));
        frStatus->OperationResult = AnsN.error;
        KKMResult = IntToHex(AnsN.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
    }
}
//-------------------------------------------------------------------------//
AnsiString __fastcall TMStarF::GetBillNumber(void)
{
AnsiString Ret;
Ret = Serial + FormatDateTime("yymmddhhmmss", Now());
while(Ret.Length() < 18) Ret = "0"+Ret;
BillNumber = Ret;
return Ret;
}
//-------------------------------------------------------------------------//
// запрос денежного регистра
long __fastcall TMStarF::GetSellSumm(void)
{
unsigned long SellSummReturn = 0;

    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char NumS[4];    //Номер счетчика
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x55, "", "000","00", 0x03};

    struct
    {
        char stx;       // STX 1
        char code[2];      // Код сообщения 1
        char KKMStatus[5];
        char KKMResult[5];
        char PrnStatus[3];
        char NumS[4];    //Номер счетчика
        char NumOper[6];    //Номер счетчика
        char Summ[16];    //Номер счетчика
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Ans;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char reg;
        char bcc;    // BCC	B	2
    } CmdS = {0x02,6,0x1a,"",121,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char SellSumm[6];
        unsigned char bcc;    // BCC	B	2
    } AnsS;
    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char reg;
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,6,0x1a,"",121,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char SellSumm[6];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    switch(FRType)
    {
    default:
    case 0:
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        memcpy((char *)&Ans,Data,sizeof(Ans));
        KKMResult = String(Ans.KKMResult);
        KKMStatus = String(Ans.KKMStatus);
        PrinterStatus = String(Ans.PrnStatus);
        SellSummReturn = atof(Ans.Summ)*100.00;
        if(KKMResult == "0000") return SellSummReturn;
    }
    else
    {
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    break;
    case 2:
      break;
    case 4:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    if(KKMResult == "0000")
      {
      strncpy((char *)&SellSummReturn,AnsS.SellSumm,4);
      Sleep(2000);
      return SellSummReturn;
      }
    break;
        case 8:
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    if(KKMResult == "0000")
      {
      strncpy((char *)&SellSummReturn,AnsSo.SellSumm,4);
      Sleep(2000);
      return SellSummReturn;
      }
    break;
    }
return -1;
}
//-------------------------------------------------------------------------//
// запрос денежного регистра
hyper __fastcall TMStarF::GetMoneyReg(unsigned short int Register)
{
if(Register == 0) return 0;
unsigned long int SellSummReturn = 0;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char reg;
        char bcc;    // BCC	B	2
    } CmdSo8 = {0x02,6,0x1a,"",0,0};
    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char reg[2];
        char bcc;    // BCC	B	2
    } CmdSo16 = {0x02,7,0x1a,"",0,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char SellSumm[6];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    switch(FRType)
    {
    default:
    case 8:
    bool res = false;
    if(Register > 255)
    {
      strncpy(CmdSo16.psw, Psw.c_str(), 4);
      memcpy(CmdSo16.reg, &Register, 2);
      res = SendData((char*) &CmdSo16);
    }
    else
    {
      strncpy(CmdSo8.psw, Psw.c_str(), 4);
      CmdSo8.reg = (Register & 0xff);
      res = SendData((char*) &CmdSo8);
    }
    if(res)
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    if(KKMResult == "0000")
      {
      memcpy((char *)&SellSummReturn,AnsSo.SellSumm,sizeof(AnsSo.SellSumm));
//      Sleep(1000);
      return SellSummReturn;
      }
    break;
    }
return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::StrihWriteTable(char Table,int Row,char Field,char * Value,int Len)
{

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char Table;
        char Row[2];
        char Field;
        char Value[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,49,0x1e,"",0,"\x00",0,0,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    switch(FRType)
    {
    default:
    case 0:
      break;
    case 2:
      break;
    case 4:
      break;
    case 8:
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    Len++;   // добавляем длину на единицу, так как в строке должен быть терминирующий ноль
    CmdSo.Table = Table;
    CmdSo.length = Len + 9;
    CmdSo.Row[0] = Row%256;
    CmdSo.Row[1] = Row/256;
    CmdSo.Field = Field;
// добавил для инициализации 04/12/18
    memset(CmdSo.Value, 0, 40);
//
    memcpy(CmdSo.Value,Value,Len);

    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
//        frStatus->OperationResult = 0x1;
//      if(frStatus->OperationResult != 0) throw My::TCustomException("Какая-то ошибка", frStatus->OperationResult);
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult =0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
  break;
  }
}
//---------------------------------------------------------------------------

bool __fastcall TMStarF::SendData(char *Command)
{
    EnterCriticalSection(&starCS); // чтобы дважды не войти в передачу данных
    static int in;
    in++;
    unsigned int bcc = 0,n,k;
//    unsigned int nOut = 0;
    AnsiString str;
    char ComCode;

    DWORD dwEvMask, dwError;
    COMSTAT csStat;

    nOut = 0;
    //n=0;
    ComCode = Command[2];  // запоминаем команду

    KKMResult = "FF01"; // не знаю зачем такой статус

    TimeOut = false;

    strcpy(SendCommand,"\x5");  // отправляем ENQ
    nOut = 1;
    if(!WaitFR(2))    // если прошел таймаут, то сообщение считается не отправленным
    {
      LeaveCriticalSection(&starCS);
      return false;
    }
    // тут мы должны получить или 0х15 или повторение предыдущего ответа
    // предыдущий ответ нужно подтвердить и продолжить работу
    strcpy(SendCommand,"\x6");   // отправляем ACK
    nOut = 1;
    if(!WaitFR(2))
    {
      LeaveCriticalSection(&starCS);
      in = 0;
      return false;
    }

// Вычисление контрольной суммы
    n = (unsigned char)Command[1]+2;   // длина
    SendCommand[0] = Command[0];
    for(unsigned int i = 1; i < n;i++)
    {
      bcc ^= (unsigned char)Command[i]; // XOR всех байтов
      SendCommand[i] = Command[i];
    }
    bcc &= 0xff;
    SendCommand[n++] = bcc;

    DataReady = false;
    //PrintingFR = false;
    nOut = n; //Присвоение значения nOut означает отправку команды в порт
    n = 0;
    k = 0;
    WaitFR(2);
    while(!DataReady)
    {
      if(n > 100)   // ждем не более 100 итераций (1500 мсек)
      {
          if(k>1)
          { // после 2-х раз по 100 завершаем с неуспешностью
            TimeOut = true;
            KKMResult = "00FF";
            nOut = 0;
            LeaveCriticalSection(&starCS);
            in = 0;
            return false;
          }
          strcpy(SendCommand,"\x5"); //Запрос повтора ответа после 100 итераций
          nOut = 1;
          if(!WaitFR(2))
          {
            LeaveCriticalSection(&starCS);
            in = 0;
            return false;
          }
          n = 0;
          k++;
      }
      n++;
      Sleep(15);
      Resume();
    }

    strcpy(SendCommand,"\x6"); //После ответа Штрих ждет подтверждения о приеме
    nOut = 1;
    if(!WaitFR(2))
    {
      LeaveCriticalSection(&starCS);
      in = 0;
      return false;
    }

    if(Slip < 0)
    {
      LeaveCriticalSection(&starCS);
      in = 0;
      return true;
    }

    bcc = 0;
    switch(FRType)
    {
    case 8:
      for(int i = 1; i < (int) Length - 1; i++)
        bcc ^= (unsigned char)Data[i];
      bcc &= 0xff;
 //   bcc = 154;
      if(((unsigned char)bcc == (unsigned char)Data[Length - 1])&&(ComCode==Data[2]))
      {  // проверка расчетной контрольной суммы и фактической и проверка соответствия по команде
         LeaveCriticalSection(&starCS);
         in = 0;
         return true;
      }
      else if(in == 1)
      {

         LeaveCriticalSection(&starCS);
         log("Ошибка КС полученной посылки");
//         in = 2;
         return false;
      }
      else
      {
         LeaveCriticalSection(&starCS);
         in = 0;
         return false;
      }
    }
}
//---------------------------------------------------------------------------
// запускается поток
void __fastcall TMStarF::Execute()
{
    static char *szComPort[] = {"COM1", "COM2", "COM3", "COM4", "COM5", "COM6"};
    static DCB    dcb;
    DWORD dwEvMask, dwError, dwConnStatus;
    COMSTAT csStat;
    static COMMTIMEOUTS cto;
    static unsigned char szRead[1024];
    unsigned long nChar;
    OVERLAPPED lpOverlapped;

    if ((hComm = CreateFile (szComPort [Port-1], GENERIC_READ  | GENERIC_WRITE | FILE_FLAG_OVERLAPPED,
                  0, NULL, OPEN_EXISTING, NULL , NULL)) == INVALID_HANDLE_VALUE)
    {
        KKMResult = "FF00";
        return;
    }

    SetupComm (hComm, 256, 256);       // allocate transmit & receive buffer
    dcb.BaudRate = Baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    if(FRType == 4 | FRType == 8) dcb.fBinary = 1;

    if (SetCommState (hComm, &dcb) < 0)
    {
        CloseHandle (hComm);                    // close the COM port.
        KKMResult = "FF00";
        return;
    }
    cto.ReadIntervalTimeout = 500;
    cto.ReadTotalTimeoutMultiplier = 50;
    cto.ReadTotalTimeoutConstant = 500;
    cto.WriteTotalTimeoutMultiplier = 50;
    cto.WriteTotalTimeoutConstant = 500;
    if(!SetCommTimeouts(hComm, &cto))
    {
        CloseHandle (hComm);                    // close the COM port.
        KKMResult = "FF00";
        return;
    }

    if (!SetCommMask(hComm, EV_RXCHAR | EV_BREAK))
    {
        CloseHandle (hComm);                    // close the COM port.
        KKMResult = "FF00";
        return;
    }
    KKMResult = "0000";

    Wait = false;          // ожидание ответа
    bool start = false;   // признак начала считывания ответа
    bool fin = false;     // признак окончания чтения ответа
    while(!Terminated)
    {
      // отправка команды в порт
        if(nOut)
        {
            AnsiString logstr = "Send ";

//            WriteFile (hComm, "\0x5", 1, &nChar, NULL);

            for(unsigned long j=0;j<nOut;j++)
            {
               logstr += IntToHex((unsigned char)SendCommand[j],2) + " ";
            }
            log(logstr);
           //EndDumpLog
          WriteFile (hComm, SendCommand, nOut, &nChar, NULL);
          nOut = 0;
          start = false;
          Wait = true;
        }
     // команда отправлена в порт
        Sleep(20);
//        WaitCommEvent(hComm, &dwEvMask, &lpOverlapped);
        ClearCommError(hComm, &dwError, &csStat);
// далее получаем и обрабатываем ответ
            while(csStat.cbInQue > 0)         // количество байт на входе больше нуля
            {
                if(ReadFile(hComm, szRead, csStat.cbInQue, &nChar, NULL))     // читаем в szRead
                {
                     //для теста
//                     csStat.cbInQue -= 3;
//                     nChar -= 3;
// логируем
                    AnsiString logstr = "Recived ";
                    for(unsigned long j=0;j<nChar;j++)
                    {
                       logstr += IntToHex(szRead[j],2) + " ";
                    }
                    log(logstr);

                    for(unsigned long i = 0; i < nChar; i++)
                    {
                        if(TimeOut) Suspend();   // останавливаем поток если был таймаут

                        if(szRead[i] == 0x02)  //отмечаем начало команды
                        {
                            if(!start) Length = 0;
                            start = true;
                        }

                        if(Slip == 1)
                        { //Что передает Штрих до начала ответа
                          //Если 06 то это ответ на предыдущую команду!
                          //А если 15, то OK
                          if(szRead[i] == 0x15)
                          {
                             SSec = 1;
                             fin = true;
                          }
                          else
                             SSec = -1;
                        }
                        if(start) Data[Length++] = szRead[i];
                        if(Length >= 1024)
                        {
                          szRead[i] = 0x03;
                          fin = true;
                        }
                        if (start)
                          {
                          switch(FRType)
                          {
                          default:
                          case 4:
                          case 8:
                            //контроль длины пакета
                            if((Length > 1)&&((unsigned char)Length == (unsigned char)Data[1]+3)) fin = true;
                            break;
                          }
                          }
                    }
                ClearCommError(hComm, &dwError, &csStat);
                }
                else break;
            }
            if(!fin)
            {
                continue;
            }
            else
            {
                start = false;
                fin = false;
                DataReady = true;
                Wait = false;
            }
    }
}
//---------------------------------------------------------------------------
// ожидание
bool __fastcall TMStarF::WaitFR(int seconds)
{
// ожидаем истечение указанного времени с приращением 10 мсек или до обнуления счетчика байтов на отправку
    for(int t=0;(t<seconds*100)&&nOut;t++)
    {
      Sleep(10);
      Resume();
    }
// цикл может закончится или по истечении времени или по обнулению счетчика байтов
    if(!nOut) return true;  // успешный выход
    
    TimeOut = true;
    KKMResult = "00FF";
    nOut = 0;
    return false;
}
//---------------------------------------------------------------------------
// скидка
void __fastcall TMStarF::BillOff(AnsiString Cash)
{
int p;
AnsiString TmpStr;
unsigned long TmpLong;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char off[5];
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } Cmd = {0x02,54,0x86,"","\x0\x0\x0\x0\x0",0,0,0,0,"",0};

        struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } Ans;
    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char off[5];
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,54,0x86,"","\x0\x0\x0\x0\x0",0,0,0,0,"",0};

        struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

if(Cash=="0.00") return;

switch(FRType)
  {
  default:
  case 0:
  case 2:
  case 4:
    Print("========================================",true);
    strncpy(Cmd.psw, Psw.c_str(), 4);
    memcpy(Cmd.text,"                                        ",40);
    TmpStr = Cash;
    p = TmpStr.Pos(".");
 // добавляем нули в конец, чтобы было, например 20.00, а не 20.0
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;

    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(Cmd.off),&TmpLong,4);
    if(SendData((char*) &Cmd))
    {
        memcpy(&Ans,Data,sizeof(Ans));
        KKMResult = IntToHex(Ans.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    GetVersion(); //А это на случай отказа в первый раз - повторный опрос
  break;
    case 8:
    Print("========================================",true);
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    memcpy(CmdSo.text,"                                        ",40);
    TmpStr = Cash;
    p = TmpStr.Pos(".");

    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;

    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdSo.off),&TmpLong,4);
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    GetVersion(); //А это на случай отказа в первый раз - повторный опрос
  break;
  }
}
//---------------------------------------------------------------------------

bool __fastcall TMStarF::PrintEAN()
{

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //c2h
        char psw[4];    // Пароль на связь	S4
        char ean[5];  //ean
        char bcc;    // BCC	B	2
    } CmdS = {0x02,0x0a,0xc2,"\x1e\x0\x0\x0","",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  default:
    break;
  case 4:
  case 8:
      //Преобразование номера чека в EAN
    char EAN[5];
    TDateTime StartTime;
    TDateTime BillTime;
    StartTime = EncodeDate(2005,1,1);
    BillTime = EncodeDate((Word)StrToInt("20"+BillNumber.SubString(7,2)), (Word)StrToInt(BillNumber.SubString(9,2)), (Word)StrToInt(BillNumber.SubString(11,2)));
    unsigned __int64 HEAN;
    unsigned __int64 const c9 = 1000000000;
    HEAN = floor(BillTime - StartTime + 0.5);
    HEAN = HEAN*24 + StrToInt(BillNumber.SubString(13,2));
    HEAN = HEAN*60 + StrToInt(BillNumber.SubString(15,2));
    HEAN = HEAN*60 + StrToInt(BillNumber.SubString(17,2));
    HEAN = HEAN + SerialID.ToInt() * c9;
    memcpy(EAN,(char *)&HEAN,5);

    if(HEAN < c9) return false;

    //memcpy(CmdS.psw, Psw.c_str(), 4);
    memcpy(CmdS.ean, EAN, 5);
      if(SendData((char*) &CmdS))
          {
          memcpy(&AnsS,Data,sizeof(AnsS));
          KKMResult = IntToHex(AnsS.error,4);
          if(AnsS.error) return false;
          KKMStatus = "0000";
          PrinterStatus = "00";
          }
      else
          {
          KKMResult = "00FF";
          KKMStatus = "0000";
          PrinterStatus = "00";
          return false;
          }
 // break;
  }
  return true;
}
//-----------------------------------------------------
bool __fastcall TMStarF::PrintEAN(unsigned __int64 HEAN)
{

    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //c2h
        char psw[4];    // Пароль на связь	S4
        char ean[5];  //ean
        char bcc;    // BCC	B	2
    } CmdS = {0x02,0x0a,0xc2,"\x1e\x0\x0\x0","",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  default:
    break;
  case 4:
  case 8:
      //Преобразование номера чека в EAN
/*    char EAN[5];
    TDateTime StartTime;
    TDateTime BillTime;
    StartTime = EncodeDate(2005,1,1);
    BillTime = EncodeDate((Word)StrToInt("20"+BillNumber.SubString(7,2)), (Word)StrToInt(BillNumber.SubString(9,2)), (Word)StrToInt(BillNumber.SubString(11,2)));
    unsigned __int64 HEAN;
    unsigned __int64 const c9 = 1000000000;
    HEAN = floor(BillTime - StartTime + 0.5);
    HEAN = HEAN*24 + StrToInt(BillNumber.SubString(13,2));
    HEAN = HEAN*60 + StrToInt(BillNumber.SubString(15,2));
    HEAN = HEAN*60 + StrToInt(BillNumber.SubString(17,2));
    HEAN = HEAN + SerialID.ToInt() * c9; */

//    memcpy(EAN,(char *)&HEAN,5);

//    if(HEAN < c9) return false;

    //memcpy(CmdS.psw, Psw.c_str(), 4);
    memcpy(CmdS.ean, (char *) &HEAN, 5);
      if(SendData((char*) &CmdS))
          {
          memcpy(&AnsS,Data,sizeof(AnsS));
          KKMResult = IntToHex(AnsS.error,4);
          if(AnsS.error) return false;
          KKMStatus = "0000";
          PrinterStatus = "00";
          }
      else
          {
          KKMResult = "00FF";
          KKMStatus = "0000";
          PrinterStatus = "00";
          return false;
          }
 // break;
  }
  return true;
}
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Cash - сумма, по разным типам платежей. 1- наличные, 4 - карты. Остальное не используем.
// 2 - кредит, 3 - тара.
void __fastcall TMStarF::BillPayment(AnsiString Cash1,AnsiString Cash2,AnsiString Cash3,AnsiString Cash4)
{
    AnsiString TmpStr;
    long TmpLong,bn,ss;
    int p;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char nal[5];
        unsigned char cred[5];
        unsigned char tara[5];
        unsigned char card[5];
        char discount[2];
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdS = {0x02,71,0x85,"","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0",0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char sdacha[5];
        unsigned char bcc;    // BCC	B	2
    } AnsS;

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char nal[5];
        unsigned char cred[5];
        unsigned char tara[5];
        unsigned char card[5];
        char discount[2];
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,71,0x85,"","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0",0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char sdacha[5];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

switch(FRType)
  {
  default:
  case 0: break;
  case 2: break;
  case 4:
    Print("========================================",true);
    Print(" ",true);
    Print("    Оплата принята с благодарностью",true);
    Print("  Работаем ежедневно с 9:00 до 22:00",true);
    strncpy(CmdS.psw, Psw.c_str(), 4);
    memcpy(CmdS.text," ",2);

    TmpStr = Cash1;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdS.nal),&TmpLong,4);

    TmpStr = Cash2;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdS.cred),&TmpLong,4);

    TmpStr = Cash3;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdS.tara),&TmpLong,4);

    TmpStr = Cash4;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdS.card),&TmpLong,4);

    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }

    if(KKMResult!="0000") return;
    Sleep(5000);
    GetVersion(); //Сюда заложено ожидание окончания печати
    if(KKMStatus == "0000")
      {
      Sleep(5000);
      GetVersion(); //А это на случай отказа в первый раз - повторный опрос
      }
  break;
  case 8:
    Print("========================================",true);
    Print(" ",true);
    Print("    Оплата принята с благодарностью",true);
    Print("  Работаем ежедневно с 9:00 до 22:00",true);
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    memcpy(CmdSo.text," ",2);

    TmpStr = Cash1;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdSo.nal),&TmpLong,4);

    TmpStr = Cash2;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdSo.cred),&TmpLong,4);

    TmpStr = Cash3;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdSo.tara),&TmpLong,4);

    TmpStr = Cash4;
    p = TmpStr.Pos(".");
    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return;
    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str());
    memcpy(&(CmdSo.card),&TmpLong,4);

    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }

    if(KKMResult!="0000") return;
    Sleep(5000);
    GetVersion(); //Сюда заложено ожидание окончания печати
    if(KKMStatus == "0000")
      {
      Sleep(5000);
      GetVersion(); //А это на случай отказа в первый раз - повторный опрос
      }
  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMStarF::SetClock(void)
{
Word Hour,Min,Sec,MSec;
TDateTime dtPresent = Now();
DecodeTime(dtPresent,Hour,Min,Sec,MSec);

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char hour;
        char min;
        char sec;
        char bcc;    // BCC	B	2
    } CmdS = {0x02,8,0x21,0,0,0,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  default:
  case 0: break;
  case 2: break;
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
    CmdS.hour = Hour;
    CmdS.min  = Min;
    CmdS.sec  = Sec;

    if(SendData((char*) &CmdS))
    {
        memcpy(&AnsS,Data,sizeof(AnsS));
        KKMResult = IntToHex(AnsS.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }

  break;
  }
}

//----------------------------------------------------------------------
bool __fastcall TMStarF::GetStatus()
{
bool res = true;

log("Вход в GetStatus");

// только для ШТРИХ
    struct
    {
        char stx;       // STX		1
        char length;
        char code;      //11h
        char psw[4];    // Пароль на связь	S4
        char bcc;    // BCC	B	2
    } CmdS = {0x02,5,0x11,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //48
        unsigned char code;      //11h
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char FRPOVersion[2];
        unsigned short FRPOSubVersion;
        unsigned char FRPODate[3];
        unsigned char FRNumber;
        unsigned short BillNumber;
        unsigned short FRFlags;
        unsigned char FRRegime;
        unsigned char FRSubRegime;
        unsigned char FRPort;
        unsigned short FPPOVersion;
        unsigned short FPPOSubVersion;
        unsigned char FPPODate[3];
        unsigned char date[3];
        unsigned char time[3];
        unsigned char FPFlags;
        unsigned long SN;
        unsigned short LastSession;
        unsigned short FPRest;
        unsigned char ReRegistration;
        unsigned char ReRegistrationRemain;
        unsigned char INN[6];
        unsigned char bcc;    // BCC	B	2
    } AnsS;

switch(FRType)
  {
  case 4:
  case 8:
    strncpy(CmdS.psw, Psw.c_str(), 4);
      if(SendData((char*) &CmdS))
      {
        memcpy(&AnsS,Data,sizeof(AnsS)); // копируем ответ в структуру
        KKMResult = IntToHex(AnsS.error,4);
//        KKMStatus = IntToHex(AnsS.FRRegime,2)+IntToHex(AnsS.FRSubRegime,2);
//        PrinterStatus = IntToHex(AnsS.FRFlags,4);
//        if(KKMResult == "0000" && Serial == "") Serial = IntToStr(AnsS.SN);
        SerialNumber = AnsS.SN;
        Mode = AnsS.FRRegime;
        SubMode = AnsS.FRSubRegime;
        DocumentNum = AnsS.BillNumber;
        Version = AnsS.FPPOVersion;
        Build = IntToStr(AnsS.FRPOSubVersion);
        AnsiString dts = (AnsiString)AnsS.date[0]+DateSeparator+(AnsiString)AnsS.date[1]+DateSeparator+(AnsiString)AnsS.date[2];
        dts += " "+(AnsiString)AnsS.time[0]+TimeSeparator+(AnsiString)AnsS.time[1]+TimeSeparator+(AnsiString)AnsS.time[2];
        dt = StrToDateTime(dts);
      }
      else
      {
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
     res = false;
      }
      break;
}
        //DumpLog
        log("KKMStatus " + KKMStatus);

  return res;
}
//-----------------------------------------------------------------
// режим
int __fastcall TMStarF::GetMode()
{
   if(GetStatus())
   {
      return Mode;
   }
 return 255;
}
//------------------------------------------------------------------
// подрежим
int __fastcall TMStarF::GetSubMode()
{
   if(GetStatus())
   {
      return SubMode;
   }
 return 0;
}
//------------------------------------------------------------------
// дата и время
TDateTime __fastcall TMStarF::GetDateTime()
{
  if(GetStatus())
   {
      return dt;
   }
 return 0;
}
//-------------------------------------------------------------------
bool __fastcall TMStarF::BillShadowLine(AnsiString Name, hyper Price, hyper Qnty, AnsiString Meas, AnsiString Code, int NDS)
{
     AnsiString str,TmpStr;
     long TmpLong;
     int m,len;
     char p;
//     int ypos = 0;
     char point;
     int ks, bs;
     char buf[41];
     int inbuf;
     int i,j,k;


// ШТРИХ-онлайн

    struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        char qnty[5];
        char price[5];
        char dep;
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,60,0x80,"","","",0,0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    switch(FRType)
    {
        case 8:
     strncpy(CmdSo.psw, Psw.c_str(), 4);
       //Код товара
       TmpStr = "//" + Code + " " + Name;
//   TmpStr = Code + " " + Name;  // для теста
       TmpStr = TmpStr.SubString(1,39);
       //Наименование товара
       strncpy(CmdSo.text,TmpStr.c_str(), TmpStr.Length()+1);
//Цена
       TmpLong = Price;     // atol(TmpStr.c_str());
       memcpy(&(CmdSo.price),&TmpLong,4);
       CmdSo.price[4] = 0; // не знаю зачем
         //Количество
       TmpLong = Qnty;
       memcpy(&(CmdSo.qnty),&TmpLong,4);
       CmdSo.qnty[4] = 0; // не знаю зачем

       CmdSo.nalog1 = NDS;
//       frStatus->OperationResult = 0xFF;

       if(SendData((char*) &CmdSo))
       {
           memcpy(&AnsSo,Data,sizeof(AnsSo));
           frStatus->OperationResult = AnsSo.error;
           KKMResult = IntToHex(AnsSo.error,4);
           KKMStatus = "0000";
           PrinterStatus = "00";
       }
       else
       {
           frStatus->OperationResult = 0xFF;
           KKMResult = "00FF";
           KKMStatus = "0000";
           PrinterStatus = "00";
           return false;
       }
       if(KKMResult != "0000") return false;
    }
return true;
}
//-------------------------------------------------------------------
void __fastcall TMStarF::PrintLine(AnsiString Name, AnsiString Price, AnsiString Qnty, AnsiString Summ, AnsiString Meas, AnsiString Code, AnsiString NDS)
{
   AnsiString StrName, TmpStr;
   int len, j;
   double Sum;
   char buf[41];
   AnsiString NDSName[3] = {"18%","10%","0%"}; // поменять при переходе на правильный НДС
//   AnsiString NDSName[3] = {"0%","10%","18%"};
   TStringList* slNameList = new TStringList();

    switch(FRType)
    {
        case 8:
       //Код товара
       StrName = Code + " " + Name;

       //Наименование товара
         if( (StrName.Length() + Meas.Length()) > 40 ) //переполнение - нужно бить по словам
         {
            Explode(StrName, " ", slNameList, 39, true);
            len = 0;
            j = 0;
            for(int i=0; i < slNameList->Count; i++) // цикл по словам
            {
               len += slNameList->Strings[i].Length() + 1; // +1 - это учет пробела
               if(len > 38) // попадают все строки больше 38
               {
                  for(int k = j; k < i; k++) // печатаем строки из целых слов меньше 40 символов не фискальным методом
                  {
                     TmpStr += slNameList->Strings[k] + " ";
                  }
                  strncpy(buf,TmpStr.c_str(), TmpStr.Length()+1);
                  len = slNameList->Strings[i].Length() + 1;      // длина крайнего слова
                  j = i;

                  TmpStr = "";
                  Print(buf,true);          // печатаем строку
                  if(frStatus->OperationResult != 0) FRInit();
               }
            }
            TmpStr = "";
            for (int i = j;i < slNameList->Count; i++ )  // обработка завершающей строки
            {
               TmpStr += slNameList->Strings[i] + " ";
            }
            if (TmpStr.Length() + Meas.Length() > 38)
            {
               TmpStr = TmpStr.SubString(1,38);
               strncpy(buf,TmpStr.c_str(), TmpStr.Length() + 1);
               Print(buf,true);
               TmpStr = AnsiString::StringOfChar(' ',38 - Meas.Length());
               TmpStr += "(" + Meas + ")";
               strncpy(buf,TmpStr.c_str(), TmpStr.Length() + 1);
               TmpStr="";
               Print(buf,true);
               if(frStatus->OperationResult != 0) FRInit();
            }
            else
            {
               TmpStr += AnsiString::StringOfChar(' ',38 - TmpStr.Length() - Meas.Length());
               TmpStr += "(" + Meas + ")";
               strncpy(buf,TmpStr.c_str(), TmpStr.Length() + 1);
               TmpStr="";
               Print(buf,true);
               if(frStatus->OperationResult != 0) FRInit();
            }
     }
     else
     {
         TmpStr = StrName;
         strncpy(buf,TmpStr.c_str(), TmpStr.Length() + 1);
         Print(buf,true);
         if(frStatus->OperationResult != 0) FRInit();
     }
     slNameList = NULL;
     delete slNameList;
     //Цена
     TmpStr = Qnty + " X " + Price;
     TmpStr = AnsiString::StringOfChar(' ',40 - TmpStr.Length()) + TmpStr;
     strncpy(buf,TmpStr.c_str(),TmpStr.Length() + 1);
     TmpStr="";
     Print(buf,true);
//       Sum = Qnty.ToDouble() * Price.ToDouble();
     TmpStr = "= " + Summ;
     TmpStr = AnsiString::StringOfChar(' ',24 - TmpStr.Length() - NDSName[NDS-1].Length()) + TmpStr;
     TmpStr = "Сумма,в т.ч.НДС " + NDSName[NDS-1] + TmpStr;
     strncpy(buf,TmpStr.c_str(),TmpStr.Length() + 1);
     TmpStr="";
     Print(buf,true);
     if(frStatus->OperationResult != 0) FRInit();
/*   новый НДС
     //   НДС
     AnsiString nds;
     if(NDS == 1) nds = MoneyAsString(MoneyAshyper(Summ)*18/118.0 +0.5);
     else if(NDS == 2) nds = MoneyAsString(MoneyAshyper(Summ)*10/110.0 +0.5);
     else nds = 0;
     TmpStr = NDSName[NDS-1]+" = "+nds;
     TmpStr = "НДС " + AnsiString::StringOfChar(' ',36 - TmpStr.Length()) + TmpStr;
     strncpy(buf,TmpStr.c_str(),TmpStr.Length() + 1);
     TmpStr="";
     Print(buf,true); */


   }
}

//--------------------------------------------------------
// разбиваем строку с разделителями
int __fastcall TMStarF::Explode(String sContent, String sSeparator, TStringList *slTokens, int iLength, bool bTrim = true)
{
  // abort if no stringlist is given
  if (slTokens == NULL) { return 0; }
  // trim the content
  if (bTrim) { sContent = sContent.Trim(); }

  // go through the content and split it like Moses split the Red Sea
  while (sContent.Length() > 0)
  {
    // find first separator in content
    int iPos = sContent.Pos(sSeparator);
    if (iPos > 0)
    {
      // первое слово
      if (sContent.SubString(1, iPos - 1).Length() > iLength && iLength != 0)
      {
         slTokens->Add(sContent.SubString(1, iLength));
         sContent = sContent.SubString(iLength + 1, sContent.Length() - iLength);
      }
      else
      {
         slTokens->Add(sContent.SubString(1, iPos - 1));
         // обрезание первого слова
         sContent = sContent.SubString(iPos + 1, sContent.Length() - iPos);
      }
      // trim the content
      if (bTrim) { sContent = sContent.Trim(); }
    }
    else
    {
      // just one token left
      slTokens->Add(sContent);
      // abort the loop with empty content
      sContent = "";
    }
  }
  return slTokens->Count;
}
//----------------------------------------------------------
long __fastcall TMStarF::SubTotal()
{
   long TmpLong;

   struct
   {
      char stx;       // STX		1
      char length;
      char code;
      char psw[4];    // Пароль на связь	S4
      char bcc;
   } CmdSo = {0x02,5,0x89,"",0};

   struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        unsigned char SubTotal[5];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

   switch (FRType)
   {
      case 8:
         strncpy(CmdSo.psw, Psw.c_str(), 4);
         if(SendData((char*) &CmdSo))
         {
            memcpy(&AnsSo,Data,sizeof(AnsSo));
            frStatus->OperationResult = AnsSo.error;
            KKMResult = IntToHex(AnsSo.error,4);
            KKMStatus = "0000";
            PrinterStatus = "00";
         }
         else
         {
            frStatus->OperationResult = 0xFF;
            KKMResult = "00FF";
            KKMStatus = "0000";
            PrinterStatus = "00";
         }
         if(KKMResult!="0000") return -1;
         memcpy(&TmpLong,&(AnsSo.SubTotal),4);


   }
return TmpLong;
}
//--------------------------------------------------------
bool __fastcall TMStarF::BillShadowMass()
{
   bool res = true;
   std::vector<BillItemLine>::iterator it;
   for(it = ShadowLineData->begin();it<ShadowLineData->end();++it)
   {
      BillShadowLine(it->Name,it->Price,it->Quantity,it->Measure, it->ScanCode,it->NDS);
      if(frStatus->OperationResult != 0) { res=false; break;}
   }
   return res;
}
//--------------------------------------------------------------------------
bool __fastcall TMStarF::PrintLineMass()
{

   std::vector<BillItemLine>::iterator it;
   for(it = PrintLineData->begin();it<PrintLineData->end();++it)
   {
      PrintLine(it->Name, MoneyAsString(it->Price), QuantityAsString(it->Quantity),
         MoneyAsString(it->Sum), it->Measure, it->ScanCode, (AnsiString)it->NDS);
      if(KKMStatus != "0000")
      {
         log("Ошибка печати строки чека: " + KKMStatus);
         return false;
      }
   }

/*   Synchronize(GetNumItems);
   for(int i=0; i < NumOfItems; i++ )
   {
         PrintLine(stPrintLine[i].Name, MoneyAsString(stPrintLine[i].Price),
         QuantityAsString(stPrintLine[i].Quantity),MoneyAsString(stPrintLine[i].Sum),
         stPrintLine[i].Measure, stPrintLine[i].ScanCode, (AnsiString)stPrintLine[i].NDS);
         PrintLine(PrintLineData)
         if(KKMStatus != "0000")
         {
            log("Ошибка печати строки чека: " + KKMStatus);
            return false;
         }
   }      */
return true;
}
//--------------------------------------------------------
void __fastcall TMStarF::GetNumItems()
{
   NumOfItems = MainWindow->Grid->RowCount - 1;
}
//------------------------------------------------------------------------------
AnsiString __fastcall TMStarF::MoneyAsString(unsigned hyper Money)
{
return String(Money/100)+Format(".%2.2D",ARRAYOFCONST(((int)(Money%100))));

}
//---------------------------------------------------------------------------
AnsiString __fastcall TMStarF::QuantityAsString(unsigned hyper Quantity)
{
return String(Quantity/1000)+Format(".%3.3D",ARRAYOFCONST(((int)(Quantity%1000))));
}
//-----------------------------------------------------------------------------
bool __fastcall TMStarF::BillComplete()
{
   int k = 0, AttMethodCalc, AttSubjectCalc;
   bool res = false;
   hyper st;
   ResetEvent(hEvent1);

  while(!res && k<3) //не факт что нужны дополнительные попытки пробить чек
  {
      k++;
      if(!FRInit()) continue;
      if(!BillOpen(stBill->Sale, stBill->CasName, stBill->CasNumber))
      {
         log("Ошибка открытия чека " + frStatus->OperationResult);
         DeleteDoc();
         continue;
//         res = BillOpen(stBill->Sale, stBill->CasName, stBill->CasNumber);  // при открытии идет сброс предыдущего чека
      }

      for(std::vector<BillItemLine>::iterator it = ShadowLineData->begin();it<ShadowLineData->end();++it)
      {

         if(it->ScanCode.Length() == 10)
         {
            AttMethodCalc = 3;
            AttSubjectCalc = 10;
         }
         else
         {
            AttMethodCalc = 4;
            AttSubjectCalc = 1;
         }

         res = FNOperation(OT_RECEIVE, it->Price, it->Quantity*1000, it->NDS, AttMethodCalc, AttSubjectCalc, it->ScanCode+" "+ it->Name, it->Measure);
//         BillShadowLine(it->Name,it->Price,it->Quantity,it->Measure, it->ScanCode,it->NDS);
         if(frStatus->OperationResult != 0)
         {
            res=false;
            break;
         }
      }

      if(!res)
      {
         log("Ошибка внесения продажи " + IntToHex(frStatus->OperationResult, 2) + "h");
         continue;
      }
//      res = PrintLineMass();
/*      if(!res)
      {
         log("Ошибка печати строк чека " + frStatus->OperationResult);
         continue;
      } */

      for(std::vector<ComboPay>::iterator it = stBill->vPay.begin(); it < stBill->vPay.end(); ++it)
      {
        if(it->PayType == INTERNATIONAL_CARD_PAYMENT) Print(it->Check, true);
      }

      //Печать информации об оплате подарочными картами
      GiftCardPrintPayments();
      PresentPrintBill();
//      res = BillClose(stBill->CasNumber);
      res = FNCloseCheckEx(stBill->vPay);
  } // конец цикла попыток пробить чек

   if(res) {stBill->Status = 0;}
   else
   {
      log("Ошибка закрытия чека " + IntToHex(frStatus->OperationResult, 2) + "h");
      stBill->Status = 1;
   }
   FRInit();
   // сообщение об окончании работы с ФР
  SetEvent(hEvent1);
  return res;
}
//------------------------------------------------------------------------
void __fastcall TMStarF::log(AnsiString s)
{
/*        //DumpLog
        if(MainWindow->Log)
          {
           ofstream errlog;
           errlog.open("errlog.txt",ios::app);
           errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz ", Now()).c_str() <<s.c_str();
           errlog << "\n";
           errlog.close();
          }
        //EndDumpLog */
   if (CreateDirectory(MainWindow->LogDir.c_str(), NULL) ||
       ERROR_ALREADY_EXISTS == GetLastError())
   {
      AnsiString fileName = MainWindow->LogDir;
      fileName += FormatDateTime("yyyymmdd", Now()) + ".log";
      if(!MainWindow->Log) return;
      ofstream errlog;
      errlog.open(fileName.c_str(), ios::app);
      errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz ", Now()).c_str() <<s.c_str();
      errlog << "\n";
      errlog.close();
   }

}
//--------------------------------------------------------------------------
// печать оплаты подарочными картами на чеке
bool __fastcall TMStarF::GiftCardPrintPayments()
{
AnsiString C,S,R;
   vector<GiftCardData>::iterator it;

   for(it = GiftItemData->begin(); it<GiftItemData->end(); ++it)
   {
      AnsiString C = "Подарочная карта "+ it->Code +" оплата";
      AnsiString S = "принята на сумму "+ it->Sum;
      AnsiString R = "остаток на карте "+ it->Residue;
      Print("----------------------------------------",true);
      Print(C.c_str(),true);
      Print(S.c_str(),true);
      Print(R.c_str(),true);
   }

/*   Synchronize(GetNumGiftCard);
for(int i = 0; i < NumOfGiftCard ; i++)
  {
    AnsiString C = "Подарочная карта "+ stGiftCard[i].Code +" оплата";
    AnsiString S = "принята на сумму "+stGiftCard[i].Sum;
    AnsiString R = "остаток на карте "+stGiftCard[i].Residue;
    Print("----------------------------------------",true);
    Print(C.c_str(),true);
    Print(S.c_str(),true);
    Print(R.c_str(),true);
  } */
return true;
}
//----------------------------------------------------------------------
void __fastcall TMStarF::GetNumGiftCard()
{
   NumOfGiftCard = MainWindow->GiftCardM->Lines->Count;
}
//-----------------------------------------------------------------------
// добавляем в чек информацию о подарков
bool __fastcall TMStarF::PresentPrintBill()
{
   AnsiString C;
   bool f = true;
   std::vector<PresentsData>::iterator it;
   for(it = PresentItemData->begin(); it < PresentItemData->end(); ++it)
   {
      if(it->Code.Length() < 10) continue;
      if(f) // выполняется только один раз
      {
         Print("--------------- ПОДАРКИ ----------------",true);
         f = false;
      }
   C = it->Name + " " + it->Code;
   Print(C.c_str(),true);
   }

/*   Synchronize(GetNumPresents);
   for(int i = 0; i < NumOfPresents ; i++)
   {
      if(stPresents[i].Code.Length() < 10) continue;
      if(f) // выполняется только один раз
      {
         Print("--------------- ПОДАРКИ ----------------",true);
         f = false;
      }
   C = stPresents[i].Name + " " + stPresents[i].Code;
   Print(C.c_str(),true);
  } */
return true;
}
//--------------------------------------------------------------------
void __fastcall TMStarF::GetNumPresents()
{
   if(MainWindow->PresentGrid->Cells[1][1].Length() < 10) {NumOfPresents;}
   else {NumOfPresents = MainWindow->PresentGrid->RowCount - 1;}
}
//-------------------------------------------------------------------

bool __fastcall TMStarF::FRInit()
{
log("Вход в FRInit mstar");
  GetStatus();
  int q = 0;
  switch(Mode){
   case 1:
      log("Выдача данных");
   break;
   case 2:
     switch(SubMode)
     {
         case 0:
            return true;
         case 1:
         case 2:
            while(SubMode == 1 || SubMode == 2)
            {
               q++;
               if(q < 5)
               {
                  MainWindow->Name->Caption = "Закончилась бумага";
                  MessageBox(GetActiveWindow(),"Замените бумагу, потом нажмите Enter","Нет бумаги",MB_OK);
               }
               else
               {
                  MainWindow->Name->Caption = "Возможно засорился датчик бумаги";
                  MessageBox(GetActiveWindow(),"Продуйте от пыли оптический датчик бумаги слева от рулона и затем нажмите ОК","Почистите датчик бумаги",MB_OK);
               }
               GetStatus();
            }
           if(SubMode == 0)
           {
               ResumePrint();
               return FRInit();
           }
            else if(SubMode == 3)
            {
               return FRInit();
            }
            else
            {
               return false;
            }
         case 3:
            ResumePrint();
            return FRInit();
         case 5:
            int k = 0;
            while(GetSubMode() == 5 && k < 60) {Sleep(1000); k++;}
            if(GetSubMode() == 0) return true;
            else return false;
     }
   case 3:
      MainWindow->Name->Caption = "Смена не закрыта, 24 часа закончились. Закройте смену!!!";
      return false;
   case 8:
   case 40:
      if (SubMode == 2)
      {
         MainWindow->Name->Caption = "Закончилась бумага";
         MessageBox(GetActiveWindow(),"Замените бумагу, потом нажмите Enter","Нет бумаги",MB_OK);
         return FRInit();
      }
      else if(SubMode == 3)
      {
         ResumePrint();
         return FRInit();
      } else if(SubMode == 5)
      {
        int k = 0;
        while(GetSubMode() == 5 && k < 60) {Sleep(1000); k++;}
        if(GetSubMode() == 0) return true;
        else return false;
      }
      else {DeleteDoc();}
      if(KKMStatus == "0000") return FRInit();
      else return false;
   case 9:
      MainWindow->Name->Caption = "Необходимо технологическое обнуление ККМ!!!";
      return false;
  }
 return false;
}
//------------------------------------------------------------------------
hyper __fastcall TMStarF::MoneyAshyper(AnsiString String)
{
int p, sign = 1;
AnsiString C,D;
unsigned hyper r,d;

String = String.Trim();
if(String.Pos("-") > 0) sign = -1;
if( (p = String.Pos(".")) == 0) p = String.Pos(",");
if( p == 0) return 100*atol(String.c_str());
else
  {
  C = String.SubString(1,p-1);
  D = String.SubString(p+1,String.Length()-p);
  r = atol(D.c_str());
  p = D.Length();
  d = 1;
  if(p>2) { for(int i=0;i<p-2;i++) d*=10; r=(r+5)/d;}
  else if(p<2) { for(int i=0;i<2-p;i++) d*=10; r*=d; }
  return sign*(100*atol(C.c_str())+r);
  }
}
//------------------------------------------------------------------------
// статус отправки в ОФД
void __fastcall TMStarF::GetConnectionStatus()
{
   struct{
      char stx;
      char length;
      unsigned char code[2];
      char psw[4];
      char bcc;
   } CmdSo = {0x02,6,{0xFF,0x39},{0x0,0x0,0x0,0x0},0};

   struct{
      unsigned char stx;
      unsigned char length;
      unsigned char code[2];
      unsigned char error;
      unsigned char status;
      unsigned char readMsgStat;
      unsigned char quantityMsg[2];
      unsigned char docNum[4];
      unsigned char docData[5];
      unsigned char bcc;
   } AnsSo;
   switch(FRType)
   {
      case 8:
         strncpy(CmdSo.psw, Psw.c_str(),4);
         if(SendData((char *) &CmdSo))
         {
            memcpy(&AnsSo, Data, sizeof(AnsSo));
            KKMResult = IntToHex(AnsSo.error, 4);
            if(AnsSo.error) return;
            KKMStatus="0000";
            PrinterStatus="00";
            frStatus->OFDstatus = AnsSo.status;
            frStatus->OFDreadMsgStat = AnsSo.readMsgStat;
            memcpy(frStatus->OFDquantityMsg, AnsSo.quantityMsg, sizeof(frStatus->OFDquantityMsg));
            memcpy(frStatus->OFDdocNum, AnsSo.docNum, sizeof(frStatus->OFDdocNum));
            memcpy(frStatus->OFDdocData, AnsSo.docData, sizeof(frStatus->OFDdocData));
         }
         else
         {
            KKMResult = "00FF";
            KKMStatus = "0000";
            PrinterStatus = "00";
            return;
         }
   }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// закрытие чека
bool __fastcall TMStarF::BillClose(int OperatorNumber)
{
    int k,p;
    AnsiString TmpStr;
    long TmpLong,bn,ss;
    int Discount = 0;
    unsigned long sum[NUM_TYPE_PAYMENTS];
    std::vector<ComboPay>::iterator it;

// ШТРИХ-онлайн
        struct
    {
        char stx;       // STX		1
        char length;
        char code;
        char psw[4];    // Пароль на связь	S4
        unsigned char nal[5];
        unsigned char mir[5];
        unsigned char visa_mc[5];
        unsigned char othercard[5];
        char discount[2];
        char nalog1;
        char nalog2;
        char nalog3;
        char nalog4;
        char text[40];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,71,0x85,"","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0\x0\x0\x0","\x0\x0",0,0,0,0,"",0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //8
        unsigned char code;
        unsigned char error;
        unsigned char operatorNumber;
        char sdacha[5];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

switch(FRType)
{
    case 8:
    Print("========================================",true);
    Print(" ",true);
// Печатаем текст в середине чека
    for(int i=0;i<6;i++)
    {
      if(stBill->MidText[i] != "") {Print((stBill->MidText[i]).c_str(),true);}
      else {break;}
    }
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    memcpy(CmdSo.text," ",2);
//    for(int i = 0; i < sizeof(sum)/sizeof(sum[0]); i++) sum[i] = 0;
    for(int i = 0; i < NUM_TYPE_PAYMENTS; i++) sum[i] = 0;
    for(it = stBill->vPay.begin(); it < stBill->vPay.end(); ++it)
    {
      Discount += it->RoundPart;
      if(it->PayType == NAL_PAYMENT)
      {
         sum[0] += it->Sum + it->Change;
      }
      else if(it->PayType == INTERNATIONAL_CARD_PAYMENT)
      {
         switch(it->CType)
         {
            case CT_MIR_DEBIT:
            case CT_MIR:
               sum[1] += it->Sum;
               break;
            case CT_VISA:
            case CT_EUROCARD:
               sum[2] += it->Sum;
               break;
            default:
               sum[3] += it->Sum;
          }
      }
    }
//    TmpStr = Cash;
/*    p = TmpStr.Pos(".");

    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return false;

    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str()); */
    memcpy(&(CmdSo.nal),sum,4);
    memcpy(&(CmdSo.mir), sum+1,4);
    memcpy(&(CmdSo.visa_mc),sum+2,4);
    memcpy(&(CmdSo.othercard), sum+3,4);
    memcpy(&(CmdSo.discount), &Discount, 2);
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
      frStatus->OperationResult = 0xff;
      KKMResult = "00FF";
      KKMStatus = "0000";
      PrinterStatus = "00";
    }

    if(frStatus->OperationResult != 0) return false;
//    Sleep(5000);
//    GetVersion(); //Сюда заложено ожидание окончания печати
//    if(KKMStatus == "0000")
//      {
//      Sleep(5000);
//      GetVersion(); //А это на случай отказа в первый раз - повторный опрос
//      }
      break;
   default:
      return false;
  }
 return true;
}
//-------------------------------------------------------------------------
bool __fastcall TMStarF::FNOperation(int OpType, hyper Price, hyper Quantity, int NDS, int AttMethodCalc, int AttSubjectCalc, AnsiString Name, AnsiString Meas)
{
     AnsiString str,TmpStr;
     long TmpLong;
     int m,len;
     char p;
//     int ypos = 0;
     char point;
     int ks, bs;
     char buf[41];
     int inbuf;
     int i,j,k;


// ШТРИХ-онлайн

    struct
    {
        char stx;       // STX		1
        char length;
        unsigned char code[2];
        char psw[4];    // Пароль на связь	S4
        char opType;
        char qnty[6];
        char price[5];
        char sum[5];
        char nalogsum[5];
        char nalog;
        char dep;
        char methodCalc;
        char subjectCalc;
        char name[128];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,160,{0xFF,0x46},{0x0,0x0,0x0,0x0},0,{0x0,0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0xFF,0xFF,0xFF,0xFF,0xFF},{0xFF,0xFF,0xFF,0xFF,0xFF},0,0,0,0,0};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //3
        unsigned char code[2];
        unsigned char error;
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

    switch(FRType)
    {
        case 8:
     CmdSo.opType = OpType;
     CmdSo.methodCalc = AttMethodCalc;
     CmdSo.subjectCalc = AttSubjectCalc;
     strncpy(CmdSo.psw, Psw.c_str(), 4);
       //Код товара
//       TmpStr = Code + " " + Name;
//   TmpStr = Code + " " + Name;  // для теста
       Name = Name.SubString(1,127);
       //Наименование товара
      Meas = "(" + Meas.Trim() + ")";
      int nameLen = Name.Length();
      int measLen = Meas.Length();
      int strNum = std::ceil((Name.Length() + Meas.Length() + 1)/(float)FR_LINE_LEN);
      Name = Name.SubString(1, FR_LINE_LEN*strNum - Meas.Length() - 1);
//       TmpStr = Name.SubString((strNum-1)*FR_LINE_LEN + 1, (int)(Name.Length() + Meas.Length() + 1));
//       TmpStr = TmpStr + AnsiString::StringOfChar(' ',(int)(FR_LINE_LEN - TmpStr.Length() - Meas.Length()));
//       Name = Name.SubString(1, (strNum - 1)*FR_LINE_LEN) + TmpStr + Meas;
//       lastSpace = Name.SubString(1, FR_LINE_LEN).LastDelimiter(" ");
      Name = Name + " " + Meas;
       strncpy(CmdSo.name,Name.c_str(), Name.Length()+1);
//Цена
       memcpy(&(CmdSo.price),&Price,4);
       CmdSo.price[4] = 0; // не знаю зачем
         //Количество
       memcpy(&(CmdSo.qnty),&Quantity,6);
       CmdSo.qnty[5] = 0; // не знаю зачем

       CmdSo.nalog = NDS;
//       frStatus->OperationResult = 0xFF;

       if(SendData((char*) &CmdSo))
       {
           memcpy(&AnsSo,Data,sizeof(AnsSo));
           frStatus->OperationResult = AnsSo.error;
       }
       else
       {
           frStatus->OperationResult = 0xFF;
           return false;
       }
       if(frStatus->OperationResult != 0x0) return false;
    }
return true;
}
//-----------------------------------------------------------------------
//расширенное закрытие чека
bool __fastcall TMStarF::FNCloseCheckEx(std::vector<ComboPay> &vPay)
{
    int k,p;
    AnsiString TmpStr;
    long TmpLong,bn,ss;
    int Discount = 0;
    unsigned long sum[NUM_TYPE_PAYMENTS];
    std::vector<ComboPay>::iterator it;

// ШТРИХ-онлайн
        struct
    {
        char stx;       // STX		1
        char length;
        char code[2];
        char psw[4];    // Пароль на связь	S4
        unsigned char nal[5];
        unsigned char mir[5];
        unsigned char visa_mc[5];
        unsigned char othercard[5];
        unsigned char sum5[5];
        unsigned char sum6[5];
        unsigned char sum7[5];
        unsigned char sum8[5];
        unsigned char sum9[5];
        unsigned char sum10[5];
        unsigned char sum11[5];
        unsigned char sum12[5];
        unsigned char sum13[5];
        unsigned char avans[5];
        unsigned char credit[5];
        unsigned char sum16[5];
        char discount;
        char nalog1[5];
        char nalog2[5];
        char nalog3[5];
        char nalog4[5];
        char nalog5[5];
        char nalog6[5];
        char taxsystem;
        char text[64];
        char bcc;    // BCC	B	2
    } CmdSo = {0x02,118,{0xFF,0x45},{0x0,0x0,0x0,0x0},
      {0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},
      {0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},
      {0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},
      {0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},0x0,{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},
      {0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0x0},0x1};

    struct
    {
        unsigned char stx;       // STX
        unsigned char length;    //16
        unsigned char code[2];
        unsigned char error;
        char sdacha[5];
        char fdnum[4];
        char fiscalpr[4];
        unsigned char bcc;    // BCC	B	2
    } AnsSo;

switch(FRType)
{
    case 8:
    Print("========================================",true);
    Print(" ",true);
// Печатаем текст в середине чека
    for(int i=0;i<6;i++)
    {
      if(stBill->MidText[i] != "") {Print((stBill->MidText[i]).c_str(),true);}
      else {break;}
    }
    strncpy(CmdSo.psw, Psw.c_str(), 4);
    memcpy(CmdSo.text," ",2);
    for(int i = 0; i < NUM_TYPE_PAYMENTS; i++) sum[i] = 0;
    for(it = vPay.begin(); it < vPay.end(); ++it)
    {
      Discount += it->RoundPart;
      if(it->PayType == NAL_PAYMENT)
      {
         sum[SUM_NAL] += it->Sum + it->Change;
      }
      else if(it->PayType == INTERNATIONAL_CARD_PAYMENT)
      {
         switch(it->CType)
         {
            case CT_MIR_DEBIT:
            case CT_MIR:
               sum[SUM_MIR_CARD] += it->Sum;
               break;
            case CT_VISA:
            case CT_EUROCARD:
               sum[SUM_EC_VISA] += it->Sum;
               break;
            default:
               sum[SUM_OTHER] += it->Sum;
          }
      }
      else if(it->PayType == GIFT_CARD_PAYMENT)
      {
         sum[SUM_PREPAY] += it->Sum;
      }
      else
      {
         log("FNCloseEx: Ошибка типа платежа: " + (AnsiString)it->PayType);
         return false;
      }
    }
//    TmpStr = Cash;
/*    p = TmpStr.Pos(".");

    if(p == 0) TmpStr += ".00";
    else if(p == TmpStr.Length()) TmpStr += "00";
    else if(p == TmpStr.Length()-1) TmpStr += "0";
    else if(p != TmpStr.Length()-2) return false;

    TmpStr =  TmpStr.SubString(1,TmpStr.Length()-3)+ TmpStr.SubString(TmpStr.Length()-1,2);
    TmpLong = atol(TmpStr.c_str()); */
    memcpy(&(CmdSo.nal),sum,4);
    memcpy(&(CmdSo.mir), sum + SUM_MIR_CARD,4);
    memcpy(&(CmdSo.visa_mc),sum + SUM_EC_VISA,4);
    memcpy(&(CmdSo.othercard), sum + SUM_OTHER,4);
    memcpy(&(CmdSo.avans), sum + SUM_PREPAY,4);
    memcpy(&(CmdSo.discount), &Discount, 1);
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
      frStatus->OperationResult = 0xff;
      KKMResult = "00FF";
      KKMStatus = "0000";
      PrinterStatus = "00";
    }

    if(frStatus->OperationResult != 0) return false;
//    Sleep(5000);
//    GetVersion(); //Сюда заложено ожидание окончания печати
//    if(KKMStatus == "0000")
//      {
//      Sleep(5000);
//      GetVersion(); //А это на случай отказа в первый раз - повторный опрос
//      }
      break;
   default:
      return false;
  }
 return true;
}
//---------------------------------------------------------------------------

/*
struct Cmd{
//mutable std::vector<char> buf_;
char *buf;
int len;

public:
Cmd(){}
Cmd(int length)
{
  len = length;
  char *buf = new char[len];
}

template <typename T>
struct ref {
char * const p_;
ref (char *p) : p_(p) {}
//operator T () const { T t; memcpy(&t, p_, sizeof(t)); return t; }
//T operator = (T t) const { memcpy(p_, &t, sizeof(t)); return t; }
};

template <typename T>
ref<T> get (size_t offset) const {
if (offset + sizeof(T) > sizeof(buf)) throw "Error";
return ref<T>(&buf[0] + offset);
}

ref<char> stx () const { return get<char>(0); }
ref<char> length () const { return get<char>(1); }
ref<char> code () const { return get<char>(2); }
char *psw () const { return &buf[0] + 4; }
ref<char> flags () const { return get<char>(6); }
ref<char> font () const { return get<char>(10); }
char *text () const { return &buf[0] + 11; }
ref<char> bcc () const { return get<char>(52); }


}; */

// Печать заданным шрифтом
void __fastcall TMStarF::PrintF(AnsiString Text, int FontType)
{

   struct
   {
      char stx;
      char length;
      char code;
      char psw[4];
      unsigned char flags;
      char font;
      unsigned char text[51];
      unsigned char bcc;
   } CmdSo = {0x2,58,0x2F,"\x0\x0\x0\x0",0x42,0x1};

   struct
   {
      char stx;       // STX
      char length;    //8
      char code;
      char error;
      char oper;
      unsigned char bcc;    // BCC	B	2
   } AnsSo;

    Text = Text.SubString(1,50);

    strncpy(CmdSo.psw, Psw.c_str(),4);
    memcpy(CmdSo.text, Text.c_str(), Text.Length());
    CmdSo.font = FontType;
    if(SendData((char*) &CmdSo))
    {
        memcpy(&AnsSo,Data,sizeof(AnsSo));
        frStatus->OperationResult = AnsSo.error;
        KKMResult = IntToHex(AnsSo.error,4);
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
    else
    {
        frStatus->OperationResult = 0xFF;
        KKMResult = "00FF";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
return;

}
