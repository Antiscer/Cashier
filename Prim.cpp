//---------------------------------------------------------------------------

#include <vcl.h>
#include "fstream.h"
#pragma hdrstop

#include "Prim.h"
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
__fastcall TPrimF::TPrimF(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    Port = 0;
    Baud = 57600;
    ErrorCount = 0;
    hComm = NULL;
    Priority = tpIdle;
    KKMResult = "FFFF";
    KKMStatus = "0000";
    PrinterStatus = "00";
    Slip = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::BillOpen(bool Sale)
{
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
        tagReq Req[8];
        char bcc[2];
        char etx;
    } Cmd = {0x02, 0x53, "", 0x30, 0, "00", "008"};

    int ypos = 0;

    for(int j = 0; j <4; j++)
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
// ===
    if(Sale) ypos++;
    strcpy(Cmd.Req[5].type, "99");
    strcpy(Cmd.Req[5].flag, "00");
    strcpy(Cmd.Req[5].x, "00");
    strcpy(Cmd.Req[5].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    if(Sale) strcpy(Cmd.Req[5].str, "      Внимание! Уцененный товар");
    else strcpy(Cmd.Req[5].str, "");
    StrWinToDos(Cmd.Req[5].str);
// ===
    if(Sale) ypos++;
    strcpy(Cmd.Req[6].type, "99");
    strcpy(Cmd.Req[6].flag, "00");
    strcpy(Cmd.Req[6].x, "00");
    strcpy(Cmd.Req[6].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    if(Sale) strcpy(Cmd.Req[6].str, "     возврату и обмену НЕ подлежит");
    else strcpy(Cmd.Req[6].str, "");
    StrWinToDos(Cmd.Req[6].str);
// ===
    if(Sale) ypos++;
    strcpy(Cmd.Req[7].type, "99");
    strcpy(Cmd.Req[7].flag, "00");
    strcpy(Cmd.Req[7].x, "00");
    strcpy(Cmd.Req[7].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    if(Sale) strcpy(Cmd.Req[7].str, "========================================");
    else strcpy(Cmd.Req[7].str, "");

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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::BillClose(AnsiString Cash,  int OperatorNumber)
{
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
        tagReq Req[9];
        char bcc[2];
        char etx;
    } Cmd = {0x02, 0x53, "", 0x30, 0, "04", "009"};

    int ypos = 0;

    for(int j = 0; j < 9; j++)
    {
        for (int i = 0; i < 41; i++) Cmd.Req[j].str[i] = 0;
    }
// ===
    strcpy(Cmd.Req[0].type, "99");
    strcpy(Cmd.Req[0].flag, "00");
    strcpy(Cmd.Req[0].x, "00");
    strcpy(Cmd.Req[0].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[0].str, "========================================");
    ypos++;
// Итог
    strcpy(Cmd.Req[1].type, "12");
    strcpy(Cmd.Req[1].flag, "00");
    strcpy(Cmd.Req[1].x, "00");
    strcpy(Cmd.Req[1].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
// Спасибо за покупку
    strcpy(Cmd.Req[2].type, "99");
    strcpy(Cmd.Req[2].flag, "00");
    strcpy(Cmd.Req[2].x, "08");
    strcpy(Cmd.Req[2].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[2].str, "ПРИНЯТА С БЛАГОДАРНОСТЬЮ");
    StrWinToDos(Cmd.Req[2].str);
    ypos++;
// Получено
    strcpy(Cmd.Req[3].type, "13");
    strcpy(Cmd.Req[3].flag, "00");    // Наличные
    strcpy(Cmd.Req[3].x, "00");
    strcpy(Cmd.Req[3].str, Cash.c_str());
    strcpy(Cmd.Req[3].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
// Сдача
    strcpy(Cmd.Req[4].type, "14");
    strcpy(Cmd.Req[4].flag, "00");
    strcpy(Cmd.Req[4].x, "00");
    strcpy(Cmd.Req[4].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
    ypos++;
// Номер чека
    strcpy(Cmd.Req[5].type, "08");
    strcpy(Cmd.Req[5].flag, "00");
    strcpy(Cmd.Req[5].x, "04");
    strcpy(Cmd.Req[5].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
// Номер кассира
    strcpy(Cmd.Req[6].type, "06");
    strcpy(Cmd.Req[6].flag, "00");
    strcpy(Cmd.Req[6].x, "22");
    strcpy(Cmd.Req[6].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
// Номер счета
    strcpy(Cmd.Req[7].type, "09");
    strcpy(Cmd.Req[7].flag, "00");
    strcpy(Cmd.Req[7].x, "32");
    strcpy(Cmd.Req[7].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strncpy(Cmd.Req[7].str, Format("%4.4d", ARRAYOFCONST((OperatorNumber))).c_str(),4);
    ypos++;
// Дата совершения операции
    strcpy(Cmd.Req[8].type, "05");
    strcpy(Cmd.Req[8].flag, "00");
    strcpy(Cmd.Req[8].x, "12");
    strcpy(Cmd.Req[8].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());

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
// Если не получен правильный ответ, то проверить закрыт ли чек 10 раз.
        for(int k = 0; k < 10; k++)
        {
            GetVersion();
//LPSTR aszMsg[1];
//aszMsg[0] = KKMResult.c_str();
//HANDLE hEventLog = RegisterEventSource(NULL, "MStarF");
//ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, 0, 0, NULL,
//1, 0, (LPCTSTR *) aszMsg, NULL);
//DeregisterEventSource(hEventLog);
            if(KKMResult != "FF01" && KKMResult != "FF02")
            {
                AnsiString str = "0x";
                str += KKMResult;
                if(!(str.ToInt() & 0x0300)) KKMResult = "0000";
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::BillZK(int OperatorNumber, char* Card)
{
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
        tagReq Req[9];
        char bcc[2];
        char etx;
    } Cmd = {0x02, 0x53, "", 0x30, 0, "04", "009"};

    int ypos = 0;

    for(int j = 0; j < 9; j++)
    {
        for (int i = 0; i < 41; i++) Cmd.Req[j].str[i] = 0;
    }
// ===
    strcpy(Cmd.Req[0].type, "99");
    strcpy(Cmd.Req[0].flag, "00");
    strcpy(Cmd.Req[0].x, "00");
    strcpy(Cmd.Req[0].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[0].str, "========================================");
    ypos++;
// Итог
    strcpy(Cmd.Req[1].type, "12");
    strcpy(Cmd.Req[1].flag, "00");
    strcpy(Cmd.Req[1].x, "00");
    strcpy(Cmd.Req[1].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
// Спасибо за покупку
    strcpy(Cmd.Req[2].type, "99");
    strcpy(Cmd.Req[2].flag, "00");
    strcpy(Cmd.Req[2].x, "08");
    strcpy(Cmd.Req[2].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[2].str, "ПРИНЯТА С БЛАГОДАРНОСТЬЮ");
    StrWinToDos(Cmd.Req[2].str);
    ypos++;
// Получено
    strcpy(Cmd.Req[3].type, "13");
    strcpy(Cmd.Req[3].flag, "02");    // Карта
    strcpy(Cmd.Req[3].x, "00");
    strncpy(Cmd.Req[3].str, Card, 20);
    StrWinToDos(Cmd.Req[3].str);
    strcpy(Cmd.Req[3].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    ypos++;
// Пустая строка (резерв)
    strcpy(Cmd.Req[4].type, "99");
    strcpy(Cmd.Req[4].flag, "00");
    strcpy(Cmd.Req[4].x, "00");
    strcpy(Cmd.Req[4].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strcpy(Cmd.Req[4].str, "");
    StrWinToDos(Cmd.Req[4].str);
//    ypos++;
    ypos++;
// Номер чека
    strcpy(Cmd.Req[5].type, "08");
    strcpy(Cmd.Req[5].flag, "00");
    strcpy(Cmd.Req[5].x, "04");
    strcpy(Cmd.Req[5].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
// Номер кассира
    strcpy(Cmd.Req[6].type, "06");
    strcpy(Cmd.Req[6].flag, "00");
    strcpy(Cmd.Req[6].x, "22");
    strcpy(Cmd.Req[6].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
// Номер счета
    strcpy(Cmd.Req[7].type, "09");
    strcpy(Cmd.Req[7].flag, "00");
    strcpy(Cmd.Req[7].x, "32");
    strcpy(Cmd.Req[7].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
    strncpy(Cmd.Req[7].str, Format("%4.4d", ARRAYOFCONST((OperatorNumber))).c_str(),4);
    ypos++;
// Дата совершения операции
    strcpy(Cmd.Req[8].type, "05");
    strcpy(Cmd.Req[8].flag, "00");
    strcpy(Cmd.Req[8].x, "12");
    strcpy(Cmd.Req[8].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());

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
// Если не получен правильный ответ, то проверить закрыт ли чек 10 раз.
        for(int k = 0; k < 10; k++)
        {
            GetVersion();
//LPSTR aszMsg[1];
//aszMsg[0] = KKMResult.c_str();
//HANDLE hEventLog = RegisterEventSource(NULL, "MStarF");
//ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, 0, 0, NULL,
//1, 0, (LPCTSTR *) aszMsg, NULL);
//DeregisterEventSource(hEventLog);
            if(KKMResult != "FF01" && KKMResult != "FF02")
            {
                AnsiString str = "0x";
                str += KKMResult;
                if(!(str.ToInt() & 0x0300)) KKMResult = "0000";
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::BillLine(char* Name, char* Price, char* Qnty, int Num)
{
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

    Cmd.etx = 0x03;
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;

    int ypos = 0;
    char* point;

    point = (char*) &Cmd.Price[0].Req[0].type[0];
    for(int j = 0; j < sizeof(tagPrice)*CHLEN; j++) *(point+j) = 0;

    int ks = 0;
    int bs = 0;
    while(true)
    {

// NNum + Name + Unit 3 строки
        AnsiString str = AnsiString(&Name[ks*121]);
        strcpy(Cmd.Price[bs].Req[0].type, "99");
        strcpy(Cmd.Price[bs].Req[0].flag, "00");
        strcpy(Cmd.Price[bs].Req[0].x, "00");
        strcpy(Cmd.Price[bs].Req[0].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        strncpy(Cmd.Price[bs].Req[0].str, str.c_str(), 40);
        StrWinToDos(Cmd.Price[bs].Req[0].str);
        ypos++;
        strcpy(Cmd.Price[bs].Req[1].type, "99");
        strcpy(Cmd.Price[bs].Req[1].flag, "00");
        strcpy(Cmd.Price[bs].Req[1].x, "00");
        strcpy(Cmd.Price[bs].Req[1].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        if(strlen(str.c_str()) > 40) strncpy(Cmd.Price[bs].Req[1].str, &(str[41]), 40);
        else Cmd.Price[bs].Req[1].str[0] = 0;
        StrWinToDos(Cmd.Price[bs].Req[1].str);
        if(strlen(Cmd.Price[bs].Req[1].str) > 0) ypos++;
        strcpy(Cmd.Price[bs].Req[2].type, "99");
        strcpy(Cmd.Price[bs].Req[2].flag, "00");
        strcpy(Cmd.Price[bs].Req[2].x, "00");
        strcpy(Cmd.Price[bs].Req[2].y, Format("%2.2d", ARRAYOFCONST((ypos))).c_str());
        if(strlen(str.c_str()) > 80) strncpy(Cmd.Price[bs].Req[2].str, &(str[81]), 40);
        else Cmd.Price[bs].Req[2].str[0] = 0;
        StrWinToDos(Cmd.Price[bs].Req[2].str);
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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::OpenSession(AnsiString OperatorName, int OperatorNumber)
{
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
        if(KKMResult == "0000") Serial = String(&Data[16]);
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
    }
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::GetVersion()
{
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
    strncpy(Cmd.psw, Psw.c_str(), 4);
    Cmd.psw[4] = 0;
    if(SendData((char*) &Cmd))
    {
        KKMResult = String(&Data[8]);
        KKMStatus = String(&Data[3]);
        PrinterStatus = String(&Data[13]);
        if(KKMResult == "0000") Serial = String(&Data[16]);
        else Serial = "";
    }
    else
    {
//        KKMResult = "FF01";
        KKMStatus = "0000";
        PrinterStatus = "00";
        Serial = "";
    }
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::ZReport()
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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::XReport()
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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::DeleteDoc()
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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::CRLF()
{
    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char qlf[3];   // Кол-во строк
        char cut[3];
        char bcc[2];    // BCC	B	2
        char etx;       // ETX		1
    } Cmd = {0x02, 0x52, "", "06", "", "", 0x03};

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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::Print(char* Text, bool WinText)
{
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
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::OpenBox()
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
}
//---------------------------------------------------------------------------
bool __fastcall TPrimF::SendData(char *Command)
{
    unsigned char bcc = 0;
    int nOut = 0;
    AnsiString str;
    unsigned long nChar;

    KKMResult = "FF01";
    if(Slip == 0)
    {
        while(Command[nOut] != 0x03) nOut++;
        nOut++;
// Вычисление контрольной суммы
        for(int i = 1; i < nOut - 3; i++) bcc += (unsigned char) Command[i];
        str = Format("%2.2x", ARRAYOFCONST(((int)bcc))).LowerCase();
        Command[nOut-3] = str[1];
        Command[nOut-2] = str[2];
    }
    else nOut = Slip;
    DataReady = false;
    Printing = false;
    if(Slip == 0) Sleep(150);
    if(Wait)
    {
        KKMResult = "FF02";
        return false;
    }
    WriteFile (hComm, Command, nOut, &nChar, NULL);
    TimeOut = false;
    Resume();
    int n = 0;
    while(!DataReady)
    {
        if(Printing)
        {
            Printing = false;
            n = 0;
        }
        else if((n > 20 & Slip == 0) | (n > 200 & Slip !=0))
        {
            KKMResult = "FF02";
            TimeOut = true;
            return false;
        }
        n++;
        if(Slip == 0) Sleep(100);
        else Sleep(10);
    }
    if(Slip < 0) return true;
    bcc = 0;
    for(int i = 1; i < (int) Length - 3; i++)
        bcc += (unsigned char) Data[i];
    str = Format("%2.2x", ARRAYOFCONST(((int)bcc))).LowerCase();
    if(Length < 19) return false;
    if( Data[Length-3] != str[1] && Data[Length-2] != str[2])
        return false;
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::Execute()
{
    static char *szComPort[] = {"COM1", "COM2", "COM3", "COM4", "COM5", "COM6"};
    static DCB    dcb;
    DWORD dwEvMask, dwError;
    COMSTAT csStat;
    static COMMTIMEOUTS cto;
    static char szRead[1024];
    unsigned long nChar;

    Wait = false;

    if ((hComm = CreateFile (szComPort [Port-1], GENERIC_READ  | GENERIC_WRITE,
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

    if (SetCommState (hComm, &dcb) < 0)
    {
        CloseHandle (hComm);                    // close the COM port.
        KKMResult = "FF00";
        return;
    }
    cto.ReadIntervalTimeout = 1000;
    cto.ReadTotalTimeoutMultiplier = 200;
    cto.ReadTotalTimeoutConstant = 1000;
    cto.WriteTotalTimeoutMultiplier = 100;
    cto.WriteTotalTimeoutConstant = 1000;
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

    bool start = false;
    bool fin = false;
    Suspend();
    while(!Terminated)
    {
L1:     Wait = true;
        WaitCommEvent(hComm, &dwEvMask, NULL);
        Wait = false;
        if (dwEvMask & EV_RXCHAR)
        {
            ClearCommError(hComm, &dwError, &csStat);
            while(csStat.cbInQue > 0)
            {
                if(ReadFile(hComm, szRead, csStat.cbInQue, &nChar, NULL))
                {
                    for(unsigned long i = 0; i < nChar; i++)
                    {
                        if(TimeOut) Suspend();
                        if(szRead[i] == 0x06)
                        {
                            if(Slip == 1)
                            {
                                fin = true;
                                Slip = -1;
                                break;
                            }
                            Printing = true; //Печать успешна
                            ErrorCount = 0; //Счетчик ошибок в 0
                            continue;
                        }
                        if(szRead[i] == 0x05) //Ошибка на регистраторе
                        {
                            //DumpLog
                            ofstream errlog;
                            errlog.open("errlog.txt",ios::app);
                            errlog << DateTimeToStr(Now()).c_str();
                            char buf[16];
                            for(unsigned long j=0;j<nChar;j++)
                              {
                              sprintf(buf," %x",szRead[j]);
                              errlog << buf;
                              }
                            errlog << "\n";
                            errlog.close();
                            //EndDumpLog
                            if(++ErrorCount > 10) szRead[i] = 0x01;
                            else szRead[i] = 0x04;
                            Sleep(10);
                            WriteFile (hComm, &szRead[i], 1, &nChar, NULL);
                            continue;
                        }
                        if(szRead[i] == 0x02)
                        {
                            start = true;
                            Length = 0;
                        }
                        if(start) Data[Length++] = szRead[i];
                        if(Length >= 1024) szRead[i] = 0x03;
                        if(szRead[i] == 0x03) fin = true;
                    }
                    ClearCommError(hComm, &dwError, &csStat);
                }
                else break;
            }
            if(!fin)
            {
                Sleep(50);
                continue;
            }
            else
            {
                start = false;
                fin = false;
                DataReady = true;
                Suspend();
            }
        }
    }
}
//-------------------------------------------------------------------------//
char __fastcall TPrimF::WinToDos(char c)
{
    if(c >= 'А' && c <= 'п') return (c - 'А' + 128);
    if(c >= 'р' && c <= 'я') return (c - 'р' + 224);
    if(c == 'Ё') return 240;
    if(c == 'ё') return 241;
    return c;
}
//-------------------------------------------------------------------------//
void __fastcall TPrimF::StrWinToDos(char* str)
{
    for(unsigned int i = 0; i < strlen(str); i++)
        str[i] = WinToDos(str[i]);
    return;
}
//---------------------------------------------------------------------------
void __fastcall TPrimF::PrintCopyCheck()
{
    struct
    {
        char stx;       // STX		1
        char code;      // Код сообщения		1
        char psw[5];    // Пароль на связь	S	4
        char bcc[2];       // BCC	B	2
        char etx;       //ETX		1
    } Cmd = {0x02, 0x54, "", "", 0x03};

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
}
//---------------------------------------------------------------------------

 