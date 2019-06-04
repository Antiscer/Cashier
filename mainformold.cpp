//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "mainform.h"
#include "screensaver.h"
#include "cash.h"
#include "goldcrown.h"
#include <Filectrl.hpp>
#include <math.h>
#include "fstream.h"
#include "pilot_nt.h"
#include "billprintc.h"
#include "SelectPayTypeFormUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMainWindow *MainWindow;
HANDLE hStarCom;

// ---------------------------------------------------------------------------
__fastcall TMainWindow::TMainWindow(TComponent* Owner)
    : TForm(Owner)
{
    DecimalSeparator = '.';
    ThousandSeparator = ',';
    NewCode = true;
    NewPrice = false;
    InputQnty = false;
    Delete = false;
    DeleteMode = 0;
    ManualZK = false;
    PayType = NO_PAYMENT;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ScannerAnswer(AnsiString code)
{
    if(Printing) return;
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    char cr = 13;
    if(NewPrice || InputQnty) return;
    for(int i = 1; i <= code.Length(); i++)
    {
        FormKeyPress(this, code[i]);
    }
    FormKeyPress(this, cr);
    ScannerEnter = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::AddToTable(int Row)
{
if(Grid->RowCount >= 100)
  {
  MessageBox (GetActiveWindow(),"����� ������� � ����",
    "��� ���������� ������� ������! ��������� ���", MB_OK);
  }

  if(Row == 0)
    {
    Row = 1;
    Grid->RowCount = 2;
    }

    unsigned hyper pr, qt, st;
    String str;
    pr = MoneyAshyper(Price->Text);
    qt = QuantityAshyper(Qnty->Text);
    st = X(pr,qt);
    Grid->Cells[0][Row] = AnsiString(Row);
    Grid->Cells[1][Row] = Nnum->Text;
    Grid->Cells[2][Row] = Name->Caption;
    GoodName = Name->Caption;
    Grid->Cells[3][Row] = Unit->Caption;
    GoodPrice = MoneyAsString(pr);
    Grid->Cells[4][Row] = GoodPrice;
    Grid->Cells[5][Row] = QuantityAsString(qt);
    Grid->Cells[6][Row] = MoneyAsString(st);
    Grid->Cells[7][Row] = NomNum->Caption;
    Grid->Cells[8][Row] = CasName;
    if(HandInput) Grid->Cells[9][Row]  = "true"; else Grid->Cells[9][Row]  = "false";
    if(SetNum)    Grid->Cells[10][Row] = "true"; else Grid->Cells[10][Row] = "false";
    if(SetPrice)  Grid->Cells[11][Row] = "true"; else Grid->Cells[11][Row] = "false";
    Grid->Cells[12][Row] = NDS;
    ScannerEnter = false;
    Grid->Row = Row;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ArrangeTable()
{
    int bs;
    String str;

    Name->Caption = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";
    Nnum->Text = "";

    if(!Grid->Cells[1][Grid->RowCount-1].IsEmpty())
    {
        for(bs = 1; bs < Grid->RowCount-1; bs++)
            if (
                (Grid->Cells[1][bs] == Grid->Cells[1][Grid->RowCount-1]
                && Grid->Cells[4][bs] == Grid->Cells[4][Grid->RowCount-1])
                ||
                QuantityAshyper(Grid->Cells[5][bs]) == 0
                ) break;
        if(bs < Grid->RowCount-1)
        {
          if(QuantityAshyper(Grid->Cells[5][bs]) == 0)
            {
            for(int ii=0;ii<Grid->ColCount;ii++) Grid->Cells[ii][bs] = Grid->Cells[ii][Grid->RowCount-1];
            Grid->Rows[Grid->RowCount-1]->Clear();
            }
          else
            {
            if(Grid->Cells[9][bs] == "true" || Grid->Cells[9][Grid->RowCount-1] == "true")
              Grid->Cells[9][bs] = "true"; else Grid->Cells[9][bs] = "false";
            if(Grid->Cells[10][bs] == "true" || Grid->Cells[10][Grid->RowCount-1] == "true")
              Grid->Cells[10][bs] = "true"; else Grid->Cells[10][bs] = "false";
            if(Grid->Cells[11][bs] == "true" || Grid->Cells[11][Grid->RowCount-1] == "true")
              Grid->Cells[11][bs] = "true"; else Grid->Cells[11][bs] = "false";
            Grid->Cells[5][bs] = QuantityAsString(
                QuantityAshyper(Grid->Cells[5][Grid->RowCount-1]) + QuantityAshyper(Grid->Cells[5][bs]));
            Grid->Cells[6][bs] = MoneyAsString(
                X(MoneyAshyper(Grid->Cells[4][bs]),QuantityAshyper(Grid->Cells[5][bs])));
            Grid->Row = bs;
            Grid->Rows[Grid->RowCount-1]->Clear();
            }
        }
        else Grid->RowCount++;
    }
    for(int i = 0; i <= Grid->ColCount; i++)
    {
        str = "g" + String(i) + ".txt";
        Grid->Cols[i]->SaveToFile(str);
    }
    CalcTotal();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CalcTotal()
{
    AnsiString TextToShow;

    unsigned hyper tt = 0;
    for(int i = 1; i < Grid->RowCount; i++)
    {
        if(!Grid->Cells[6][i].IsEmpty())
        {
          tt += MoneyAshyper(Grid->Cells[6][i]);
        }
    }
    TotalSum = MoneyAsString(tt);

  if( tt > 20000000)
  {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� �� ����� ������ ��� �� ����� ����� 20 ���.���\n������� ���\n", MB_OK);
  }

    TSum->Text = TotalSum;

//    if(TotalSum.Pos(".00") > 0) TotalSum = TotalSum.SubString(1,TotalSum.Length()-3);
    if(GoodPrice.Pos(".00") > 0) GoodPrice = GoodPrice.SubString(1,GoodPrice.Length()-3);

    TextToShow = GoodName.SubString(1,18-GoodPrice.Length());
    while(TextToShow.Length() < 20-GoodPrice.Length()) TextToShow += " ";
    TextToShow += GoodPrice;
    TextToShow += "�����:";
    while(TextToShow.Length() + TotalSum.Length() < 40) TextToShow += " ";
    TextToShow += TotalSum;
    ShowOnDisplay(TextToShow.c_str());
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::Seek(AnsiString Code)
{
bool ret;
AnsiString S;

while(Code.Length() < 12) Code += " ";

PriceQuery->SQL->Text = "SELECT * FROM Price WHERE ScanCode = '"+Code+"'";
PriceQuery->Active = true;

ret = (PriceQuery->FieldByName("ScanCode")->AsString == Code);

//�������� ���
if(ret)
  {
  S = PriceQuery->FieldByName("NDS")->AsString;
  if( S != "1" && S != "2" && S!="3" )
    {
        MessageBox (GetActiveWindow(),"�������� ����������",
                "�� ��������� ���������� ��� �� ���� ����� � 1�", MB_OK);
    ret = false;
    }
  }

//��������� 0 ����
if(ret)
  if( MoneyAshyper(PriceQuery->FieldByName("Price")->AsString) == 0)
  {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� � ������� �����, ������� ���� ��� ������� ������", MB_OK);
  }

if(ret)
  {
  S = PriceQuery->FieldByName("Name")->AsString;
  Name->Caption = S.Trim();

  Unit->Caption = PriceQuery->FieldByName("Meas")->AsString;
  Price->Text = PriceQuery->FieldByName("Price")->AsString;
  MainWindow->NomNum->Caption = PriceQuery->FieldByName("NumCode")->AsString;
  Qnty->Text = "1";
  if( MainWindow->PriceQuery->FieldByName("CSN")->AsString == "0" ) cansetnum_flag = false;
  else cansetnum_flag = true;

  NDS = MainWindow->PriceQuery->FieldByName("NDS")->AsString;

  if( MainWindow->PriceQuery->FieldByName("Off")->AsString == "1" )
    Name->Caption = "������: "+ Name->Caption;
  }

PriceQuery->Active = false;
return ret;
}

//---------------------------------------------------------------------------
void __fastcall TMainWindow::CodeEnter(AnsiString Code)
{
    if(Seek(Code))
    {
        AddToTable(Grid->RowCount-1);
        PlayWav("nnumok.wav");
        HandInput = !ScannerEnter;
        SetNum = false;
        SetPrice = false;
    }
    else
    {
        PlayWav("oy.wav");
        if(Grid->RowCount > 2) Grid->RowCount--;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ExitClick(TObject *Sender)
{
    Clock->Terminate();
    Star->Terminate();
    MainWindow->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormCreate(TObject *Sender)
{
    int index;
    int FRT, i;
    ScreenSaverStarted = false;
    CasName = "";
    CasNumber = 0;
    AnsiString Path = Application->ExeName.SubString(1, Application->ExeName.LastDelimiter("\\"));
    InputFile = Path + "nnum.txt";
    SQLServer = "CASHTEST";
    OutputDir = Path;
    Psw = "0000";
    Department = "AA";
    ServerDirectory = "\\\\data\\kassa\\";
    SupportEMail = "support@colorlon.ru";
    CommPort = 1;
    CommPortD = 0;
    MstarCom = 1;
    ZKPort = 1;
    FRT = 0;
    Log = false;

    if(FileExists("cashier.ini"))
    {
        TStringList *IniList = new TStringList;
        IniList->LoadFromFile("cashier.ini");
// ����� 1�
        index = IniList->IndexOf("[Sklad]");
        if(++index > 0 && index < IniList->Count)
        {
            Department = IniList->Strings[index];
        }
// Com ���� ������������
        index = IniList->IndexOf("[registrator]");
        if(++index > 0 && index < IniList->Count)
        {
            if(IniList->Strings[index][1] > '0' && IniList->Strings[index][1] < '7')
                MstarCom = IniList->Strings[index].SubString(1,1).ToInt();
            else MstarCom = 1;
        }
// �������� ����� ������������
        index = IniList->IndexOf("[registrator_baud]");
        if(++index > 0 && index < IniList->Count)
        {
          BaudMStar = IniList->Strings[index].SubString(1,6).ToInt();
        }
// ��� ������������
        index = IniList->IndexOf("[registrator_type]");
        if(++index > 0 && index < IniList->Count)

        if(IniList->Strings[index]=="����-08��") FRT = 2;
        else if(IniList->Strings[index]=="�����-����-��-�") FRT = 4;
        else FRT = 0;

// Com ���� �������
        index = IniList->IndexOf("[scanner]");
        if(++index > 0 && index < IniList->Count)
        {
            if(IniList->Strings[index][1] > '0' && IniList->Strings[index][1] < '7')
                CommPort = IniList->Strings[index].SubString(1,1).ToInt();
            else CommPort = 1;
        }
// Com ���� �����
        index = IniList->IndexOf("[�������]");
        if(++index > 0 && index < IniList->Count)
        {
            if(IniList->Strings[index][1] > '0' && IniList->Strings[index][1] < '7')
                CommPortD = IniList->Strings[index].SubString(1,1).ToInt();
            else CommPortD = 1;
        }
// Com ���� ZK
        index = IniList->IndexOf("[goldcrown]");
        if(++index > 0 && index < IniList->Count)
        {
            if(IniList->Strings[index][1] > '0' && IniList->Strings[index][1] < '7')
                ZKPort = IniList->Strings[index].SubString(1,1).ToInt();
            else ZKPort = 1;
        }
// ���������� ����� ������������
        index = IniList->IndexOf("[nnumdir]");
        if(++index > 0 && index < IniList->Count)
            if(DirectoryExists(IniList->Strings[index]))
            {
                InputFile = IniList->Strings[index];
                if(!IniList->Strings[index].IsPathDelimiter(IniList->Strings[index].Length()))
                    InputFile += "\\";
                InputFile += "nnum.txt";
            }
// SQL server
        index = IniList->IndexOf("[SQLServer]");
        if(++index > 0 && index < IniList->Count)
                SQLServer = IniList->Strings[index];
// SQL server
        index = IniList->IndexOf("[SQLServerCentral]");
        if(++index > 0 && index < IniList->Count)
                SQLServerCentral = IniList->Strings[index];
// Log
        index = IniList->IndexOf("[Log]");
        if(++index > 0 && index < IniList->Count) Log = true;

// ���������� ����� ������
        index = IniList->IndexOf("[datadir]");
        if(++index > 0 && index < IniList->Count)
            if(DirectoryExists(IniList->Strings[index]))
            {
                OutputDir = IniList->Strings[index];
                if(!IniList->Strings[index].IsPathDelimiter(IniList->Strings[index].Length()))
                    OutputDir += "\\";
            }
// ����������, ���� ��������� �������� ���� ����� �������� �����
        index = IniList->IndexOf("[server_directory]");
        if(++index > 0 && index < IniList->Count)
        {
            ServerDirectory = IniList->Strings[index];
        }

// ���� ���������� ��������� � ����������
        index = IniList->IndexOf("[support_email]");
        if(++index > 0 && index < IniList->Count)
        {
            SupportEMail = IniList->Strings[index];
        }

// ����� ��������� ����� ����
        index = IniList->IndexOf("[CopyBillHeader]");
        BillHeadCount = 0;
        while(++index > 0 && index < IniList->Count && IniList->Strings[index].c_str()[0] != '[' && BillHeadCount<6)
          BillHead[BillHeadCount++] = IniList->Strings[index];
        delete IniList;
    }

    StatusBar->Panels->Items[2]->Text = Department
        + " ��" + String(MstarCom)
        + " ��" + String(CommPort)
        + " ZK" + String(ZKPort);

    Grid->Cells[0][0] = "�";
    Grid->Cells[1][0] = "���";
    Grid->Cells[2][0] = "������������";
    Grid->Cells[3][0] = "��.���";
    Grid->Cells[4][0] = "����";
    Grid->Cells[5][0] = "����������";
    Grid->Cells[6][0] = "�����";
    if(FileExists("g6.txt"))
    {
        TStringList* Lst = new TStringList;
        Lst->LoadFromFile("g6.txt");
        Grid->RowCount = Lst->Count;
        if(Grid->RowCount < 2) Grid->RowCount = 2;
        Lst->Clear();
        delete Lst;
        for(int i = 0; i <= Grid->ColCount; i++)
        {
            String str = "g" + String(i) + ".txt";
            if(FileExists(str)) Grid->Cols[i]->LoadFromFile(str);
        }
    }

    while
        (
        Grid->Cells[0][Grid->RowCount-1].IsEmpty()
        || Grid->Cells[0][Grid->RowCount-1] == " "
        )
    {
            Grid->Rows[Grid->RowCount-1]->Clear();
            if(Grid->RowCount > 1) Grid->RowCount--;
            else break;
    }
    CalcTotal();

    CardType = NO_CARD;
    Clock = new TClockThread(true);
    Reader = new TSmartCardReader(true);
    CopyRes = new TCopyRes(true);
    Scan = new TScanner(true);
    Scan->Port = CommPort;

    switch(FRT)
      {
      case 0:
      default:
        Psw = "0000";
        break;
      case 2:
        Psw = "AERF";
        break;
      case 4:
        Psw = "\x1e\x0\x0\x0";
        break;
      }
    Star = new TMStarF(true);
    Star->Port = MstarCom;
    Star->Baud = BaudMStar;
    Star->Psw = Psw;
    Star->FRType = FRT;

    Star->Resume();
    Scan->Resume();
    Clock->Resume();
    Reader->Resume();
    CopyRes->Resume();

    Star->GetVersion();
    i = 0;
    while(Star->KKMResult == "00FF")
         {
         GetKKMError();
         if(i++ == 3)
           {
           MessageBox(GetActiveWindow(),"�� �������� �����������, ������ ��������� ����������. ���������� ����� �� ���������.","�� ����������� ����� ������������",MB_OK);
           return;
           }
         else
           {
           MessageBox(GetActiveWindow(),"��������� � �������� �����������, ���������, ���� �� ������ � ������� ���������","�� ����������� ����� ������������",MB_OK);
           Star->GetVersion();
           }
         }

         AnsiString str = "0x";
         str += Star->KKMStatus;
         switch(Star->FRType)
           {
           case 0:
           default:
               if(str.ToInt() & 0x0001) Session = true;
                    else Session = false;
                    break;
                    case 2:
                    if(str.ToInt() & 0x0008) Session = true;
                    else Session = false;
                    break;
                    case 4:
                    if(str.ToInt() == 0x0200) Session = true;
                    else Session = false;
                    break;
                    }

                    Star->StrihWriteTable(1,1,6,"\x00",1);
                    Star->StrihWriteTable(1,1,7,"\x01",1);
                    Star->StrihWriteTable(1,1,20,"\x01",1);
                    Star->StrihWriteTable(1,1,21,"\x01",1);
                    Star->StrihWriteTable(1,1,15,"\x00",1);
                    Star->StrihWriteTable(1,1,17,"\x01",1);
                    Star->StrihWriteTable(6,1,1,"\x00\x00",2);
                    Star->StrihWriteTable(6,1,2,"��� ���",7);
                    Star->StrihWriteTable(6,2,1,"\xe8\x03",2);  //1000
                    Star->StrihWriteTable(6,2,2,"��� 10%",7);
                    Star->StrihWriteTable(6,3,1,"\x08\x07",2);  //1800
                    Star->StrihWriteTable(6,3,2,"��� 18%",7);
                    Star->StrihWriteTable(4,8,1," ",2);
                    Star->StrihWriteTable(4,9,1," ",2);
                    Name->Caption = "KKM " + Star->Serial + " �����";

    CashConnection->ConnectionString = SQLServer;

    CashConnection->Connected = true;

    if(Department.Length() != 9)
      {
      MessageBox (GetActiveWindow(),"����������� ������ �����","�������� �������� ���� [Sklad] � cashier.ini", MB_OK);
      return;
      }

   ArrangeTable();
//   Sverka(true);

   GoodName = "";
   GoodPrice = "";
//   TotalSum = "0.00";
   ShowOnDisplay("");
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::GetKKMError()
{
  AnsiString str, hex, log;
  switch(Star->FRType)
  {
  default:
  case 0:
    hex = "0x" + Star->KKMResult;
    switch (hex.ToInt())
    {
        case 0x0000:
            Name->Caption = "������";
            return;
        case 0xFF00:
            str = "������ ����������� ������������ � ����������.";
            break;
        case 0xFF01:
            str = "������ ������.";
            break;
        case 0xFF02:
            str = "����������� �� ��������.";
            break;
        case 0x0001:
            str = "������ � ���������� ������, ������� ����������.";
            break;
        case 0x0002:
            str = "�� ������� �����.";
            break;
        case 0x0003:
            str = "�������� ������ ������� ������� � ���������� ������.";
            break;
        case 0x0004:
            str = "��������� ����� ���� �������.";
            break;
        case 0x0005:
            str = "�������� ������ ���� �������.";
            break;
        case 0x0006:
            str = "������ ������ �������.";
            break;
        case 0x0007:
            str = "�������� ����.";
            break;
        case 0x0008:
            str = "�������� �����.";
            break;
        case 0x0009:
            str = "���� ������ ��������� ����, ������������������ � ���������� ������.";
            break;
        case 0x000A:
            str = "�������� �������� �������������. �������� �����������.";
            break;
        case 0x000B:
            str = "����������� ������� ��.";
            break;
        case 0x000C:
            str = "�� ������� �����.";
            break;
        case 0x000D:
            str = "����������� ������";
            break;
        case 0x000E:
            str = "������������ �������� ������. ";
            break;
        case 0x000F:
            str = "������������ �������� ������. ";
            break;
        case 0x0010:
            str = "������ ��������� �������.";
            break;
        case 0x0011:
            str = "�������� ������ ���������� ����������.";
            break;
        case 0x0012:
            str = "�������� ������ �� �����.";
            break;
        case 0x0013:
            str = "�������� ������ ���������������.";
            break;
        case 0x0014:
            str = "������� �� ��������������.";
            break;
        case 0x0015:
            str = "�������� ���� ������� ��� ���������.";
            break;
        case 0x0016:
            str = "������ ������ ���������� ������.";
            break;
        case 0x0017:
            str = "������������ ��������.";
            break;
        case 0x0018:
            str = "������������ ���� ������� ����� ������� �����.";
            break;
        case 0x0019:
            str = "�������� ������ �������.";
            break;
        case 0x001A:
            str = "���� ��� ����� ���������� ��������� � ����� ������ �����������.";
            break;
        case 0x001B:
            str = "����������� ������";
            break;
        case 0x001C:
            str = "������ � ������������ ����������.";
            break;
        case 0x001D:
            str = "��� ����� �������.";
            break;
        case 0x001E:
            str = "��������  ����������� �����.";
            break;
        case 0x001F:
            str = "��� ���������� �������.";
            break;
        case 0x0024:
            str = "����� ������ ����.";
            break;
        case 0x0025:
            str = "������ �� �������";
            break;
        case 0x0029:
            str = "������������ ������������ ���������� ���������.";
            break;
        case 0x002A:
            str = "������� ��������� ��� �� ��������� ��������� ��������.";
            break;
        default:
            str = "����������� ������: " + hex;
    }
    break;
  case 2:
    hex = "0x" + Star->KKMResult;
    switch (hex.ToInt()/256)
    {
        case 0x00:
            if(hex.ToInt()==0x0100) str = "������ ������";
            else str = "������";
            return;
        case 0x01:
            str = "�������� ������ ���������.";
            break;
        case 0x02:
            str = "�������� ������ ���� "+hex.SubString(1,4);
            break;
        case 0x03:
            str = "�������� ����/�����";
            break;
        case 0x04:
            str = "�������� ����������� �����.";
            break;
        case 0x05:
            str = "�������� ������ �������� ������";
            break;
        case 0x06:
            str = "��� ������� � ����� �������";
            break;
        case 0x07:
            str = "���������� ������� \"������ ������\"";
            break;
        case 0x08:
            str = "����� ���������� ������, ��� �� 24 ����";
            break;
        case 0x09:
            str = "��������� ������������ ����� ���������� ���� "+hex.SubString(1,2);
            break;
        case 0x0A:
            str = "��������� ������������ ����� ���������";
            break;
        case 0x0B:
            str = "������������ ��������";
            break;
        case 0x0C:
            str = "�������� ���� ��� ��������� "+hex.SubString(1,4);
            break;
        case 0x0D:
            str = "��� ������ ��������� ��������� ��� ������� �� ���������";
            break;
        case 0x0E:
            str = "������������ ��������� ���� ����� ������� ����� "+hex.SubString(1,4);
            break;
        case 0x0F:
            str = "������� ������� ���������";
            break;
        case 0x10:
            str = "������������ ��������� ��������: ";
            switch(hex.ToInt())
               {
               case 0xFF10:
                    str+="������ �� ����";
                    break;
               case 0xFE10:
                    str+="����� ����";
                    break;
               case 0xFD10:
                    str+="�������� �������� �� ���������";
                    break;
               case 0xFC10:
                    str+="�������� � �����";
                    break;
               case 0xFB10:
                    str+="������������ �����";
                    break;
               case 0xFA10:
                    str+="����������� ������� ������/������� ��������� 99,99";
                    break;
               }
            break;
        case 0x11:
            str = "�������� �������� �� �������� ��-�� ���������� ������";
            break;
        case 0x12:
            str = "��� ������� �������� ��� ���������� ��������";
            break;
        case 0x13:
            str = "�������� �������� ��������� ���� �� ������ ��������";
            break;
        case 0x14:
            str = "���������� ��������� ������������ (���� ���������� ������)";
            break;
        case 0x15:
            str = "���������� ������� �����";
            break;
        case 0x16:
            str = "������� ��� ������";
            break;
        case 0x17:
            str = "������������ ������ ��������";
            break;
        case 0x18:
            str = "������� �� ����� � ������";
            break;
        case 0x19:
            str = "������ ������ � �����";
            break;
        case 0x1A:
            str = "���������� �������� ������������";
            break;
        case 0x1B:
            str = "�������� ������ ������� � ��";
            break;
        case 0x1C:
            str = "��� ��� ���������������";
            break;
        case 0x1D:
            str = "��������� ����� ������������";
            break;
        case 0x1E:
            str = "�������� ����� ������";
            break;
        case 0x1F:
            str = "�������� G-����";
            break;
        case 0x20:
            str = "�������� ����� ���� ������";
            break;
        case 0x21:
            str = "����������� �� �������� (������� ������)";
            break;
        case 0x22:
            str = "������ ������";
            break;
        case 0x23:
            str = "�������� ��������� ���";
            break;
        case 0x24:
            str = "������� ����� �������� � ���������";
            break;
        case 0x25:
            str = "���������� ������� �����";
            break;
        case 0x27:
            str = "�������� ����� ���� �������";
            break;
        case 0x28:
            str = "�������� ��������� ��������";
            break;
        case 0x29:
            str = "����� ��� �������";
            break;
        case 0x2B:
            str = "�������� ����";
            break;
        case 0x2C:
            str = "��� ����� ��� ���������� ������/���������";
            break;
        case 0x2D:
            str = "������ ������/��������� ��� ����������";
            break;
        case 0x2E:
            str = "���������� ������� �����: �������� ����������";
            break;
        case 0x2F:
            str = "������ ������/��������� �� ������";
            break;
        case 0x30:
            str = "���������� ������ �� �������� "+hex.SubString(1,2);
            break;
        case 0x31:
            str = "���� ��������� ������ � �� �����, ��� ���� ���� ��������";
            break;
        case 0x32:
            str = "���������� ������������� ��";
            break;
        case 0x33:
            str = "��������� ��� ��";
            break;
        case 0x34:
            str = "������������ ��������� ������ �� ������";
            break;
        case 0x35:
            str = "������������ ����� �� ����";
            break;
        case 0x36:
            str = "����������� ������� ����";
            break;
        case 0x37:
            str = "�������� ��������� ����";
            break;
        case 0x38:
            str = "������� ������ �� ����";
            break;
        case 0x39:
            str = "������� �������� � ����";
            break;
        case 0x3A:
            str = "�������� ����������� ����� ������ �� ����";
            break;
        case 0x3B:
            str = "��������� �������� ����";
            break;
        case 0x3C:
            str = "��� ���������� ����� � ����";
            break;
        case 0x3D:
            str = "�������� ����������� ����� � ������� ����";
            break;
        case 0x3E:
            str = "���������� ���� �� ���������";
            break;
        case 0x3F:
            str = "������ � ���� �����������";
            break;
        case 0x40:
            str = "������ � ���� �� ����������������";
            break;
        case 0x41:
            str = "��������� ��������� ���";
            break;
        case 0x42:
            str = "�������� ���� � ����� � ������� ����";
            break;
        case 0x43:
            str = "����������� ����� ������������ ����";
            break;
        case 0x44:
            str = "������������ ����";
            break;
        case 0x45:
            str = "����� ����������� ���������";
            break;
        case 0x50:
            str = "������������ ��������� ����������� �����";
            break;
        case 0x51:
            str = "��������� ���������� ����������� �����";
            break;
        default:
            str = "����������� ������: " + hex;
    }
    break;
    case 4:
      hex = "0x" + Star->KKMResult;
      switch (hex.ToInt())
        {
        case 0x01:
            str = "���������� ���������� ��1, ��2 ��� ����";
            break;
        case 0x02:
            str = "�� ����������� �� 1";
            break;
        case 0x03:
            str = "�� ����������� �� 2";
            break;
        case 0x04:
            str = "�� ������������ ��������� � ������� ��������� � ��";
            break;
        case 0x05:
            str = "�� ��� ����������� ������";
            break;
        case 0x06:
            str = "�� �� � ������ ������ ������";
            break;
        case 0x07:
            str = "�� ������������ ��������� � ������� ��� ������ ���������� ��";
            break;
        case 0x08:
            str = "�� ������� �� �������������� � ������ ���������� ��";
            break;
        case 0x09:
            str = "�� ������������ ����� �������";
            break;
        case 0x0A:
            str = "�� ������ ������ �� BCD";
            break;
        case 0x0B:
            str = "�� ���������� ������ ������ �� ��� ������ �����";
            break;
        case 0x11:
            str = "�� �� ������� ��������";
            break;
        case 0x12:
            str = "�� ��������� ����� ��� ������";
            break;
        case 0x13:
            str = "�� ������� ���� ������ ���� ��������� ������ � ��";
            break;
        case 0x14:
            str = "�� ������� ������� ������ �� �����������";
            break;
        case 0x15:
            str = "�� ����� ��� �������";
            break;
        case 0x16:
            str = "�� ����� �� �������";
            break;
        case 0x17:
            str = "�� ����� ������ ����� ������ ������ ��������� �����";
            break;
        case 0x18:
            str = "�� ���� ������ ����� ������ ���� ��������� �����";
            break;
        case 0x19:
            str = "�� ��� ������ � ��";
            break;
        case 0x1A:
            str = "�� ������� ��������������� � �� �����������";
            break;
        case 0x1B:
            str = "�� ��������� ����� �� ������";
            break;
        case 0x1C:
            str = "�� � �������� ��������� ���� ������������ ������";
            break;
        case 0x1D:
            str = "�� ���������� ��������� ������ ������� ������";
            break;
        case 0x1E:
            str = "�� ������� ��������������� �� �����������";
            break;
        case 0x1F:
            str = "�� ����������� ������ ���������";
            break;
        case 0x20:
            str = "�� ������������ ��������� �������� ��� ����������";
            break;
        case 0x21:
            str = "�� ���������� ����� ������ ����������� ��������� ��������";
            break;
        case 0x22:
            str = "�� �������� ����";
            break;
        case 0x23:
            str = "�� ��� ������ �����������";
            break;
        case 0x24:
            str = "�� ������� ����������� �����������";
            break;
        case 0x25:
            str = "�� ��� ����������� � ������������� �������";
            break;
        case 0x26:
            str = "�� �������� �������� ����� ������ ����� ����";
            break;
        case 0x2B:
            str = "���������� �������� ���������� �������";
            break;
        case 0x2C:
            str = "�� ��������� ����� ( ������� ���������� )";
            break;
        case 0x2D:
            str = "�� ����� ���� �� ������ ������ ����� ������";
            break;
        case 0x2E:
            str = "�� � �� ��� ����� ��� �������";
            break;
        case 0x30:
            str = "�� �� ������������ , ���� ����� ������ ���������� ����������";
            break;
        case 0x32:
            str = "�� ��������� ���������� ������ �������";
            break;
        case 0x33:
            str = "�� ������������ ��������� � �������";
            break;
        case 0x34:
            str = "�� ��� ������";
            break;
        case 0x35:
            str = "�� ������������ �������� ��� ������ ����������";
            break;
        case 0x36:
            str = "�� ������������ ��������� � ������� ��� ������ ���������� ��";
            break;
        case 0x37:
            str = "�� ������� �� �������������� � ������ ���������� ��";
            break;
        case 0x38:
            str = "�� ������ � ���";
            break;
        case 0x39:
            str = "�� ���������� ������ �� ��";
            break;
        case 0x3A:
            str = "�� ������������ ���������� �� ��������� � �����";
            break;
        case 0x3B:
            str = "�� ������������ ���������� � �����";
            break;
        case 0x3C:
            str = "�� ���� : �������� ��������������� �����";
            break;
        case 0x3D:
            str = "�� ����� �� ������� - �������� ����������";
            break;
        case 0x3E:
            str = "�� ������������ ���������� �� ������� � �����";
            break;
        case 0x3F:
            str = "�� ������������ ���������� �� ������� � �����";
            break;
        case 0x40:
            str = "�� ������������ ��������� ������";
            break;
        case 0x41:
            str = "�� ������������ ��������� ������ ���������";
            break;
        case 0x42:
            str = "�� ������������ ��������� ������ ����� 2";
            break;
        case 0x43:
            str = "�� ������������ ��������� ������ ����� 3";
            break;
        case 0x44:
            str = "�� ������������ ��������� ������ ����� 4";
            break;
        case 0x45:
            str = "�� C ���� ����� ������ ������ ����� ����";
            break;
        case 0x46:
            str = "�� �� ������� ���������� � �����";
            break;
        case 0x47:
            str = "�� ������������ ���������� �� ������� � �����";
            break;
        case 0x48:
            str = "�� ������������ ����� ����";
            break;
        case 0x49:
            str = "�� �������� ���������� � �������� ���� ������� ����";
            break;
        case 0x4A:
            str = "�� ������ ��� ? �������� ����������";
            break;
        case 0x4B:
            str = "�� ����� ���� ����������";
            break;
        case 0x4C:
            str = "�� ������������ ���������� �� ������� ������� � �����";
            break;
        case 0x4D:
            str = "�� �������� ����������� ������� ����� ������ ����� ����";
            break;
        case 0x4E:
            str = "�� ����� ��������� 24 ����";
            break;
        case 0x4F:
            str = "�� �������� ������";
            break;
        case 0x50:
            str = "�� ���� ������ ���������� �������";
            break;
        case 0x51:
            str = "�� ������������ ���������� ��������� � �����";
            break;
        case 0x52:
            str = "�� ������������ ���������� �� ���� ������ 2 � �����";
            break;
        case 0x53:
            str = "�� ������������ ���������� �� ���� ������ 3 � �����";
            break;
        case 0x54:
            str = "�� ������������ ���������� �� ���� ������ 4 � �����";
            break;
        case 0x55:
            str = "�� ��� ������ ? �������� ����������";
            break;
        case 0x56:
            str = "�� ��� ��������� ��� �������";
            break;
        case 0x57:
            str = "�� ���� : ���������� �������� ���� �� ��������� � ��";
            break;
        case 0x58:
            str = "�� �������� ������� ����������� ������";
            Star->ResumePrint();
            break;
        case 0x59:
            str = "�� �������� ������ ������ ����������";
            break;
        case 0x5A:
            str = "�� ������ ��������� ���������� � ����";
            break;
        case 0x5B:
            str = "�� ������������ ��������� ��������";
            break;
        case 0x5C:
            str = "�� �������� ���������� 24";
            break;
        case 0x5D:
            str = "�� ������� �� ����������";
            break;
        case 0x5E:
            str = "�� ������������ ��������";
            break;
        case 0x5F:
            str = "�� ������������� ���� ����";
            break;
        case 0x60:
            str = "�� ������������ ��� ���������";
            break;
        case 0x61:
            str = "�� ������������ ��������� ����";
            break;
        case 0x62:
            str = "�� ������������ ��������� ����������";
            break;
        case 0x63:
            str = "�� ������������ ��������� ������";
            break;
        case 0x64:
            str = "�� �� �����������";
            break;
        case 0x65:
            str = "�� �� ������� ����� � ������";
            break;
        case 0x66:
            str = "�� ������������ ����� � ������";
            break;
        case 0x67:
            str = "�� ������ ����� � ��";
            break;
        case 0x68:
            str = "�� �� ������� ����� �� ������� �������";
            break;
        case 0x69:
            str = "�� ������������ ����� �� ������� �������";
            break;
        case 0x6A:
            str = "�� ������ ������� � ������ ������ �� I2C";
            break;
        case 0x6B:
            str = "�� ��� ������� �����";
            break;
        case 0x6C:
            str = "�� ��� ����������� �����";
            break;
        case 0x6D:
            str = "�� �� ������� ����� �� ������";
            break;
        case 0x6E:
            str = "�� ������������ ����� �� ������";
            break;
        case 0x6F:
            str = "�� ������������ �� ������� � �����";
            break;
        case 0x70:
            str = "�� ������������ ��";
            break;
        case 0x71:
            str = "�� ������ ���������";
            break;
        case 0x72:
            str = "�� ������� �� �������������� � ������ ���������";
            break;
        case 0x73:
            str = "�� ������� �� �������������� � ������ ������";
            break;
        case 0x74:
            str = "�� ������ ���";
            break;
        case 0x75:
            str = "�� ������ �������";
            break;
        case 0x76:
            str = "�� ������ ��������: ��� ��������� � ��������������";
            break;
        case 0x77:
            str = "�� ������ �������� : ��� ������� � ��������";
            break;
        case 0x78:
            str = "�� ������ ��";
            break;
        case 0x79:
            str = "�� ������ ��";
            break;
        case 0x7A:
            str = "�� ���� �� �������������";
            break;
        case 0x7B:
            str = "�� ������ ������������";
            break;
        case 0x7C:
            str = "�� �� ��������� ����";
            break;
        case 0x7D:
            str = "�� �������� ������ ����";
            break;
        case 0x7E:
            str = "�� �������� �������� � ���� �����";
            break;
        case 0x7F:
            str = "�� ������������ ��������� ����� ����";
            break;
        case 0x80:
            str = "�� ������ ����� � ��";
            break;
        case 0x81:
            str = "�� ������ ����� � ��";
            break;
        case 0x82:
            str = "�� ������ ����� � ��";
            break;
        case 0x83:
            str = "�� ������ ����� � ��";
            break;
        case 0x84:
            str = "�� ������������ ����������";
            break;
        case 0x85:
            str = "�� ������������ �� �������� � �����";
            break;
        case 0x86:
            str = "�� ������������ �� �������� � �����";
            break;
        case 0x87:
            str = "�� ������������ �� ��������� ������ � �����";
            break;
        case 0x88:
            str = "�� ������������ �� ��������� ������� � �����";
            break;
        case 0x89:
            str = "�� ������������ �� �������� � �����";
            break;
        case 0x8A:
            str = "�� ������������ �� ��������� � ����";
            break;
        case 0x8B:
            str = "�� ������������ �� ������� � ����";
            break;
        case 0x8C:
            str = "�� ������������� ���� �������� � ����";
            break;
        case 0x8D:
            str = "�� ������������� ���� ������ � ����";
            break;
        case 0x8E:
            str = "�� ������� ���� ����";
            break;
        case 0x8F:
            str = "�� ����� �� ���������������";
            break;
        case 0x90:
            str = "�� ���� ��������� ������ , ������������� � ����������";
            break;
        case 0x91:
            str = "�� ����� �� ������� ���� ������ ��� ������ ���������� ������";
            break;
        case 0x92:
            str = "�� ��������� �����";
            break;
        case 0x93:
            str = "�� �������������� ��� ������ �������";
            break;
        case 0x94:
            str = "�� �������� ����� �������� � ����";
            break;
        case 0xA0:
            str = "�� ������ ����� � ����";
            break;
        case 0xA1:
            str = "�� ���� �����������";
            break;
        case 0xA2:
            str = "���� ���� : ������������ ������ ��� �������� �������";
            break;
        case 0xA3:
            str = "���� ������������ ��������� ����";
            break;
        case 0xA4:
            str = "���� ������ ����";
            break;
        case 0xA5:
            str = "���� ������ �� � ������� ����";
            break;
        case 0xA6:
            str = "���� �������� ��������� ������ ����";
            break;
        case 0xA7:
            str = "���� ���� �����������";
            break;
        case 0xA8:
            str = "���� ���� : �������� ���� � �����";
            break;
        case 0xA9:
            str = "���� ���� : ��� ����������� ������";
            break;
        case 0xAA:
            str = "���� ������������ ���� ( ������������� ���� ��������� )";
            break;
        case 0xB0:
            str = "�� ���� : ������������ � ��������� ����������";
            break;
        case 0xB1:
            str = "�� ���� : ������������ � ��������� �����";
            break;
        case 0xB2:
            str = "�� ���� : ��� ��������������";
            break;
        case 0xC0:
            str = "�� �������� ���� � ������� ( ���� � ����� )";
            break;
        case 0xC1:
            str = "�� ���� : �������� ����� � �������� �������� ������";
            break;
        case 0xC2:
            str = "�� ���������� ���������� � ����� �������";
            break;
        case 0xC3:
            str = "�� ������������ ������ ���� � ����";
            Star->Print("\n�� ������������ ������ ���� � ����\n\n",true);
            Star->XReport();
            break;
        case 0xC4:
            str = "�� ������������ ������� ����";
            break;
        case 0xC5:
            str = "�� ����� ����������� ��������� ����";
            break;
        case 0xC6:
            str = "�� ���������� �������� �����������";
            break;
        case 0xC7:
            str = "�� ���� �� ������������� � ������ ������";
            break;
        case 0xFD:
            str = "����������� �� ����� ������� ���";
            break;
        case 0xFE:
            str = "����������� �� ��������� �������";
            break;
        case 0xFF:
            str = "����������� �� ��������";
            break;
        default:
            str = "����������� ������";
            break;
        }
    break;
  }

    Name->Caption = str;
    Name->Repaint();

    log = "\n\n";
    log += str;
    log += "\n��������� ��������: ";
    log += Star->KKMResult;
    log += "\n��������� ���: ";
    log += Star->KKMStatus;
    log += "\n��������� ��������: ";
    log += Star->PrinterStatus;

    PlayWav("oy.wav");

    Star->DeleteDoc();
    Star->CRLF();

    LPSTR aszMsg[1];
    str = log;
    str += "\n";
    str += CasName;
    str += " (";
    str += String(CasNumber);
    str += ")";
    aszMsg[0] = str.c_str();
    HANDLE hEventLog = RegisterEventSource(NULL, MainWindow->Caption.c_str());
    ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, 0, hex.ToInt(), NULL,
        1, 0, (LPCTSTR *) aszMsg, NULL);
    DeregisterEventSource(hEventLog);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::KeyPressDelete(char Key)
{
    String str;
/*    unsigned hyper pr = 0; */
    if(Key == 27)
    {
        DeleteMode = 0;
        Delete = false;
        Name->Caption = "";
        Unit->Caption = "";
        Price->Text = "";
        Qnty->Text = "";
        Nnum->Text = "";
        return;
    }
    switch(DeleteMode)
    {
        case 1:
            str = Price->Text;
            if(Key == 13 && str.Length() > 0)
            {
                /* pr = MoneyAshyper(str);
                if(pr > 0)
                {  */
                    DeleteMode = 2;
                    //Qnty->Text = "";
                    PlayWav("delnnum.au");
                /* }
                else PlayWav("oy.wav"); */
                return;
            }
            if(isdigit(Key) || Key == '.') str += Key;
            if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
            Price->Text = str;
            return;
        case 0:
            str = Nnum->Text;
            if(Key == 13 && str.Length() > 0)
            {
                if(Seek(Nnum->Text))
                {
                    PlayWav("delnnum.au");
                    if(CardType == MANAGER_CARD)
                    {
                        DeleteMode = 1;
                        //Price->Text = "";
                    }
                    else
                    {
                        DeleteMode = 2;
                        //Qnty->Text = "";
                    }
                }
                else
                {
                    KeyPressDelete(27);
                    PlayWav("oy.wav");
                }
                return;
            }
            if(isdigit(Key)) str += Key;
            if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
            Nnum->Text = str;
            return;
        case 2:
            str = Qnty->Text;
            if(Key == 13 && str.Length() > 0)
            {
                /* pr = MoneyAshyper(str);
                if(pr >= 0)
                {  */
                    for(int i = 1; i < Grid->RowCount; i++)
                    {
                        if
                            (
                            Nnum->Text == Grid->Cells[1][i]
                            && MoneyAshyper(Price->Text) == MoneyAshyper(Grid->Cells[4][i])
                            && QuantityAshyper(Qnty->Text) <= QuantityAshyper(Grid->Cells[5][i])
                            )
                            {
                            Grid->Cells[5][i] = QuantityAsString(QuantityAshyper(Grid->Cells[5][i])
                                -  QuantityAshyper(Qnty->Text));
                            Grid->Cells[6][i] = MoneyAsString( X(MoneyAshyper(Grid->Cells[4][i]),QuantityAshyper(Grid->Cells[5][i])));
                            if(MoneyAshyper(Grid->Cells[6][i]) <= 0)
                            {
                                for(int k = i; k < Grid->RowCount-1; k++)
                                    for(int j = 1; j <= Grid->ColCount; j++)
                                        Grid->Cells[j][k] = Grid->Cells[j][k+1];
                                Grid->Rows[Grid->RowCount-1]->Clear();
                                if(Grid->RowCount > 2) Grid->RowCount--;
                            }
                            CalcTotal();
                            PlayWav("deleted.au");

                            ofstream file;
                            TDateTime dtPresent = Now();
                            AnsiString FileName = OutputDir + "_deleted.txt";
                            file.open(FileName.c_str(), ios::app);
                            file << Star->Serial.c_str()
                            << "\t" << CasName.c_str() << "\t" << String(CasNumber).c_str()
                            << "\t"
                            << dtPresent.DateTimeString().c_str() << "\t"
                            << Nnum->Text.c_str() << "\t"
                            << Name->Caption.c_str() << "\t"
                            << Unit->Caption.c_str() << "\t"
                            << Price->Text.c_str() << "\t"
                            << Qnty->Text.c_str() << "\t"
                            << MoneyAsString(X(MoneyAshyper(Price->Text),QuantityAshyper(Qnty->Text))).c_str()<< endl;
                            file.close();
                            SendDeletedLine();

                            KeyPressDelete(27);
                            Name->Caption = "�������";
                            for(int i = 0; i <= Grid->ColCount; i++)
                            {
                                str = "g" + String(i) + ".txt";
                                Grid->Cols[i]->SaveToFile(str);
                            }
                            return;
                        }
                    }
                    PlayWav("oy.wav");
                    KeyPressDelete(27);
                    Name->Caption = "�������� ������ ��� ��������";
                    return;
                /*}*/
            }
            if(isdigit(Key) || Key == '.') str += Key;
            if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
            Qnty->Text = str;
            return;
        default: KeyPressDelete(27);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormKeyPress(TObject *Sender, char &Key)
{
    if(Printing) return;
    String str;
    unsigned hyper pr = 0;
    if(Delete && !NewPrice && !InputQnty & NewCode)
    {
        KeyPressDelete(Key);
        return;
    }
    if(!Session)
    {
        PlayWav("oy.wav");
        Name->Caption = "�� ������� �����";
        return;
    }
    if(NewPrice)
    {
        str = Price->Text;
        if(Key == 27)
        {
            NewCode = true;
            NewPrice = false;
            InputQnty = false;
            Name->Caption = "";
            Unit->Caption = "";
            Price->Text = "";
            Qnty->Text = "";
            Nnum->Text = "";
        }
        if(Key == 13 && str.Length() > 0)
        {
            pr = MoneyAshyper(str);
            if(pr > 0)
            {
                NewPrice = false;
                PlayWav("nnumok.wav");
                AddToTable(Grid->RowCount-1);
                CalcTotal();
            }
            else PlayWav("oy.wav");
            return;
        }
        if(isdigit(Key) || Key == '.') str += Key;
        if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
        Price->Text = str;
        return;
    }
    if(!InputQnty)
    {
        if(!NewCode) str = Nnum->Text;
        else str = "";
        if(Key == 27)
        {
            NewCode = true;
            NewPrice = false;
            InputQnty = false;
            Name->Caption = "";
            Unit->Caption = "";
            Price->Text = "";
            Qnty->Text = "";
            Nnum->Text = "";
        }
        if(Key == 13 && str.Length() > 0)
        {
            CodeEnter(str);
            NewCode = true;
            CalcTotal();
            return;
        }
        if(isdigit(Key))
        {
            if(NewCode)
            {
                NewCode = false;
                Unit->Caption = "";
                Price->Text = "";
                Qnty->Text = "";
                Name->Caption = "";
                TSum->Text = "0.00";
                Recived->Text = "0.00";
                Change->Text = "0.00";
                ArrangeTable();
            }
            str += Key;
        }
        if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
        Nnum->Text = str;
    }
    else
    {
        str = Qnty->Text;
        if(Key == 27)
        {
            NewCode = true;
            NewPrice = false;
            InputQnty = false;
            Name->Caption = "";
            Unit->Caption = "";
            Price->Text = "";
            Qnty->Text = "";
            Nnum->Text = "";
        }
        if(Key == 13 && str.Length() > 0)
        {
            pr = MoneyAshyper(str);
            if(pr > 0)
            {
                InputQnty = false;
                PlayWav("nnumok.wav");
                AddToTable(Grid->RowCount-1);
                CalcTotal();
            }
            else PlayWav("oy.wav");
            return;
        }
        if(isdigit(Key) || Key == '.') str += Key;
        if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
        Qnty->Text = str;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormResize(TObject *Sender)
{
    Grid->ColWidths[0] = 20;
    Grid->ColWidths[2] = ClientWidth - Grid->ColWidths[1]*5 - 30;
    Grid->Height = TSum->Top - Grid->Top - 20;
    StatusBar->Panels->Items[0]->Width =
        ClientWidth
        - StatusBar->Panels->Items[1]->Width
        - StatusBar->Panels->Items[2]->Width
        - StatusBar->Panels->Items[3]->Width
        - StatusBar->Panels->Items[4]->Width ;
    Name->Width = ClientWidth - Name->Left*2;

}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormShow(TObject *Sender)
{
    Grid->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::PlayWav(AnsiString FileName)
{
    Player->FileName = FileName;
    Player->Open();
    try
    {
        Player->Wait = true;
        Player->Play();
    }
    __finally
    {
        Player->Close();
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ReaderAnswer(hyper result)
{
    String str;
    int i;

    Cancel->Enabled = false;
    if(result)
    {
        str = String(Reader->Data);
        PlayWav("cardout.wav");
        CardType = NO_CARD;
        if(ScreenSaverStarted)
          {
          Saver->Text = CasName + " ����(�) "+DateTimeToStr(Now());
          Saver->Show();
          }
    }
    else if(Reader->Data[0] == 'K')
    {
        CasName = String(&Reader->Data[5]);
        CasNumber = AnsiString(&Reader->Data[1]).ToInt();
        str = "����������� ������ " + String(&Reader->Data[5]);
        PlayWav("cardin.wav");
        CardType = KASSIR_CARD;
        if(ScreenSaverStarted) Saver->Close();
    }
    else if(Reader->Data[0] == 'M')
    {
        CasName = String(&Reader->Data[5]);
        CasNumber = AnsiString(&Reader->Data[1]).ToInt();
        str = "����������� �������� " + String(&Reader->Data[5]);
        PlayWav("cardin.wav");
        CardType = MANAGER_CARD;
        Cancel->Enabled = true;
        if(ScreenSaverStarted) Saver->Close();
    }
    else
    {
        str = "�������� ������";
        PlayWav("oy.wav");
        CardType = ILLEGAL_CARD;
        if(ScreenSaverStarted)
        {
          Saver->Text = "������ ������ �����-����: " + String(Reader->Data);
          Saver->Show();
        }
    }

    for(i=0;i<CasName.Length() && (unsigned char)CasName.c_str()[i]>= (unsigned char)0x20 ; i++);
    if((i < CasName.Length() && CasName.Length() > 0)||(CasName.Length() > 32))
      {
        CasName = "";
        str = "�������� ������";
        PlayWav("oy.wav");
        CardType = ILLEGAL_CARD;
        if(ScreenSaverStarted)
        {
          Saver->Text = "������ ������, �������� �������� ��������";
          Saver->Show();
        }
      }

        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"Cardreader  "<<CasName.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */

    StatusBar->Panels->Items[0]->Text = str;

    if(CardType == MANAGER_CARD || CardType == KASSIR_CARD) ShowOnDisplay("  ����� ����������!");
    else ShowOnDisplay("       �����            �� ��������");
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::MeasureClick(TObject *Sender)
{
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    if(!cansetnum_flag && CardType == KASSIR_CARD) return;
    if(Nnum->Text.IsEmpty()) return;
    InputQnty = true;
    SetNum = true;
    Qnty->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::PriceChangeClick(TObject *Sender)
{
    if(CardType != MANAGER_CARD) return;
    if(Nnum->Text.IsEmpty()) return;
    NewPrice = true;
    SetPrice = true;
    Price->Text = "";
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::DeleteAllClick(TObject *Sender)
{
    if(CardType != MANAGER_CARD) return;
    if
    (
        MessageBox (GetActiveWindow(), "������� ���� ������?",
            "�������������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    ) return;

    ArrangeTable();
    if(Grid->RowCount > 2) Grid->RowCount--;
    NewCode = true;
    NewPrice = false;
    InputQnty = false;
    Delete = false;
    DeleteMode = 0;

    ofstream file;
    TDateTime dtPresent = Now();
    AnsiString FileName = OutputDir + "_deleted.txt";
    file.open(FileName.c_str(), ios::app);
    for(int i = 1; i < Grid->RowCount; i++)
    {
        file << Star->Serial.c_str()
            << "\t" << CasName.c_str() << "\t" << String(CasNumber).c_str()
            << "\t"
            << dtPresent.DateTimeString().c_str() << "\t"
            << Grid->Cells[1][i].c_str() << "\t"
            << Grid->Cells[2][i].c_str() << "\t"
            << Grid->Cells[3][i].c_str() << "\t"
            << Grid->Cells[4][i].c_str() << "\t"
            << Grid->Cells[5][i].c_str() << "\t"
            << Grid->Cells[6][i].c_str() << endl;
    }
    file.close();
    SendDeletedBill();

    for(int i = 1; i < Grid->RowCount; i++)
        Grid->Rows[i]->Clear();
    Grid->RowCount = 2;
    Nnum->Text = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";
    Name->Caption = "";
    CalcTotal();
    for(int i = 0; i <= Grid->ColCount; i++)
    {
        String str = "g" + String(i) + ".txt";
        DeleteFile(str);
    }
    PlayWav("deleted.au");
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::DeleteNnumClick(TObject *Sender)
{
    if(CardType != MANAGER_CARD) return;
    if(!Delete)
    {
        Delete = true;
        NewCode = true;
        NewPrice = false;
        InputQnty = false;
        Name->Caption = "�������� �������";
        Nnum->Text = "";
        Unit->Caption = "";
        Price->Text = "";
        Qnty->Text = "";
        ArrangeTable();
        if(Grid->RowCount > 2) Grid->RowCount--;
        PlayWav("warning.wav");
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::X_ReportClick(TObject *Sender)
{
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;

        Star->Print(BillHead[0].c_str(),true);
        Star->Print(BillHead[1].c_str(),true);
        Star->Print(" ",true);

        Star->XReport();
        if(Star->KKMResult != "0000") GetKKMError();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::S_CloseClick(TObject *Sender)
{
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    PlayWav("warning.wav");
    if
    (
        MessageBox (GetActiveWindow(), "������� �����?",
            "�������������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    ) return;

      Star->Print(BillHead[0].c_str(),true);
      Star->Print(BillHead[1].c_str(),true);
      Star->Print(" ",true);

      Star->ZReport();
      if(Star->KKMResult == "0000")
      {
          Session = false;
          Name->Caption = "����� �������";
          PlayWav("s_close.au");
          SendCloseSession();
      }
      else GetKKMError();
    SendFiles();
    WriteCloseRetail();
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::S_OpenClick(TObject *Sender)
{
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    PlayWav("warning.wav");
    if
    (
        MessageBox (GetActiveWindow(), "������� �����?",
            "�������������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    ) return;

    Star->OpenSession("����� " + Star->Serial, CasNumber);
    if(Star->KKMResult == "0000")
    {
        Session = true;
        Name->Caption = "����� �������";
        PlayWav("s_open.au");
        SendOpenSession();
        X_ReportClick(this);
        if(Star->KKMResult == "0000") Name->Caption = "����� �������";
    }
    else GetKKMError();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::BoxClick(TObject *Sender)
{
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    Star->OpenBox();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CashClick(TObject *Sender)
{
    AnsiString str,TextToShow;
    int RC;

    Printing = true;
    PayType = NAL_PAYMENT;

    if(CheckSale()) Sale = true;
    else Sale = false;
    if(Sale)
    {
        PlayWav("warning.wav");
        if
        (
            MessageBox (GetActiveWindow(), "��������! ���������� �����! ����������?",
                "������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {Printing = false; return;}
    }

    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD)
        {
        Printing = false;
        return;
        }

    PlayWav("cash.au");
    ArrangeTable();
    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty()) Grid->RowCount--;

    if(CheckSale0())
    {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� � ������� �����, ������� ���� ��� ������� ������", MB_OK);
      Printing = false; return;
    }

    if(CashForm->ShowModal() != mrOk) {Printing = false; return;}
    Change->Text = MoneyAsString(MoneyAshyper(Recived->Text) - MoneyAshyper(TSum->Text));
    Recived->Repaint();
    Change->Repaint();

    TextToShow = "��������:";
    while( TextToShow.Length()+Recived->Text.Length() < 20) TextToShow += " ";
    TextToShow += Recived->Text;
    TextToShow += "�����:";
    while( TextToShow.Length() + Change->Text.Length() < 40) TextToShow += " ";
    TextToShow += Change->Text;
    ShowOnDisplay(TextToShow.c_str());

    if(Star->Serial == "") //���� �������� ����� �� ���������, �������� �� �����!
      {
      Star->GetVersion();
      if(Star->Serial == "")
        {
        Printing = false;
        return;
        }
      }

    Star->OpenBox();
    Present();

for(RC=0;RC<5;RC++)
    {

    Star->Print(BillHead[0].c_str(),true);
    Star->Print(BillHead[1].c_str(),true);
    Star->Print(" ",true);

    Star->BillOpen(Sale, CasName, CasNumber);
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        if(Star->KKMResult != "0000") return;
        else continue;
    }

    if(Star->FRType)
      {
      for(int i = 1; i < Grid->RowCount; i++)
       {
        if(
        (Grid->Cells[1][i]=="") ||
        (Grid->Cells[2][i]=="") ||
        (Grid->Cells[3][i]=="") ||
        (Grid->Cells[4][i]=="") ||
        (Grid->Cells[5][i]=="") ||
        (Grid->Cells[7][i]=="") ||
        (Grid->Cells[12][i]=="")
        )
        {
        Name->Caption = "�������� ��� - ����� ������������� ������";
        Printing = false;
        PlayWav("oy.wav");
        return;
        }

        strncpy(&cMeas[26*(i-1)], Grid->Cells[3][i].c_str(), 25);
        cMeas[26*(i-1)+25] = 0;
        strncpy(&cCode[21*(i-1)], Grid->Cells[1][i].c_str(), 20);
        cCode[21*(i-1)+20] = 0;
        strncpy(&cName[131*(i-1)], Grid->Cells[2][i].c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
       Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);
      }
    else
      {
      for(int i = 1; i < Grid->RowCount; i++)
       {
        str = Grid->Cells[1][i];
        str += " (";
        str += Grid->Cells[3][i];
        str += ") ";
        str += Grid->Cells[2][i];
        strncpy(&cName[131*(i-1)], str.c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
       Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);
      }

    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        if(Star->KKMResult != "0000") return;
        else continue;
    }
    Star->BillClose(Recived->Text, CasNumber);
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Star->GetVersion();
        Sverka(true);
        if(Star->KKMResult != "0000") return;
        else continue;
    }
break;
}
if(RC == 5)
  {
  Printing = false;
  PlayWav("oy.wav");
  return;
  }

  PlayWav("cash.au");

    Name->Caption = "";
    Nnum->Text = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";

    WriteRetail(PayType);

    Printing = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SendMessage()
{
    AnsiString dt = "��� ";
    dt += DateTimeToStr(Now());
    dt += ". ����� ";
    dt += Star->Serial;
    dt += ". ����� ";
    dt += TSum->Text;
    dt += ". ������ ";
    dt += CasName;
    dt += "\n";
    for(int i = 1; i < Grid->RowCount; i++)
    {
        dt += Grid->Cells[0][i];
        dt += " ";
        dt += Grid->Cells[1][i];
        dt += " ";
        dt += Grid->Cells[2][i];
        dt += " ";
        dt += Grid->Cells[5][i];
        dt += " ";
        dt += Grid->Cells[3][i];
        dt += " �� ";
        dt += Grid->Cells[4][i];
        dt += " ���. �� ����� ";
        dt += Grid->Cells[6][i];
        dt += " ���.\n";
    }
    dt += "����� �� ����� ";
    dt += Star->Serial;
    dt += " ";
//    dt += StatusBar->Panels->Items[4]->Text;
    dt += "\n";
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SendDeletedLine()
{
    AnsiString dt = "������ ����� ";
    dt += DateTimeToStr(Now());
    dt += ". ����� ";
    dt += Star->Serial;
    dt += ". ������ ";
    dt += CasName;
    dt += "\n";
    dt += Nnum->Text.c_str();
    dt += " ";
    dt += Name->Caption.c_str();
    dt += " ";
    dt += Qnty->Text.c_str();
    dt += " ";
    dt += Unit->Caption.c_str();
    dt += " �� ";
    dt += Price->Text.c_str();
    dt += " ���.\n";
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SendDeletedBill()
{
    AnsiString dt = "������ ������ ";
    dt += DateTimeToStr(Now());
    dt += ". ����� ";
    dt += Star->Serial;
    dt += ". ������ ";
    dt += CasName;
    dt += "\n";
    for(int i = 1; i < Grid->RowCount; i++)
    {
        dt += Grid->Cells[0][i];
        dt += " ";
        dt += Grid->Cells[1][i];
        dt += " ";
        dt += Grid->Cells[2][i];
        dt += " ";
        dt += Grid->Cells[5][i];
        dt += " ";
        dt += Grid->Cells[3][i];
        dt += " �� ";
        dt += Grid->Cells[4][i];
        dt += " ���.\n";
    }
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SendCloseSession()
{
    AnsiString dt = "������ ����� ������� ";
    dt += DateTimeToStr(Now());
    dt += ". ����� ";
    dt += Star->Serial;
    dt += ". ������ ";
    dt += CasName;
    dt += "\n";
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SendOpenSession()
{
    AnsiString dt = "������ ����� ������� ";
    dt += DateTimeToStr(Now());
    dt += ". ����� ";
    dt += Star->Serial;
    dt += ". ������ ";
    dt += CasName;
    dt += "\n";
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ArrangeTable();
    Reader->TerminateCancel();
    CashierServer->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CashierServerClientError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ZKClick(TObject *Sender)
{
    AnsiString str;

    PayType = NO_PAYMENT;

    if(CheckSale()) Sale = true;
    else Sale = false;
    if(Sale)
    {
        PlayWav("warning.wav");
        if
        (
            MessageBox (GetActiveWindow(), "��������! ���������� �����! ����������?",
                "������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {Printing = false; return;}
    }

    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;

    Printing = true;
    PlayWav("cash.au");
    ArrangeTable();
    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty()) Grid->RowCount--;

    if(CheckSale0())
    {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� � ������� �����, ������� ���� ��� ������� ������", MB_OK);
      Printing = false; return;
    }

    if(ManualZK)
    {
        if
        (
            MessageBox (GetActiveWindow(), "����������� �����������?",
                "������� ������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {Printing = false; return;}
    }
    else
    {
        if(!PayZK(TSum->Text))
        {
            Printing = false;
            return;
        }
    }
    PlayWav("cash.au");
    Change->Text = "0.00";
    Recived->Text = "��";
    Recived->Repaint();
    Change->Repaint();

    if(Star->Serial == "") //���� �������� ����� �� ���������, �������� �� �����!
      {
      Star->GetVersion();
      if(Star->Serial == "")
         {
         Printing = false;
         return;
         }
      }

//    Star->OpenBox();
    Star->BillOpen(Sale,CasName,CasNumber);
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Printing = false;
        PlayWav("oy.wav");
        return;
    }

      for(int i = 1; i < Grid->RowCount; i++)
       {
        str = Grid->Cells[1][i];
        str += " (";
        str += Grid->Cells[3][i];
        str += ") ";
        str += Grid->Cells[2][i];
        strncpy(&cName[131*(i-1)], str.c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
        Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);

    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Printing = false;
        PlayWav("oy.wav");
        return;
    }
    Star->BillZK(CasNumber, "������� ������");
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Printing = false;
        PlayWav("oy.wav");
        return;
    }
    if(Star->FRType) Star->CRLF();
    PlayWav("cash.au");

    Name->Caption = "";
    Nnum->Text = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";

    WriteRetail(PayType);

    Printing = false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::PayZK(AnsiString Sum)
{
    AnsiString ErrorString[49] =
    {
        "����������� ������",
        "������ �� �������������",
        "�������� ������ �����",
        "������� �����",
        "������ ������ �����",
        "�������� ��������",
        "����� ���������� �� ������������ ������",
        "�������� ����� �����",
        "�������� ����� �������",
        "�������� �������",
        "����� �����������",
        "�������� ��������� �����",
        "�������� ������� �����",
        "������ ������ �� �����",
        "������ ������ � ��� ���������",
        "������ ����� ��� �����������",
        "������ �����������",
        "����� � ������ ������",
        "�������� �� ������������",
        "����������� ���������",
        "�������� ����� ������",
        "�������� ����� �����",
        "��������������� ������",
        "����������� ��� ������",
        "����������� ������",
        "������ � ���������",
        "����������� ������",
        "����������� ������",
        "����������� ������",
        "����������� ������",
        "����� ����������",
        "�� �� �����",
        "���������� ������ �� �������",
        "����� �������������",
        "������ � ���� ����������",
        "��� ������ ������",
        "����������� ������",
        "����������� ������",
        "����������� ������",
        "����������� ������",
        "�������� ������� ����� ���������� �� ������������� (��� ���)",
        "����������� ����������� �������",
        "�������� ������ (���������� ������� ��������)",
        "����� ����� ��������",
        "����������� ��� �������",
        "����������� ��� �������",
        "���� ��� ������� �� �������������",
        "���� ��� ������� �� �������������",
        "���������� ���������� ���������"
    };
    bool success = false;
    GoldCrown = new TGoldCrown(true);
    GoldCrown->Port = ZKPort;
    GoldCrown->Resume();
    while(GoldCrown->Result == -1000) Sleep(100);
    if(GoldCrown->ZKInit(Sum))
    {
        if(GoldCrown->Result)
        {
            if(GoldCrown->Result < 0 || GoldCrown->Result > 48) GoldCrown->Result = 0;
            PlayWav("oy.wav");
            Name->Caption = ErrorString[GoldCrown->Result];
        }
        else success = true;
    }
    else
    {
            PlayWav("oy.wav");
            Name->Caption = "������ ����� � ���������� " + String(GoldCrown->Result);
    }
    TerminateThread((HANDLE) GoldCrown->Handle,0);
    GoldCrown->Close();
    GoldCrown->Terminate();
    GoldCrown->WaitFor();
    delete GoldCrown;
    return success;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ZKManualClick(TObject *Sender)
{
    ManualZK = true;
    ZKClick(this);
    ManualZK = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SBClick(TObject *Sender)
{
    AnsiString str;
    char Scard[10];
    int RC;

    if(CheckSale()) Sale = true;
    else Sale = false;
    if(Sale)
    {
        PlayWav("warning.wav");
        if
        (
            MessageBox (GetActiveWindow(), "��������! ���������� �����! ����������?",
                "������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {Printing = false; return;}
    }

    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;

    Printing = true;
    PlayWav("cash.au");
    ArrangeTable();
    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty()) Grid->RowCount--;

    if(CheckSale0())
    {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� � ������� �����, ������� ���� ��� ������� ������", MB_OK);
      Printing = false; return;
    }

    if(ManualSB)
    {
        if
        (
            MessageBox (GetActiveWindow(), "����������� �����������?",
                "�������� ��", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {Printing = false; return;}
        strcpy(Scard,"��");
        SelectPayType();
    }
    else
    {

        if(!PaySB(TSum->Text, Scard))
        {
            PlayWav("oy.wav");
            Name->Caption = String(PayType);
            Printing = false;
            return;
        }
    }
    ShowOnDisplay("   ������ ������          �������");
    PlayWav("cash.au");

//    if
//    (
//        MessageBox (GetActiveWindow(), "��� ��������?\n������� [���] ��� ������ �������� �� �����.",
//            "�������� �� �����", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1) != IDYES
//    )
//    if(CancelMC(TSum->Text))
//          {
//            MessageBox (GetActiveWindow(), "�������� �� ����� ��������!!!",
//                "�������� �� �����", MB_OK | MB_ICONINFORMATION);
//
//          Printing = false;
//          return;
//          }

    Change->Text = "0.00";
    Recived->Text = Scard;
    Recived->Repaint();
    Change->Repaint();

    if(Star->Serial == "") //���� �������� ����� �� ���������, �������� �� �����!
      {
      Star->GetVersion();
      if(Star->Serial == "") return;
      }

    Present();

    Printing = true;

for(RC=0;RC<5;RC++)
  {

    Star->Print(" ",true);
    Star->Print(" ",true);
    Star->Print(" ",true);
    Star->Print(" ",true);
    Star->Print(BillHead[0].c_str(),true);
    Star->Print(BillHead[1].c_str(),true);
    Star->Print(" ",true);
//    Star->OpenBox();

    Star->BillOpen(Sale,CasName,CasNumber);
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        if(Star->KKMResult != "0000") return;
        else continue;
    }

    if(Star->FRType)
      {
      for(int i = 1; i < Grid->RowCount; i++)
       {
        if(
        (Grid->Cells[1][i]=="") ||
        (Grid->Cells[2][i]=="") ||
        (Grid->Cells[3][i]=="") ||
        (Grid->Cells[4][i]=="") ||
        (Grid->Cells[5][i]=="") ||
        (Grid->Cells[7][i]=="") ||
        (Grid->Cells[12][i]=="")
        )
        {
        Name->Caption = "�������� ��� - ����� ������������� ������";
        Printing = false;
        PlayWav("oy.wav");
        return;
        }
        strncpy(&cMeas[26*(i-1)], Grid->Cells[3][i].c_str(), 25);
        cMeas[26*(i-1)+25] = 0;
        strncpy(&cCode[21*(i-1)], Grid->Cells[1][i].c_str(), 20);
        cCode[21*(i-1)+20] = 0;
        strncpy(&cName[131*(i-1)], Grid->Cells[2][i].c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
       Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);
      }
    else
      {
      for(int i = 1; i < Grid->RowCount; i++)
       {
        if(
        (Grid->Cells[1][i]=="") ||
        (Grid->Cells[2][i]=="") ||
        (Grid->Cells[3][i]=="") ||
        (Grid->Cells[4][i]=="") ||
        (Grid->Cells[5][i]=="") ||
        (Grid->Cells[7][i]=="") ||
        (Grid->Cells[12][i]=="")
        )
        {
        Name->Caption = "�������� ��� - ����� ������������� ������";
        Printing = false;
        PlayWav("oy.wav");
        return;
        }

        str = Grid->Cells[1][i];
        str += " (";
        str += Grid->Cells[3][i];
        str += ") ";
        str += Grid->Cells[2][i];
        strncpy(&cName[131*(i-1)], str.c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
       Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);
      }

    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        if(Star->KKMResult != "0000") return;
        else continue;
    }

    Star->BillZK(CasNumber,"");
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Star->GetVersion();
        Sverka(true);
        if(Star->KKMResult != "0000") return;
        else continue;
    }
break;
}

if(RC==5)
   {
   Printing = false;
   PlayWav("oy.wav");
   return;
   }

    PlayWav("cash.au");

    Name->Caption = "";
    Nnum->Text = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";

    WriteRetail(PayType);

    Printing = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SBManualClick(TObject *Sender)
{
    ManualSB = true;
    SBClick(this);
    ManualSB = false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::PaySB(AnsiString Sum, char* Scard)
{
    int ret;
    auth_answer Answer;

    Printing = true;

    Answer.TType = 1; // ������
    Answer.Amount = MoneyAshyper(Sum); // ����� � ��������
    Answer.CType = 0; // ��� ����� -- ��� ���� ����� ��������� ��� �����!
    Answer.Check = NULL;

    SelectPayType();
    if(PayType == NAL_PAYMENT)
       {
//      CashClick(this);
        GlobalFree((HGLOBAL)Answer.Check);
        Printing = false;
       return false;
       }
    else if(PayType == SBERCARD_PAYMENT ) Answer.CType = 9;
    else if(PayType == INTERNATIONAL_CARD_PAYMENT) Answer.CType = 3;
    else
      {
      GlobalFree((HGLOBAL)Answer.Check);
      Printing = false;
      return false;
      }

    ret = card_authorize(NULL, &Answer);
    if(ret)
    {
        GlobalFree((HGLOBAL)Answer.Check);
        Printing = false;
        return false;
    }
    if(strcmp(Answer.RCode,"0") && strcmp(Answer.RCode,"00"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        Printing = false;
        return false;
    }

    if(CheckFile(Answer.Check, "Check.txt")<20)
    {
        GlobalFree((HGLOBAL)Answer.Check);
        Printing = false;
        return false;
    }

    AnsiString CFN = "c:\\data\\SB"+Now()+".txt";
    CheckFile(Answer.Check,CFN.c_str());

    PlayWav("cash.au");
    switch(Answer.CType)
    {
        case 1:
            strcpy(Scard,"Visa");
            break;
        case 2:
            strcpy(Scard,"EC/MC");
            break;
        case 3:
            strcpy(Scard,"��");
            break;
        case 9:
            strcpy(Scard,"��");
            break;
        default:
            strcpy(Scard,"��");
    }
    Star->Print(Answer.Check, true);
    if(strcmp(Star->KKMResult.c_str(), "0000"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        GetKKMError();
        Printing = false;
        return false;
    }

    GlobalFree((HGLOBAL)Answer.Check);
    Printing = false;
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SBDayCloseClick(TObject *Sender)
{
    int ret;
    auth_answer Answer;

    Answer.Check = NULL;

    Printing = true;
    PlayWav("cash.au");

    ret = close_day(&Answer);
    if(ret||(HGLOBAL)Answer.Check==NULL)
    {
        GlobalFree((HGLOBAL)Answer.Check);
        PlayWav("oy.wav");
        Name->Caption = AnsiString(Answer.AMessage);
        Printing = false;
        return;
    }
    if(strcmp(Answer.RCode,"0") && strcmp(Answer.RCode,"00"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        PlayWav("oy.wav");
        Name->Caption = AnsiString(Answer.AMessage);
        Printing = false;
        return;
    }
    AnsiString FileName;
    Word Year, Month, Day;
    TDateTime dtPresent = Now();
    DecodeDate(dtPresent, Year, Month, Day);

    FileName = OutputDir;
    FileName += Star->Serial;
    FileName += AnsiString(Year).SubString(3,2);
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Month)));
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Day)));
    FileName += ".";
    ret = 0;
    while(FileExists(FileName+String(ret))) ret++;
    FileName += String(ret);
        CheckFile(Answer.Check, FileName.c_str());
        PlayWav("cash.au");
        Star->Print(Answer.Check, true);
        if(strcmp(Star->KKMResult.c_str(),"0000"))
        {
            GetKKMError();
        }
        else Star->CRLF();
    PlayWav("cash.au");
    GlobalFree((HGLOBAL)Answer.Check);
    Printing = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CheckRepeatClick(TObject *Sender)
{
    ifstream file;
    if(!FileExists("check.txt"))
    {
        PlayWav("oy.wav");
        Name->Caption = "��������� ��� �� �����������";
        return;
    }
    file.open("check.txt");
    if(!file)
    {
        PlayWav("oy.wav");
        Name->Caption = "������ �������� ����� ����";
        return;
    }
    hyper pos = 0;
    char c;
    while(file.get(c)) pos++;
    file.close();
    char* str = new char[pos+1];
    file.open("check.txt");
    file.read(str, pos);
    file.close();
    str[pos] = 0;
    Star->Print(str, true);
    delete str;
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
    }
    else Star->CRLF();
}
//---------------------------------------------------------------------------
int __fastcall TMainWindow::CheckFile(char* Check, char* FileName)
{
    int i;
    ofstream file;
    file.open(FileName);
    for(i = 0; Check[i]; i++) file.put(Check[i]);
    file.close();
    return i;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CancelMC(AnsiString Sum)
{
    int ret;
    auth_answer Answer;

    Printing = true;

    Answer.TType = 8; // ������
    Answer.Amount = MoneyAshyper(Sum); // ����� � ��������
    Answer.CType = 0; // ��� �����
    Answer.Check = NULL;

    ret = card_authorize(NULL, &Answer);
    if(ret)
    {
        PlayWav("oy.wav");
        Name->Caption = AnsiString(Answer.AMessage);
        GlobalFree((HGLOBAL)Answer.Check);
        Printing = false;
        return false;
    }
    if(strcmp(Answer.RCode,"0") && strcmp(Answer.RCode,"00"))
    {
        PlayWav("oy.wav");
        Name->Caption = AnsiString(Answer.AMessage);
        GlobalFree((HGLOBAL)Answer.Check);
        Printing = false;
        return false;
    }
    CheckFile(Answer.Check, "check.txt");
    PlayWav("cash.au");
    Star->Print(Answer.Check, true);
    if(strcmp(Star->KKMResult.c_str(), "0000"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        GetKKMError();
        Printing = false;
        return true;
    }
    else Star->CRLF();
    MessageBox (GetActiveWindow(), "���� ��� �� �������",
        "�������� �� �����", MB_OK | MB_ICONINFORMATION);
    Star->Print(Answer.Check, true);
    if(strcmp(Star->KKMResult.c_str(), "0000"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        GetKKMError();
        Printing = false;
        return true;
    }
    else Star->CRLF();
    GlobalFree((HGLOBAL)Answer.Check);
    Printing = false;
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CancelClick(TObject *Sender)
{
    if(MoneyAshyper(TSum->Text) <= 0) return;
    if(CardType != MANAGER_CARD) return;

    Printing = true;
    ArrangeTable();
    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty() && Grid->RowCount > 2 ) Grid->RowCount--;

    if(CancelMC(TSum->Text))
        MessageBox (GetActiveWindow(), "�������� �� ����� ��������!!!",
        "�������� �� �����", MB_OK | MB_ICONINFORMATION);

    Printing = false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckSale()
{
    for(int i = 1; i < Grid->RowCount; i++)
    {
        if(Grid->Cells[2][i].LowerCase().Pos("������")) return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckSale0()
{
    for(int i = 1; i < Grid->RowCount; i++)
    {
        if(MoneyAshyper(Grid->Cells[4][i])==0) return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::SendFiles()
{
AnsiString FileName, SumFile,SFN,SSF;
Word Year, Month, Day;
TDateTime dtPresent = Now();
DecodeDate(dtPresent, Year, Month, Day);
int rowcnt;

PriceQuery->SQL->Text = "select count(*) as Rowcnt from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"'";
    try
      {
      PriceQuery->Active = true;
      }
      catch( ... )
      {
      Name->Caption = "������ SQL ��� ������������ �����";
      PriceQuery->Active = false;
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }
if(PriceQuery->RecordCount)
  rowcnt = PriceQuery->FieldByName("rowcnt")->AsInteger;
else rowcnt = -1;

PriceQuery->Active = false;

if(!rowcnt) WriteCloseRetail(); //���� ���� ���� ������ � ���� �� ����� ����!

return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::SendError()
{
static ErrorCode = 0;
hyper i,err;
AnsiString EFN;
ofstream errorfile;

err = GetLastError();

if((!err)||ErrorCode == err ) return true;
ErrorCode = err;

for(i=1;FileExists("err"+IntToStr(i)+".txt");i++);
EFN = "err"+IntToStr(i)+".txt";
errorfile.open(EFN.c_str(),O_WRONLY);
errorfile << ErrorCode ;
errorfile.close();
return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CopyCheckClick(TObject *Sender)
{
if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
BillPrint->QuickRep1->Print();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::N1Click(TObject *Sender)
{
Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::N3Click(TObject *Sender)
{
    AnsiString str;

    if(Star->FRType != 2) return;
    if(CardType != MANAGER_CARD) return;

    if(CheckSale()) Sale = true;
    else Sale = false;
    if(Sale)
    {
        PlayWav("warning.wav");
        if
        (
            MessageBox (GetActiveWindow(), "��������! ���������� �����! ����������?",
                "������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {Printing = false; return;}
    }

    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;

    Printing = true;
    PlayWav("cash.au");
    ArrangeTable();
    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty() && Grid->RowCount > 2) Grid->RowCount--;

    if(CheckSale0())
    {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� � ������� �����, ������� ���� ��� ������� ������", MB_OK);
      Printing = false; return;
    }

    if(CashForm->ShowModal() != mrOk) {Printing = false; return;}
    Change->Text = MoneyAsString(MoneyAshyper(Recived->Text) - MoneyAshyper(TSum->Text));
    Recived->Repaint();
    Change->Repaint();

//    Star->OpenBox();
    Star->BillReturn(Sale, CasName, CasNumber);
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Printing = false;
        PlayWav("oy.wav");
        return;
    }

    if(Star->FRType)
      {
      for(int i = 1; i < Grid->RowCount; i++)
       {
        strncpy(&cMeas[26*(i-1)], Grid->Cells[3][i].c_str(), 25);
        cMeas[26*(i-1)+25] = 0;
        strncpy(&cCode[21*(i-1)], Grid->Cells[1][i].c_str(), 20);
        cCode[21*(i-1)+20] = 0;
        strncpy(&cName[131*(i-1)], Grid->Cells[2][i].c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
        Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);
      }
    else
      {
      for(int i = 1; i < Grid->RowCount; i++)
       {
        str = Grid->Cells[1][i];
        str += " (";
        str += Grid->Cells[3][i];
        str += ") ";
        str += Grid->Cells[2][i];
        strncpy(&cName[131*(i-1)], str.c_str(), 130);
        cName[131*(i-1)+130] = 0;
        strncpy(&cPrice[16*(i-1)], Grid->Cells[4][i].c_str(), 15);
        cPrice[16*(i-1)+15] = 0;
        strncpy(&cQnty[16*(i-1)], Grid->Cells[5][i].c_str(), 15);
        cQnty[16*(i-1)+15] = 0;
        strncpy(&cNDS[2*(i-1)], Grid->Cells[12][i].c_str(), 1);
        cNDS[2*(i-1)+1] = 0;
       }
        Star->BillLine(cName, cPrice, cQnty, Grid->RowCount-1,cMeas,cCode,cNDS);
      }

    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Printing = false;
        PlayWav("oy.wav");
        return;
    }
    Star->BillClose(Recived->Text, CasNumber);
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Printing = false;
        Sleep(5000);
        Sverka(true);
        return;
    }
    Star->CRLF();

    PlayWav("cash.au");

    Name->Caption = "";
    Nnum->Text = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";

    AnsiString FileName, SumFile;
    Word Year, Month, Day;
    TDateTime dtPresent = Now();
    DecodeDate(dtPresent, Year, Month, Day);
    ofstream af1, so;
    ifstream si;

    FileName = OutputDir;
    FileName += Star->Serial;
    FileName += AnsiString(Year).SubString(3,2);
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Month)));
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Day)));
    SumFile = FileName + ".sum";
    FileName += ".res";

    af1.open(FileName.c_str(), ios::app);
    for(int i = 1; i < Grid->RowCount; i++)
    {
        af1 << Star->Serial.c_str()
            << " " << CasName.c_str() << " (" << String(CasNumber).c_str()
            << ")" << endl
            << dtPresent.DateTimeString().c_str() << endl
            << Grid->Cells[1][i].c_str() << "\t" << Grid->Cells[7][i].c_str() << endl
            << Grid->Cells[2][i].c_str() << endl
            << Grid->Cells[3][i].c_str() << endl
            << Grid->Cells[4][i].c_str() << endl
            << Grid->Cells[5][i].c_str() << endl
            << Grid->Cells[6][i].c_str() << endl
            << "�������������" << endl;
    }
    if(af1.fail())
    {
        PlayWav("warning.wav");
        Name->Caption = "������ ������ ����� ������. ���������� ���������� ������!";
    }
    af1.close();
    char sum[20] = "0.00";
    if(FileExists(SumFile))
    {
        si.open(SumFile.c_str());
        si >> sum;
        si.close();
    }
    strncpy(sum, MoneyAsString(MoneyAshyper(sum)+ MoneyAshyper(TSum->Text)).c_str(), 19);
    so.open(SumFile.c_str());
    so << sum;
    so.close();
//    StatusBar->Panels->Items[4]->Text = String(sum);
    SendMessage();

    for(int i = 1; i < Grid->RowCount; i++) Grid->Rows[i]->Clear();
    Grid->RowCount = 2;
    ArrangeTable();
    Printing = false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::WriteRetail(AnsiString PayType)
{
    AnsiString FileName, SumFile, str;
    Word Year, Month, Day;
    TDateTime dtPresent = Now();
    ofstream af1, so;
    ifstream si;
    char buf[256];
    int o;

    if(PayType == NO_PAYMENT)
      {
      SelectPayType();
      PayType=MainWindow->PayType;
      }

    DecodeDate(dtPresent, Year, Month, Day);

    FileName = OutputDir;
    FileName += Star->Serial;
    FileName += AnsiString(Year).SubString(3,2);
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Month)));
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Day)));
    SumFile = FileName + ".sum";
    FileName += ".res";

    if(CheckSale0())
    {
      PlayWav("warning.wav");
      MessageBox (GetActiveWindow(),"��������!",
              "����� � ������� �����, ������� ���� ��� ������� ������", MB_OK);
      Printing = false; return false;
    }

    af1.open(FileName.c_str(), ios::app);
    for(int i = 1; i < Grid->RowCount; i++)
    {
        strncpy(buf,Grid->Cells[1][i].c_str(),20);
        for(o=0;o<20 && buf[o]; o++);
        for(;o<20;o++) buf[o] = ' ';
        strncpy(buf+20,Grid->Cells[7][i].c_str(),20);

        af1 << Star->Serial.c_str()
            << " " << CasName.c_str() << " (" << String(CasNumber).c_str()
            << ")" << endl
            << dtPresent.DateTimeString().c_str() << endl
            << buf << endl
            << Grid->Cells[2][i].c_str() << endl
            << Grid->Cells[3][i].c_str() << endl
            << Grid->Cells[4][i].c_str() << endl
            << Grid->Cells[5][i].c_str() << endl
            << Grid->Cells[6][i].c_str() << endl
            << PayType.c_str() << endl;
    }
    if(af1.fail())
    {
        PlayWav("warning.wav");
        Name->Caption = "������ ������ ����� ������. ���������� ���������� ������!";
    }
    af1.close();
    char sum[20] = "0.00";
    if(FileExists(SumFile))
    {
        si.open(SumFile.c_str());
        si >> sum;
        si.close();
    }
    strncpy(sum, String(String(sum) + " "+TSum->Text).c_str(), 19);
    so.open(SumFile.c_str());
    so << sum;
    so.close();
//    StatusBar->Panels->Items[4]->Text = String(sum);
    SendMessage();

    //������� �� SQL
    AnsiString SDT = FormatDateTime("dd.mm.yyyy hh:mm:ss",dtPresent);
    AnsiString GN,GNA;

   //������� ������� �����  bill
    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("delete from bill");

    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }

    // ������� ���� �� ����� ����������� � ������� bill
    for(int i = 1; i < Grid->RowCount; i++)
    {
    //������������ ������� � ��������� � ������������
    GN = Grid->Cells[2][i];
    GNA = "";
    for(int GNC=1;GNC <= GN.Length(); GNC++)
      if(GN.SubString(GNC,1) == "'") GNA += "''";
        else if(GN.SubString(GNC,1) != "\t") GNA += GN.SubString(GNC,1);

    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("insert into bill (NumCode, Quantity, Price, Operator)");
    PriceQuery->SQL->Add("values ('"+Grid->Cells[7][i]+"',"+Grid->Cells[5][i]+","+Grid->Cells[4][i]+",left('"+Grid->Cells[8][i]+"',15))" );

    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }

    if(PriceQuery->RowsAffected != 1)
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }
    }

    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("set dateformat dmy");
    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }

   //������ ����� � ������� ����������
    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("insert into retail (BillDateTime,BillNumber,NumCode, Sklad ,Quantity, Price, PayType, SCash,Operator,flag)");
    PriceQuery->SQL->Add("select CURRENT_TIMESTAMP,'" + Star->BillNumber + "',NumCode ,'"+Department+"',Quantity,Price,'"+PayType+"','"+Star->Serial+"',Operator,101" );
    PriceQuery->SQL->Add("from bill");

    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }

   //������� ������
   for(int i = 1; i < Grid->RowCount; i++) Grid->Rows[i]->Clear();
   Grid->RowCount = 2;
   GoodName = "";
   GoodPrice = "";
   TotalSum = "0.00";
   ShowOnDisplay("   ����� ����������!");

   ArrangeTable();

    //�������� ��������
    PriceQuery->SQL->Clear();
    if(FileExists("stock.sql"))
    {
      si.open("stock.sql", ios::binary);
      while(true)
      {
          if(si.eof()) break;
          if(!si.getline(buf,255))
            {
            if(si.eof()) break;
            break;
            }
          str = buf;
          if((o = str.Pos("@FIRSTDAYINPERIOD")) > 0)
            str = str.SubString(1,o-1)+"'"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+"01'";
          PriceQuery->SQL->Add(str);
      }
      si.close();

    try
      {
      PriceQuery->Open();
      }
      catch( ... )
      {
      }

    //����� �������� �������� ����� ...
      while(!PriceQuery->Eof)
        {
        str = PriceQuery->FieldByName("NumCode")->AsString+" "+PriceQuery->FieldByName("Name")->AsString+" �� ������� � ��������: "+PriceQuery->FieldByName("Quantity")->AsString;
        MessageBox (GetActiveWindow(),str.c_str(),"������� ������ ����� �������",MB_OK);
        PriceQuery->Next();
        }
      PriceQuery->Close();
    }

    ShowOnDisplay("  ����� ����������!");
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::WriteCloseRetail(void)
{
    TDateTime dtPresent=Now();
    //������� �� SQL
    AnsiString SDT = FormatDateTime("dd.mm.yyyy hh:mm:ss",dtPresent);
    AnsiString GN,GNA;

    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("set dateformat dmy");
    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }

    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("insert into retail (BillDateTime,BillNumber,NumCode, Sklad, Quantity, Price, PayType,SCash,Operator,flag)");
    PriceQuery->SQL->Add("values ('"+SDT+"','0','0','"+Department+"',0,0,'','"+Star->Serial+"',left('"+CasName+"',15),100)" );
    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }
    if(PriceQuery->RowsAffected != 1)
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return false;
      }
  return true;
}

//---------------------------------------------------------------------------
void __fastcall TMainWindow::N4Click(TObject *Sender)
{
Sverka(false);
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::Sverka(bool Silent)
{
hyper LFRSumm;
Word Year, Month, Day;
TDateTime dtPresent = Now();
unsigned hyper FRSumm,USumm,USummM, BillSumm;
AnsiString Str;

LFRSumm = Star->GetSellSumm();
FRSumm = LFRSumm;

//��������� ��������� ����� �� �������
DecodeDate(dtPresent, Year, Month, Day);

PriceQuery->SQL->Text = "select sum(round([price]*[quantity],2)) as Summ from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"'";
    try
      {
      PriceQuery->Active = true;
      }
      catch( ... )
      {
      Name->Caption = "������ SQL ��� ������ ����";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return;
      }
if(PriceQuery->RecordCount) USumm = MoneyAshyper(PriceQuery->FieldByName("Summ")->AsString);
else
  {
  Name->Caption = "������ SQL ��� ������ ����";
  PriceQuery->Active = false;
  PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
  return;
  }
PriceQuery->Active = false;

PriceQuery->SQL->Text = "select sum([price]*[quantity]) as Summ from delbill where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"'";
    try
      {
      PriceQuery->Active = true;
      }
      catch( ... )
      {
      Name->Caption = "������ SQL ��� ������ ����";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      }
if(PriceQuery->RecordCount)
  {
  USummM = MoneyAshyper(PriceQuery->FieldByName("Summ")->AsString);
  USumm += USummM;
  }
else
  {
  Name->Caption = "������ SQL ��� ������ ����";
  PriceQuery->Active = false;
  PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
  return;
  }
PriceQuery->Active = false;

if(USumm == FRSumm) {if(!Silent) MessageBox(GetActiveWindow(),"����� ����� ����� X-������","������ ������",MB_OK);}
else
  {
  BillSumm = MoneyAshyper(TSum->Text);
  if(USumm + BillSumm == FRSumm)
    {
    if(!Silent) MessageBox(GetActiveWindow(),"��� ��� ������, ����������","������ ������",MB_OK);
    Star->BillNumber = Star->GetBillNumber();
    if(!Silent || PayType == NO_PAYMENT) SelectPayType();
    WriteRetail(PayType);
    }
  else
    {
    Str = "���� "+MoneyAsString(USumm)+" X-����� "+MoneyAsString(FRSumm);
    MessageBox(GetActiveWindow(),Str.c_str(),"������ ������",MB_OK);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::COLClick(TObject *Sender)
{
    Star->Print(BillHead[0].c_str(),true);
    Star->Print(BillHead[1].c_str(),true);
    Star->Print(" ",true);
Star->PrintCopyCheck();
Star->CRLF();
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::DelLastBillClick(TObject *Sender)
{
Word Year, Month, Day;
TDateTime dtPresent = Now();
AnsiString str;

//��������� ������ ����������
if(CardType != MANAGER_CARD) return;

//�������� �����
    ArrangeTable();
    if(Grid->RowCount > 2) Grid->RowCount--;
    NewCode = false;
    NewPrice = false;
    InputQnty = false;
    Delete = false;
    DeleteMode = 0;

    ofstream file;
    AnsiString FileName = OutputDir + "_deleted.txt";
    file.open(FileName.c_str(), ios::app);
    for(int i = 1; i < Grid->RowCount; i++)
    {
        file << Star->Serial.c_str()
            << "\t" << CasName.c_str() << "\t" << String(CasNumber).c_str()
            << "\t"
            << dtPresent.DateTimeString().c_str() << "\t"
            << Grid->Cells[1][i].c_str() << "\t"
            << Grid->Cells[2][i].c_str() << "\t"
            << Grid->Cells[3][i].c_str() << "\t"
            << Grid->Cells[4][i].c_str() << "\t"
            << Grid->Cells[5][i].c_str() << "\t"
            << Grid->Cells[6][i].c_str() << endl;
    }
    file.close();
    SendDeletedBill();

    for(int i = 1; i < Grid->RowCount; i++)
        Grid->Rows[i]->Clear();
    Grid->RowCount = 2;
    Nnum->Text = "";
    Unit->Caption = "";
    Price->Text = "";
    Qnty->Text = "";
    Name->Caption = "";
    CalcTotal();
    for(int i = 0; i <= Grid->ColCount; i++)
    {
        String str = "g" + String(i) + ".txt";
        DeleteFile(str);
    }

//������� ��� �� �����
DecodeDate(dtPresent, Year, Month, Day);
PriceQuery->SQL->Text = "select * from retail where billnumber > '0' and billdatetime in (select max(billdatetime) as billdatetime from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"')";
    try
      {
      PriceQuery->Active = true;
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return;
      }

Grid->RowCount = 1;
for(int i = 1;i <= PriceQuery->RecordCount; i++)
  {
  Grid->Cells[0][i] = i;
  Grid->Cells[1][i] = PriceQuery->FieldByName("ScanCode")->AsString;
  Grid->Cells[2][i] = PriceQuery->FieldByName("Name")->AsString;
  Grid->Cells[3][i] = PriceQuery->FieldByName("Meas")->AsString;
  Grid->Cells[4][i] = PriceQuery->FieldByName("Price")->AsString;
  Grid->Cells[5][i] = PriceQuery->FieldByName("Quantity")->AsString;
  Grid->Cells[6][i] = PriceQuery->FieldByName("Summ")->AsString;
  Grid->Cells[7][i] = PriceQuery->FieldByName("NumCode")->AsString;
  Grid->Cells[8][i] = PriceQuery->FieldByName("Operator")->AsString;
  PriceQuery->Next();
  }
if(Grid->RowCount < 2) Grid->RowCount = 2;
ArrangeTable();
PriceQuery->Active = false;
CalcTotal();

//��� ��� �� ��� ���?
    if
    (
        MessageBox (GetActiveWindow(), "�������� ���� ���?",
            "�������������", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    )
      {
      for(int i = 1; i < Grid->RowCount; i++)
        Grid->Rows[i]->Clear();
      Grid->RowCount = 2;
      Nnum->Text = "";
      Unit->Caption = "";
      Price->Text = "";
      Qnty->Text = "";
      Name->Caption = "";
      CalcTotal();
      for(int i = 0; i <= Grid->ColCount; i++)
        {
        String str = "g" + String(i) + ".txt";
        DeleteFile(str);
        }
      CalcTotal();
      return;
      }

//������� ���
PriceQuery->SQL->Text = "delete from retail where billnumber > '0' and billdatetime in (select max(billdatetime) as billdatetime from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"')";
    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      Name->Caption = "������ ������ SQL. ���������� ���������� ������!";
      PlayWav("oy.wav");
        //DumpLog
        if(Log)
          {
          ofstream errlog;
          errlog.open("errlog.txt",ios::app);
          errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz", Now()).c_str() <<"������ SQL "<<PriceQuery->SQL->Text.c_str();
          errlog << "\n";
          errlog.close();
          }
        //EndDumpLog */
      return;
      }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SelectPayType(void)
{
PayType = "";
while(PayType == "")
  {
  SelectPayTypeForm->Close();
  SelectPayTypeForm->ShowModal();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ShowOnDisplay(char * Message)
{
    static int first = 1;

    char *szComPort[] = {"COM1", "COM2", "COM3", "COM4", "COM5", "COM6"};
    DCB dcb;
    DWORD dwEvMask, dwError;
    COMSTAT csStat;
    COMMTIMEOUTS cto;
    unsigned long nChar;
    OVERLAPPED lpOverlapped;

    char str[41],in[256];
    int i;

    if( CommPortD < 1 || CommPortD > 6) return;

    for(i=0; Message[i] && i<40; i++) str[i] = Message[i];
    for(   ; i<40; i++) str[i] = ' ';
    str[i] = 0;

    if(first)
    {
    if ((hCommD = CreateFile (szComPort[CommPortD-1], GENERIC_READ | GENERIC_WRITE | FILE_FLAG_OVERLAPPED,
                  0, NULL, OPEN_EXISTING, NULL , NULL)) == INVALID_HANDLE_VALUE)
    {
        CommPortD = 0;
        return;
    }

    SetupComm (hCommD, 256, 256);       // allocate transmit & receive buffer
    dcb.BaudRate = 2400;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (SetCommState (hCommD, &dcb) < 0)
    {
        CloseHandle (hCommD);                    // close the COM port.
        CommPortD = 0;
        return;
    }
    cto.ReadIntervalTimeout = 1000;
    cto.ReadTotalTimeoutMultiplier = 1000;
    cto.ReadTotalTimeoutConstant = 2000;
    cto.WriteTotalTimeoutMultiplier = 1000;
    cto.WriteTotalTimeoutConstant = 2000;
    if(!SetCommTimeouts(hCommD, &cto))
    {
        CloseHandle (hCommD);                    // close the COM port.
        CommPortD = 0;
        return;
    }
    if (!SetCommMask(hCommD, EV_RXCHAR | EV_BREAK))
    {
        CloseHandle (hCommD);                    // close the COM port.
        CommPortD = 0;
        return;
    }
    first = 0;
    }

    Star->StrWinToDos(str,40);
    for(int de=0;de<2; de++) WriteFile (hCommD, str,40,&nChar, NULL);
//    CloseHandle (hCommD);                    // close the COM port.
}


void __fastcall TMainWindow::L1Click(TObject *Sender)
{
    AnsiString MessageA = "   �������          ���������������";
    unsigned long nChar;

    char str[40],*Message,in[256];
    int i;

    Message = MessageA.c_str();

    for(i=0; Message[i] && i<40; i++) str[i] = Message[i];
    for(   ; i<40; i++) str[i] = ' ';

    WriteFile (hCommD, "\x04\x01\x49\x3A\x17", 5, &nChar, NULL); //DSP850
    Sleep(200);
    WriteFile (hCommD, "\x04\x01\x50\x31\x17", 5, &nChar, NULL); //DSP850
    Sleep(200);
    WriteFile (hCommD, "\x04\x01\x43\x31\x58\x17", 6, &nChar, NULL); //DSP850
    Sleep(200);

    Star->StrWinToDos(str,40);
    WriteFile (hCommD, str,40,&nChar, NULL);

}
//---------------------------------------------------------------------------
unsigned hyper __fastcall TMainWindow::MoneyAshyper(AnsiString String)
{
int p;
AnsiString C,D;
unsigned hyper r,d;

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
  return 100*atol(C.c_str())+r;
  }
}
//---------------------------------------------------------------------------
unsigned hyper __fastcall TMainWindow::QuantityAshyper(AnsiString String)
{
int p;
AnsiString C,D;
unsigned hyper r,d;

if( (p = String.Pos(".")) == 0) p = String.Pos(",");
if( p == 0) return 1000*atol(String.c_str());
else
  {
  C = String.SubString(1,p-1);
  D = String.SubString(p+1,String.Length()-p);
  r = atol(D.c_str());
  p = D.Length();
  d = 1;
  if(p>3) { for(int i=0;i<p-3;i++) d*=10; r=(r+5)/d; }
  else if(p<3) { for(int i=0;i<3-p;i++) d*=10; r*=d;}
  return 1000*atol(C.c_str())+r;
  }
}
//---------------------------------------------------------------------------
unsigned hyper __fastcall TMainWindow::X(unsigned hyper Price,unsigned hyper Quantity)
{
return (Price*Quantity+500)/1000;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::MoneyAsString(unsigned hyper Money)
{
return String(Money/100)+Format(".%2.2D",ARRAYOFCONST(((int)(Money%100))));

}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::QuantityAsString(unsigned hyper Quantity)
{
return String(Quantity/1000)+Format(".%3.3D",ARRAYOFCONST(((int)(Quantity%1000))));
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::Present(void)
{
Word Year, Month, Day;
ifstream file;
hyper pos = 0;
int i,BoldFlag;
char c,str[80],s[80];
AnsiString AS;

if( MoneyAshyper(TSum->Text) < 30000 ) return; //���� ����� ������ 300��� - ��� �������
if(!FileExists("coupon.txt")) return; //���� ��� ����� ������ - ��� �������

//���� ���������� ������� ��������� - ��� �������
PriceQuery->SQL->Text = "SELECT count(*) as c FROM putin.db_cash.dbo.Coupon";
try
  {
  PriceQuery->Active = true;
  }
  catch( ... )
  {
  return;
  }
if( PriceQuery->FieldByName("c")->AsInteger >= 12000)
  {
  PriceQuery->Active = false;
  return;
  }
PriceQuery->Active = false;

//����������� �������
PriceQuery->SQL->Clear();
PriceQuery->SQL->Add("insert into putin.db_cash.dbo.coupon (CouponDateTime, ACash)");
PriceQuery->SQL->Add("select CURRENT_TIMESTAMP,'"+Star->Serial+"'");

    try
      {
      PriceQuery->ExecSQL();
      }
      catch( ... )
      {
      return;
      }

//��, �������
PlayWav("podarok.wav");

//������ ������
    file.open("coupon.txt");
    if(!file)
    {
        PlayWav("oy.wav");
        Name->Caption = "������ �������� ����� ����";
        return;
    }

    while(!file.eof())
      {
      file.getline(str,80);
      pos = 0;
      BoldFlag = 0;
      for(i=0; (str[pos]!='\n') && str[pos];)
        {
        if(str[pos] == '<')
          {
          pos++;
          if(str[pos] == 'b') BoldFlag = 1;
          if(str[pos] == 'd')
            {
            AS = FormatDateTime("dd.mm.yy",Now()+7);
            for(int j=0;AS.c_str()[j];j++) s[i++] = AS.c_str()[j];
            }
          while((str[pos]!='>') && (str[pos]!='\n') && (str[pos])) pos++;
          pos++;
          }
        else s[i++] = str[pos++];
        }
      s[i] = 0;
      if(BoldFlag) Star->PrintBold(s,true); else Star->Print(s,true);
      }
    Star->CRLF();
    file.close();
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::SynchronizeServer()
{
static int FailCountPrice = 0;
static int FailCountRetail = 0;
AnsiString MaxId,Command,GN,GNA;
long i;
DWORD ReturnCode;

//����� ������������� �����-�����
AnsiString CmdString;
STARTUPINFO lpStartupInfo;  // pointer to STARTUPINFO
PROCESS_INFORMATION lpProcessInformation;  // pointer to PROCESS_INFORMATION


memset(&lpStartupInfo,0,sizeof(lpStartupInfo));
memset(&lpProcessInformation,0,sizeof(lpProcessInformation));

lpStartupInfo.cb = sizeof(lpStartupInfo);
lpStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
lpStartupInfo.wShowWindow = SW_HIDE;
CmdString = "Synprice.exe";

if(!CreateProcess(
    NULL, // pointer to name of executable module
    CmdString.c_str(),	// pointer to command line string
    NULL,	// pointer to process security attributes
    NULL,	// pointer to thread security attributes
    false,	// handle inheritance flag
    CREATE_NEW_CONSOLE|IDLE_PRIORITY_CLASS,	// creation flags
    NULL,	// pointer to new environment block
    NULL,	// pointer to current directory name
    &lpStartupInfo,	// pointer to STARTUPINFO
    &lpProcessInformation 	// pointer to PROCESS_INFORMATION
   ))
   FailCountPrice++;
else
   {
   //��������� ��������� � �������� ��� ����������
   WaitForSingleObject(lpProcessInformation.hProcess, INFINITE);
   GetExitCodeProcess(lpProcessInformation.hProcess,&ReturnCode);
   if(ReturnCode)
     FailCountPrice++;
   else
     FailCountPrice = 0;
   CloseHandle(lpProcessInformation.hProcess);
   }

CmdString = "Synret.exe";

if(!CreateProcess(
    NULL, // pointer to name of executable module
    CmdString.c_str(),	// pointer to command line string
    NULL,	// pointer to process security attributes
    NULL,	// pointer to thread security attributes
    false,	// handle inheritance flag
    CREATE_NEW_CONSOLE|IDLE_PRIORITY_CLASS,	// creation flags
    NULL,	// pointer to new environment block
    NULL,	// pointer to current directory name
    &lpStartupInfo,	// pointer to STARTUPINFO
    &lpProcessInformation 	// pointer to PROCESS_INFORMATION
   ))
   FailCountRetail++;
else
   {
   //��������� ��������� � �������� ��� ����������
   WaitForSingleObject(lpProcessInformation.hProcess, INFINITE);
   GetExitCodeProcess(lpProcessInformation.hProcess,&ReturnCode);
   if(ReturnCode)
     FailCountRetail ++;
   else
     FailCountRetail = 0;
   CloseHandle(lpProcessInformation.hProcess);
   }

  if(!FailCountRetail && !FailCountPrice)
      {
      StatusBar->Panels->Items[1]->Text = "����� � �������� ����";
      StatusBar->Update();
      return true;
      }

    if(FailCountRetail > 1)
      {
      if(FailCountPrice>1)
        StatusBar->Panels->Items[1]->Text = "����� ���!";
      else
        StatusBar->Panels->Items[1]->Text = "����-�� �������-���";
      }
    else
      {
      if(FailCountPrice>1)
        StatusBar->Panels->Items[1]->Text = "����-��� �������-��";
      else
        return false;
      }

//PlayWav("oy.wav");
return false;
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::L2Click(TObject *Sender)
{
SynchronizeServer();
}
//---------------------------------------------------------------------------



void __fastcall TMainWindow::FormDblClick(TObject *Sender)
{
SynchronizeServer();
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::N16Click(TObject *Sender)
{
SelectPayType();
WriteRetail(PayType);
}
//---------------------------------------------------------------------------

