//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "mainform.h"
#include "screensaver.h"
#include "cash.h"
#include <Filectrl.hpp>
#include <math.h>
#include "fstream.h"
#include "pilot_nt.h"
#include "billprintc.h"
//#include "SelectPayTypeFormUnit.h"
#include "FreeCheck.h"
#include "ComplexPay.h"
//#include "fSverka.h"
#include <inifiles.hpp>
#include <math.hpp>
#include <boost/regex.hpp>
#include <vector>
#include <map>

#define CHEQUE_FILENAME "cheque.txt"
#define FUNC_AUTH "card_authorize13"
#define DDL_NAME "pilot_nt.dll"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//typedef int (*dynamic_card_auth)(char *, struct auth_answer13);
//typedef int (*dynamic_card_complete_multi_auth13)(char* track2, struct auth_answer13* auth_ans, struct preauth_rec*  pPreAuthList, int NumAuths);
//HMODULE dll = LoadLibrary(DDL_NAME);
typedef unsigned hyper Kopeyki;

TMainWindow *MainWindow;
HANDLE hStarCom;
bool scflag = false;
BillData * mfBill = new BillData;
FRStatus * frStatus = new FRStatus;
// выражение типа ключ-значение "Название платежа"
static std::map<AnsiString, AnsiString> PayName;
std::map<AnsiString, AnsiString>::iterator it;
// шаблон для поиска в ассоциативном массиве по значению
template<class Iterator, class Value>
    Iterator find_value(Iterator begin, Iterator end, const Value& value)
{
    while (begin!=end)
    {
        if ((*begin).second==value) return begin;
        ++begin;
    }
    return end;
}

// ---------------------------------------------------------------------------
__fastcall TMainWindow::TMainWindow(TComponent* Owner)
    : TForm(Owner)
{
    DecimalSeparator = '.';
    ThousandSeparator = ',';
    NewCode = true;
    NewPrice = false;
    InputQnty = false;
    InputMoney = false;
    NowSearching = false;
    Delete = false;
    DeleteMode = 0;
    ManualZK = false;
    PayType = NO_PAYMENT;
    Qnty->Color = clBtnFace;
    SoundDir = "Sound";
}
//---------------------------------------------------------------------------
// Запускается на событие считывания сканером ШК
void __fastcall TMainWindow::ScannerAnswer(AnsiString code)
{
    if(CheckBool(Printing) || NewPrice || InputQnty || InputMoney || NowSearching ) return;
    if(!NoCard && CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;

    char cr = 13; // перевод строки
    for(int i = 1; i <= code.Length(); i++)
    {
        FormKeyPress(this, code[i]);
    }
    FormKeyPress(this, cr);
    ScannerEnter = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::AddToTable(TStringGrid *Grid)
{

    if(CheckBool(Printing)) return;

    if(PresentEnter){
       if(!VerifyPresent(IDNom->Caption, PresentGrid,5)){
          PlaySound("oy.wav",0,SND_ASYNC);
          ClearForm();
       }
       else {
          PlaySound("nnumok.wav",0,SND_ASYNC);
       }
      ScannerEnter = false;
      if(VerifyAllPresent(PresentGrid, 1)){
         PresentEnter = false;
         PresentLabel->Caption = "Все подарки собраны";
         PresentLabel->Color = clLime;
         NowSearching = false;
         if(PayFlag == 1)CashClick(this);
         else if(PayFlag == 10) SBClick(this);
      }
      return;
    }

    while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("101");
    int Row = Grid->RowCount;
    if(!Grid->Cells[0][Grid->RowCount - 1].IsEmpty())Grid->RowCount = Grid->RowCount + 1;  // Если строка не пронумерована (пустая), то добавляем строку
    if(Grid->RowCount == 2 && Grid->Cells[0][1].IsEmpty()) Row = 1; // обходим вторую пустую строку на начальном этапе

    LeaveCriticalSection(&CS); log("001");

    unsigned hyper pr, qt, st;
    String str;
    Grid->Cells[0][Row] = AnsiString(Row);
    if(Nnum->Text.Length() == 13) {Grid->Cells[1][Row] = Nnum->Text.SubString(1,12);}
    else {Grid->Cells[PG_CODE_COL][Row] = Nnum->Text;}
    Grid->Cells[PG_NAME_COL][Row] = Name->Caption;
    GoodName = Name->Caption;
    pr = MoneyAshyper(Price->Text);
    qt = QuantityAshyper(Qnty->Text);
    st = X(pr,qt);
    Grid->Cells[PG_MEASURE_COL][Row] = Unit->Caption;
    GoodPrice = MoneyAsString(pr);
    Grid->Cells[PG_PRICE_COL][Row] = GoodPrice;
    Grid->Cells[PG_QUANTITY_COL][Row] = QuantityAsString(qt);
    Grid->Cells[PG_COST_COL][Row] = MoneyAsString(st);
    Grid->Cells[PG_KASSIR_COL][Row] = CasName;
    if(HandInput) Grid->Cells[PG_HANDINPUT_COL][Row]  = "true"; else Grid->Cells[9][Row]  = "false";
    if(SetNum)    Grid->Cells[PG_SETNUM_COL][Row] = "true"; else Grid->Cells[10][Row] = "false";
    if(SetPrice)  Grid->Cells[PG_SETPRICE_COL][Row] = "true"; else Grid->Cells[11][Row] = "false";
    Grid->Cells[PG_NDS_COL][Row] = NDS;
    Grid->Cells[PG_IDNOM_COL][Row] = IDNom->Caption;
//    Grid->Cells[14][Row] = MoneyAsString(st);  // по умолчанию скидки нет, расчетов картами нет
    Grid->Cells[PG_CORRECTION_COL][Row] = 0;  // по умолчанию сумма коррекции 0
    ScannerEnter = false;
    Grid->Row = Row;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::AlterLastRow(TStringGrid *Grid)
{
//if(Grid->RowCount >= 100)
 // {
 // MessageBox (GetActiveWindow(),"Много товаров в чеке",
 //   "Чек необходимо разбить надвое! Печатайте чек", MB_OK);
 // }

    while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("102");
    int Row = Grid->RowCount - 1;
    LeaveCriticalSection(&CS); log("002");

    unsigned hyper pr, qt, st;
    String str;
    pr = MoneyAshyper(Price->Text);
    qt = QuantityAshyper(Qnty->Text);
    st = X(pr,qt);
    GoodName = Name->Caption;
    Grid->Cells[0][Row] = AnsiString(Row);
    if(Nnum->Text.Length() == 13) {Grid->Cells[1][Row] = Nnum->Text.SubString(1,12);}
    else {Grid->Cells[1][Row] = Nnum->Text;}
    Grid->Cells[2][Row] = Name->Caption;
    if(PresentEnter){
       Grid->Cells[3][Row] = QuantityAsString(qt);
       Grid->Cells[4][Row] = Unit->Caption;
       GoodPrice = "";
    }
    else{
    Grid->Cells[3][Row] = Unit->Caption;
    GoodPrice = MoneyAsString(pr);
    Grid->Cells[4][Row] = GoodPrice;
    Grid->Cells[5][Row] = QuantityAsString(qt);
    Grid->Cells[6][Row] = MoneyAsString(st);
    Grid->Cells[8][Row] = CasName;
    Grid->Row = Row;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ArrangeTable()
{
    int bs;
    String str;

   ClearForm();
    //Если чек не пустой
    //Строки с одинаковым идентификатором товара и ценой сложить в одну - но только если это не подарочная карта

    while(!TryEnterCriticalSection(&CS))
    {
      log("Попытка входа в критическую секцию при сжатии чека неудачна, пробуем еще");
      Sleep(1000);
     }
    if(Grid->Cells[1][Grid->RowCount-1].Length()!= 10)
    for(bs=1;bs<Grid->RowCount-1;bs++)
    { // если у строк одинаковый IDNom и цена, то складываем строки
      if( (Grid->Cells[13][bs] == Grid->Cells[13][Grid->RowCount-1]) && (Grid->Cells[4][bs] == Grid->Cells[4][Grid->RowCount-1]))
      { // складываем количество
        Grid->Cells[5][bs] = QuantityAsString(
                QuantityAshyper(Grid->Cells[5][Grid->RowCount-1]) + QuantityAshyper(Grid->Cells[5][bs]));
        // обновляем сумму строки
        Grid->Cells[6][bs] = MoneyAsString(
                X(MoneyAshyper(Grid->Cells[4][bs]),QuantityAshyper(Grid->Cells[5][bs])));
        //Удалить последнюю строку
        Grid->RowCount = Grid->RowCount - 1;
        Grid->Row = bs;
        }
     }

//    }

   SaveGridToFile(Grid, ProgramPath + "gridrows.txt");

//   GiftCardM->Lines->SaveToFile(ProgramPath + "g99.txt");

   LeaveCriticalSection(&CS);
   CalcTotal();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CalcTotal()
{

    unsigned hyper tt = 0, qt;
    while(!TryEnterCriticalSection(&CS)) Sleep(1000);

      for(int i = 1; i < Grid->RowCount; i++)
      {
//        Grid->Cells[PG_ESTIMATED_PRICE_COL][i] = Grid->Cells[PG_PRICE_COL][i];
        if(!Grid->Cells[PG_COST_COL][i].IsEmpty())
        {
          tt += MoneyAshyper(Grid->Cells[PG_COST_COL][i]);
        }
      }
//    }
    // вычитаем оплату подарочными картами
//    tt-=GiftCardCalcPayments();
    TotalSum = MoneyAsString(tt);
    log("total calculated")   ;
  if( tt > 2000000000)
  {
      PlaySound("warning.wav",0,SND_ASYNC);
      MessageBox (GetActiveWindow(),"ВНИМАНИЕ!",
              "Касса не может выбить чек на сумму более 20 млн.руб\nУдалите чек\n", MB_OK);
  }


    if(PayType == NAL_PAYMENT)
    {
      //Запишем скидку
      TOff->Text = MoneyAsString(tt%Round);
      //Пересчитаем с учетом скидки сумму чека
      TSum->Text = MoneyAsString(tt - MoneyAshyper(TOff->Text));
    }
    else
    {
      TSum->Text = TotalSum;
      TOff->Text = MoneyAsString(0);
    }
    TOff->Repaint();
    RecivedCalc();

//    if(TotalSum.Pos(".00") > 0) TotalSum = TotalSum.SubString(1,TotalSum.Length()-3);
    if(GoodPrice.Pos(".00") > 0) GoodPrice = GoodPrice.SubString(1,GoodPrice.Length()-3);

    TextToShow = GoodName.SubString(1,18-GoodPrice.Length());
    while(TextToShow.Length() < 20-GoodPrice.Length()) TextToShow += " ";
    TextToShow += GoodPrice;
    TextToShow += "Всего:";
    while(TextToShow.Length() + TotalSum.Length() < 40) TextToShow += " ";
    TextToShow += TotalSum;

    log("now show on display");
    log(TextToShow);
    ShowOnDisplay(TextToShow.c_str());

    LeaveCriticalSection(&CS);
}
//---------------------------------------------------------------------------
// Поиск товара по считанному ШК в таблице Price
bool __fastcall TMainWindow::Seek(AnsiString Code)
{
bool ret;
bool add5 = false;
AnsiString S,W;
int Gift = 0;

static AnsiString LName,LMeas,LPrice,LIDNom,LCSN,LNDS,LOff;

if(CheckBool(Printing)) return false;

if(Code.Length()==10)  if(GiftCardIsInCheck(Code)) return SeekGiftCard(Code);
if(Code.Length() == 13 && ScanCodeCheckDigit) Code = Code.SubString(1,12);

if(Code != LastScancode )
{
   if(Code.Length() == 26) //Обработка кода весов digi-100
   {
      W = Code.SubString(21,6); //Расширенную часть считаем
      add5 = true; //Признак расширенного штрих-кода
      Code = Code.SubString(4,12);
      Nnum->Text = Code;
   }

   while(Code.Length() < 12) Code += " ";
   // 4-NDS - это исправлено для ОФД.
   // новый НДС
   PriceQuery->SQL->Text = "SELECT ScanCode,Name,Price,Meas,CSN,[Off],4-NDS as NDS,Sklad,Special,sys.fn_varbintohexstr(IDNom) as IDNom,Gift FROM Price WHERE ScanCode = '"+Code+"'";
   // старый НДС
   //PriceQuery->SQL->Text = "SELECT ScanCode,Name,Price,Meas,CSN,[Off],NDS,Sklad,Special,sys.fn_varbintohexstr(IDNom) as IDNom,Gift FROM Price WHERE ScanCode = '"+Code+"'";
   try
   {
      PriceQuery->Active = true;
   }
   catch (EOleException &eException)
   {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return false;
   }
   ret = (PriceQuery->FieldByName("ScanCode")->AsString == Code);

   if(ret)
   {
      LastScancode = PriceQuery->FieldByName("ScanCode")->AsString;
      LName = PriceQuery->FieldByName("Name")->AsString;
      LMeas = PriceQuery->FieldByName("Meas")->AsString;
      LPrice = PriceQuery->FieldByName("Price")->AsString;
      LIDNom = PriceQuery->FieldByName("IDNom")->AsString;
      LCSN = PriceQuery->FieldByName("CSN")->AsString;
      LNDS = MainWindow->PriceQuery->FieldByName("NDS")->AsString;
      LOff = PriceQuery->FieldByName("Off")->AsString;
      Gift = PriceQuery->FieldByName("Gift")->AsInteger;
      LName = LName.TrimRight();
      LMeas = LMeas.TrimRight();
   }
}
else
{
   ret = true;
}

//Проверка НДС
if(ret)
  {
  S = LNDS;
  if( S != "1" && S != "2" && S!="3" )
    {
        MessageBox (GetActiveWindow(),"Сообщите товароведу",
                "Не правильно установлен НДС на этот товар в 1С", MB_OK);
    ret = false;
    }
  }

//Обработка 0 цены
if(ret)
  if( MoneyAshyper(LPrice) == 0)
  {
      PlaySound("warning.wav",0,SND_ASYNC);
      MessageBox (GetActiveWindow(),"ВНИМАНИЕ!",
              "Товар с нулевой ценой, укажите цену или удалите строку", MB_OK);
  }

if(ret)
  {
  S = LName.Trim();
  Name->Caption = S;
//  Present(S.UpperCase(),Gift);


  Unit->Caption = LMeas;
  Price->Text = LPrice;
  IDNom->Caption = LIDNom;
  Qnty->Text = "1";

  if( LCSN == "0" ) cansetnum_flag = false;
  else cansetnum_flag = true;

  if(add5)
   {
   cansetnum_flag = false; //Если количество в штрих-коде, то вручную ввести его нельзя
   if(LMeas == "кг") W = W.SubString(1,3) + "." + W.SubString(4,3);
   Qnty->Text = W;
   }

  NDS = LNDS;

  if( LOff == "1" )
    Name->Caption = "Уценен: "+ Name->Caption;
  }

PriceQuery->Active = false;
PriceQuery->Close();
if (cansetnum_flag && ret && !Delete) MeasureClick(this);
return ret;
}

//---------------------------------------------------------------------------
void __fastcall TMainWindow::CodeEnter(AnsiString Code)
{

    if(Seek(Code))
    {
        if(PresentEnter)
        {
            AddToTable(PresentGrid);   // добавляем в таблицу подарков
            HandInput = !ScannerEnter;
            NowSearching = false;
            return;
        }
        else if(BillPickup)
        {
           if(MoveItemBetweenTable(IDNom->Caption, PickupGrid, DeliveryGrid))
           {
               PlaySound("nnumok.wav",0,SND_ASYNC);
               HandInput = !ScannerEnter;
           }
           else
           {
               PlaySound("oy.wav",0,SND_ASYNC);
           }
        }
        else
        {
            AddToTable(Grid);
            PlaySound("nnumok.wav",0,SND_ASYNC);
            HandInput = !ScannerEnter;
        }
//        SetNum = false;
//        SetPrice = false;
    }
    else if(Code.Length() == 10)
    {  // если нужная сумма после пробития ПК набирается, то идем на печать чека
      if(GridSum(ComboPayGrid, CP_SUM_COL) + RoundSum() == MoneyAshyper(TSum->Text))
      {
         NowSearching = false;
         PayType = GIFT_CARD_PAYMENT;
         CalcTotal();
         ComboPayClick();
      }
    }
    else if(SeekBill(Code))
    {
      ShowDeliveryPanel(true);
      PlaySound("bill.wav",0,SND_ASYNC);
    }
    else if(GetDeliveryDoc(Code, true).DocID != 0)
    {
      ShowDeliveryPanel(true);
      PlaySound("bill.wav",0,SND_ASYNC);
    }
    else
    {
        if(Code.Length() != 10) PlaySound("oy.wav",0,SND_ASYNC);
        //if(Grid->RowCount > 2) Grid->RowCount--;
    }
    NowSearching = false;
}
//---------------------------------------------------------------------------
// выполняется перед выходом из программы
void __fastcall TMainWindow::ExitClick(TObject *Sender)
{
    ArrangeTable();
    Clock->Terminate();
//    Star->Terminate();
    MainWindow->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormCreate(TObject *Sender)
{
    int index;
    int FRT, i;
    Round=1;
    ScreenSaverStarted = false;
    CasName = "";
    CasNumber = 0;
    AnsiString Path = Application->ExeName.SubString(1, Application->ExeName.LastDelimiter("\\"));
    InputFile = Path + "nnum.txt";
    SQLServer = "CASHTEST";
    OutputDir = Path;
    Psw = "0000";
    Department = "";
    ServerDirectory = "\\\\data\\kassa\\";
    SupportEMail = "support@colorlon.ru";
    CommPort = 1;
    CommPortD = 0;
    MstarCom = 1;
    ZKPort = 1;
    FRT = 0;
    Log = false;
    LastScancode = "";
    TextToShow = "                                        ";
    AnsiString s;
    GoodName = "";
    GoodPrice = "";
    PresentEnter = false;
    Session = false;
    InitializeCriticalSection(&CS); log("000");
    hEvent1 = CreateEvent(NULL, false, false, "evBillSuccess");
    hPrnEvent = CreateEvent(NULL, false, false, "evPrintSuccess"); // событие окончания печати чека
    evConnStatus = CreateEvent(NULL, false, false, "ConnectionStatus");
    ProgramPath = ExtractFilePath(Application->ExeName);
    Payments = false;
    ScanCodeCheckDigit = false;
//    DWORD asd = GetTickCount();
// заполняем ячейки заголовков
   ComboPayGrid->ColCount = 14;
   ComboPayGrid->Cells[0][0] = "№";
   ComboPayGrid->Cells[CP_NAME_COL][0] = "Вид расчета";
   ComboPayGrid->Cells[CP_SUM_COL][0] = "Сумма";
   ComboPayGrid->Cells[CP_DEL_COL][0] = "Удалить";
   ComboPayGrid->Cells[CP_STATUS_COL][0] = "Статус";
   ComboPayGrid->Cells[CP_PAY_TYPE_COL][0] = "Тип платежа";
   ComboPayGrid->Cells[CP_RRN_COL][0] = "RRN";
   ComboPayGrid->Cells[CP_ROUND_COL][0] = "Остаток";
   ComboPayGrid->Cells[CP_CARD_ID_COL][0] = "Номер карты";
   ComboPayGrid->Cells[CP_CARD_TYPE_COL][0] = "Тип карты";
   ComboPayGrid->Cells[CP_CARD_NAME_COL][0] = "Имя с карты";
   ComboPayGrid->Cells[CP_HASH_COL][0] = "Hash";
   ComboPayGrid->Cells[CP_CHECK_COL][0] = "Чек";
   ComboPayGrid->Cells[CP_SBOWN_COL][0] = "Признак карты СБ";

   ClearGrid(ComboPayGrid);
//   Заполняем данные по типам платежей
   PayName.insert(std::make_pair(NAL_PAYMENT,"Наличные"));
   PayName.insert(std::make_pair(INTERNATIONAL_CARD_PAYMENT,"Банковская карта"));
   PayName.insert(std::make_pair(GIFT_CARD_PAYMENT,"Подарочная карта"));
   GiftCardM->Visible = false;

//   ListOfColors = new THashedStringList;

    DownBool(Printing);

// считываем INI-файл
    if(FileExists(ProgramPath + "cashier.ini"))
    {
      TIniFile *Ini = new TIniFile(ProgramPath + "cashier.ini");
      TStringList *Header = new TStringList;
      TStringList *HeadBillClose = new TStringList;
//      boost::shared_ptr<TStringList> HeadBillClose(new TStringList);
      Round = atol(Ini->ReadString("General","Round",0).c_str());
      Department = Ini->ReadString("General","Sklad",0);
      MstarCom = Ini->ReadInteger("Registrator","COM",0);
      BaudMStar = Ini->ReadInteger("Registrator","baud",4800);
      if (Ini->ReadString("Registrator","type","") == "РИТЕЙЛ-01Ф"){ FRT = 8; }
      else if(Ini->ReadString("Registrator","type","") == "ШТРИХ-МИНИ-ФР-К"){ FRT = 4; }
      else if (Ini->ReadString("Registrator","type","") == "ПРИМ-08ТК") { FRT = 2; }
      else {FRT = 0;}
      if (Ini->ReadString("Registrator","online","") == "yes") { FROnline  = true;}
      CommPort = Ini->ReadInteger("scanner","COM",0);
      if(Ini->ReadString("scanner","CheckDigit","") == "yes") ScanCodeCheckDigit = true;
      CommPortD = Ini->ReadInteger("Display","COM",0);
      ZKPort = Ini->ReadInteger("goldcrown","COM",0);
      SQLServer = Ini->ReadString("SQLServer","LocalConnectionString","");
      SQLServerCentral = Ini->ReadString("SQLServer","CentralConnectionString","");
      if(Ini->ReadString("Log","enable","no") == "yes") Log = True;
      LogDir = Ini->ReadString("Log","dir","");
      if(ExtractFileDrive(LogDir) == "")
      {
         // относительный путь
         LogDir = ExcludeTrailingBackslash(ProgramPath + LogDir);
         LogDir += "\\";
      }
      else if(ExtractFileDir(LogDir) != "")
      {  //абсолютный путь
         LogDir = ExcludeTrailingBackslash(LogDir);
         LogDir += "\\";
      }
      else
      {
         LogDir = ExcludeTrailingBackslash(ProgramPath);
      }
      OutputDir = Ini->ReadString("General","datadir","c:\\data\\");
      SoundDir = Ini->ReadString("General","SoundDir","Sound");
      Ini->ReadSectionValues("CopyBillHeader", Header);  // считываем все значения в секции
      for(int i = 0; i < 3; i++)  // максимум 3 строки, остальные срежутся
      {
         if (i < Header->Count)
         {
            BillHead[i] = Header->Strings[i].SubString(Header->Strings[i].Pos("=") + 1, Header->Strings[i].Length());
         }
         else
         {
            BillHead[i] = "  ";
         }
      }
      delete Header;
      Header = NULL;
// читаем заголовок закрытия чека (текст в середине чека)
      Ini->ReadSectionValues("BillCloseHeader", HeadBillClose);  // в случае с умным указателем нужно HeadBillClose.get()
      for(int i=0;i<6;i++)    
      {
         if(i < HeadBillClose->Count)
         {
            mfBill.MidText[i] = HeadBillClose->Strings[i].SubString(HeadBillClose->Strings[i].Pos("=")+1, HeadBillClose->Strings[i].Length());
         }
         else
         {
            mfBill.MidText[i]="";
         }
      }
    delete HeadBillClose;
    HeadBillClose = NULL;
    delete Ini;
    Ini = NULL;
    }

    log("Запуск программы. Инициализация считана из файла");

/*    StatusBar->Panels->Items[2]->Text = Department
        + " РГ" + String(MstarCom)
        + " СК" + String(CommPort)
        + " ZK" + String(ZKPort); */

    Grid->Cells[0][0] = "№";
    Grid->Cells[1][0] = "Код";
    Grid->Cells[2][0] = "Наименование";
    Grid->Cells[3][0] = "Ед.изм";
    Grid->Cells[4][0] = "Цена";
    Grid->Cells[5][0] = "Кол-во";
    Grid->Cells[6][0] = "Сумма";

    LoadGridFromFile(Grid, ProgramPath + "gridrows.txt");
    LoadGridFromFile(ComboPayGrid, ProgramPath + "paygrid.txt");

// Инициализация таблицы подарков
    PresentGrid->Visible = true;
    PresentGrid->Cells[0][0] = "№";
    PresentGrid->Cells[1][0] = "Код";
    PresentGrid->Cells[2][0] = "Наименование";
    PresentGrid->Cells[3][0] = "Кол-во";
    PresentGrid->Cells[4][0] = "Ед.изм";
    PresentGrid->Cells[5][0] = "IDNom";
    PresentGrid->Cells[6][0] = "IDStock";
    PresentGrid->Cells[7][0] = "Номинал";

//   GiftCardClearPayments(); //Восстановление платежей по подарочным картам
//   if(FileExists(ProgramPath + "g99.txt"))
//   {
//      GiftCardM->Lines->LoadFromFile(ProgramPath + "g99.txt");
//      log("Платежи по картам восстановлены");
//   }

    ArrangeTable();
    log("Чек сжат");
    log("Сумма чека посчитана");

    CardType = NO_CARD;
    Reader = new TSmartCardReader(true);
    log("Кардридер создан");

    // объект необходим для запуска модулей синхронизации
    CopyRes = new TCopyRes(true);
    log("Задание запуска модулей создано");

    Scan = new TScanner(true);
    Scan->Port = CommPort;
    log("Сканер создан");

    // создание объекта фискального регистратора
    Star = new TMStarF(true);
    Star->Port = MstarCom;
    Star->Baud = BaudMStar;
    Star->FRType = FRT;
    log("Регистратор создан");
// данные чека
    Star->stBill = &mfBill;   // присваиваем указатель, для того чтобы данные структуры были доступны в потоке ФР

    SelectPayTypeForm = new TSelectPayTypeForm(this);
    FormSverka = new TFormSverka(this);

    ClearBillData(); // очищаем структуру
// Инициализация векторов
    Star->frStatus = &frStatus; // статус ФР
    Star->PrintLineData = &PrintLineData;  // Печать номенквлатуры в чеке
    Star->ShadowLineData = &ShadowLineData; // Передача в чек фискальной номенклатуры
    Star->GiftItemData = &GiftItemData; // данные по подарочным картам
    Star->PresentItemData = &PresentItemData; // данные по подаркам

    Clock = new TClockThread(Star, &frStatus, true);  // передаем конструктору потока указатели потока ФР и структуры статуса ФР
    log("Часы созданы");

    switch(FRT)
      {
      case 0:
      default:
        Psw = "0000";
        Star->Psw = "0000";
        break;
      case 2:
        Psw = "AERF";
        Star->Psw = "AERF";
        break;
      case 4: // ШТРИХ-МИНИ-ФР-К
      case 8: // РИТЕЙЛ-01Ф
        Psw = "\x1e\x0\x0\x0";
        Star->Psw = "\x1e\x0\x0\x0";
        break;
      }

    Star->Resume();
    log("Регистратор запущен");
    Scan->Resume();
    log("Сканер запущен");
    Clock->Resume();
    log("Часы запущены");
    Reader->Resume();
    log("Кардридер запущен");
    CopyRes->Resume();
    log("Передача данных запущена");

//   ShowMessage(Star->GetDate());
    Star->FRInit();
    Star->GetVersion();
/*    Star->GetStatus();
    if (Star->Mode == 8)
    {
      Star->ResumePrint();
      Star->DeleteDoc();  // удаляем документ если он не  закрыт из-за сбоя
      Star->GetStatus();
    } */
     log("Номер ФР считан");
//    ShowMessage(Star->FRStatus.FRRegime);
    ParamCount();
    CentralConnection->ConnectionString = SQLServerCentral;
    CashConnection->ConnectionString = SQLServer;
    CashConnection->Connected = true;
    CashConnection->Errors;
    log("СУБД подключена");

  // Считываем параметры командной строки
  NoCard = false;
  for (int i=1;i<=ParamCount();i++)
  {
    s = ParamStr(i);
    if ( s.SubString(1,8) == "/serial:")
      {
      Star->Serial = s.SubString(9,6);
      }
    if ( s.SubString(1,8) == "/nocard") NoCard = true;
  }

   if(!GetSerialID())
   {
     MessageBox(GetActiveWindow(),"Не вижу номер ФР, работа программы невозможна. Необходимо выйти из программы.","Не определился номер регистратора",MB_OK);
     Application->Terminate();
     return;
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
                    case 8:
                    if(Star->Mode == 2)
                    {
                     Name->Caption = "Смена открыта";
                     Session = true;
                    }
                    else if (Star->Mode == 3)
                    {
                       Name->Caption = "Смена не закрыта, 24 часа закончились. Закройте смену!!!";
                       Session = false;
                    }
                    else if (Star->Mode == 4)
                    {
                       Name->Caption = "Смена закрыта";
                       Session = false;
                    }
                    else if (Star->Mode == 9)
                    {
                       Name->Caption = "Общий сбой кассы!!! Требуется технологическое обнуление.";
                       Session = false;
                    }
                    else
                    {
                       Name->Caption = "KKM " + Star->Serial + " не готов - перезайдите в программу.";
                       Session = false;
                    }
/*                    if (Star->Mode == 4)
                    { // доступно для изменения только при закрытой смене
                        Star->StrihWriteTable(6,3,1,"\x00\x00",2); // Налоговая ставка 0%
                        Star->StrihWriteTable(6,3,2,"НДС 0%",7);   // Описание ставки
                        Star->StrihWriteTable(6,2,1,"\xe8\x03",2); // Налоговая ставка 1000 (10%)
                        Star->StrihWriteTable(6,2,2,"НДС 10%",7);  // Описание ставки
                        Star->StrihWriteTable(6,1,1,"\x08\x07",2); // Налоговая ставка 1800 (18%)
                        Star->StrihWriteTable(6,1,2,"НДС 18%",7);  // Описание ставки   // новый НДС
                     } */

//                    try
//                    {
                        Star->StrihWriteTable(5,3,1,"КAРТОЙ VISA/MC",14);
                        Star->StrihWriteTable(5,4,1,"ДРУГИМИ КАРТАМИ",15);
                        Star->StrihWriteTable(5,14,1,"ПОДАРОЧНЫМИ КАРТАМИ",20);
                        Star->StrihWriteTable(1,1,1,"\x01",1);     // номер кассы 1
                        Star->StrihWriteTable(1,1,5,"\x02",1);     // не используется в новой версии
                        Star->StrihWriteTable(1,1,35, "\x01",1);    // печать типов оплаты 1
                        Star->StrihWriteTable(1,1,6,"\x00",1);     // работа с денежным ящиком 0
                        Star->StrihWriteTable(1,1,7,"\x01",1);     // отрезка чека 1
                        Star->StrihWriteTable(1,1,20,"\x01",1);    // печать чека по закрытию 1
                        Star->StrihWriteTable(1,1,21,"\x01",1);    // промотка перед отрезкой 1
                        Star->StrihWriteTable(1,1,15,"\x00",1);    // автоматический перевод времени 0
                        Star->StrihWriteTable(1,1,17,"\x00",1);    // печать заголовка 1
                        Star->StrihWriteTable(17,1,3,"\x02",1);    // Режим исчисления скидок
                        Star->StrihWriteTable(17,1,12,"\x0",1);
                        Star->StrihWriteTable(1,1,41,"\x01",1);    // добавляем квадратный штрих
                        for (int i = 12; i <= 14; i++)  // строки с 12 по 14 включительно печатаются в чеке
                        {
                           Star->StrihWriteTable(4,i,1,BillHead[i-12].c_str(),(BillHead[i-12]).Length()+1); // Заголовок чека
                        }
//                     }
//                     catch(const My::TCustomException* E)
//                     {
//                        log("Ошибка записи в таблицу " + E->Message + " " + IntToStr(E->ErrorCode));
//                        ShowMessage(E.Message + " " + IntToStr(E.ErrorCode));
//                     }



                    delete BillHead;
                    break;
           }
   CheckStockDataModel(); // проверка и создание таблиц для акций если их нет
    PriceQuery->SQL->Text = "select id, Sklad, Name from Sklad where Sklad=0x" + Department.Trim();
    try
    {
      PriceQuery->Active = true;
      DepartmentId = PriceQuery->FieldByName("id")->AsString;
      DepartmentName = PriceQuery->FieldByName("Name")->AsString;
      PriceQuery->Active = false;
    }
    catch (EOleException &eException)
    {
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      AnsiString Str;
      Str ="Измените описание поля [Sklad]="+Department+" длина="+String(Department.Length())+" в cashier.ini";
      MessageBox (GetActiveWindow(),Str.c_str(),"Неправильно описан склад", MB_OK);
      PriceQuery->Active = false;
      return;
    }
    StatusBar->Panels->Items[2]->Text = DepartmentName;

/*
    if(Department.Length() != 32)
    {
      AnsiString Str;
      Str ="Измените описание поля [Sklad]="+Department+" длина="+String(Department.Length())+" в cashier.ini";
      MessageBox (GetActiveWindow(),Str.c_str(),"Неправильно описан склад", MB_OK);
      return;
    } */
  PriceQuery->SQL->Text = "IF NOT EXISTS (SELECT table_name FROM information_schema.tables WHERE table_name = 'Payment') BEGIN CREATE TABLE [dbo].[payment]([BillDateTime] [datetime],[SCash] [char](8),[PayType] [char](2),[Sklad] [binary](16),[ BillNumber] [char](18),[Summ] [numeric](14, 2),[Operator] [char](15)) END";
  try
    {
    PriceQuery->ExecSQL();
    }
catch (EOleException &eException)
{
   AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
   log(errormsg);
}

//Delivery dl =  GetDeliveryDoc("131893833821", true);
//DeliveryPrint(&dl);
   ShowOnDisplay("");

//   Sverka(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::GetKKMError()
{
  AnsiString str, log;
  int hex;
  str = "";
  switch(Star->FRType)
  {
    case 4:
    case 8:
    hex = frStatus.OperationResult;
      switch (hex)
        {
        case 0x01:
            str = "Неисправен накопитель ФП1, ФП2 или часы";
            break;
        case 0x02:
            str = "ФП Отсутствует ФП 1";
            break;
        case 0x03:
            str = "ФП Отсутствует ФП 2";
            break;
        case 0x04:
            str = "ФП Некорректные параметры в команде обращения к ФП";
            break;
        case 0x05:
            str = "ФП Нет запрошенных данных";
            break;
        case 0x06:
            str = "ФП ФП в режиме вывода данных";
            break;
        case 0x07:
            str = "ФП Некорректные параметры в команде для данной реализации ФП";
            break;
        case 0x08:
            str = "ФП Команда не поддерживается в данной реализации ФП";
            break;
        case 0x09:
            str = "ФП Некорректная длина команды";
            break;
        case 0x0A:
            str = "ФП Формат данных не BCD";
            break;
        case 0x0B:
            str = "ФП Неисправна ячейка памяти ФП при записи итога";
            break;
        case 0x11:
            str = "ФП Не введена лицензия";
            break;
        case 0x12:
            str = "ФП Заводской номер уже введен";
            break;
        case 0x13:
            str = "ФП Текущая дата меньше даты последней записи в ФП";
            break;
        case 0x14:
            str = "ФН Исчерпан ресурс хранения";
            break;
        case 0x15:
            str = "ФП Смена уже открыта";
            break;
        case 0x16:
            str = "ФП Смена не открыта";
            break;
        case 0x17:
            str = "ФП Номер первой смены больше номера последней смены";
            break;
        case 0x18:
            str = "ФП Дата первой смены больше даты последней смены";
            break;
        case 0x19:
            str = "ФП Нет данных в ФП";
            break;
        case 0x1A:
            str = "ФП Область перерегистраций в ФП переполнена";
            break;
        case 0x1B:
            str = "ФП Заводской номер не введен";
            break;
        case 0x1C:
            str = "ФП В заданном диапазоне есть поврежденная запись";
            break;
        case 0x1D:
            str = "ФП Повреждена последняя запись сменных итогов";
            break;
        case 0x1E:
            str = "ФП Область перерегистраций ФП переполнена";
            break;
        case 0x1F:
            str = "ФП Отсутствует память регистров";
            break;
        case 0x20:
            str = "ФП Переполнение денежного регистра при добавлении";
            break;
        case 0x21:
            str = "ФП Вычитаемая сумма больше содержимого денежного регистра";
            break;
        case 0x22:
            str = "ФП Неверная дата";
            break;
        case 0x23:
            str = "ФП Нет записи активизации";
            break;
        case 0x24:
            str = "ФП Область активизаций переполнена";
            break;
        case 0x25:
            str = "ФП Нет активизации с запрашиваемым номером";
            break;
        case 0x26:
            str = "ФР Вносимая клиентом сумма меньше суммы чека";
            break;
        case 0x2B:
            str = "Невозможно отменить предыдущую команду";
            break;
        case 0x2C:
            str = "ФР Обнулённая касса ( гашение невозможно )";
            break;
        case 0x2D:
            str = "ФР Сумма чека по секции меньше суммы сторно";
            break;
        case 0x2E:
            str = "ФР В ФР нет денег для выплаты";
            break;
        case 0x30:
            str = "ФР ФР заблокирован , ждет ввода пароля налогового инспектора";
            break;
        case 0x32:
            str = "ФР Требуется выполнение общего гашения";
            break;
        case 0x33:
            str = "ФР Некорректные параметры в команде";
            break;
        case 0x34:
            str = "ФР Нет данных";
            break;
        case 0x35:
            str = "ФР Некорректный параметр при данных настройках";
            break;
        case 0x36:
            str = "ФР Некорректные параметры в команде для данной реализации ФР";
            break;
        case 0x37:
            str = "ФР Команда не поддерживается в данной реализации ФР";
            break;
        case 0x38:
            str = "ФР Ошибка в ПЗУ";
            break;
        case 0x39:
            str = "ФР Внутренняя ошибка ПО ФР";
            break;
        case 0x3A:
            str = "ФР Переполнение накопления по надбавкам в смене";
            break;
        case 0x3B:
            str = "ФР Переполнение накопления в смене";
            break;
        case 0x3C:
            str = "ФР ЭКЛЗ : неверный регистрационный номер";
            break;
        case 0x3D:
            str = "ФР Смена не открыта - операция невозможна";
            break;
        case 0x3E:
            str = "ФР Переполнение накопления по секциям в смене";
            break;
        case 0x3F:
            str = "ФР Переполнение накопления по скидкам в смене";
            break;
        case 0x40:
            str = "ФР Переполнение диапазона скидок";
            break;
        case 0x41:
            str = "ФР Переполнение диапазона оплаты наличными";
            break;
        case 0x42:
            str = "ФР Переполнение диапазона оплаты типом 2";
            break;
        case 0x43:
            str = "ФР Переполнение диапазона оплаты типом 3";
            break;
        case 0x44:
            str = "ФР Переполнение диапазона оплаты типом 4";
            break;
        case 0x45:
            str = "Cумма всех типов оплаты меньше итога чека";
            break;
        case 0x46:
            str = "ФР Не хватает наличности в кассе";
            break;
        case 0x47:
            str = "ФР Переполнение накопления по налогам в смене";
            break;
        case 0x48:
            str = "ФР Переполнение итога чека";
            break;
        case 0x49:
            str = "ФР Операция невозможна в открытом чеке данного типа";
            break;
        case 0x4A:
            str = "ФР Открыт чек ? операция невозможна";
            break;
        case 0x4B:
            str = "ФР Буфер чека переполнен";
            break;
        case 0x4C:
            str = "ФР Переполнение накопления по обороту налогов в смене";
            break;
        case 0x4D:
            str = "ФР Вносимая безналичной оплатой сумма больше суммы чека";
            break;
        case 0x4E:
            str = "ФР Смена превысила 24 часа";
            break;
        case 0x4F:
            str = "ФР Неверный пароль";
            break;
        case 0x50:
            str = "ФР Идет печать предыдущей команды";
            break;
        case 0x51:
            str = "ФР Переполнение накоплений наличными в смене";
            break;
        case 0x52:
            str = "ФР Переполнение накоплений по типу оплаты 2 в смене";
            break;
        case 0x53:
            str = "ФР Переполнение накоплений по типу оплаты 3 в смене";
            break;
        case 0x54:
            str = "ФР Переполнение накоплений по типу оплаты 4 в смене";
            break;
        case 0x55:
            str = "ФР Чек закрыт ? операция невозможна";
            break;
        case 0x56:
            str = "ФР Нет документа для повтора";
            break;
        case 0x57:
            str = "ФР ЭКЛЗ : количество закрытых смен не совпадает с ФП";
            break;
        case 0x58:
            str = "ФР Ожидание команды продолжения печати";
            Star->ResumePrint();
            break;
        case 0x59:
            str = "ФР Документ открыт другим оператором";
            break;
        case 0x5A:
            str = "ФР Скидка превышает накопления в чеке";
            break;
        case 0x5B:
            str = "ФР Переполнение диапазона надбавок";
            break;
        case 0x5C:
            str = "ФР Понижено напряжение 24";
            break;
        case 0x5D:
            str = "ФР Таблица не определена";
            break;
        case 0x5E:
            str = "ФР Некорректная операция";
            break;
        case 0x5F:
            str = "ФР Отрицательный итог чека";
            break;
        case 0x60:
            str = "ФР Переполнение при умножении";
            break;
        case 0x61:
            str = "ФР Переполнение диапазона цены";
            break;
        case 0x62:
            str = "ФР Переполнение диапазона количества";
            break;
        case 0x63:
            str = "ФР Переполнение диапазона отдела";
            break;
        case 0x64:
            str = "ФР ФП отсутствует";
            break;
        case 0x65:
            str = "ФР Не хватает денег в секции";
            break;
        case 0x66:
            str = "ФР Переполнение денег в секции";
            break;
        case 0x67:
            str = "ФР Ошибка связи с ФП";
            break;
        case 0x68:
            str = "ФР Не хватает денег по обороту налогов";
            break;
        case 0x69:
            str = "ФР Переполнение денег по обороту налогов";
            break;
        case 0x6A:
            str = "ФР Ошибка питания в момент ответа по I2C";
            break;
        case 0x6B:
            str = "ФР Нет чековой ленты";
            break;
        case 0x6C:
            str = "ФР Нет контрольной ленты";
            break;
        case 0x6D:
            str = "ФР Не хватает денег по налогу";
            break;
        case 0x6E:
            str = "ФР Переполнение денег по налогу";
            break;
        case 0x6F:
            str = "ФР Переполнение по выплате в смене";
            break;
        case 0x70:
            str = "ФР Переполнение ФП";
            break;
        case 0x71:
            str = "ФР Ошибка отрезчика";
            break;
        case 0x72:
            str = "ФР Команда не поддерживается в данном подрежиме";
            break;
        case 0x73:
            str = "ФР Команда не поддерживается в данном режиме";
            break;
        case 0x74:
            str = "ФР Ошибка ОЗУ";
            break;
        case 0x75:
            str = "ФР Ошибка питания";
            break;
        case 0x76:
            str = "ФР Ошибка принтера: нет импульсов с тахогенератора";
            break;
        case 0x77:
            str = "ФР Ошибка принтера : нет сигнала с датчиков";
            break;
        case 0x78:
            str = "ФР Замена ПО";
            break;
        case 0x79:
            str = "ФР Замена ФП";
            break;
        case 0x7A:
            str = "ФР Поле не редактируется";
            break;
        case 0x7B:
            str = "ФР Ошибка оборудования";
            break;
        case 0x7C:
            str = "ФР Не совпадает дата";
            break;
        case 0x7D:
            str = "ФР Неверный формат даты";
            break;
        case 0x7E:
            str = "ФР Неверное значение в поле длины";
            break;
        case 0x7F:
            str = "ФР Переполнение диапазона итога чека";
            break;
        case 0x80:
            str = "ФР Ошибка связи с ФП";
            break;
        case 0x81:
            str = "ФР Ошибка связи с ФП";
            break;
        case 0x82:
            str = "ФР Ошибка связи с ФП";
            break;
        case 0x83:
            str = "ФР Ошибка связи с ФП";
            break;
        case 0x84:
            str = "ФР Переполнение наличности";
            break;
        case 0x85:
            str = "ФР Переполнение по продажам в смене";
            break;
        case 0x86:
            str = "ФР Переполнение по покупкам в смене";
            break;
        case 0x87:
            str = "ФР Переполнение по возвратам продаж в смене";
            break;
        case 0x88:
            str = "ФР Переполнение по возвратам покупок в смене";
            break;
        case 0x89:
            str = "ФР Переполнение по внесению в смене";
            break;
        case 0x8A:
            str = "ФР Переполнение по надбавкам в чеке";
            break;
        case 0x8B:
            str = "ФР Переполнение по скидкам в чеке";
            break;
        case 0x8C:
            str = "ФР Отрицательный итог надбавки в чеке";
            break;
        case 0x8D:
            str = "ФР Отрицательный итог скидки в чеке";
            break;
        case 0x8E:
            str = "ФР Нулевой итог чека";
            break;
        case 0x8F:
            str = "ФР Касса не фискализирована";
            break;
        case 0x90:
            str = "ФР Поле превышает размер , установленный в настройках";
            break;
        case 0x91:
            str = "ФР Выход за границу поля печати при данных настройках шрифта";
            break;
        case 0x92:
            str = "ФР Наложение полей";
            break;
        case 0x93:
            str = "ФР Восстановление ОЗУ прошло успешно";
            break;
        case 0x94:
            str = "ФР Исчерпан лимит операций в чеке";
            break;
        case 0xA0:
            str = "ФР Ошибка связи с ЭКЛЗ";
            break;
        case 0xA1:
            str = "ФР ЭКЛЗ отсутствует";
            break;
        case 0xA2:
            str = "ЭКЛЗ ЭКЛЗ : Некорректный формат или параметр команды";
            break;
        case 0xA3:
            str = "ЭКЛЗ Некорректное состояние ЭКЛЗ";
            break;
        case 0xA4:
            str = "ЭКЛЗ Авария ЭКЛЗ";
            break;
        case 0xA5:
            str = "ЭКЛЗ Авария КС в составе ЭКЛЗ";
            break;
        case 0xA6:
            str = "ЭКЛЗ Исчерпан временной ресурс ЭКЛЗ";
            break;
        case 0xA7:
            str = "ЭКЛЗ ЭКЛЗ переполнена";
            break;
        case 0xA8:
            str = "ЭКЛЗ ЭКЛЗ : Неверные дата и время";
            break;
        case 0xA9:
            str = "ЭКЛЗ ЭКЛЗ : Нет запрошенных данных";
            break;
        case 0xAA:
            str = "ЭКЛЗ Переполнение ЭКЛЗ ( отрицательный итог документа )";
            break;
        case 0xB0:
            str = "ФР ЭКЛЗ : Переполнение в параметре количество";
            break;
        case 0xB1:
            str = "ФР ЭКЛЗ : Переполнение в параметре сумма";
            break;
        case 0xB2:
            str = "ФР ЭКЛЗ : Уже активизирована";
            break;
        case 0xC0:
            str = "ФР Контроль даты и времени ( дату и время )";
            break;
        case 0xC1:
            str = "ФР ЭКЛЗ : суточный отчёт с гашением прервать нельзя";
            break;
        case 0xC2:
            str = "ФР Превышение напряжения в блоке питания";
            break;
        case 0xC3:
            str = "ФР Несовпадение итогов чека и ЭКЛЗ";
            Star->Print("\nФР Несовпадение итогов чека и ЭКЛЗ\n\n",true);
            Star->XReport();
            break;
        case 0xC4:
            str = "ФР Несовпадение номеров смен";
            break;
        case 0xC5:
            str = "ФР Буфер подкладного документа пуст";
            break;
        case 0xC6:
            str = "ФР Подкладной документ отсутствует";
            break;
        case 0xC7:
            str = "ФР Поле не редактируется в данном режиме";
            break;
        case 0xFD:
            str = "Регистратор не может закрыть чек";
            break;
        case 0xFE:
            str = "Регистратор не принимает команду";
            break;
        case 0xFF:
            str = "Регистратор не отвечает";
            break;
        default:
            str = "Неизвестная ошибка";
            break;
        }
    break;
  }

    Name->Caption = str;
    Name->Repaint();

    log = "\n\n";
    log += str;
    log += "\nРезультат операции: ";
    log += Star->KKMResult;
    log += "\nСостояние ККМ: ";
    log += Star->KKMStatus;
    log += "\nСостояние принтера: ";
    log += Star->PrinterStatus;

    PlaySound("oy.wav",0,SND_ASYNC);

//    Star->DeleteDoc();
//    Star->CRLF();

    LPSTR aszMsg[1];
    str = log;
    str += "\n";
    str += CasName;
    str += " (";
    str += String(CasNumber);
    str += ")";
    aszMsg[0] = str.c_str();
//    HANDLE hEventLog = RegisterEventSource(NULL, MainWindow->Caption.c_str());
//    ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, 0, hex.ToInt(), NULL,
//        1, 0, (LPCTSTR *) aszMsg, NULL);
//    DeregisterEventSource(hEventLog);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::KeyPressDelete(char Key)
{
    String str, code;
    //while(!TryEnterCriticalSection(&CS)) Sleep(1000);
    //LeaveCriticalSection(&CS);

/*    unsigned hyper pr = 0; */
    if(Key == 27)
    {
        DeleteMode = 0;
        Delete = false;
        NewCode = true;
        NewPrice = false;
        InputQnty = false;
        ClearForm();
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
                    PlaySound("delnnum.wav",0,SND_ASYNC);
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
                     PlaySound("delnnum.wav",0,SND_ASYNC);
                    if(CardType == MANAGER_CARD) DeleteMode = 1;
                    else DeleteMode = 2;
                }
                else
                {
                    KeyPressDelete(27);
                    PlaySound("oy.wav",0,SND_ASYNC);
                }
                NowSearching = false;
                return;
            }
            if(isdigit(Key)) str += Key;
            if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
            Nnum->Text = str;
                        // выводим подсказку
               if(str.Length() == 12 && ScanCodeCheckDigit)
               {
                  Name->Caption = "Завершаюшая цифра " + AnsiString(CheckSum(str));
               }
               else if(str.Length() == 13 && ScanCodeCheckDigit)
               {
                  if(CheckSumEAN(str)) Name->Caption = "";
                  else Name->Caption = "Ошибка!!! Завершаюшая цифра " + AnsiString(CheckSum(str));
               }
               else
               {
                  Name->Caption = "";
               }

            return;
        case 2:     //****
            str = Qnty->Text;
            if(Key == 13 && str.Length() > 0)
            {
                    if(Nnum->Text.Length() == 13 && ScanCodeCheckDigit) code = Nnum->Text.SubString(1,12);
                    else code = Nnum->Text;
                    for(int i = 1; i < Grid->RowCount; i++)
                    {
                        if
                            (
                            code == Grid->Cells[PG_CODE_COL][i]
                            && MoneyAshyper(Price->Text) == MoneyAshyper(Grid->Cells[PG_PRICE_COL][i])
                            && QuantityAshyper(Qnty->Text) <= QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i])
                            )
                            {
                            Grid->Cells[5][i] = QuantityAsString(QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i])
                                -  QuantityAshyper(Qnty->Text));
                            Grid->Cells[6][i] = MoneyAsString( X(MoneyAshyper(Grid->Cells[PG_PRICE_COL][i]),QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i])));
                            if(MoneyAshyper(Grid->Cells[PG_COST_COL][i]) <= 0)
                            {
                                for(int k = i; k < Grid->RowCount-1; k++)
                                    for(int j = 1; j <= Grid->ColCount; j++)
                                        Grid->Cells[j][k] = Grid->Cells[j][k+1];
                                Grid->Rows[Grid->RowCount-1]->Clear();
                                if(Grid->RowCount > 2) Grid->RowCount--;
                            }
                            CalcTotal();
                            PlaySound("deleted.wav",0,SND_ASYNC);

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
                            Name->Caption = "Удалено";
//                            for(int i = 0; i <= Grid->ColCount; i++)
//                            {
//                                str = "g" + String(i) + ".txt";
//                                Grid->Cols[i]->SaveToFile(str);
//                            }
                           SaveGridToFile(Grid, ProgramPath + "gridrows.txt");
//                           GiftCardM->Lines->SaveToFile(ProgramPath + "g99.txt");
                            return;
                        }
                    }
                    PlaySound("oy.wav",0,SND_ASYNC);
                    KeyPressDelete(27);
                    Name->Caption = "Неверные данные для удаления";
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
// отрабатываем нажатие клавиш на форме, так же обрабатывается ввод со сканера ШК
void __fastcall TMainWindow::FormKeyPress(TObject *Sender, char &Key)
{
    if(CheckBool(Printing) || NowSearching ) return;

    if(GetPayStatus(ComboPayGrid, 1) == CP_OLD_STATUS)  // очищаем платежи со со старыми статусами
    {
      ClearGrid(ComboPayGrid);
      SaveGridToFile(ComboPayGrid, ProgramPath + "paygrid.txt");
    }
/*    if(Payments && Nnum->Text.Length() > 10)
    {
      Name->Caption = "Сканирование товара только после отмены платежа!";
      return;
    }        */

    String str;
    unsigned hyper pr;// = 0;

    if(Delete)
    {
        KeyPressDelete(Key);
        return;
    }
    if(!Session)  // если смена не открыта
    {
      PlaySound("oy.wav",0,SND_ASYNC);
        Name->Caption = "Не открыта смена";
        return;
    }
    if(NewPrice)   // ввод отличной от price цены
    {
        str = Price->Text;
        if(Key == 27)      // Esc
        {
            NewCode = true;
            NewPrice = false;
            InputQnty = false;
            ClearForm();
        }
        if(Key == 13 && str.Length() > 0)  //Enter и если строка не пуста
        {
            pr = MoneyAshyper(str); // цена
            if(pr > 0)
            {
                NewPrice = false;
                SetPrice = false;
                PlaySound("nnumok.wav",0,SND_ASYNC);
                AlterLastRow(Grid);
                CalcTotal();
            }
            else PlaySound("oy.wav",0,SND_ASYNC);
            return;
        }
        if(isdigit(Key) || Key == '.') str += Key;
        if(Key == 8 && str.Length() > 0) str.Delete(str.Length(),1);
        Price->Text = str;
        return;
    }
    if(!InputQnty)        // если не вводим количество
    {
        if(!NewCode) str = Nnum->Text;  // если не вводим новый код, то считываем строку из Nnum (код товара)
        else str = "";
        if(Key == 32) {ArrangeTable(); return;}
        if(Key == 27)  // Esc
        {                    // очищаем все
            if(BillPickup)
            {
               if(MessageBox (GetActiveWindow(), "Уверены в закрытии формы?",
                  "Подтверждение", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
               {
                  ClearForm();
                  ShowDeliveryPanel(false);
               }
            }
            else
            {
               NewCode = true;
               NewPrice = false;
               InputQnty = false;
               ClearForm();
               PresentEnter = false;
               InfoShow(PresentEnter, false);
               ClearGrid(PresentGrid);
             }

        }
        // основное условие штатного ввода товара
        if(Key == 13 && str.Length() > 0)   //
        {
            if(str.Length() > 10 && Payments && !PresentEnter)
            {
               Name->Caption = "Добавление товаров только после отмены платежей!";
               return;
            }
            NowSearching = true;  // устанавливаем статус поиска кода
            if (CheckSumEAN(str))
            {
               if(str.Length() == 13 && ScanCodeCheckDigit) {str = str.SubString(1,12);}
            }
            else
            {
               Name->Caption = "Ошибка чтения штрихкода";
               PlaySound("oy.wav",0,SND_ASYNC);
               NewCode = true;
               NewPrice = false;
               InputQnty = false;
               NowSearching = false;
               return;
            }
            CodeEnter(str);       // товар с кодом вносится в таблицу чека
            NewCode = true;       // готов для ввода кода
            CalcTotal();          // Вычисление суммы
            NowSearching = false; // возвращаем статус поиска
            return;
        }
        if(isdigit(Key))
        {
            if(NewCode)
            {
                NewCode = false;
                ClearForm();
                TSum->Text = "0.00";
                Recived->Text = "0.00";
                Change->Text = "0.00";
                TOff->Text = "0.00";
                ArrangeTable();
            }
            str += Key;
            if(str.Length() == 12 && ScanCodeCheckDigit)
            {
               Name->Caption = "Завершаюшая цифра " + AnsiString(CheckSum(str));
            }
            else if(str.Length() == 13 && ScanCodeCheckDigit)
            {
               if(CheckSumEAN(str)) Name->Caption = "";
               else Name->Caption = "Ошибка!!! Завершаюшая цифра " + AnsiString(CheckSum(str));
            }

        }
        if(Key == 8 && str.Length() > 0)
        {
            str.Delete(str.Length(),1);
            if(str.Length() == 12 && ScanCodeCheckDigit){ Name->Caption = "Завершаюшая цифра " + AnsiString(CheckSum(str));}
            else if(str.Length() < 12) { Name->Caption = "";}
        }
        Nnum->Text = str;
    }
    else
    {
        str = Qnty->Text;
        if(Key == 27) return;
        if(Key == 13 && str.Length() > 0)
        {
            pr = MoneyAshyper(str);
            if(pr > 0)
            {

                if(BillPickup)
                {
                  MoveItemBetweenTable(IDNom->Caption, PickupGrid, DeliveryGrid);
                }
                else
                {
                  AlterLastRow(Grid);
                  CalcTotal();
                }
                InputQnty = false;
                SetNum = false;
                Qnty->Color = clBtnFace;
                PlaySound("nnumok.wav",0,SND_ASYNC);
            }
            else PlaySound("oy.wav",0,SND_ASYNC);
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
     int summ = 0;
     PresentGrid->Top = TSum->Top;
     PresentGrid->ColWidths[0] = 30;
     PresentGrid->ColWidths[1] = 130;
     PresentGrid->Left = TSum->Left + TSum->Width + 3;
//     PresentGrid->Width = GiftCardM->Left - PresentGrid->Left - 3;
     PresentGrid->ColWidths[2] = PresentGrid->Width - PresentGrid->ColWidths[1] - PresentGrid->ColWidths[0] - 10;
     Grid->Height = PresentGrid->Top - Grid->Top - 3;
     Grid->ColWidths[0] = 30;
     Grid->ColWidths[PG_CODE_COL] = 140;
     Grid->ColWidths[PG_MEASURE_COL] = 60;
     Grid->ColWidths[PG_PRICE_COL] = 90;
     Grid->ColWidths[PG_QUANTITY_COL] = 80;
     Grid->ColWidths[PG_COST_COL] = 100;
     for(int i = 0; i < 7; i++)
     {
        summ += Grid->ColWidths[i];
     }
     summ -= Grid->ColWidths[2];
     Grid->ColWidths[2] = ClientWidth - summ - 10;
    StatusBar->Panels->Items[0]->Width =
        ClientWidth
        - StatusBar->Panels->Items[1]->Width
        - StatusBar->Panels->Items[2]->Width
        - StatusBar->Panels->Items[3]->Width
        - StatusBar->Panels->Items[4]->Width ;
    Name->Width = ClientWidth - Name->Left*2;
    PresentLabel->Left = ClientWidth - PresentLabel->Width - 10;
    PresentLabel->Top = Grid->Top - PresentLabel->Height - 5;
    ComboPayGrid->Top = TSum->Top;
    ComboPayGrid->Left = PresentGrid->Left + PresentGrid->Width + 3;
    ComboPayGrid->Width = MainWindow->Width - ComboPayGrid->Left - 6;
    ComboPayGrid->Height = StatusBar->Top - ComboPayGrid->Top - 3;
    ComboPayGrid->ColWidths[0] = 30;
    ComboPayGrid->ColWidths[CP_DEL_COL] = 40;
    ComboPayGrid->ColWidths[CP_NAME_COL] = (ComboPayGrid->Width - ComboPayGrid->ColWidths[0] - ComboPayGrid->ColWidths[3])/2;
    ComboPayGrid->ColWidths[CP_SUM_COL] = (ComboPayGrid->Width - ComboPayGrid->ColWidths[0] - ComboPayGrid->ColWidths[3])/2;
    // подстраиваем поле ШК под 12 или 13 символов в зависимости от настройки
    if(ScanCodeCheckDigit) Nnum->Width = 149; else Nnum->Width = 139;
    InitDeliveryPanel();
//    ShowDeliveryPanel(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormShow(TObject *Sender)
{
    Grid->SetFocus();
    MainWindow->Show();
    GetSverka(true);
}
//---------------------------------------------------------------------------
// обработка картридера для регистрации
void __fastcall TMainWindow::ReaderAnswer(hyper result)
{
    String str;
    int i;
    if(result == -2146434967)    // фильтруем часто возникающую ошибку
    {
        scflag = true;
        return;
    }
    else if(result == 1  && scflag)
    {
        scflag = false;
        return;
    }
    if(result == 1)
    {
        str = String(Reader->Data);
        PlaySound("cardout.wav",0,SND_ASYNC);
        CardType = NO_CARD;
        if(ScreenSaverStarted && !NoCard)
          {
          Saver->Text = CasName + " ушел(а) "+DateTimeToStr(Now());
          Saver->Show();
          }
    }
    else if(Reader->Data[0] == 'K')
    {
        CasName = String(&Reader->Data[5]);
        CasNumber = AnsiString(&Reader->Data[1]).ToInt();
        str = "Подключился кассир " + CasName;
        PlaySound("cardin.wav",0,SND_ASYNC);
        CardType = KASSIR_CARD;
        if(ScreenSaverStarted) Saver->Close();
    }
    else if(Reader->Data[0] == 'M')
    {
        CasName = String(&Reader->Data[5]);
        CasNumber = AnsiString(&Reader->Data[1]).ToInt();
        str = "Подключился менеджер " + CasName;
        PlaySound("cardin.wav",0,SND_ASYNC);
        CardType = MANAGER_CARD;
        if(ScreenSaverStarted) Saver->Close();
    }
    else
    {
        str = "неверные данные";
        PlaySound("oy.wav",0,SND_ASYNC);
        CardType = ILLEGAL_CARD;
        if(ScreenSaverStarted)
        {
          Saver->Text = "Ошибка ридера смарт-карт: " + String(Reader->Data);
          Saver->Show();
        }
    }

    for(i=0;i<CasName.Length() && (unsigned char)CasName.c_str()[i]>= (unsigned char)0x20 ; i++);
    if((i < CasName.Length() && CasName.Length() > 0)||(CasName.Length() > 32))
      {
        CasName = "";
        str = "неверные данные";
        PlaySound("oy.wav",0,SND_ASYNC);
        CardType = ILLEGAL_CARD;
        if(ScreenSaverStarted)
        {
          Saver->Text = "Ошибка ридера, вставьте карточку повторно";
          Saver->Show();
        }
      }

    log("Cardreader  " + CasName);
    AnsiString Kassir = CasName;
    Star->StrihWriteTable(2,30,2,Kassir.c_str(),Kassir.Length()+1);
    StatusBar->Panels->Items[0]->Text = str;

    if(NoCard || CardType == MANAGER_CARD || CardType == KASSIR_CARD) ShowOnDisplay("  ДОБРО ПОЖАЛОВАТЬ!");
    else ShowOnDisplay("       КАССА            НЕ РАБОТАЕТ");
    scflag = false;
}
//---------------------------------------------------------------------------
// установка количества  F2
void __fastcall TMainWindow::MeasureClick(TObject *Sender)
{
    if(PresentEnter) return;
    if(!NoCard && CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    if(!cansetnum_flag && CardType == KASSIR_CARD) return;
    if(Nnum->Text.IsEmpty()) return;
    PlaySound("k.wav",0,SND_SYNC);
    InputQnty = true;
    Qnty->Color = clLime;
    SetNum = true;
    Qnty->Text = "";
}
//---------------------------------------------------------------------------
// изменения цены  F4
void __fastcall TMainWindow::PriceChangeClick(TObject *Sender)
{
    if(PresentEnter || BillPickup) return;
    if(CardType != MANAGER_CARD) return;
    if(Nnum->Text.IsEmpty()) return;
    NewPrice = true;
    SetPrice = true;
    Price->Text = "";
}
//---------------------------------------------------------------------------
// удаление всего списка покупок
void __fastcall TMainWindow::DeleteAllClick(TObject *Sender)
{
    if(CardType != MANAGER_CARD) return;
    if(Payments)
    {
       Name->Caption = "Удаление списка только после отмены всех платежей!";
       return;
    }
    if
    (
        MessageBox (GetActiveWindow(), "Удалить весь список?",
            "Подтверждение", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    ) return;

    PresentLabel->Visible = false;
    PresentEnter = false;
    ArrangeTable();
//    if(Grid->RowCount > 2) Grid->RowCount--;
    NewCode = true;
    NewPrice = false;
    InputQnty = false;
    Qnty->Color = clBtnFace;
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
    // подарки тоже скидываем
    for(int i = 1; i < PresentGrid->RowCount; i++)
    {
        file << Star->Serial.c_str()
            << "\t Подарки"
            << "\t"
            << dtPresent.DateTimeString().c_str() << "\t"
            << PresentGrid->Cells[1][i].c_str() << "\t"
            << PresentGrid->Cells[2][i].c_str() << "\t"
            << PresentGrid->Cells[3][i].c_str() << "\t"
            << PresentGrid->Cells[4][i].c_str() << "\t"
            << PresentGrid->Cells[5][i].c_str() << "\t"
            << PresentGrid->Cells[6][i].c_str() << endl;
    }
    file.close();
//    SendDeletedBill();

//    for(int i = 1; i < Grid->RowCount; i++)
//        Grid->Rows[i]->Clear();
//   Grid->RowCount = 1;
//   PresentGrid->RowCount = 1;
   ClearGrid(Grid);
   ClearGrid(PresentGrid);
//   while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("107");
//   GiftCardClearPayments();
//   LeaveCriticalSection(&CS); log("007");
   ClearForm();
   CalcTotal();
//    for(int i = 0; i <= Grid->ColCount; i++)
//    {
//        String str = "g" + String(i) + ".txt";
//        DeleteFile(str);
//    }
      PlaySound("deleted.wav",0,SND_ASYNC);
}
//---------------------------------------------------------------------------
// удаление отдельной позиции
void __fastcall TMainWindow::DeleteNnumClick(TObject *Sender)
{
    if(CardType != MANAGER_CARD) return;
    if(Payments)
    {
      Name->Caption = "Удаление позиции только после отмены всех платежей!";
      return;
    }
    if(!Delete)
    {
        Delete = true;
        NewCode = true;
        NewPrice = false;
        InputQnty = false;

        ClearForm();
        Name->Caption = "Удаление позиции";
        ArrangeTable();
//        if(Grid->RowCount > 2) Grid->RowCount--;
        PlaySound("warning.wav",0,SND_ASYNC);
        PresentEnter = false;
        ClearPresentClick(this);
    }
}
//---------------------------------------------------------------------------
// Х-отчет
void __fastcall TMainWindow::X_ReportClick(TObject *Sender)
{
    if(!NoCard && (CardType != KASSIR_CARD && CardType != MANAGER_CARD)) return;

//        Star->Print(BillHead[0].c_str(),true);
//        Star->Print(BillHead[1].c_str(),true);
//        Star->Print(BillHead[2].c_str(),true);
//        Star->Print(BillHead[3].c_str(),true);
//        Star->Print(" ",true);
        N19Click(this);
        frReport();
        Star->XReport();
        if(Star->KKMResult != "0000") GetKKMError();
}
//---------------------------------------------------------------------------
// закрытие смены
void __fastcall TMainWindow::S_CloseClick(TObject *Sender)
{
    if(!NoCard && (CardType != KASSIR_CARD && CardType != MANAGER_CARD)) return;
    if(!GetSverka(true))
    {
      PlaySound("warning.wav",0,SND_ASYNC);
      if(MessageBox (GetActiveWindow(), "ИТОГИ НЕ ИДУТ!!! Уверены в закрытии смены?",
            "Подтверждение", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
      ) return;
    } else
      PlaySound("warning.wav",0,SND_ASYNC);
      if(MessageBox (GetActiveWindow(), "Закрыть смену?",
            "Подтверждение", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    ) return;

      N19Click(this);
      frReport();
//      return;
      Star->ZReport();
      if(Star->KKMResult == "0000")
      {
          Session = false;
          Name->Caption = "Смена закрыта";
          PlaySound("s_close.wav",0,SND_ASYNC);
//          SendCloseSession();
      }
      else {GetKKMError();}
//    SendFiles();
    WriteCloseRetail();
}
//---------------------------------------------------------------------------
// открытие смены
void __fastcall TMainWindow::S_OpenClick(TObject *Sender)
{
    if(Session)
    {
      Name->Caption = "Сессия уже открыта!";
      return;
    }
    if(!NoCard && CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    PlaySound("warning.wav",0,SND_ASYNC);
    if
    (
        MessageBox (GetActiveWindow(), "Открыть смену?",
            "Проверьте время компьютера", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    ) return;
    Star->SetClock();
    // меняем НДС на 20%
    Word Year, Month, Day;
    TDateTime dt = Now();
    DecodeDate(dt, Year, Month, Day);
    if(Year == 2019)
    {
      Star->StrihWriteTable(6,1,1,"\xD0\x07",2); // Налоговая ставка 2000 (20%)
      Star->StrihWriteTable(6,1,2,"НДС 20%",7);  // Описание ставки
      Star->StrihWriteTable(6,5,1,"\xD0\x07",2); // Налоговая ставка 2000 (20%) на предоплату
      Star->StrihWriteTable(6,5,2,"НДС 20/120",10);  // Описание ставки
    }
    else
    {
      Star->StrihWriteTable(6,1,1,"\x08\x07",2); // Налоговая ставка 1800 (18%)
      Star->StrihWriteTable(6,1,2,"НДС 18%",7);  // Описание ставки   // новый НДС
      Star->StrihWriteTable(6,1,1,"\x08\x07",2); // Налоговая ставка 1800 (18%)
      Star->StrihWriteTable(6,5,2,"НДС 18/118",10);  // Описание ставки
    }
    Star->OpenSession("Касса " + Star->Serial, CasNumber);
    if(Star->KKMResult == "0000")
    {
        Session = true;
        Name->Caption = "Смена открыта";
        PlaySound("s_open.wav",0,SND_ASYNC);
//        SendOpenSession();
//        Sleep(1000);
//        X_ReportClick(this);
        if(Star->KKMResult == "0000") Name->Caption = "Смена открыта";
    }
    else GetKKMError();
}
//---------------------------------------------------------------------------
// открытие денежного ящика
void __fastcall TMainWindow::BoxClick(TObject *Sender)
{
    if(CardType != KASSIR_CARD && CardType != MANAGER_CARD) return;
    Star->OpenBox();
}
//---------------------------------------------------------------------------
// расчет наличными
void __fastcall TMainWindow::CashClick(TObject *Sender)
{
//   SendMessage(Application->Handle, WM_CLOSE, 0, 0);
// для отладки
//ftc("check.txt");
//PrintFileOnFR("check.txt");
//   auth_answer Answer1;
//   Answer1.Check="test text test text test text";
//   AnsiString b = "test text ";
//   AnsiString a = AnsiString(Answer1.Check);
//   Star->BillOpen(false,"test", 1);
//   Star->Print( Answer1.Check, true);
//   return;
 PayFlag = 1;
 PayType = NAL_PAYMENT;
 log(" Нажата клавиша F10.");
 ComboPayClick();
 return;

   AnsiString str;
   int RC;
   double qt,pr,cost,err,sum;
   long st;
   bool res = false;
   log(" Нажата клавиша F10.");
// Выходим при несоблюдении условий
    if(SetNum||SetPrice||NowSearching||CheckBool(Printing)||Grid->Cells[0][1].IsEmpty()) return;
    if(!Session){
     Name->Caption = "Не открыта смена";
     PlaySound("oy.wav",0, SND_ASYNC);
     return;
    }

    SetBool(Printing);
    PayType = NAL_PAYMENT;
    PayFlag = 1;
// Проверка наличия акции, если акция есть, то выходим из функции расчета для ввода подарков
if(!StocksProcessing()) { DownBool(Printing); PayType = NO_PAYMENT; return;}
    if(CheckSale()) Sale = true;
    else Sale = false;
    if(Sale)
    {
        PlaySound("warning.wav",0,SND_ASYNC);
        if
        (
            MessageBox (GetActiveWindow(), "Внимание! Уценненный товар! Продолжить?",
                "Уценка", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {DownBool(Printing);PayType = NO_PAYMENT; return;}
    }

    if(!NoCard && CardType != KASSIR_CARD && CardType != MANAGER_CARD)
        {
        DownBool(Printing);
        PayType = NO_PAYMENT;
        return;
        }
      PlaySound("cash.wav",0,SND_ASYNC);

    if(CheckSale0())
    {
      PlaySound("warning.wav",0,SND_ASYNC);
      MessageBox (GetActiveWindow(),"ВНИМАНИЕ!",
              "Товар с нулевой ценой или количеством, укажите или удалите строку", MB_OK);
      DownBool(Printing); PayType = NO_PAYMENT; return;
    }


    //Запишем скидку
    TOff->Text = MoneyAsString(MoneyAshyper(TSum->Text)%Round);
    TOff->Repaint();
    //Пересчитаем с учетом скидки сумму чека
    TSum->Text = MoneyAsString(MoneyAshyper(TSum->Text) - MoneyAshyper(TOff->Text));
    //Подменим текст на табло
    TextToShow = TextToShow.SubString(1,40-TSum->Text.Length())+TSum->Text;
    ShowOnDisplay(TextToShow.c_str());
// окно с принятой суммой
    if(CashForm->ShowModal() != mrOk)
    {
      DownBool(Printing);
      PayType = NO_PAYMENT;
      PresentLabel->Visible = false;
      ClearGrid(PresentGrid);
      return;
    }
    PresentLabel->Visible = false;
    Change->Text = MoneyAsString(MoneyAshyper(Recived->Text) - MoneyAshyper(TSum->Text));   // сдача
    Recived->Repaint();
    Change->Repaint();

    TextToShow = "Получено:";
    while( TextToShow.Length()+Recived->Text.Length() < 20) TextToShow += " ";
    TextToShow += Recived->Text;
    TextToShow += "Сдача:";
    while( TextToShow.Length() + Change->Text.Length() < 40) TextToShow += " ";
    TextToShow += Change->Text;
    ShowOnDisplay(TextToShow.c_str());

    if(Star->Serial == "" || Star->SerialID == "") //Если серийный номер не определен, работать не можем!
    {
      MessageBox(NULL,"Нет номера ФР или ID","Печать чека невозможна",MB_OK);
      DownBool(Printing);
      PayType = NO_PAYMENT;
      return;
    }

//  FRInit();
  res = CreateBillBody(true);

/*    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        Star->GetVersion();
        Sverka(true);
        if(Star->KKMResult != "0000"){PayType = NO_PAYMENT; return;}
        else continue;
    }
 */
if(!res)  // если что-то пошло не удачно
  { // необходимо сделать расширенную обработку ошибок
   log("Ошибка пробития чека " + IntToHex(frStatus.OperationResult, 2) + "h");
   GetSverka(true);
   Star->FRInit();
   DownBool(Printing);
   PayType = NO_PAYMENT;
   PlaySound("oy.wav",0,SND_ASYNC);
   return;
  }
 PlaySound("cash.wav",0,SND_ASYNC);

 WritePayment(PayType,TSum->Text);
 WritePayment("11",TOff->Text);
 WritePayment("12",MoneyAsString(GiftCardCalcPayments()));
 WriteRetail(PayType);
 ClearForm();
 DownBool(Printing);
 PayType = NO_PAYMENT;
 }
//---------------------------------------------------------------------------

void __fastcall TMainWindow::SendMessageAB()
{
    AnsiString dt = "Чек ";
    dt += DateTimeToStr(Now());
    dt += ". Касса ";
    dt += Star->Serial;
    dt += ". Сумма ";
    dt += TSum->Text;
    dt += ". Кассир ";
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
        dt += " по ";
        dt += Grid->Cells[4][i];
        dt += " руб. на сумму ";
        dt += Grid->Cells[6][i];
        dt += " руб.\n";
    }
    dt += "Всего на кассе ";
    dt += Star->Serial;
    dt += " ";
//    dt += StatusBar->Panels->Items[4]->Text;
    dt += "\n";
// Рудимент, использование не нужно.
/*    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
       CashierServer->Socket->Connections[n]->SendText(dt);
   }
*/
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SendDeletedLine()
{
    AnsiString dt = "Удален товар ";
    dt += DateTimeToStr(Now());
    dt += ". Касса ";
    dt += Star->Serial;
    dt += ". Кассир ";
    dt += CasName;
    dt += "\n";
    dt += Nnum->Text.c_str();
    dt += " ";
    dt += Name->Caption.c_str();
    dt += " ";
    dt += Qnty->Text.c_str();
    dt += " ";
    dt += Unit->Caption.c_str();
    dt += " по ";
    dt += Price->Text.c_str();
    dt += " руб.\n";
// Комментируем рудимент
/*    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
*/
}
//---------------------------------------------------------------------------
/* void __fastcall TMainWindow::SendDeletedBill()
{
//while(!TryEnterCriticalSection(&CS)) Sleep(1000);
//LeaveCriticalSection(&CS);

    AnsiString dt = "Удален список ";
    dt += DateTimeToStr(Now());
    dt += ". Касса ";
    dt += Star->Serial;
    dt += ". Кассир ";
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
        dt += " по ";
        dt += Grid->Cells[4][i];
        dt += " руб.\n";
    }
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}

*/
//---------------------------------------------------------------------------
/*
void __fastcall TMainWindow::SendCloseSession()
{
    AnsiString dt = "Удален Смена закрыта ";
    dt += DateTimeToStr(Now());
    dt += ". Касса ";
    dt += Star->Serial;
    dt += ". Кассир ";
    dt += CasName;
    dt += "\n";
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}

*/

//---------------------------------------------------------------------------
/* void __fastcall TMainWindow::SendOpenSession()
{
    AnsiString dt = "Удален Смена открыта ";
    dt += DateTimeToStr(Now());
    dt += ". Касса ";
    dt += Star->Serial;
    dt += ". Кассир ";
    dt += CasName;
    dt += "\n";
    for(int n = 0; n < CashierServer->Socket->ActiveConnections; n++)
    {
        CashierServer->Socket->Connections[n]->SendText(dt);
    }
}

*/

//---------------------------------------------------------------------------
void __fastcall TMainWindow::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    ArrangeTable();
    SaveGridToFile(ComboPayGrid, ProgramPath + "paygrid.txt");
    Reader->TerminateCancel();
//    CashierServer->Close();
    DeleteCriticalSection(&CS); log("Удаление критической сессии");
    log("Выход из программы");
}
//---------------------------------------------------------------------------
/* void __fastcall TMainWindow::CashierServerClientError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;
}
*/
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SBClick(TObject *Sender)
{
    AnsiString str;
    char Scard[10];
    int RC, err;
    double qt,pr,cost;
    bool res = false;
    long st;
    PayFlag = 10;
   log(" Нажата клавиша Ctrl+B");
   PayType = INTERNATIONAL_CARD_PAYMENT;
   ComboPayClick();
   return;

// Выходим при несоблюдении условий
    if(SetNum||SetPrice||NowSearching||CheckBool(Printing)||Grid->Cells[0][1].IsEmpty()) return;
    if(!Session){
     Name->Caption = "Не открыта смена";
     PlaySound("oy.wav",0, SND_ASYNC);
     return;
    }
    SetBool(Printing);

if(!StocksProcessing()) { DownBool(Printing); PayType = NO_PAYMENT; return;}    // проверка акций

    if(CheckSale()) Sale = true;
    else Sale = false;
    if(Sale)
    {
        PlaySound("warning.wav",0,SND_ASYNC);
        if
        (
            MessageBox (GetActiveWindow(), "Внимание! Уценненный товар! Продолжить?",
                "Уценка", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {DownBool(Printing); return;}
    }

    if(!NoCard && CardType != KASSIR_CARD && CardType != MANAGER_CARD)
      {
      DownBool(Printing);
      return;
      }

    PlaySound("cash.wav",0,SND_ASYNC);
//    ArrangeTable();
//    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty()) Grid->RowCount--;

    if(CheckSale0())
    {
      PlaySound("warning.wav",0,SND_ASYNC);
      MessageBox (GetActiveWindow(),"ВНИМАНИЕ!",
              "Товар с нулевой ценой или количеством, укажите или удалите строку", MB_OK);
      DownBool(Printing); return;
    }

    if(ManualSB)
    {
        if
        (
            MessageBox (GetActiveWindow(), "Авторизация произведена?",
                "Терминал СБ", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {DownBool(Printing); return;}
        strcpy(Scard,"ПК");
        SelectPayType();
    }
    else
    {
    TOff->Text = MoneyAsString(0); // сбросим скидку, если она есть
    bool psb = PaySB(TSum->Text, Scard);
    // для отладки
//    psb = true;
//    mfBill.BankCheck = ftc("check.txt");  //
        if(!psb)
        {
            PlaySound("oy.wav",0,SND_ASYNC);
            Name->Caption = String(PayType);
            DownBool(Printing);
            return;
        }

    }

    if( (MoneyAshyper(GetLastCardCheckSumm()) != MoneyAshyper(TSum->Text)) && !ManualSB )
      {
        AnsiString InfoStr = "Сумма текущего чека "+TSum->Text+", а последнего платежа по карте "+GetLastCardCheckSumm();
        MessageBox(GetActiveWindow(),InfoStr.c_str(),"Проверьте факт оплаты по карте",MB_OK);
        DownBool(Printing);
        return;
      }

    ShowOnDisplay("   Оплата картой         успешна        ");
    PlaySound("cash.wav",0,SND_ASYNC);

    Change->Text = "0.00";
    Recived->Text = Scard;
    Recived->Repaint();
    Change->Repaint();

    if(Star->Serial == "" || Star->SerialID == "") //Если серийный номер не определен, работать не можем!
      {
      MessageBox(NULL,"Нет номера ФР или ID","Печать чека невозможна",MB_OK);
      DownBool(Printing);
      return;
      }

//  FRInit();
  res = CreateBillBody(true);

if(!res)
{
   log("Ошибка пробития чека " + IntToHex(frStatus.OperationResult, 2) + "h");
   GetSverka(true);
   Star->FRInit();
   PayType = NO_PAYMENT;
   DownBool(Printing);
   PlaySound("oy.wav",0,SND_ASYNC);
   return;
}

    WritePayment(PayType,TSum->Text);
    WritePayment("12",MoneyAsString(GiftCardCalcPayments()));
    WriteRetail(PayType);
    PlaySound("cash.wav",0,SND_ASYNC);
    ClearForm();
    DownBool(Printing);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SBManualClick(TObject *Sender)
{
    if(CardType != MANAGER_CARD) return;
    if(SetNum||SetPrice||NowSearching||CheckBool(Printing)) return;
    log("Нажата СБ автономно");
    ManualSB = true;
    if
        (
            MessageBox (GetActiveWindow(), "Авторизация произведена?",
                "Терминал СБ", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {DownBool(Printing); return;}
//        strcpy(Scard,"ПК");
        SelectPayType();
        ComboPayClick();
        ManualSB = false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::PaySB(AnsiString Sum, char* Scard)
{
    int ret;
//    bool res=false;
    auth_answer Answer;
    // DWORD dw;

    Answer.TType = 1; // Оплата
    Answer.Amount = MoneyAshyper(Sum); // Сумма в копейках
    Answer.CType = 0;
    Answer.Check = NULL;

//    SelectPayType();
//    Answer.Check = (ftc("check.txt")).c_str();
    if(PayType == NAL_PAYMENT)
       {
//      CashClick(this);
        GlobalFree((HGLOBAL)Answer.Check);
        DownBool(Printing);
       return false;
       }
//    else if(PayType == SBERCARD_PAYMENT ) Answer.CType = 9;
    else if(PayType == INTERNATIONAL_CARD_PAYMENT) Answer.CType = 3;
    else
      {
      GlobalFree((HGLOBAL)Answer.Check);
      DownBool(Printing);
      return false;
      }
// для отладки
//    mfBill.BankCheck = AnsiString(Answer.Check);
//    mfBill.vBankCheck.push_back(AnsiString(Answer.Check));
//   return true;

    ret = card_authorize(NULL, &Answer);
    if(ret)
    {
         log("Ошибка инициализации библиотеки pilot_nt " + ret);
        GlobalFree((HGLOBAL)Answer.Check);
        DownBool(Printing);
        return false;
    }
    if(strcmp(Answer.RCode,"0") && strcmp(Answer.RCode,"00"))
    {
        log("Ошибка транзакции по банковским картам. " + (AnsiString)Answer.RCode);
        GlobalFree((HGLOBAL)Answer.Check);
        DownBool(Printing);
        return false;
    }

    if(CheckFile(Answer.Check, (ProgramPath + "Check.txt").c_str())<20)
    {
        log("Ошибка формирования банковского чека");
        GlobalFree((HGLOBAL)Answer.Check);
        DownBool(Printing);
        return false;
    }

    AddToFile((ProgramPath + "Check.txt").c_str(),"c:\\data\\checks.txt");

    PlaySound("cash.wav",0,SND_ASYNC);
    switch(Answer.CType)
    {
        case 1:
            strcpy(Scard,"Visa");
            break;
        case 2:
            strcpy(Scard,"EC/MC");
            break;
        case 3:
            strcpy(Scard,"МК");
            break;
        case 9:
            strcpy(Scard,"СК");
            break;
        default:
            strcpy(Scard,"ПК");
    }
      mfBill.BankCheck = AnsiString(Answer.Check);

    GlobalFree((HGLOBAL)Answer.Check);
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::SBDayCloseClick(TObject *Sender)
{
    int ret;
    auth_answer Answer;

    Answer.Check = NULL;

    SetBool(Printing);
    PlaySound("cash.wav",0,SND_ASYNC);

    ret = close_day(&Answer);
    if(ret||(HGLOBAL)Answer.Check==NULL)
    {
        GlobalFree((HGLOBAL)Answer.Check);
        PlaySound("oy.wav",0,SND_ASYNC);
        Name->Caption = AnsiString(Answer.AMessage);
        DownBool(Printing);
        return;
    }
    if(strcmp(Answer.RCode,"0") && strcmp(Answer.RCode,"00"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        PlaySound("oy.wav",0,SND_ASYNC);
        Name->Caption = AnsiString(Answer.AMessage);
        DownBool(Printing);
        return;
    }
    AnsiString FileName;
    AnsiString CommonFileName;
    Word Year, Month, Day;
    TDateTime dtPresent = Now();
    DecodeDate(dtPresent, Year, Month, Day);

    FileName = OutputDir;
    FileName += Star->Serial;
    FileName += AnsiString(Year).SubString(3,2);
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Month)));
    FileName += Format("%2.2D",ARRAYOFCONST(((int)Day)));
//    FileName += ".";
    CommonFileName = FileName+".txt";
    ret = 0;
    while(FileExists(FileName+String(ret))) ret++;
    FileName += String(ret);
        CheckFile(Answer.Check, FileName.c_str());
        PlaySound("cash.wav",0,SND_ASYNC);
        AddToFile(FileName.c_str(),CommonFileName.c_str());
        PrintFileOnFR(CommonFileName.c_str());
/*      Star->Print(Answer.Check, true);
        if(strcmp(Star->KKMResult.c_str(),"0000"))
        {
            GetKKMError();
        }
        else Star->CRLF(); */
    PlaySound("cash.wav",0,SND_ASYNC);
    GlobalFree((HGLOBAL)Answer.Check);
    DownBool(Printing);
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CheckRepeatClick(TObject *Sender)
{
PrintFileOnFR((ProgramPath + "Check.txt").c_str());
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


/*int __fastcall TMainWindow::CheckFile(char* Check, char* FileName)
{
    int i;
    ofstream file;
    file.open(FileName);
    for(i = 0; Check[i]; i++)
    {
      if(isalpha(Check[i]))
      file.put(Check[i]);

    }
    file.close();
    return i;
}
*/

//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CancelMC(AnsiString Sum)
{
    int ret;
    auth_answer Answer;

    SetBool(Printing);

    Answer.TType = 8; // Отмена
    Answer.Amount = MoneyAshyper(Sum); // Сумма в копейках
    Answer.CType = 0; // Тип карты
    Answer.Check = NULL;

    ret = card_authorize(NULL, &Answer);
    if(ret)
    {
        PlaySound("oy.wav",0,SND_ASYNC);
        Name->Caption = AnsiString(Answer.AMessage);
        GlobalFree((HGLOBAL)Answer.Check);
        DownBool(Printing);
        return false;
    }
    if(strcmp(Answer.RCode,"0") && strcmp(Answer.RCode,"00"))
    {
        PlaySound("oy.wav",0,SND_ASYNC);
        Name->Caption = AnsiString(Answer.AMessage);
        GlobalFree((HGLOBAL)Answer.Check);
        DownBool(Printing);
        return false;
    }
    CheckFile(Answer.Check, (ProgramPath + "check.txt").c_str());
    PlaySound("cash.wav",0,SND_ASYNC);
    Star->Print(Answer.Check, true);
    if(strcmp(Star->KKMResult.c_str(), "0000"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        GetKKMError();
        DownBool(Printing);
        return true;
    }
    else Star->CRLF();
    MessageBox (GetActiveWindow(), "Этот чек на подпись",
        "Операции по карте", MB_OK | MB_ICONINFORMATION);
    Star->Print(Answer.Check, true);
    if(strcmp(Star->KKMResult.c_str(), "0000"))
    {
        GlobalFree((HGLOBAL)Answer.Check);
        GetKKMError();
        DownBool(Printing);
        return true;
    }
    else Star->CRLF();
    GlobalFree((HGLOBAL)Answer.Check);
    DownBool(Printing);
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::CancelClick(TObject *Sender)
{
    if(MoneyAshyper(TSum->Text) <= 0) return;
    if(CardType != MANAGER_CARD) return;

    SetBool(Printing);
    ArrangeTable();
    if(Grid->Cells[6][Grid->RowCount-1].IsEmpty() && Grid->RowCount > 2 ) Grid->RowCount--;

    if(CancelMC(TSum->Text))
        MessageBox (GetActiveWindow(), "Операция по карте отменена!!!",
        "Операции по карте", MB_OK | MB_ICONINFORMATION);

    DownBool(Printing);
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckSale()
{
    for(int i = 1; i < Grid->RowCount; i++)
    {
        if(Grid->Cells[2][i].LowerCase().Pos("уценен")) return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckSale0()
{
    for(int i = 1; i < Grid->RowCount; i++)
    {
     if(!Grid->Cells[1][i].IsEmpty())
      {
        if(MoneyAshyper(Grid->Cells[4][i])==0) return true;
        if(QuantityAshyper(Grid->Cells[5][i])==0) return true;
      }
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

PriceQuery->SQL->Clear();
PriceQuery->SQL->Text = "select count(*) as Rowcnt from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"'";
   try
   {
      PriceQuery->Active = true;
   }
   catch (EOleException &eException)
   {
      Name->Caption = "Ошибка SQL при автозакрытии смены";
      PriceQuery->Active = false;
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return false;
   }
if(PriceQuery->RecordCount)
  rowcnt = PriceQuery->FieldByName("rowcnt")->AsInteger;
else rowcnt = -1;

PriceQuery->Active = false;
PriceQuery->Close();

if(!rowcnt) WriteCloseRetail(); //Чтоб хоть одна запись в день от кассы была!

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
// копия чека на принтере
void __fastcall TMainWindow::CopyCheckClick(TObject *Sender)
{
return;
//if(!NoCard && (CardType != KASSIR_CARD && CardType != MANAGER_CARD)) return;
//BillPrint->QuickRep1->Print();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::N1Click(TObject *Sender)
{
Close();
}
//---------------------------------------------------------------------------
// возврат ошибочного чека
void __fastcall TMainWindow::ReturnClick(TObject *Sender)
{
   if(!NoCard && CardType != MANAGER_CARD) return;
   if(SetNum||SetPrice||NowSearching||CheckBool(Printing)) return;
   if(!Session)
   {
     Name->Caption = "Не открыта смена";
     PlaySound("oy.wav",0, SND_ASYNC);
     return;
   }

 if(frReturnForm->ShowModal() != mrOk)
   {
      return;
   }
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
    log("Запись чека в базу");

/*    if(PayType == NO_PAYMENT)
    {
      SelectPayType();
      PayType=MainWindow->PayType;
    } */

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
      PlaySound("warning.wav",0,SND_ASYNC);
      MessageBox (GetActiveWindow(),"ВНИМАНИЕ!",
              "Товар с нулевой ценой, укажите цену или удалите строку", MB_OK);
      DownBool(Printing); return false;
    }

    af1.open(FileName.c_str(), ios::app);
    for(int i = 1; (i < Grid->RowCount)&& !Grid->Cells[1][i].IsEmpty(); i++)
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
        PlaySound("warning.wav",0,SND_ASYNC);
        Name->Caption = "Ошибка записи файла данных. Немедленно прекратить работу!";
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
    SendMessageAB();

    //Скинуть на SQL
    AnsiString SDT = FormatDateTime("dd.mm.yyyy hh:mm:ss",dtPresent);

/*  PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("set dateformat dmy");
    try
    {
      PriceQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return false;
    }           */
    AnsiString TSQL = "";
    int paynum=0;

   //Запись счета в таблицу реализации из таблицы Bill
    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("BEGIN TRY");
    PriceQuery->SQL->Add("BEGIN TRAN");
    PriceQuery->SQL->Add("DECLARE @BillDateTime DateTime = CURRENT_TIMESTAMP");
    PriceQuery->SQL->Add("DECLARE @Guid as uniqueidentifier");
    PriceQuery->SQL->Add("INSERT INTO retail (BillDateTime,BillNumber,IDNom, Sklad ,Quantity, Price, PayType, SCash,Operator,flag)");
    PriceQuery->SQL->Add("SELECT @BillDateTime,'" + Star->BillNumber + "' ,IDNom,0x"+Department+",Quantity,Price,'"+PayType+"','"+Star->Serial+"',Operator, 101" );
    PriceQuery->SQL->Add("FROM bill");

    std::vector<ComboPay>::iterator it;
    for(it = mfBill.vPay.begin();it < mfBill.vPay.end(); ++it)
    {
      paynum++;
      PriceQuery->SQL->Add("SELECT @Guid = CAST(CAST(" + Star->Serial + " AS BINARY(3)) + CAST(" + DepartmentId + " AS BINARY(1)) + CAST(" + paynum + " AS binary(4)) + CAST(CURRENT_TIMESTAMP as varbinary(8)) AS uniqueidentifier)");
      PriceQuery->SQL->Add("INSERT INTO RetPay (Id, BillNumber,PayNum,PayType,Amount,RoundPart)");
      PriceQuery->SQL->Add("VALUES (@Guid,'"+Star->BillNumber+"',"+paynum+",'"+it->PayType+"',"+MoneyAsString(it->Sum)+","+MoneyAsString(it->RoundPart)+")");
      if(it->PayType == INTERNATIONAL_CARD_PAYMENT || it->PayType == GIFT_CARD_PAYMENT)
      {
         PriceQuery->SQL->Add("INSERT INTO RetCardData (RetPayID,CardID,CType,CardName,SberOwnCard,RRN,[Check])");
         PriceQuery->SQL->Add("VALUES (@Guid,'"+AnsiString(it->CardID)+"',"+it->CType+",'"+it->CardName+"',"+it->SberOwnCard+",'"+it->RRN+"','"+ it->Check + "')");
      }
    }
    PriceQuery->SQL->Add("COMMIT TRAN");
    PriceQuery->SQL->Add("END TRY");
    PriceQuery->SQL->Add("BEGIN CATCH");
    PriceQuery->SQL->Add("IF @@TRANCOUNT > 0 ROLLBACK TRAN;");
    PriceQuery->SQL->Add("DECLARE @ErrorMessage NVARCHAR(4000);");
    PriceQuery->SQL->Add("DECLARE @ErrorSeverity INT;");
    PriceQuery->SQL->Add("DECLARE @ErrorState INT;");
    PriceQuery->SQL->Add("SELECT @ErrorMessage = ERROR_MESSAGE(), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();");
    PriceQuery->SQL->Add("RAISERROR (@ErrorMessage, @ErrorSeverity, @ErrorState);");
    PriceQuery->SQL->Add("END CATCH");
    _di_Errors        errCollection;
    _di_Error        errSingle;
    int iCount = 0;
    int SQL_RAISERROR_CODE =0;
   try
   {
      PriceQuery->ExecSQL();
      errCollection = CashConnection->Errors;
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
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      return false;
   }
   catch (_di_Errors &errCollection)
   {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg, Desc;
      int errorCode = 0;
      int iCount = errCollection->Count;
      _di_Error        errSingle;
      for(int i = 0; i < iCount; i++)
      {
         errCollection->Get_Item(i,errSingle);
         errSingle->Get_NativeError(errorCode);
         errormsg = " Ошибка SQL: Code = " + IntToStr(errSingle->NativeError) + " Description: " + errSingle->Description + " TSQL: " + PriceQuery->SQL->Text;
         log(errormsg);
      }
      return false;
   }

   WritePresent(PresentGrid);
   log("WritePresent");
   UpdateCentralCounts(PresentGrid);
   log("UpdateCentralCounts");
   UpdateLocalCounts();
   log("UpdateCentralCounts");
   ActivateGiftCards(PresentGrid, 1); //Активация подарочных карт
   log("ActivateGiftCards(PresentGrid)");
   ActivateGiftCards(Grid, 1); //Активация проданных подарочных карт
   log("ActivateGiftCards(Grid)");
   GiftCardDoPayments(); //Оплата картами
   log("GiftCardDoPayments");
   GiftCardClearPayments(); //Очистка поля оплаты картами
   log("GiftCardClearPayments");

   //Очистка Экрана
//   for(int i = 1; i < Grid->RowCount; i++) Grid->Rows[i]->Clear();

//   while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("110");
//   Grid->RowCount = 1;
//   PresentGrid->RowCount = 1;
//   LeaveCriticalSection(&CS); log("010");
   ClearGrid(Grid);
   ClearGrid(PresentGrid);
   GoodName = "";
   GoodPrice = "";
   TotalSum = "0.00";
//   ShowOnDisplay("   ДОБРО ПОЖАЛОВАТЬ!");

   ArrangeTable();
/*
    //Проверка остатков
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

    //После проверки остатков нужно ...
      while(!PriceQuery->Eof)
        {
        str = PriceQuery->FieldByName("ScanCode")->AsString+" "+PriceQuery->FieldByName("Name")->AsString+" Не хватает в магазине: "+PriceQuery->FieldByName("Quantity")->AsString;
        MessageBox (GetActiveWindow(),str.c_str(),"Продажа товара сверх остатка",MB_OK);
        PriceQuery->Next();
        }
      PriceQuery->Close();
    }
*/

    ShowOnDisplay("  ДОБРО ПОЖАЛОВАТЬ!");
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::WriteCloseRetail(void)
{
    TDateTime dtPresent=Now();
    //Скинуть на SQL
    AnsiString SDT = FormatDateTime("yyyymmdd hh:mm:ss",dtPresent);
    AnsiString GN,GNA;

    while
    (
    Grid->Cells[0][Grid->RowCount-1].IsEmpty()
    || Grid->Cells[0][Grid->RowCount-1] == " "
    )
    {
            while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("111");
            Grid->Rows[Grid->RowCount-1]->Clear();
            if(Grid->RowCount > 2) Grid->RowCount--;
            else break;
            LeaveCriticalSection(&CS); log("011");
    }
/*
   PriceQuery->SQL->Clear();
   PriceQuery->SQL->Text = "select count(*) as Rowcnt from retail where billdatetime > '" + SDT + "'";
   try
   {
      PriceQuery->Active = true;
   }
   catch (EOleException &eException)
   {
      Name->Caption = "Ошибка SQL при автозакрытии смены";
      PriceQuery->Active = false;
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      return false;
   }

  */
    PriceQuery->SQL->Clear();

    PriceQuery->SQL->Add("declare @p as numeric(15,2)");
    PriceQuery->SQL->Add("set @p = (select ISNULL(sum(RoundPart),0) from RetPay rp");
    PriceQuery->SQL->Add("inner join (select BillNumber from retail where BillDateTime > (select isnull(max(billdatetime),0) from retail where flag = 100)and flag>100 group by BillNumber) bn ON bn.BillNumber = rp.BillNumber)");
    PriceQuery->SQL->Add("insert into retail (BillDateTime,BillNumber,IDNom, Sklad, Quantity, Price, PayType,SCash,Operator,flag)");
    PriceQuery->SQL->Add("values ('"+SDT+"','0',0,0x"+Department+",0,@p");
    PriceQuery->SQL->Add(",'','"+Star->Serial+"',left('"+CasName+"',15),100)" );
    try
    {
      PriceQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      return false;
    }
    if(PriceQuery->RowsAffected != 1)
      {
      Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      log("Ошибка SQL " + PriceQuery->SQL->Text);
      return false;
      }
  return true;
}

//---------------------------------------------------------------------------
void __fastcall TMainWindow::N4Click(TObject *Sender)
{
GetSverka(false);
}
//---------------------------------------------------------------------------

bool __fastcall TMainWindow::GetSverka(bool Silent)
{

//FormSverka->ShowModal(true);
if(FormSverka->ShowModal(Silent) == mrOk)
{
   return true;
}
else
{
   return false;
}

/* hyper LFRSumm;
Word Year, Month, Day;
//TDateTime dtPresent; // = Now();
TDateTime LastEndSession;
unsigned hyper FRSumm,USumm,USkidka,USumPK,BillSumm;
AnsiString Str;

//LFRSumm = Star->GetSellSumm();

LFRSumm = Star->GetMoneyReg(4180);
if(LFRSumm == -1) return;
FRSumm = LFRSumm;


// Время последней записи с флагом 100
PriceQuery->SQL->Text = "select ISNULL(max(billdatetime),0) as LastEndSession from retail where SCash = '" + Star->Serial +"' and flag=100";
    try
    {
      PriceQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка при сверке сумм";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return;
    }
   if(PriceQuery->RecordCount)  LastEndSession = PriceQuery->FieldByName("LastEndSession")->AsDateTime;

   PriceQuery->Active = false;

//Определим показания Учета за сегодня
PriceQuery->SQL->Text = "select ISNULL(sum(s),0) as Summ from (select BillNumber,paytype,SUM(round(price*quantity,2)) as s from retail where BillDateTime > '" + FormatDateTime("yyyymmdd hh:mm:ss", LastEndSession) + "' and flag = 101 group by BillNumber,paytype)a";
    try
    {
      PriceQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка при сверке сумм";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return;
    }
   if(PriceQuery->RecordCount) USumm = MoneyAshyper(PriceQuery->FieldByName("Summ")->AsString);      // сумма по базе Учета
   else
   {
     PriceQuery->Active = false;
     return;
   }
PriceQuery->Active = false;
PriceQuery->Close();

// Скидка за текущий день
//PriceQuery->SQL->Text = "select sum([price]*[quantity]) as Summ from delbill where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"'";
   PriceQuery->SQL->Clear();
   PriceQuery->SQL->Add("select isnull(sum(s),0) as Skidka from ");
   PriceQuery->SQL->Add("(select BillNumber,(SUM(round(price*quantity,2)) % 0.5) as s from retail where ");
   PriceQuery->SQL->Add("BillDateTime>'"  + FormatDateTime("yyyymmdd hh:mm:ss", LastEndSession) + "' ");
   PriceQuery->SQL->Add("and PayType = ' 1' ");
   PriceQuery->SQL->Add("and flag = 101 and SCash = '" + Star->Serial + "' ");
   PriceQuery->SQL->Add("group by BillNumber) a");
    try
    {
      PriceQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL при сверке сумм";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return;
    }
if(PriceQuery->RecordCount)
  {
  USkidka = MoneyAshyper(PriceQuery->FieldByName("Skidka")->AsString);
  USumm -= USkidka;
  }
else
  {
     PriceQuery->Active = false;
     return;
  }
PriceQuery->Active = false;
PriceQuery->Close();
// Подарочные карты
   CentralQuery->SQL->Clear();
   CentralQuery->SQL->Add("select sum(Amount) as SumPK from GiftCardPayment where PayDate>'" + FormatDateTime("yyyymmdd  hh:mm:ss",LastEndSession) + "' and SCash='" + Star->Serial+ "'");
    try
    {
      CentralQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return;
    }

   if(CentralQuery->RecordCount)
  {
  USumPK = MoneyAshyper(CentralQuery->FieldByName("SumPK")->AsString);
  USumm -= USumPK;
  }
  CentralQuery->Active = false;

if(USumm == FRSumm) {if(!Silent) MessageBox(GetActiveWindow(),"Итоги учета равны X-отчету","Сверка итогов",MB_OK);}
else
  {
  BillSumm = MoneyAshyper(TSum->Text);

  if(USumm + BillSumm == FRSumm)
    {
    if(!Silent) MessageBox(GetActiveWindow(),"Чек был пробит, записываем","Сверка итогов",MB_OK);
    Star->BillNumber = Star->GetBillNumber();
    if(!Silent && PayType == NO_PAYMENT) SelectPayType();
    WriteRetail(PayType);
    }
  else
    {
    Str = "Учет "+MoneyAsString(USumm)+" X-отчет "+MoneyAsString(FRSumm);
    MessageBox(GetActiveWindow(),Str.c_str(),"Сверка итогов",MB_OK);
    }
  } */
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::COLClick(TObject *Sender)
{

    Star->Print(BillHead[0].c_str(),true);
    Star->Print(BillHead[1].c_str(),true);
    Star->Print(BillHead[2].c_str(),true);
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

//Разрешено только менеджерам
if(CardType != MANAGER_CARD) return;

//Очистить экран
    ArrangeTable();
    while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("112");
    if(Grid->RowCount > 2) Grid->RowCount=1;
    LeaveCriticalSection(&CS); log("012");

    NewCode = false;
    NewPrice = false;
    InputQnty = false;
    Qnty->Color = clBtnFace;
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
//    SendDeletedBill();

/*    while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("112");
    for(int i = 1; i < Grid->RowCount; i++)
        Grid->Rows[i]->Clear();
    Grid->RowCount = 2;
    LeaveCriticalSection(&CS); log("012");
*/
    ClearGrid(Grid);

    ClearForm();
    CalcTotal();
//    for(int i = 0; i <= Grid->ColCount; i++)
//    {
//        String str = "g" + String(i) + ".txt";
//        DeleteFile(str);
//    }

//Вернуть чек на экран
DecodeDate(dtPresent, Year, Month, Day);
PriceQuery->SQL->Text = "select * from retail where billnumber > '0' and billdatetime in (select max(billdatetime) as billdatetime from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"')";
    try
    {
      PriceQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return;
    }

ClearGrid(Grid);
GiftCardClearPayments();
for(int i = 1;i <= PriceQuery->RecordCount; i++)
  {
  Grid->Cells[0][i] = i;
  Grid->Cells[1][i] = PriceQuery->FieldByName("ScanCode")->AsString;
  Grid->Cells[2][i] = PriceQuery->FieldByName("Name")->AsString;
  Grid->Cells[3][i] = PriceQuery->FieldByName("Meas")->AsString;
  Grid->Cells[4][i] = PriceQuery->FieldByName("Price")->AsString;
  Grid->Cells[5][i] = PriceQuery->FieldByName("Quantity")->AsString;
  Grid->Cells[6][i] = PriceQuery->FieldByName("Summ")->AsString;
  Grid->Cells[13][i] = PriceQuery->FieldByName("IDNom")->AsString;
  Grid->Cells[8][i] = PriceQuery->FieldByName("Operator")->AsString;
  PriceQuery->Next();
  }
if(Grid->RowCount < 2) ClearGrid(Grid);
ArrangeTable();
PriceQuery->Active = false;
PriceQuery->Close();
CalcTotal();

//Тот или не тот чек?
    if
    (
        MessageBox (GetActiveWindow(), "Отменить этот чек?",
            "Подтверждение", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
    )
      {
      ClearGrid(Grid);
      ClearForm();
      CalcTotal();
      SaveGridToFile(Grid, ProgramPath + "gridrows.txt");
      CalcTotal();
      return;
      }

//Удалить чек
PriceQuery->SQL->Text = "delete from retail where billnumber > '0' and billdatetime in (select max(billdatetime) as billdatetime from retail where billdatetime > '"+AnsiString(Year)+Format("%2.2D",ARRAYOFCONST(((int)Month)))+Format("%2.2D",ARRAYOFCONST(((int)Day)))+"')";
    try
    {
      PriceQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
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
//delete SelectPayTypeForm;
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
    log("now open port for display");
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
    cto.ReadIntervalTimeout = 0;
    cto.ReadTotalTimeoutMultiplier = 50;
    cto.ReadTotalTimeoutConstant = 100;
    cto.WriteTotalTimeoutMultiplier = 50;
    cto.WriteTotalTimeoutConstant = 100;
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
    log("Com port opened");
    first = 0;
    }

    Star->StrWinToDos(str,40);
    log("now clear port error ...");
    ClearCommError(hCommD, &dwError, &csStat);
    log("now write to port ...");
    WriteFile(hCommD, str,40,&nChar, NULL);
    log("... done");
//    CloseHandle (hCommD);                    // close the COM port.
}


void __fastcall TMainWindow::L1Click(TObject *Sender)
{
    AnsiString MessageA = "   ДИСПЛЕЙ          ИНИЦИАЛИЗИРОВАН";
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
hyper __fastcall TMainWindow::MoneyAshyper(AnsiString String)
{
int p, sign = 1;
AnsiString C,D;
unsigned hyper r,d;

String = String.Trim();
if(String.Pos("-") > 0)
{
   sign = -1;
   String = String.SubString(2,String.Length());
}
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
void __fastcall TMainWindow::Present(AnsiString s,int Gift)
{
Word Year, Month, Day;
bool IsIt = false;
int p;

  if(Gift)
  {
  PlaySound("podar15.wav",0,SND_ASYNC);
    return;
  }

//Август 2016
    TDateTime dtPresent = Now();
    DecodeDate(dtPresent, Year, Month, Day);
    if((Year != 2016)||(Month!=8)) return;

        p = s.Pos("ЛИНОЛЕУМ");
        if( (p>0) &&
            (p==1 || !isrus(s.SubString(p-1,1)))&&
            (p==s.Length() - 7  || !isrus(s.SubString(p+8,1))) &&
            (s.Pos("КЛЕЙ") == 0) &&
            (s.Pos("ПЛИТКА") == 0) &&
            (s.Pos("ГЕЛЬ") == 0) &&
            (s.Pos("СВАРКА") == 0)
          ) IsIt = true;

//Доставка
if(IsIt)   PlaySound("fd.wav",0,SND_ASYNC);;

}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::SynchronizeServer()
{
static int FailCountPrice = 0;
static int FailCountRetail = 0;
AnsiString MaxId,Command,GN,GNA;
long i;
DWORD ReturnCode;

//Вызов синхронизации прайс-листа
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
   //Дождаться окончания и получить код завершения
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
   //Дождаться окончания и получить код завершения
   WaitForSingleObject(lpProcessInformation.hProcess, INFINITE);
   GetExitCodeProcess(lpProcessInformation.hProcess,&ReturnCode);
   if(ReturnCode)
     FailCountRetail ++;
   else
     FailCountRetail = 0;
   CloseHandle(lpProcessInformation.hProcess);
   }
// синхронизация таблиц акций

CmdString = "SynStocks.exe";

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
   //Дождаться окончания и получить код завершения
   WaitForSingleObject(lpProcessInformation.hProcess, INFINITE);
   GetExitCodeProcess(lpProcessInformation.hProcess,&ReturnCode);
   if(ReturnCode)
     FailCountRetail ++;
   else
     FailCountRetail = 0;
   CloseHandle(lpProcessInformation.hProcess);
   }

// Синхронизация подарков

CmdString = "SynGivePresents.exe";

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
   //Дождаться окончания и получить код завершения
   WaitForSingleObject(lpProcessInformation.hProcess, INFINITE);
   GetExitCodeProcess(lpProcessInformation.hProcess,&ReturnCode);
   if(ReturnCode)
     FailCountRetail ++;
   else
     FailCountRetail = 0;
   CloseHandle(lpProcessInformation.hProcess);
   }

   // проверка
  if(!FailCountRetail && !FailCountPrice)
      {
      StatusBar->Panels->Items[1]->Text = "Связь с сервером есть";
      StatusBar->Update();
      return true;
      }

    if(FailCountRetail > 1)
      {
      if(FailCountPrice>1)
        StatusBar->Panels->Items[1]->Text = "Связи нет!";
      else
        StatusBar->Panels->Items[1]->Text = "Цены-Ок Продажи-нет";
      }
    else
      {
      if(FailCountPrice>1)
        StatusBar->Panels->Items[1]->Text = "Цены-Нет Продажи-Ок";
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
CreateBillBody(false);
InsertToBill(Grid);
Star->GetBillNumber();
WriteRetail(PayType);
CalcTotal();
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::AddToFile(char* Input,char* Output)
{
ifstream InF;
ofstream OutF;
char str[180];

InF.open(Input);
if(!InF)
  {
  PlaySound("oy.wav",0,SND_ASYNC);
  Name->Caption = "Ошибка открытия файла на чтение";
  return false;
  }
OutF.open(Output,ios::app);
if(!OutF)
  {
  InF.close();
  PlaySound("oy.wav",0,SND_ASYNC);
  Name->Caption = "Ошибка открытия файла на запись";
  return false;
  }
while(!InF.eof())
{
   InF.getline(str,180);
   OutF << str << '\n';
}
OutF << "\n\n=== Конец документа ===\n\n";
OutF.close();
InF.close();
return true;
}

//----------------------------------
bool __fastcall TMainWindow::PrintFileOnFR(char* Input)
{
ifstream file;
char str[41];

   file.open(Input);
    if(!file)
    {
        PlaySound("oy.wav",0,SND_ASYNC);
        Name->Caption = "Ошибка открытия файла для печати";
        return false;
    }
    while(!file.eof())
    {
      file.getline(str,41);
      Star->Print(str, true);
      if(file.eof()) break;
      if(file.fail()) file.clear();
    }
    file.close();
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
        return false;
    }
    else Star->CRLF();
    return true;
}
//----------------------------------
bool __fastcall TMainWindow::GetSerialID()
{
  bool res;
  AnsiString ret;

  if(Star->Serial == "") return false;

  //Получим идентификатор локально
  PriceQuery->SQL->Text = "select Id from SCash where SCash = '" + Star->Serial + "'";
  res = true;
  try
    {
    PriceQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      res = false;
    }

  if(res)
  {
    ret = PriceQuery->FieldByName("Id")->AsString;
    if( ret != "")
      {
      Star->SerialID = ret;
      PriceQuery->Active = false;
      PriceQuery->Close();
      return true;
      }
  }
  PriceQuery->Active = false;
  PriceQuery->Close();

  //Если локально не получили, получим центрально
  CentralQuery->SQL->Text = "select Id from SCash where SCash = '" + Star->Serial + "'";
  res = true;
  try
  {
//    CentralConnection->Connected = true;
      CentralQuery->Active = true;
  }
  catch (EOleException &eException)
  {
    AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
    log(errormsg);
    res = false;
  }

  if(res)
  {
    ret = CentralQuery->FieldByName("Id")->AsString;
    if( ret != "")
    {
      Star->SerialID = ret;
    }
    else
    {
      res = false;
    }
  }
 CentralQuery->Active = false;
  if(res)
  {
   //И добавим результат локально если удалось извлечь его центрально
   //res = true;
   PriceQuery->SQL->Text = "IF NOT EXISTS (SELECT table_name FROM information_schema.tables WHERE table_name = 'SCash') BEGIN CREATE TABLE [dbo].[SCash]([Id][int],[SCash][nvarchar](8)) END";
   try
   {
      PriceQuery->ExecSQL();
   }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      return false;
    }

//   PriceQuery->Active = false;
   PriceQuery->Close();
   PriceQuery->SQL->Text = "INSERT INTO SCAsh VALUES ("+ret+",'"+Star->Serial+"')";
   try
   {
      PriceQuery->ExecSQL();
   }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      return false;
    }

//   PriceQuery->Active = false;
   PriceQuery->Close();
   return true;
  }
  else //Если же результата нет и центрально, нужно регистрировать ФР в центре
  {
     CentralQuery->SQL->Text = "INSERT INTO SCAsh VALUES ('"+Star->Serial+"')";
    try
    {
        CentralQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {

      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
      log(errormsg);
      CentralQuery->Active = false;
      return false;
    }
     CentralQuery->Active = false;
     CentralQuery->Close();
     CentralConnection->Connected = false;
  //Если вставка успешно выполнилась, нужно прогнать цикл еще раз
   return GetSerialID();
  }
}

//---------------------------------------------------------------------------
void __fastcall TMainWindow::FreeCheckClick(TObject *Sender)
{
  if(CardType != MANAGER_CARD) return;
  FreeCheckForm->ShowModal();
}
//---------------------------------------------------------------------------
//Возвращает сумму последнего оплаченного по карте чека или "" в лучае ошибки
AnsiString __fastcall TMainWindow::GetLastCardCheckSumm(void)
{
AnsiString ret = "нет";
int i;

   // boost::regex_iterator regit

    if(FileExists(ProgramPath + "check.txt"))
    {
        TStringList *CheckList = new TStringList;
        CheckList->LoadFromFile(ProgramPath + "check.txt");
        boost::cmatch what;
        boost::regex reg("Сумма \\(Руб\\):\\s+(\\d+\\.\\d\\d)");
         std::string s = CheckList->Text.c_str();
        if(boost::regex_search(s, what, reg))
        {
        AnsiString str;
        ret = what.str(1).c_str();
        }
        delete CheckList;
    }

return ret;
}
//Устанавливает время на ФР в соответствии с временем на компьютере
void __fastcall TMainWindow::N17Click(TObject *Sender)
{
    Star->SetClock();
    if(Star->KKMResult != "0000")
    {
        GetKKMError();
    }

    Name->Caption = "Время установлено";
    Name->Repaint();
}
//---------------------------------------------------------------------------
//Выключает печать чека по закрытию
void __fastcall TMainWindow::N18Click(TObject *Sender)
{
                    Star->StrihWriteTable(1,1,20,"\x00",1);
                    Star->StrihWriteTable(1,1,21,"\x00",1);
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckBool(bool & b)
{
bool r;
while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("114");
r = b;
LeaveCriticalSection(&CS); log("014");
return r;
}



//---------------------------------------------------------------------------
void __fastcall TMainWindow::SetBool(bool & b)
{
while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("115");
b = true;
LeaveCriticalSection(&CS); log("015");
}




//---------------------------------------------------------------------------
void __fastcall TMainWindow::DownBool(bool & b)
{
while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("116");
b = false;
LeaveCriticalSection(&CS); log("016");
}

//---------------------------------------------------------------------------
bool __fastcall TMainWindow::isrus(AnsiString s)
{
return !(s.UpperCase() == s.LowerCase());
}

//---------------------------------------------------------------------------
void __fastcall TMainWindow::log(AnsiString s)
{
        //DumpLog
/*        if(Log)
          {
           ofstream errlog;
           errlog.open("errlog.txt",ios::app);
           errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz ", Now()).c_str() <<s.c_str();
           errlog << "\n";
           errlog.close();
          }
        EndDumpLog
if (CreateDirectory(OutputFolder.c_str(), NULL) ||
    ERROR_ALREADY_EXISTS == GetLastError())
{
    // CopyFile(...)
}
else
{
     // Failed to create directory.
}

        */

//   TDateTime Date = CurrentDate();
   if (CreateDirectory(LogDir.c_str(), NULL) ||
       ERROR_ALREADY_EXISTS == GetLastError())
   {
      AnsiString fileName = LogDir;
      fileName += FormatDateTime("yyyymmdd", Now()) + ".log";
      if(!Log) return;
      ofstream errlog;
      errlog.open(fileName.c_str(), ios::app);
      errlog << FormatDateTime("dd-mm-yy' at 'hh:mm:ss:zzz ", Now()).c_str() <<s.c_str();
      errlog << "\n";
      errlog.close();
   }

}
//---------------------------------------------------------------------------
// ищем подарочную карту по коду, определяем ее состояние
bool __fastcall TMainWindow::SeekGiftCard(AnsiString Code)
{
  //Поиск в таблице подарочных карт
  bool res,ret;
  AnsiString Message,IDNomL,LPrice,LIDNom,LCSN,LNDS,LOff,Gift,LName,LMeas,LValidity,LStartDate;
  ret = true;

  CentralQuery->SQL->Text = "SELECT ScanCode,Validity,Nominal,Balance,ISNULL(StartDate,CURRENT_TIMESTAMP) as StartDate,Balance,flag, FROM GiftCard WHERE Scancode = '" + Code + "'";
  res = true;
  try
  {
    CentralConnection->Connected = true;
    CentralQuery->Active = true;
  }
  catch (EOleException &eException)
  {
    CentralQuery->Active = false;
    AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
    log(errormsg);
    res = false;
  }

  if(!res)
  {
      PlaySound("warning.wav",0,SND_ASYNC);
      Message = "Нет связи с центральным сервером. Продажа подарочных карт невозможна. Попробуйте позже.";
      MessageBox(GetActiveWindow(),Message.c_str(),"ВНИМАНИЕ!",MB_OK);
      ret = false;
  }
  else
  {
      String s = CentralQuery->FieldByName("ScanCode")->AsString;
      CardNominal = CentralQuery->FieldByName("Nominal")->AsString;
      ret = (CentralQuery->FieldByName("ScanCode")->AsString == Code);
      if(!ret)
      {  //Такая подарочная карта не найдена
         PlaySound("warning.wav",0,SND_ASYNC);
         Message = "Подарочная карта "+Code+" не выпускалась. Попробуйте другую.";
         MessageBox (GetActiveWindow(),Message.c_str(),"ВНИМАНИЕ!",
         MB_OK);
      }
      else
      {  //Такая подарочная карта найдена
         if(CentralQuery->FieldByName("flag")->AsInteger > 1)
         { //Уже активирована
            AnsiString CardBalance = CentralQuery->FieldByName("Balance")->AsString;
            CentralQuery->Active = false;
            PlaySound("warning.wav",0,SND_ASYNC);
            if(ShowGiftCard(Code,"Принять карту к оплате?",MB_YESNO)) //Карта принимается к оплате
               GiftCardAddPayment(Code,CardBalance);
            ret = false;
         }
         else
         { //Карта найдена и может быть активирована
//         //Ищем товар, соответствующий карте
            IDNomL = CentralQuery->FieldByName("IDNom")->AsString;
            PriceQuery->SQL->Text = "SELECT ScanCode,Name,Price,Meas,CSN,[Off],NDS,Sklad,Special,sys.fn_varbintohexstr(IDNom) as IDNom,Gift FROM Price WHERE IDNom = "+IDNomL;
            try
            {
               PriceQuery->Active = true;
            }
            catch (EOleException &eException)
            {
               PlaySound("oy.wav",0,SND_ASYNC);
               AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
               log(errormsg);
               PriceQuery->Active = false;
               PriceQuery->Close();
            }
            ret = (PriceQuery->FieldByName("IDNom")->AsString == IDNomL) || PresentEnter;
            if(ret)
            {
               LName = PriceQuery->FieldByName("Name")->AsString;
               LMeas = PriceQuery->FieldByName("Meas")->AsString;
               LPrice = PriceQuery->FieldByName("Price")->AsString;
//            LIDNom = PriceQuery->FieldByName("IDNom")->AsString;
               LIDNom = IDNomL;
               LCSN = PriceQuery->FieldByName("CSN")->AsString;
               LNDS = MainWindow->PriceQuery->FieldByName("NDS")->AsString;
               LOff = PriceQuery->FieldByName("Off")->AsString;
               Gift = PriceQuery->FieldByName("Gift")->AsInteger;
               LName = LName.Trim();
               LMeas = LMeas.TrimRight();
               LValidity = CentralQuery->FieldByName("Validity")->AsString;
               LStartDate = CentralQuery->FieldByName("StartDate")->AsString;
               LName += " действительна по "+FormatDV(LStartDate,LValidity);
               Name->Caption = LName;
               Unit->Caption = LMeas;
               Price->Text = LPrice;
               IDNom->Caption = LIDNom;
               Qnty->Text = "1";

               if( LCSN == "0" ) cansetnum_flag = false;
               else cansetnum_flag = true;

               NDS = LNDS;

               if( LOff == "1" )
               Name->Caption = "Уценен: "+ Name->Caption;
            }
//
         }
      }
   }

   try
   {
      PriceQuery->Active = false;
      PriceQuery->Close();
      CentralQuery->Active = false;
      CentralConnection->Connected = false;
   }
   catch (EOleException &eException)
   {
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
   }

  return ret;

}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::FormatN(AnsiString N)
{
AnsiString R;
R = N;
if(R.Pos(".") == 0) R = R + ".00";
if(R.Pos(".") == 1) R = "0" + R;
if(R.Pos(".") == R.Length()) R = R + "00";
if(R.Pos(".") == R.Length()-1) R = R + "0";
return R;
}

//---------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::FormatD(AnsiString D)
{
AnsiString R;
R = D.SubString(1,10);
return R;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::FormatDV(AnsiString D,AnsiString V)
{
Word y,m,d,v;

AnsiString R;
TDateTime t;
R = D.SubString(1,10);
t = StrToDate(R);
v = StrToInt(V);
DecodeDate(t,y,m,d);
y += v/12;
m += v%12;
if(m>12) {m-=12; y++;}
if(m==2 && d == 29) d = 28;
t = EncodeDate(y,m,d);
R = DateToStr(t);

return R;
}

//---------------------------------------------------------------------------
bool __fastcall TMainWindow::ShowGiftCard(AnsiString Code,AnsiString CapMsg,int ButtonType)
{
  bool res;
  AnsiString Message,PayD,PayS;
  Message = "Подарочная карта "+Code;

  CentralQuery->SQL->Text = "select * from GiftCard where Scancode = '" + Code + "'";
  res = true;
  try
  {
    CentralConnection->Connected = true;
    CentralQuery->Active = true;
  }
  catch (EOleException &eException)
  {
     Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
     log(errormsg);
     res = false;
  }

  if(!res)
        {
        PlaySound("warning.wav",0,SND_ASYNC);
        Message = "Нет связи с центральным сервером. Продажа подарочных карт невозможна. Попробуйте позже.";
        MessageBox (GetActiveWindow(),Message.c_str(),"ВНИМАНИЕ!",
         MB_OK);
        }
  else
  {
   res = (CentralQuery->FieldByName("ScanCode")->AsString == Code);
   if(res)
     {
     AnsiString Balance = CentralQuery->FieldByName("Balance")->AsString;
     AnsiString Nominal = CentralQuery->FieldByName("Nominal")->AsString;
     AnsiString StartDate = CentralQuery->FieldByName("StartDate")->AsString;
     AnsiString Validity = CentralQuery->FieldByName("Validity")->AsString;
     Message += "\nОстаток\t\t\t" + FormatN(Balance);
     Message += "\nНоминал\t\t\t" + FormatN(Nominal);
     Message += "\nДействительна с " + FormatD(StartDate)+" по "+ FormatDV(StartDate,Validity);
//    Теперь собираем информацию по платежам по данной карте
     CentralQuery->Active = false;
     CentralQuery->SQL->Text = "select * from GiftCardPayment where Scancode = '" + Code + "' order by PayDate";
  res = true;
  try
    {
    CentralQuery->Active = true;
    }
  catch (EOleException &eException)
  {
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
     log(errormsg);
     res = false;
  }
    if(!res || (CentralQuery->FieldByName("ScanCode")->AsString != Code))
     { //Нет платежей
        Message += "\nРасчет по этой карте не производился.";
     }
     else
     {//Вывести расчеты
     Message += "\nРасчет по карте производился:";
     for(int i=1; i <= CentralQuery->RecordCount ;i++)
       {
       PayD = CentralQuery->FieldByName("PayDate")->AsString;
       PayS = CentralQuery->FieldByName("Amount")->AsString;
       Message += "\n" + FormatD(PayD) + "\t\t\t"+ FormatN(PayS);
       CentralQuery->Next();
       }
     }

//
     }
  }

  try
    {
    CentralQuery->Active = false;
    CentralQuery->Close();
    CentralConnection->Connected = false;
    }
  catch (EOleException &eException)
  {
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
     log(errormsg);
  }

  return (MessageBox (GetActiveWindow(),Message.c_str(),CapMsg.c_str(), ButtonType) == ID_YES);
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::ActivateGiftCards(TStringGrid *Grid,int RowNum)
{
    // Активация подарочных карт
    AnsiString Code;
    for(int i = 1; i < Grid->RowCount; i++)
    {
       Code = Grid->Cells[RowNum][i];
       if(Code.Length() == 10)
       {
         ActivateGiftCard(Code);
        //ShowGiftCard(Code,"Карта активирована",MB_OK);
       }
    }
}
//---------------------------------------------------------------------------

bool __fastcall TMainWindow::ActivateGiftCard(AnsiString Code)
{
  CentralQuery->SQL->Clear();
  CentralQuery->SQL->Add("update GiftCard set flag = 2,ActivationDate = CURRENT_TIMESTAMP,StartDate = ISNULL(StartDate,CURRENT_TIMESTAMP),Balance = Nominal");
  CentralQuery->SQL->Add(",Sklad = 0x" + Department);
  CentralQuery->SQL->Add(",Scash = '" + Star->Serial+"'");
  CentralQuery->SQL->Add(",Seller = left('" + CasName+"',15)");
  CentralQuery->SQL->Add(",BillNumber = '" + Star->BillNumber+"'");
  CentralQuery->SQL->Add(" where Scancode = '"+ Code +"'");

    try
    {
      CentralConnection->Connected = true;
      CentralQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {
        Name->Caption = "Ошибка активации карты!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
        log(errormsg);
        CentralConnection->Connected = false;
        return false;
     }
      CentralConnection->Connected = false;
      return true;
}
//---------------------------------------------------------------------------
// расчет платежа по подарочной карте
bool __fastcall TMainWindow::GiftCardAddPayment(AnsiString Code,AnsiString Balance)
{
// На данный момент карта найдена в таблице и информация по ней в рекордсете

unsigned hyper Summ,Bal,CardPay,Rem;
AnsiString CP,Remain;

CalcTotal();
Summ = MoneyAshyper(TSum->Text) - GridSum(ComboPayGrid, CP_SUM_COL);
Bal = MoneyAshyper(Balance);
if(Bal > Summ) CardPay = Summ; else CardPay = Bal;   // условие
if(CardPay <= 0) return false;
Rem = Bal-CardPay;
CP = MoneyAsString(CardPay);
Remain = MoneyAsString(Rem);
int Row = ComboPayGrid->RowCount;
if(!ComboPayGrid->Cells[0][ComboPayGrid->RowCount - 1].IsEmpty())ComboPayGrid->RowCount = ComboPayGrid->RowCount + 1;  // Если строка не пронумерована (пустая), то добавляем строку
if(ComboPayGrid->RowCount == 2 && ComboPayGrid->Cells[0][1].IsEmpty()) Row = 1; // обходим вторую пустую строку на начальном этапе
ComboPayGrid->Cells[0][Row] = AnsiString(Row);
ComboPayGrid->Cells[CP_NAME_COL][Row] = "ПК " + Code;
ComboPayGrid->Cells[CP_SUM_COL][Row] = CP;
ComboPayGrid->Cells[CP_DEL_COL][Row] = "-";
ComboPayGrid->Cells[CP_PAY_TYPE_COL][Row] = GIFT_CARD_PAYMENT;
ComboPayGrid->Cells[CP_RESIDUE_COL][Row] = Remain;
ComboPayGrid->Cells[CP_CARD_TYPE_COL][Row] = 99;

//AddComboPay(GIFT_CARD_PAYMENT, CP, ComboPayGrid);
ComboPayGrid->Cells[CP_CARD_ID_COL][ComboPayGrid->RowCount - 1] = Code;
//ComboPayGrid->Cells[CP_CARD_TYPE_COL][ComboPayGrid->RowCount - 1] = "99";
SetPayStatus(ComboPayGrid, ComboPayGrid->RowCount - 1, CP_COMPLETE_STATUS);
/*
while(CP.Length()<10) CP = " "+CP;
while(Remain.Length()<10) Remain = " "+Remain;
GiftCardM->Lines->Add(Code+" на сумму "+CP+"  остаток "+Remain); */
return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::GiftCardClearPayments(void)
{
  GiftCardM->Lines->Clear();
  GiftCardM->Repaint();
  return true;
}
//---------------------------------------------------------------------------
// суммируем все подарочные карты
unsigned hyper __fastcall TMainWindow::GiftCardCalcPayments(void)
{
unsigned hyper s = 0;
AnsiString L;
/*
for(int i = 0; i < GiftCardM->Lines->Count ; i++)
  {
  L = GiftCardM->Lines->Strings[i];
  L = L.SubString(21,10);
  s += MoneyAshyper(L);
  }
  */  // изменения для комбинированных расчетов
   for(int i = 1; i <= ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == GIFT_CARD_PAYMENT
         && GetPayStatus(ComboPayGrid, i) < CP_APPROVE_STATUS)
      {
         L = ComboPayGrid->Cells[CP_SUM_COL][i];
         s += MoneyAshyper(L);
      }

   }
return s;
}
//---------------------------------------------------------------------------
// отражение оплаты подарочными картами на сервере
/*bool __fastcall TMainWindow::GiftCardDoPayments()
{
   AnsiString C,S;
  for(int i = 0; i < GiftCardM->Lines->Count ; i++)
  {
  S = GiftCardM->Lines->Strings[i];
  C = S;
  C = C.SubString(1,10);
  S = S.SubString(21,10);
  GiftCardDoPayment(C,S);
  }

   for(int i = 1; i < ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == GIFT_CARD_PAYMENT)
      {
        GiftCardDoPayment(ComboPayGrid->Cells[CP_CARD_ID_COL][i],
            ComboPayGrid->Cells[CP_SUM_COL][i]);
      }
   }
return true;
} */
//---------------------------------------------------------------------------
// отражение оплаты подарочными картами на сервере
bool __fastcall TMainWindow::GiftCardDoPayments()
{
   bool res = false;
   if(GiftItemData.size() == 0) return true;
   std::vector<GiftCardData>::iterator it;
//Изменить баланс карты

   try{
   CentralConnection->Connected = true;
   CentralQuery->SQL->Clear();
   CentralConnection->BeginTrans();
   for(it = GiftItemData.begin(); it < GiftItemData.end(); ++it)
   {
//      CentralQuery->SQL->Add("DECLARE @date smalldatetime = CURRENT_TIMESTAMP");
      CentralQuery->SQL->Add("UPDATE GiftCard SET Balance = Balance - " + it->Sum );
      CentralQuery->SQL->Add(",flag = (CASE WHEN Balance - "+ it->Sum + " <= 0 then 3 else 2 end)");
      CentralQuery->SQL->Add("WHERE Scancode = '"+it->Code+"'");
      CentralQuery->SQL->Add("INSERT INTO GiftCardPayment (Scancode,Amount,Billnumber,Sklad,Scash,Seller,PayDate) VALUES (");
      CentralQuery->SQL->Add("'"+it->Code+"'");
      CentralQuery->SQL->Add(", " + it->Sum);
      CentralQuery->SQL->Add(", '" + Star->BillNumber+"'");
      CentralQuery->SQL->Add(", 0x" + Department);
      CentralQuery->SQL->Add(", '" + Star->Serial+"'");
      CentralQuery->SQL->Add(", LEFT('" + CasName+"',15)");
      CentralQuery->SQL->Add(", CURRENT_TIMESTAMP)");
   }

//      CentralConnection->Commands[0] = "Select 1";
      CentralQuery->ExecSQL();
   }
   catch (EOleException &eException)
   {

         Name->Caption = "Ошибка транзакции с подарочной картой!";
         PlaySound("oy.wav",0,SND_ASYNC);
         AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
         CentralConnection->RollbackTrans();
         log(errormsg);
         CentralConnection->Connected = false;
         return false;
    }
    CentralConnection->CommitTrans();
    CentralQuery->Active = false;
    CentralConnection->Connected = false;
return true;
}
//---------------------------------------------------------------------------
/*
// печать оплаты подарочными картами на чеке
bool __fastcall TMainWindow::GiftCardPrintPayments()
{
AnsiString C,S,R;

for(int i = 0; i < GiftCardM->Lines->Count ; i++)
  {
  S = GiftCardM->Lines->Strings[i];
  C = S;
  R = S;
  C = C.SubString(1,10);
  S = S.SubString(21,10);
  R = R.SubString(41,10);
  C = "Подарочная карта "+C+" оплата";
  S = "принята на сумму "+S;
  R = "остаток на карте "+R;
  Star->Print("----------------------------------------",true);
  Star->Print(C.c_str(),true);
  Star->Print(S.c_str(),true);
  Star->Print(R.c_str(),true);
  }
return true;
} */
//---------------------------------------------------------------------------
// печать информации по подарочным картам
void __fastcall TMainWindow::N19Click(TObject *Sender)
{
  AnsiString s,n,c;
  bool res;

  CentralQuery->SQL->Clear();
  CentralQuery->SQL->Text = "select [Nominal], count(*) as Cnt from [GiftCard] where [SCash] = '" + Star->Serial + "' and [ActivationDate] > CAST(GETDATE() AS DATE) group by [Nominal] order by [Nominal]";
  res = true;
  try
    {
    CentralConnection->Connected = true;
    CentralQuery->Active = true;
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Какая-то ошибка.";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
      log(errormsg);
      CentralConnection->Connected = false;
      CentralQuery->Active = false;
      res = false;
    }

  if(!res)
        {
        PlaySound("warning.wav",0,SND_ASYNC);
        s = "Нет связи с центральным сервером. Получение информации по подарочным картам невозможно. Попробуйте позже.";
        MessageBox (GetActiveWindow(),s.c_str(),"ВНИМАНИЕ!",
         MB_OK);
        }
  else
  {
     if(CentralQuery->RecordCount == 0)
       {
       Star->Print("  Подарочные карты не продавались",true);
       }
     else
       {
       Star->Print("  Продано подарочных карт:",true);
       }

     for(int i=1; i <= CentralQuery->RecordCount ;i++)
       {
       n = CentralQuery->FieldByName("Nominal")->AsString;
       c = CentralQuery->FieldByName("Cnt")->AsString;
       while(n.Length() < 10) n = " "+n;
       s =  n + " - " + c + " ШТ.";
       Star->Print(s.c_str(),true);
       CentralQuery->Next();
       }
       Star->Print("----------------------------------------",true);
  }

  try
  {
    CentralQuery->Active = false;
    CentralQuery->Close();
    CentralConnection->Connected = false;
  }
  catch (EOleException &eException)
  {
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
     log(errormsg);
  }
}
//---------------------------------------------------------------------------
// подарочную карту в оплату
bool __fastcall TMainWindow::GiftCardIsInCheck(AnsiString Code)
{
/* for(int i = 0; i < GiftCardM->Lines->Count ; i++)
  if(GiftCardM->Lines->Strings[i].SubString(1,10) == Code)
    {
        PlaySound("warning.wav",0,SND_ASYNC);
        MessageBox (GetActiveWindow(),"Такая карта уже принята к оплате","ВНИМАНИЕ!",MB_OK);
        return false;
    }
 */

   for(int i = 1; i < ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_CARD_ID_COL][i] == Code
         && ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == GIFT_CARD_PAYMENT)
      {
         PlaySound("warning.wav",0,SND_ASYNC);
         MessageBox (GetActiveWindow(),"Такая карта уже принята к оплате","ВНИМАНИЕ!",MB_OK);
         return false;
      }
   }
for(int i = 1;i<Grid->RowCount;i++)
  if(Grid->Cells[1][i] == Code )
    {
        PlaySound("warning.wav",0,SND_ASYNC);
        MessageBox (GetActiveWindow(),"Такая карта уже есть в чеке","ВНИМАНИЕ!",MB_OK);
        return false;
    }

  return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::WritePayment(AnsiString PayType,AnsiString Summ)
{
    if(MoneyAshyper(Summ) == 0 ) return true;

   //Запись счета в таблицу реализации
    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("insert into payment (BillDateTime,BillNumber, Sklad ,Summ, PayType, SCash,Operator)");
    PriceQuery->SQL->Add("values (CURRENT_TIMESTAMP,'" + Star->BillNumber + "',0x"+Department+","+Summ+",'"+PayType+"','"+Star->Serial+"',left('"+CasName+"',15))");
    try
    {
      PriceQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {
      Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      return false;
    }
return true;
}

//--------------------------------------------------------------------------
// Инициализация таблиц для акций
void __fastcall TMainWindow::CheckStockDataModel()
{

  PriceQuery->SQL->Clear();
  if(FileExists("StockTables.sql")) // файл скрипта
  {
     TStringList *SQLQuery = new TStringList;
     SQLQuery->LoadFromFile(ProgramPath + "StockTables.sql");

     for (int i=0; i < SQLQuery->Count ; i++)
     {
         PriceQuery->SQL->Add(SQLQuery->Strings[i]);
     }
      delete SQLQuery;
  }
  else
  {
     ShowMessage ("Нет файла");
  }
  try
  {
    //PriceQuery->Active = true;
    PriceQuery->ExecSQL();
  }
  catch (EOleException &eException)
  {
    Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
    PlaySound("oy.wav",0,SND_ASYNC);
    AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
    log(errormsg);
  }

PriceQuery->Active = false;
PriceQuery->Close();
}
// ---------------------------------------------------------------------------
// Проверяем акции в чеке
bool __fastcall TMainWindow::StocksInCheck()
{
int StockCount = 0;
int Quantity = 0;
bool ret = false; // флаг выполнения условий акции (запрос возвращает не NULL)
bool res = false;  // флаг исполнения условий
AnsiString str = "delete from PotentialPresent; "; // сначала чистим таблицу

//Выводим все акции, которые могут быть в чеке
PriceQuery->SQL->Clear();
PriceQuery->SQL->Add("select sys.fn_varbintohexstr(cond.IDStock) as IDStock, cond.Priority, cond.Condition, st.Description,st.WavFileName, (MaxPresentCount - CountUP) as Remain from PreConditions cond ");
PriceQuery->SQL->Add("inner join (select p.IDStock from PreProduct p inner join Bill c ON p.IDnom = c.IDNom inner join PreSklad sk ON sk.IDStock = p.IDStock where sk.Sklad = 0x" + Department + "  group by p.IDStock) stock ON cond.IDStock = stock.IDStock ");
PriceQuery->SQL->Add("inner join PreStock st ON st.IDStock = stock.IDStock");
PriceQuery->SQL->Add("Where st.Active is not null and (st.StartStock<CURRENT_TIMESTAMP and DATEADD(day,1, st.StopStock) > CURRENT_TIMESTAMP) and (MaxPresentCount < 0 or (CountUP < MaxPresentCount))");
PriceQuery->SQL->Add("Order By cond.IDStock, cond.Priority DESC");
  try
  {
    PriceQuery->Active = true;
  }
  catch (EOleException &eException)
  {
     Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
     errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
     log(errormsg);
     PriceQuery->Active = false;
     return false;
   }
   StockCount = PriceQuery->RecordCount;
   if(StockCount == 0)    // если запрос ничего не вернул, значит нет акций
   {
      PriceQuery->Active = false;
      PriceQuery->Close();
      PriceQuery->SQL->Clear();
      PriceQuery->SQL->Add("EXECUTE sp_executesql N'");   // очищаем таблицу от старых данных
      PriceQuery->SQL->Add(str + "'");
      try
      {
        PriceQuery->ExecSQL();
//        PriceQuery->Open();
      }
      catch (EOleException &eException)
      {
        Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
        errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
        log(errormsg);
        res = false;
       }
      PriceQuery->Active = false;
      PriceQuery->Close();
      return false;
   }

   std::vector<Stock> vStocks;
   std::vector<Stock>::iterator it;
   AnsiString ID = ""; // предыдущий IDStock
   for(int i = 0; i < StockCount; i++)
   {
      vStocks.push_back(Stock(PriceQuery->FieldByName("IDStock")->AsString,
         PriceQuery->FieldByName("Priority")->AsString,
         PriceQuery->FieldByName("Condition")->AsString,
         PriceQuery->FieldByName("Description")->AsString,
         PriceQuery->FieldByName("WavFileName")->AsString,
         PriceQuery->FieldByName("Remain")->AsString));
      PriceQuery->Next();
   }

    PriceQuery->Active = false;
    PriceQuery->Close();

 // Пробегаемся по всем возможным запросам
      for(it = vStocks.begin() ; it < vStocks.end(); ++it)
      {
// условия отсортированы в обратном порядке, поэтому цикл исполняется
// до первого возращающего > 0 строк запроса для действующего IDStock
// остальные строки данного IDStock игнорируются
        if(it->IDStock == ID && res){
           ID = it->IDStock;
           continue;
        }
// выполняем запрос из колонки Condition
//        Condition[i] = StringReplace(Condition[i],"\r\n"," ",TReplaceFlags()<<rfReplaceAll<<rfIgnoreCase);
        PriceQuery->SQL->Clear();
        PriceQuery->SQL->Add("EXECUTE sp_executesql N'");
        PriceQuery->SQL->Add(it->Condition + "'");
        try
        {
        PriceQuery->Active = true;
//        PriceQuery->Open();
        }
        catch (EOleException &eException)
        {
           Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
           PlaySound("oy.wav",0,SND_ASYNC);
           AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
           errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
           PriceQuery->Active = false;
           log(errormsg);
       }
        ID = it->IDStock; // текущий IDStock
        if(PriceQuery->RecordCount > 0) { // если запрос возвращает 0 записей, то флаг ложь
           res = true;
           ret = true;
           Quantity = PriceQuery->FieldByName("quantity")->AsInteger;
           // если количество превышает остаток
           if(StrToInt(it->Remain) > 0 && Quantity > StrToInt(it->Remain)) Quantity = StrToInt(it->Remain);
            // перебираем полученные от запроса-условия строки
           for(int k = 0; k < PriceQuery->RecordCount; k++)
           {     // считываем возможные подарки строками с разделителями
              str += "INSERT INTO PotentialPresent ([IDStock],[IDNom],[Quantity],[Description], [WavFileName], [Remain]) VALUES ";
              str += "("+PriceQuery->FieldByName("IDStock")->AsString+","+PriceQuery->FieldByName("IDNom")->AsString+", ";
              str += PriceQuery->FieldByName("quantity")->AsString+",N'"+it->Description+"',N'"+it->WavFileName+"'," + it->Remain + ");";
              PriceQuery->Next();
           }
        }
        else res = false;
        PriceQuery->Active = false;
        PriceQuery->Close();
      } // конец большого цикла
// выполняем вставку данных в PotentialPresent
      str = StringReplace(str,"'","''",TReplaceFlags()<<rfReplaceAll<<rfIgnoreCase);
      PriceQuery->SQL->Clear();
      PriceQuery->SQL->Add("EXECUTE sp_executesql N'");
      PriceQuery->SQL->Add(str + "'");
      try
      {
        PriceQuery->ExecSQL();
      }
      catch (EOleException &eException)
      {
        Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
        errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
        log(errormsg);
        res = false;
      }
        PriceQuery->Active = false;
        PriceQuery->Close();
 vStocks.clear();
 return ret;
}
//---------------------------------------------------------------------
// Проверяем акции в чеке
/*
bool __fastcall TMainWindow::StocksInCheck()
{
int StockCount = 0;
int Quantity = 0;
bool ret = false; // флаг выполнения условий акции (запрос возвращает не NULL)
bool res;  // флаг исполнения условий
AnsiString str = "delete from PotentialPresent; "; // сначала чистим таблицу

//Выводим все акции, которые могут быть в чеке
PriceQuery->SQL->Clear();
PriceQuery->SQL->Add("select sys.fn_varbintohexstr(cond.IDStock) as IDStock, cond.Priority, cond.Condition, st.Description,st.WavFileName, (MaxPresentCount - CountUP) as Remain from PreConditions cond ");
PriceQuery->SQL->Add("inner join (select p.IDStock from PreProduct p inner join Bill c ON p.IDnom = c.IDNom inner join PreSklad sk ON sk.IDStock = p.IDStock where sk.Sklad = 0x" + Department + "  group by p.IDStock) stock ON cond.IDStock = stock.IDStock ");
PriceQuery->SQL->Add("inner join PreStock st ON st.IDStock = stock.IDStock");
PriceQuery->SQL->Add("Where st.Active is not null and (st.StartStock<CURRENT_TIMESTAMP and DATEADD(day,1, st.StopStock) > CURRENT_TIMESTAMP) and (MaxPresentCount < 0 or (CountUP < MaxPresentCount))");
PriceQuery->SQL->Add("Order By cond.IDStock, cond.Priority DESC");
  try
  {
    PriceQuery->Active = true;
  }
  catch (EOleException &eException)
  {
     Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
     errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
     log(errormsg);
     return false;
   }
   StockCount = PriceQuery->RecordCount;
   if(StockCount == 0)    // если запрос ничего не вернул, значит нет акций
   {
      PriceQuery->Active = false;
      PriceQuery->Close();
      PriceQuery->SQL->Clear();
      PriceQuery->SQL->Add("EXECUTE sp_executesql N'");   // очищаем таблицу от старых данных
      PriceQuery->SQL->Add(str + "'");
      try
      {
        PriceQuery->ExecSQL();
//        PriceQuery->Open();
      }
      catch (EOleException &eException)
      {
        Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
        errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
        log(errormsg);
        res = false;
       }
      PriceQuery->Active = false;
      PriceQuery->Close();
      return false;
   }
 /*
   std::vector<Stock> vStock;
   for(int i=0;i<StockCount;i++)
   {
      vStock.push_back(Stock(PriceQuery->FieldByName("IDStock")->AsString,
         PriceQuery->FieldByName("Priority")->AsString,
         PriceQuery->FieldByName("Condition")->AsString,
         PriceQuery->FieldByName("Description")->AsString,
         PriceQuery->FieldByName("WavFileName")->AsString,
         PriceQuery->FieldByName("Remain")->AsString));
      PriceQuery->Next();
   }

   AnsiString *IDStock = new AnsiString[StockCount];
   AnsiString *Priority = new AnsiString[StockCount];
   AnsiString *Condition = new AnsiString[StockCount];
   AnsiString *Description = new AnsiString[StockCount];
   AnsiString *WavFileName = new AnsiString[StockCount];
   AnsiString *Remain = new AnsiString[StockCount];
   AnsiString ID = ""; // предыдущий IDStock
   for(int i = 0; i < StockCount; i++){
      IDStock[i] = PriceQuery->FieldByName("IDStock")->AsString;
      Priority[i] = PriceQuery->FieldByName("Priority")->AsString;
      Condition[i] = PriceQuery->FieldByName("Condition")->AsString;
      Description[i] = PriceQuery->FieldByName("Description")->AsString;
      WavFileName[i] = PriceQuery->FieldByName("WavFileName")->AsString;
      Remain[i] = PriceQuery->FieldByName("Remain")->AsString;
      PriceQuery->Next();
    }

    PriceQuery->Active = false;
    PriceQuery->Close();
 // Пробегаемся по всем возможным запросам
      for(int i = 0; i < StockCount; i++){
// условия отсортированы в обратном порядке, поэтому цикл исполняется
// до первого возращающего > 0 строк запроса для действующего IDStock
// остальные строки данного IDStock игнорируются
        if(IDStock[i] == ID && res){
           ID = IDStock[i];
           continue;
        }
// выполняем запрос из колонки Condition
//        Condition[i] = StringReplace(Condition[i],"\r\n"," ",TReplaceFlags()<<rfReplaceAll<<rfIgnoreCase);
        PriceQuery->SQL->Clear();
        PriceQuery->SQL->Add("EXECUTE sp_executesql N'");
        PriceQuery->SQL->Add(Condition[i] + "'");
        try
        {
        PriceQuery->Active = true;
//        PriceQuery->Open();
        }
        catch (EOleException &eException)
        {
           Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
           PlaySound("oy.wav",0,SND_ASYNC);
           AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
           errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
           log(errormsg);
       }
        ID = IDStock[i]; // текущий IDStock
        if(PriceQuery->RecordCount > 0) { // если запрос возвращает 0 записей, то флаг ложь
           res = true;
           ret = true;
           Quantity = PriceQuery->FieldByName("quantity")->AsInteger;
           // если количество превышает остаток
           if(StrToInt(Remain[i]) > 0 && Quantity > StrToInt(Remain[i])) Quantity = StrToInt(Remain[i]);
            // перебираем полученные от запроса-условия строки
           for(int k = 0; k < PriceQuery->RecordCount; k++){     // считываем возможные подарки строками с разделителями
              str += "INSERT INTO PotentialPresent ([IDStock],[IDNom],[Quantity],[Description], [WavFileName], [Remain]) VALUES ";
              str += "("+PriceQuery->FieldByName("IDStock")->AsString+","+PriceQuery->FieldByName("IDNom")->AsString+", ";
              str += PriceQuery->FieldByName("quantity")->AsString+",N'"+Description[i]+"',N'"+WavFileName[i]+"'," + Remain[i] + ");";
              PriceQuery->Next();
           }
        }
        else res = false;
        PriceQuery->Active = false;
        PriceQuery->Close();
      } // конец большого цикла
// выполняем вставку данных в PotentialPresent
      str = StringReplace(str,"'","''",TReplaceFlags()<<rfReplaceAll<<rfIgnoreCase);
      PriceQuery->SQL->Clear();
      PriceQuery->SQL->Add("EXECUTE sp_executesql N'");
      PriceQuery->SQL->Add(str + "'");
      try
      {
        PriceQuery->ExecSQL();
      }
      catch (EOleException &eException)
      {
        Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
        errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
        log(errormsg);
        res = false;
      }
        PriceQuery->Active = false;
        PriceQuery->Close();

 delete [] IDStock;
 delete [] Priority;
 delete [] Condition;
 delete [] Description;
 delete [] WavFileName;
 delete [] Remain;
 return ret;
}
 */
//---------------------------------------------------------------------
// разбиваем строку с разделителями
int __fastcall TMainWindow::Explode(String sContent, String sSeparator, TStringList *slTokens, bool bTrim = true)
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
      // insert front/first part of content into stringlist
      slTokens->Add(sContent.SubString(1, iPos - 1));
      // shorten the content to the rest
      sContent = sContent.SubString(iPos + 1, sContent.Length() - iPos);
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
//--------------------------------------------------------------------------
/*void __fastcall TMainWindow::StockInfoShow(bool Visible)
{
   String ID="";
//   TStringList *slTockens = new TStringList;
if(Visible){
//   StockInfo->Clear();
//   StockInfo->Lines->Add("Данный чек попадает под следующие акции:");
/*   for(int i = 0;i < PresentList->Count; i++){
      Explode(PresentList->Strings[i],"|", slTockens,true);
      if(slTockens->Strings[0] != ID){
      StockInfo->Lines->Add("Акция " + AnsiString(Num));
      StockInfo->Lines->Add("Описание: " + slTockens->Strings[3]);
      StockInfo->Lines->Add("Подарок:\t" + SeekName(slTockens->Strings[1]) + "   - " + slTockens->Strings[2]+ " шт.");
      Num++;
      }
      StockInfo->Lines->Add("\t" + SeekName(slTockens->Strings[1]) + "   - " + slTockens->Strings[2]+ " шт.");
      ID = slTockens->Strings[0];
      slTockens->Clear();
   }



  PriceQuery->SQL->Clear();
  PriceQuery->SQL->Add("SELECT p.IDStock, p.IDNom, (Count(p.Quantity)) as Quantity, p.[Description], c.Name");
  PriceQuery->SQL->Add("FROM [PotentialPresent] p");
  PriceQuery->SQL->Add("INNER JOIN price c ON p.IDNom = c.IDNom");
  PriceQuery->SQL->Add("GROUP BY IDStock, p.IDNom, p.Quantity, p.[Description], c.Name");
  try
        {
        PriceQuery->Active = true;
        }
   catch( ... )
        {
           Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
           PlaySound("oy.wav",0,SND_ASYNC);
           log("Ошибка проверки выполнения условий акций " + PriceQuery->SQL->Text);
        }
   PriceQuery->Active = false;
   PriceQuery->Close();

// поднимаем низ сетки для выдачи информации об акциях
   Grid->Height = StockInfo->Top - Grid->Top - 20;
   }
 InfoShow(PresentEnter,Visible);
//   delete slTockens;
} */
//------------------------------------------------------------------------------
void __fastcall TMainWindow::InfoShow(bool PrEnter, bool Visible)
{
   PresentLabel->Caption = "Акция!!! Ввод подарков.";
   PresentLabel->Color = clRed;
   PresentLabel->Visible = PrEnter;
//   StockInfo->Visible = Visible;
   PresentGrid->Visible = !Visible || !PrEnter;
   PlaySound("",0, SND_SYNC);
}

//-------------------------------------------------------------------------
// поиск по IDNom
/*AnsiString __fastcall TMainWindow::SeekName(AnsiString IDNom)
{
   PriceQuery->SQL->Clear();
   PriceQuery->SQL->Add("select * price where Sklad = 0x" + Department);
   PriceQuery->SQL->Add("and IDNom = " + IDNom);
    try
    {
      PriceQuery->Active = true;
    }
    catch( ... )
    {
      Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      log("Ошибка проверки выполнения условий акций " + PriceQuery->SQL->Text);
    }

    if(PriceQuery->RecordCount == 0)
    {
      CentralQuery->SQL->Clear();
      CentralQuery->SQL->Text = "select TOP 1 * from GiftCard where Sklad = 0x" + Department + " and IDNom = " + IDNom;
      try
      {
         CentralQuery->Active = true;
      }
      catch(EOleException &eException)
      {
         AnsiString errormsg="EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\""  + CentralQuery->SQL->Text;
//       ShowMessage(errormsg);
         log(errormsg);
       }
    }

  AnsiString res = PriceQuery->FieldByName("Name")->AsString;


    PriceQuery->Active = false;
    PriceQuery->Close();



return res;
}
*/
//-------------------------------------------------------------------
// проверяем подарки на соответствие
bool __fastcall TMainWindow::VerifyPresent(AnsiString IDNom, TStringGrid *StrGrid, int colNum)
{
 int r;
// bool res = true;
 TStringList *slTockens = new TStringList;
     for(int i = 1;i < StrGrid->RowCount;i++)
     {  // дважды одну и ту же карту не считаем
        if(StrGrid->Cells[1][i] == Nnum->Text && Nnum->Text.Length() == 10) return false;
       Explode(StrGrid->Cells[colNum][i],"|",slTockens,true); // разбиваем на несколько IDNom
       for(int k = 0; k < slTockens->Count;k++){              // сделано если есть подарки на выбор, т.е. несколько IDNom
        if(IDNom == slTockens->Strings[k] && StrGrid->Cells[1][i] == "")
        {
           StrGrid->Cells[1][i] = Nnum->Text;                     // если место не занято, вводим сканкод
           StrGrid->Cells[colNum][i] = IDNom;         // меняем на фактический IDNom
           StrGrid->Row = i;
         if(Nnum->Text.Length() == 10)
         {
           StrGrid->Cells[7][i] = CardNominal;
         }
         else
         {
           StrGrid->Cells[7][i] = Price->Text;
         }
           return true;
        }
       }
      }
  return false;  // если не нашли подходящего IDNom в подарках
}
//----------------------------------------------------------------------------
// проеверка все ли подарки в чеке введены
bool __fastcall TMainWindow::VerifyAllPresent(TStringGrid *StrGrid, int colNum)
{
AnsiString str="";
     for(int i = 1; i < StrGrid->RowCount;i++)
     {
         if(StrGrid->Cells[0][1] == "") return true;
         str = StrGrid->Cells[colNum][i];
         if(str.Length() < 10) return false;
     }
return true;
}

//----------------------------------------------------------------------------
bool __fastcall TMainWindow::InsertToBill(TStringGrid *Grid)
{
    AnsiString GN,GNA;
    bool res = true;
   //Очистка таблицы счета  bill
    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("delete from bill");
    try
    {
      PriceQuery->ExecSQL();
    }
    catch (EOleException &eException)
    {
        Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
        log(errormsg);
        return false;
     }

    // Строчки чека по одной скидываются в таблицу bill
    for(int i = 1; i < Grid->RowCount && !Grid->Cells[1][i].IsEmpty(); i++)
    {
    //Обрабатываем кавычки и табуляции в наименовании
    GN = Grid->Cells[2][i];
    GNA = "";
    for(int GNC=1;GNC <= GN.Length(); GNC++)
      if(GN.SubString(GNC,1) == "'") GNA += "''";
        else if(GN.SubString(GNC,1) != "\t") GNA += GN.SubString(GNC,1);

    PriceQuery->SQL->Clear();
    PriceQuery->SQL->Add("insert into bill (IDNom, Quantity, Price, Operator)");
    PriceQuery->SQL->Add("values ("+Grid->Cells[13][i]+","+Grid->Cells[5][i]+","+Grid->Cells[4][i]+",left('"+Grid->Cells[8][i]+"',15))" );

    try
      {
      PriceQuery->ExecSQL();
      }
      catch (EOleException &eException)
      {
        Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
        PlaySound("oy.wav",0,SND_ASYNC);
        AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
        log(errormsg);
        PriceQuery->Active = false;
        PriceQuery->Close();
        return false;
       }

    if(PriceQuery->RowsAffected != 1)
      {
      res = false;
      Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
        //DumpLog
      log("Ошибка SQL " + PriceQuery->SQL->Text);
      PriceQuery->Active = false;
      PriceQuery->Close();
      return res;
      }
    }
    PriceQuery->Active = false;
    PriceQuery->Close();
return res;
}
//------------------------------------------------------------------------------
// добавление товара в Grid подарков
void __fastcall TMainWindow::AddToPresentGrid(TStringGrid *Grid)
{
 //bool res;
 ClearGrid(Grid);
 AnsiString ID ="";
 AnsiString str="";
 int s = 0;     // счетчик строк в Grid
 CentralQuery->Active = false;
 CentralQuery->Close();

 PriceQuery->SQL->Clear();
 PriceQuery->SQL->Add("SELECT sys.fn_varbintohexstr(p.IDStock) as IDStock,sys.fn_varbintohexstr(p.IDNom) as IDNom, ");
 PriceQuery->SQL->Add("(select case when p.Remain < 0 Then p.Quantity ");
 PriceQuery->SQL->Add("when p.[Quantity] > p.Remain Then p.Remain else p.[Quantity] end) as Quantity,p.[Description],p.[WavFileName],c.[Name], p.Remain FROM PotentialPresent p");
 PriceQuery->SQL->Add("LEFT JOIN price c ON p.IDNom = c.IDNom");
 PriceQuery->SQL->Add("ORDER BY IDStock");
 try
 {
     PriceQuery->Active = true;
 }
 catch (EOleException &eException)
 {
     Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
     log(errormsg);
     PriceQuery->Active = false;
     PriceQuery->Close();
     return;
 }

   for(int i = 0;i< PriceQuery->RecordCount;i++)
   {
      if(ID == PriceQuery->FieldByName("IDStock")->AsString){  // сравнение текущего и предыдущего IDStock
         str = Grid->Cells[5][s];                                  // если совпадает, тогда считаем, что подарков по данной акции несколько на выбор
         str += "|" + PriceQuery->FieldByName("IDNom")->AsString;    // сращиваем подарки через разделитель и помещаем в одну строку
         Grid->Cells[5][s] = str;
      }
      else     // переходим к обработке следующей акции
      {
// если подарков 2 и более, то разбиваем их по строкам Grid
        for(int k = 0;k < PriceQuery->FieldByName("Quantity")->AsInteger;k++)
        {
         while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("101");
         Grid->RowCount = s + 2;
         LeaveCriticalSection(&CS); log("001");
         Grid->Cells[0][s+1] = s + 1;
         Grid->Cells[2][s+1] = PriceQuery->FieldByName("Name")->AsString;
         Grid->Cells[3][s+1] = "1";        //PriceQuery->FieldByName("Quantity")->AsString; // пока каждый подарок уникален
         Grid->Cells[4][s+1] = "шт.";
         Grid->Cells[5][s+1] = PriceQuery->FieldByName("IDNom")->AsString;
         Grid->Cells[6][s+1] = PriceQuery->FieldByName("IDStock")->AsString;
         s++;
        }
       }
       ID = PriceQuery->FieldByName("IDStock")->AsString;
       PriceQuery->Next();
   }
   PriceQuery->Active = false;
   PriceQuery->Close();
// заполним номиналы карт
   for(int j = 1; j < Grid->RowCount; j++)
   {
         TStringList *slTockens = new TStringList;
         Explode(Grid->Cells[5][j],"|",slTockens,true);
         CentralQuery->SQL->Clear();
         CentralQuery->SQL->Text = "select TOP 1 Nominal from GiftCard where IDNom = " + slTockens->Strings[0];  // получаем первый номинал
         try
         {
            CentralQuery->Active = true;
         }
         catch(EOleException &eException)
         {
            Name->Caption = "Ошибка центрального SQL. Работа с Акциями временно невозможна";
            PlaySound("oy.wav",0,SND_ASYNC);
            AnsiString errormsg= "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
            log(errormsg);
            CentralQuery->Active = false;
            CentralQuery->Close();
            return;
         }
         Grid->Cells[7][j] = CentralQuery->FieldByName("Nominal")->AsString;
         if(Grid->Cells[2][j] == ""){  // не нашли подарок в price
            Grid->Cells[2][j] = "* Подарочная карта " + CentralQuery->FieldByName("Nominal")->AsString + "руб.";
            if(CentralQuery->FieldByName("Nominal")->AsString == "") Grid->Cells[2][j] = "Не найден подарок";
         }
      CentralQuery->Active = false;
      CentralQuery->Close();
      PriceQuery->Active = false;
      PriceQuery->Close();
      delete slTockens;
   }
   Grid->Row =  s;
}
// -------------------------------------------------------------------------
// Очистка верхних форм и полей
void __fastcall TMainWindow::ClearForm()
{
//            NewCode = true;
//            NewPrice = false;
//            InputQnty = false;
            Qnty->Color = clBtnFace;
            Name->Caption = "";
            Unit->Caption = "";
            Price->Text = "";
            Qnty->Text = "";
            Nnum->Text = "";
            PresentLabel->Caption = "";
            PresentLabel->Color = clBtnFace;

}
//------------------------------------------------------------------------
void __fastcall TMainWindow::WritePresent(TStringGrid *Grid)
{
   AnsiString str="";

   for(int i = 1; i < Grid->RowCount; i++)
   {
      if(!Grid->Cells[1][i].IsEmpty()){
      str += "INSERT INTO [PreGivePresents] ([IDStock],[BillDateTime],[SCash],[ScanCode],[BillNumber],[Sklad],[IDnom],[Quantity],[Amount]) ";
      str += "VALUES ("+Grid->Cells[6][i]+",CURRENT_TIMESTAMP,'" + Star->Serial + "','" +Grid->Cells[1][i].SubString(1,12)+ "','"+ Star->BillNumber +"',0x"+Department+","+Grid->Cells[5][i]+","+Grid->Cells[3][i]+","+Grid->Cells[7][i]+");";
      }
   }
   if(str.IsEmpty()) return;
   PriceQuery->SQL->Clear();
   PriceQuery->SQL->Text = str;
   try
   {
    PriceQuery->ExecSQL();
   }
   catch (EOleException &eException)
   {
     Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
     errormsg = "Ошибка проверки выполнения условий акций " + errormsg;
     log(errormsg);
   }
 PriceQuery->Active = false;
 PriceQuery->Close();
}
// -------------------------------------------------------------------
void __fastcall TMainWindow::ClearGrid(TStringGrid *Grid)
{
   while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("Попытка очистки таблицы");
   for(int i = 1; i< Grid->RowCount; i++)
   {
      Grid->Rows[i]->Clear();
   }
   Grid->RowCount = 2;
    LeaveCriticalSection(&CS); log("Попытка очистки успешна");
}
//-------------------------------------------------------------------------
// добавляем в чек информацию о подарков
bool __fastcall TMainWindow::PresentPrintBill(TStringGrid *Grid)
{
AnsiString C;
bool f = true;

for(int i = 1; i < Grid->RowCount ; i++)
  {
   if(Grid->Cells[1][i].Length() < 10) continue;
   if(f) // выполняется только один раз
   {
     Star->Print("--------------- ПОДАРКИ ----------------",true);
     f = false;
   }

   C = Grid->Cells[2][i] + " " + Grid->Cells[1][i];
   Star->Print(C.c_str(),true);
  }
return true;
}

// --------------------------------------------------------------------
void __fastcall TMainWindow::ClearPresentClick(TObject *Sender)
{
    ClearGrid(PresentGrid);
    PresentEnter = False;
    PresentLabel->Visible = false;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::UpdateLocalCounts()
{
AnsiString str;
// запрашиваем счетчики из центральной базы
CentralQuery->SQL->Clear();
CentralQuery->SQL->Text = "select sys.fn_varbintohexstr(IDStock) as IDStock,MaxPresentCount,CountUP from PreStock where MaxPresentCount > 0";
   try
   {
       CentralQuery->Active = true;
   }
   catch (EOleException &eException)
   {
     Name->Caption = "Ошибка центрального сервера. Работа с акциями недоступна!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + CentralQuery->SQL->Text;
     errormsg = "Ошибка проверки чтения счетчиков " + errormsg;
     log(errormsg);
     return false;
   }
// обновляем счетчики в локальной базе
  for(int i = 0; i< CentralQuery->RecordCount; i++)
  {
      str += "update PreStock SET CountUP = " + CentralQuery->FieldByName("CountUP")->AsString;
      str += " where IDStock = " + CentralQuery->FieldByName("IDStock")->AsString + "; ";
      CentralQuery->Next();
  }
  PriceQuery->SQL->Clear();
  PriceQuery->SQL->Text = str;
   try
   {
       PriceQuery->ExecSQL();
   }
   catch (EOleException &eException)
   {
     Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
     errormsg = "Ошибка записи счетчиков подарков " + errormsg;
     log(errormsg);
     return false;
   }
   PriceQuery->Active = false;
   PriceQuery->Close();
   CentralQuery->Active = false;
   CentralQuery->Close();
  return true;
}
//---------------------------------------------------------------------
bool __fastcall TMainWindow::UpdateCentralCounts(TStringGrid *Grid)
{
 int rows;
 bool res = true;
   AnsiString IDNomPr;
   for(int i = 1;i< Grid->RowCount; i++)
   {
      if(Grid->Cells[1][i]!= ""){
         CentralQuery->SQL->Clear();
         CentralQuery->SQL->Add("Update PreStock SET CountUP = t.CountUp");
         CentralQuery->SQL->Add("From (select IDStock,(CountUp + " + Grid->Cells[3][i] + ") AS CountUp from PreStock where CountUP + " + Grid->Cells[3][i] + " <= MaxPresentCount and IDStock = " + Grid->Cells[6][i] + ") t");
         CentralQuery->SQL->Add("Where PreStock.IDStock = t.IDStock");
     try
       {
         rows = CentralQuery->ExecSQL();
       }
       catch(EOleException &eException)
       {
         AnsiString Str="EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\""  + CentralQuery->SQL->Text;
//         ShowMessage(Str);
         log(eException.Message);
         rows = 0;
         res = false;
       }
       Grid->Cells[8][i] = rows;
       res = (rows != 0);
//       CentralQuery->Requery(CentralQuery->ExecuteOptions);
       CentralQuery->Active = false;
       CentralQuery->Close();
       }

    }
   return res;
}
//-------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckCentralCounts(TStringGrid *Grid)
{
   bool res = true;
   int Quantity = 0;
   int i, k;
   AnsiString ID = "";
   i = 1;
   while (i < Grid->RowCount)
   {
      if(ID == Grid->Cells[6][i]){i++; continue;}
      if(Grid->Cells[1][i].Length() < 10) {i++; continue;}

      ID = Grid->Cells[6][i];
      Quantity = StrToInt(Grid->Cells[3][i]);
      for(int k = i + 1; k < Grid->RowCount; k++)
      {
         if(Grid->Cells[1][k].Length() < 10) continue;
         if(ID == Grid->Cells[6][k])
         {
            Quantity = Quantity + StrToInt(Grid->Cells[3][i]);
         }
      }

         CentralQuery->SQL->Clear();
         CentralQuery->SQL->Add("select [IDStock],[MaxPresentCount],[CountUP] from [PreStock] ");
         CentralQuery->SQL->Add(" where ([MaxPresentCount] < 0 or ([MaxPresentCount] > 0 and [CountUp] + " + IntToStr(Quantity)  + " <= [MaxPresentCount])) ");
         CentralQuery->SQL->Add(" and [IDStock] = " + Grid->Cells[6][i]);
         try
         {
            CentralQuery->Active = true;
         }
         catch(EOleException &eException)
          {
            AnsiString Str="EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\""  + CentralQuery->SQL->Text;
//            ShowMessage(Str);
            log(Str);
            res = false;
          }
         if(CentralQuery->RecordCount == 0)
         {
            res = false;
            CentralQuery->Active = false;
            CentralQuery->Close();
            return res;
         }
         CentralQuery->Active = false;
         CentralQuery->Close();
      i++;
   }
return res;
}
//--------------------------------------------------------------------
// сохранение Grid в файл или его удаление, если Grid пустой
void __fastcall TMainWindow::SaveGridToFile(TStringGrid *Grid, AnsiString FileName)
{
AnsiString str = "";
AnsiString Separator = "|";
TStringList *slTable = new TStringList;
if(FileExists(FileName)) DeleteFile(FileName);
// Cells[0][1] индикатор пустой первой после заголовка строки
if(Grid->Cells[0][1].IsEmpty()) return;
   for(int i = 1;i < Grid->RowCount; i++)
   {
      for(int k = 0;k < Grid->ColCount; k++)
      {
         str += Grid->Cells[k][i] + Separator;
      }
      str = StringReplace(str, "\r", "%R%", TReplaceFlags()<< rfReplaceAll << rfIgnoreCase);
      str = StringReplace(str, "\n", "%N%", TReplaceFlags()<< rfReplaceAll << rfIgnoreCase);
      slTable->Add(str.SubString(1,str.Length()-1));
      str = "";
   }
   if(slTable->Count != 0) slTable->SaveToFile(FileName);
   delete slTable;
}
//--------------------------------------------------------------------
// Загрузка файла в Grid
void __fastcall TMainWindow::LoadGridFromFile(TStringGrid *Grid, AnsiString FileName)
{
if(!FileExists(FileName)) return;
AnsiString str = "";
AnsiString Separator = "|";
TStringList *slTable = new TStringList;
TStringList *slTockens = new TStringList;
slTable->LoadFromFile(FileName);
if(slTable->Count == 0) return;
while(!TryEnterCriticalSection(&CS))
    { log("Function:LoadGridFromFile Вход в критическую секцию");
      Sleep(1000);
     }
Grid->RowCount = slTable->Count + 1;
   for(int i = 1;i < Grid->RowCount; i++)
   {
      Explode(slTable->Strings[i-1], Separator, slTockens, false);
      for(int k = 0; k < slTockens->Count; k++)
      {
         AnsiString str = StringReplace(slTockens->Strings[k], "%R%", "\r", TReplaceFlags()<< rfReplaceAll << rfIgnoreCase);
         str = StringReplace(str, "%N%", "\n", TReplaceFlags()<< rfReplaceAll << rfIgnoreCase);
         Grid->Cells[k][i] = str;
      }
      slTockens->Clear();
   }
   LeaveCriticalSection(&CS);
   delete slTable;
   delete slTockens;
   log("Чек загружен.");
}
//-----------------------------------------------------------------------
// читаем параметры из INI-файла
//void __fastcall MainWindow::ReadParams()
//{
//   TIniFile * Ini = new TIniFile(ExtractFilePath(Application->ExeName)) + "cashier.ini");

//}

// вывод меток
//------------------------------------------------------------------------
// обработка акций
bool __fastcall TMainWindow::StocksProcessing()
{
// заполняем таблицу Bill
    ArrangeTable();
    InsertToBill(Grid);

// Проверяем подарки в чеке, в случае если они есть - выводим сообщение для кассира
      if(!PresentEnter && (PresentGrid->Cells[0][1].IsEmpty())){   // если мы еще не в режиме ввода подарков и строки подарков еще не считаны
         if(StocksInCheck()) { // если подарки есть
           PresentEnter = true;
//           StockInfoShow(false);
           InfoShow(PresentEnter, false);
           AddToPresentGrid(PresentGrid); // считываем подарки
           DownBool(Printing);
           PlayStockSound();
           return false;
           }
        }
        else   // если мы уже в режиме или в таблице уже есть данные по подаркам
        {
            if(!VerifyAllPresent(PresentGrid, 1)) if(MessageBox (GetActiveWindow(),
              "Введены не все подарки, продолжить печать чека?","ВНИМАНИЕ!", MB_OKCANCEL | MB_DEFBUTTON2	| MB_ICONQUESTION	) == IDCANCEL){
                  PresentEnter = true;
                  PresentLabel->Visible = true;
                  DownBool(Printing);
                  PayType = NO_PAYMENT;
                  return false;
              }
            if(!CheckCentralCounts(PresentGrid))
            {
               PresentEnter = true;
               Name->Caption = "Некоторые подарки закончились, необходимо пересканировать подарки";
               ClearGrid(PresentGrid);
               UpdateLocalCounts();
               StocksInCheck();
               AddToPresentGrid(PresentGrid); // считываем подарки
               if(PresentGrid->RowCount > 1)
               {
                  PresentLabel->Caption = "Пересканируйте подарки";
                  PresentLabel->Color = clYellow;
               }
               else
               {
                  PresentEnter = false;
                  PresentLabel->Visible = false;
               }
               DownBool(Printing);
               PayType = NO_PAYMENT;
               return false;
            }
        }
    PresentEnter = false;
return true;
// закончен блок обработки акций

}
//--------------------------------------------------------------
// Проигрывает файл указанный в акции
void __fastcall TMainWindow::PlayStockSound()
{

//   PriceQuery->SQL->Clear();
      PriceQuery->SQL->Text = "select WavFileName from PotentialPresent where WavFileName <> ''";
      try
      {
        PriceQuery->Active = true;
      }
      catch(EOleException &eException)
      {
         Name->Caption = "Ошибка записи SQL. Немедленно прекратить работу!";
         PlaySound("oy.wav",0,SND_ASYNC);
         AnsiString errormsg= "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
         log(errormsg);
         return;
      }
//      AnsiString *WavFileName = new AnsiString[PriceQuery->RecordCount];
      for(int i = 0; i < PriceQuery->RecordCount; i++)
      {
//         ShowMessage((SoundDir + "\\" + PriceQuery->FieldByName("WavFileName")->AsString).c_str());
         PlaySound((SoundDir + "\\" + PriceQuery->FieldByName("WavFileName")->AsString).c_str(),0,SND_ASYNC);
         PriceQuery->Next();
      }
        PriceQuery->Active = false;
        PriceQuery->Close();
}
//-----------------------------------------------------------------------
/* void __fastcall TMainWindow::GetKKMStatus()
{
   TMStarF
}
*/

//-----------------------------------------------------------------------
// размазывает скидку по позициям, считает ошибку округления
void __fastcall TMainWindow::DiscountCalc(int Rnd)
{
//   int Cost_orig = Price_orig * Quantity_orig;
   hyper Err;
   unsigned hyper TotalOrig = 0, TotalDisc = 0;
   hyper Discount = 0, pr, qt;
   double ratio = 0, ss, DiscSum,sum,tmpPK;
   int StrNum = 0;
   AnsiString Code;

   if(Grid->Cells[0][1].IsEmpty()) return;  // защита от деления на ноль
// считаем сумму чека без скидки в копейках
   for(int i = 1; i < Grid->RowCount; i++)
   {
      // сначала очищаем от предыдущих расчетов
      Grid->Cells[PG_ESTIMATED_PRICE_COL][i] = Grid->Cells[PG_PRICE_COL][i];
      Grid->Cells[PG_CORRECTION_COL][i] = 0;
      if(!Grid->Cells[PG_COST_COL][i].IsEmpty())
      {
         // Цена как есть
        TotalOrig += X(MoneyAshyper(Grid->Cells[PG_PRICE_COL][i]), QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i]));  // стоимость без скидки
      }
   }
//   CalcTotal(); // в TSum->Text сумма чека с вычетом ПК и без округления
//   TotalOrig -= MoneyAshyper(TOff->Text);  //  отнимаем сумму округления
   Discount = GiftCardCalcPayments();// + TotalOrig%Rnd;   // сумарный вычет подарочными картами
   if (Discount == 0) return;  // если ПК нет, то и считать нечего
   TotalDisc = TotalOrig - Discount;  // сумма за вычетом ПК
   // расскидываем вычет подарочных карт равномерно по всем позициям
   ratio = ((double)TotalOrig - Discount)/(double)TotalOrig;       // примерный коэффициент
   DiscSum = 0;
   while(!TryEnterCriticalSection(&CS)) Sleep(1000); log("Попытка входа в критическую секцию: дисконт");
   for (int i = 1; i < Grid->RowCount; i++)    // считаем цены для в копейках
   {
       ss = ratio*MoneyAshyper(Grid->Cells[PG_PRICE_COL][i]);
       ss = RoundTo(ss, 0);
       Grid->Cells[PG_ESTIMATED_PRICE_COL][i] = MoneyAsString(ss);
       DiscSum += RoundTo(ss * QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i])/1000.0, 0); // сумма со всеми вычетами
   }
   DiscSum = RoundTo(DiscSum,0);
   Err = TotalDisc - DiscSum;     // считаем расхождения между оригинальной суммой и полученной
   if (Err != 0)
   {
      for(int i = 1; i < Grid->RowCount; i++)     // ищем первый товар с единичным количеством или весовой
      {
         qt = QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i]);
         if (qt == 1000 ) // || qt%1000 > 0
         {
            StrNum = i;
            Grid->Cells[PG_CORRECTION_COL][i] = FloatToStr((double)Err/100);
//            Grid->Cells[PriceCol][StrNum] = MoneyAsString(MoneyAshyper(Grid->Cells[PriceCol][StrNum]) + Err);
//            Grid->Cells[CostCol][StrNum] = MoneyAsString(MoneyAshyper(Grid->Cells[PriceCol][StrNum]) + Err);
            break;
         }
      }
      if (StrNum == 0)    // если единичного товара нет, тогда вычленяем одну единицу из последней позиции не весовой позиции
      {
         for(int i = Grid->RowCount -1; i > 0; i--)
         {
            qt = QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i]);
            if (qt%1000 == 0)
            {
               StrNum = i;
               Grid->Cells[PG_CORRECTION_COL][i] = FloatToStr((double)Err/100);
               break;
            }
         }
      }
      if (StrNum == 0) //   если не нашли множественного товара, тогда скорее всего есть только весовой - выбираем последнюю позицию в чеке
      {
         std::vector<int> GCRowNum;
         std::vector<int>::iterator it;
         Grid->Cells[PG_CORRECTION_COL][Grid->RowCount - 1] = FloatToStr((double)Err/100);
         pr = MoneyAshyper(Grid->Cells[PG_ESTIMATED_PRICE_COL][Grid->RowCount - 1]);
         qt = QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][Grid->RowCount - 1]);
//            TStringList * GCNum = new TStringList; // номер карты
//            TStringList * GCBal = new TStringList; // баланс карты
//            TStringList * GCResid = new TStringList; // остаток карты
//            for(int k = 0; k < GiftCardM->Lines->Count;k++) // считываем все карты уже введенные на кассе
            for(int k = 1; k < ComboPayGrid->RowCount; k++)
            {
//               GCNum->Add(GiftCardM->Lines->Strings[k].SubString(1,10));
//               GCBal->Add(GiftCardM->Lines->Strings[k].SubString(21,10).Trim());
//               GCResid->Add(GiftCardM->Lines->Strings[k].SubString(41,10).Trim());
               if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][k] == GIFT_CARD_PAYMENT)
               {
                  GCRowNum.push_back(k);
//                  GCNum->Add(ComboPayGrid->Cells[CP_CARD_ID_COL][k]);
//                  GCBal->Add(ComboPayGrid->Cells[CP_SUM_COL][k]);
               }
            }
            hyper SumG = 0; //MoneyAshyper(TSum->Text); // + MoneyAshyper(TOff->Text); // просто сумма без округления
            for(int k=1; k<Grid->RowCount;k++)
            {
               SumG += MoneyAshyper(Grid->Cells[PG_COST_COL][k]);
            }
            SumG -= GiftCardCalcPayments();
            sum = RoundTo(SumExcept(PG_ESTIMATED_PRICE_COL, true) + pr*qt/1000.0,0);   // сумма
//            sum -= (long)sum%Round;

            while(SumG - sum > 0)  // делаем, чтобы err была отрицательной и максимальной (ближе к нулю)
            {
               pr++;
               sum = RoundTo(SumExcept(PG_ESTIMATED_PRICE_COL, true) + pr * qt/1000.0,0);
//               TOff->Text = MoneyAsString((long)sum%Round);
//               sum -= (long)sum%Round;
            }
            Grid->Cells[PG_ESTIMATED_PRICE_COL][Grid->RowCount - 1] = MoneyAsString(pr);
// формула учитывает сумму желательных платежей по всем ПК в чеке
// желательная сумма ПК = сумма остатков между оригинальной и расчетной ценой + разница сумм всех позиций кроме этой
            if(SumG - sum != 0)
            {
//               tmpPK = RoundTo((MoneyAshyper(Grid->Cells[4][Grid->RowCount - 1]) - pr)*qt/1000.0 - SumExcept(14, true) + SumExcept(4, true), 0); // это желательная сумма всех платежей по ПК
               tmpPK = (int)((MoneyAshyper(Grid->Cells[PG_PRICE_COL][Grid->RowCount - 1]) - pr)*qt/1000.0
                     - SumExcept(PG_ESTIMATED_PRICE_COL, true) + SumExcept(PG_PRICE_COL, true));
/*               GiftCardClearPayments();
               for(int j = 0; j< GCNum->Count; j++)   // после очистки записываем все карты до тех пор, пока не обнуляется желаемая сумма
               {
                  if(tmpPK < 0) break;
                  GiftCardAddPayment1(GCNum->Strings[j].Trim(), GCBal->Strings[j], GCResid->Strings[j],MoneyAsString(tmpPK));
                  tmpPK -= MoneyAshyper(GCBal->Strings[j]);
               }  */
               for(it = GCRowNum.begin(); it < GCRowNum.end(); ++it)
               {
                  if(tmpPK < 0)
                  {
                     RemoveRow(ComboPayGrid, *it);
                     continue;
                  }
                  GiftCardSumRenew(ComboPayGrid->Cells[CP_CARD_ID_COL][*it],ComboPayGrid->Cells[CP_SUM_COL][*it],
                      ComboPayGrid->Cells[CP_RESIDUE_COL][*it], tmpPK);
                  tmpPK -= MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][*it]);
               }
            }
         Grid->Cells[PG_CORRECTION_COL][Grid->RowCount - 1] = FloatToStr((double)(SumG - sum)/100);
/*         delete GCNum;
         delete GCBal;
         delete GCResid; */
      }
      LeaveCriticalSection(&CS);
   }
return;
}
//---------------------------------------------------------------------
double __fastcall TMainWindow::RoundTo(double val, int Digit)
{
   double div = 1.0;
   if(Digit >=0)
   {
      while(Digit--) div *=10.0;
   }
   else
   {
      while(Digit++) div /=10.0;
   }
return floor(val*div+0.5)/div;
}
// --------------------------------------------------------------
bool __fastcall TMainWindow::GiftCardAddPayment1(AnsiString Code,AnsiString Balance, AnsiString Residual, AnsiString Sum)
{
// На данный момент карта найдена в таблице и информация по ней в рекордсете

unsigned hyper Summ,Bal,CardPay,Rem;
AnsiString CP,Remain;

// CalcTotal();
Summ = MoneyAshyper(Sum);
Bal = MoneyAshyper(Balance) + MoneyAshyper(Residual);
if(Bal > Summ) CardPay = Summ; else CardPay = Bal;   // условие
if(CardPay <= 0) return false;
Rem = Bal-CardPay;
CP = MoneyAsString(CardPay);
Remain = MoneyAsString(Rem);
while(CP.Length()<10) CP = " "+CP;
while(Remain.Length()<10) Remain = " "+Remain;
GiftCardM->Lines->Add(Code+" на сумму "+CP+"  остаток "+Remain);
return false;
}
//----------------------------------------------------------------------
// сумма всех строк кроме той, что с коррекцией
hyper __fastcall TMainWindow::SumExcept(int Rows, bool N)
{
   hyper sum = 0;
   for(int i = 1; i < Grid->RowCount; i++)
   {
      if(Grid->Cells[15][i] == "0" && N)
      {
         sum += RoundTo(MoneyAshyper(Grid->Cells[Rows][i])*QuantityAshyper(Grid->Cells[5][i])/1000.0, 0);
      }
      else if (Grid->Cells[15][i] != "0" && !N)
      {
         sum += RoundTo(MoneyAshyper(Grid->Cells[Rows][i])*QuantityAshyper(Grid->Cells[5][i])/1000.0, 0);
      }
   }
return sum;
}


//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CreateBillBody(bool onReceipt)
{
 double qt,pr,cost,err,sum,tmpPK;
 long st;
 AnsiString str;
 bool res = false;
 unsigned int Rnd = 1;
 DWORD dw;
 std::vector<ComboPay>::iterator it;
 if(PayType == NO_PAYMENT)
 {
   log("Не определен тип платежа PayType.");
   return false;
 }
 if(PayType == NAL_PAYMENT) Rnd = Round;

// Предварительно очищаем вектора
   PrintLineData.clear();   // то что печатается на чеке
   ShadowLineData.clear();  // то что уходит в налоговую (с учетом скидок и надбавок по ФЗ-54)
// цикл по всем позициям грида
      for(int i = 1; i < Grid->RowCount; i++)
      {
        // проверка на пустоту
        if(
           (Grid->Cells[1][i]=="") ||
           (Grid->Cells[2][i]=="") ||
           (Grid->Cells[3][i]=="") ||
           (Grid->Cells[4][i]=="") ||
           (Grid->Cells[5][i]=="") ||
           (Grid->Cells[13][i]=="") ||
           (Grid->Cells[12][i]=="")
        )
        {
           Name->Caption = "Испорчен чек - нужно отсканировать заново";
           DownBool(Printing);
           PlaySound("oy.wav",0,SND_ASYNC);
           return false;
        }
        pr = MoneyAshyper(Grid->Cells[4][i]);
        qt = QuantityAshyper(Grid->Cells[5][i]);
// Заполняем вектор реальных данных для налоговой
         ShadowLineData.push_back(BillItemLine(Grid->Cells[PG_CODE_COL][i], Grid->Cells[PG_NAME_COL][i], Grid->Cells[PG_MEASURE_COL][i],
           MoneyAshyper(Grid->Cells[PG_PRICE_COL][i]), QuantityAshyper(Grid->Cells[PG_QUANTITY_COL][i]), 0, StrToInt(Grid->Cells[PG_NDS_COL][i])));
//       }
      } // конец цикла по строкам грида
// Создаем и заполняем структуру с характеристиками продажи
   mfBill.Sale = Sale;
   mfBill.CasName = CasName;
   mfBill.CasNumber = CasNumber;
   mfBill.Recived = Recived->Text;
   mfBill.Round = Rnd;

// заполняем структуру информацией платежей по подарочным картам
//   hyper Spk =0;
   GiftItemData.clear();

   for(int i = 1; i < ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == GIFT_CARD_PAYMENT &&
         ComboPayGrid->Cells[CP_STATUS_COL][i] == CP_COMPLETE_STATUS)
      {
         GiftItemData.push_back(GiftCardData(ComboPayGrid->Cells[CP_CARD_ID_COL][i],
         ComboPayGrid->Cells[CP_SUM_COL][i], ComboPayGrid->Cells[CP_RESIDUE_COL][i]));
      }
   }
// заполняем суммы
   mfBill.vPay.clear();
   ComboPay sPay;
//   memset(&sPay,0,sizeof(sPay));
   for(int i=1; i< ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_STATUS_COL][i] != CP_COMPLETE_STATUS) continue;
      sPay.PayType = ComboPayGrid->Cells[CP_PAY_TYPE_COL][i];
      if(sPay.PayType == NAL_PAYMENT)
      {
         sPay.Sum = MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][i]) - MoneyAshyper(Change->Text);
         sPay.Change = MoneyAshyper(Change->Text);
         Star->OpenBox();
      }
      else
      {
         sPay.Sum = MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][i]);
         sPay.Change = 0;
      }
      AnsiString Check = ComboPayGrid->Cells[CP_CHECK_COL][i].SubString(1,999);
      Check = ShieldingString(Check);
      strcpy(sPay.RRN, ComboPayGrid->Cells[CP_RRN_COL][i].c_str());
      strcpy(sPay.CardName, ComboPayGrid->Cells[CP_CARD_NAME_COL][i].c_str());
      strcpy(sPay.Check, Check.c_str());
      sPay.SberOwnCard = StrToIntDef(ComboPayGrid->Cells[CP_SBOWN_COL][i].c_str(),0);
      sPay.RoundPart = StrToIntDef(ComboPayGrid->Cells[CP_ROUND_COL][i],0);
      sPay.CType = StrToIntDef(ComboPayGrid->Cells[CP_CARD_TYPE_COL][i],0);
      strcpy(sPay.CardID,ComboPayGrid->Cells[CP_CARD_ID_COL][i].c_str());

      mfBill.vPay.push_back(sPay);
   }
   // если нам не нужно печатать чек и данных по платежам тоже нет
   if(!onReceipt && mfBill.vPay.size() == 0)
   {
      sPay.PayType = PayType;
      sPay.Sum = MoneyAshyper(TSum->Text);
      sPay.Change = MoneyAshyper(Change->Text);
      sPay.RoundPart = MoneyAshyper(TOff->Text);
      if(MoneyAshyper(GetLastCardCheckSumm()) == sPay.Sum)
      {
         TStringList *CheckList = new TStringList;
         CheckList->LoadFromFile(ProgramPath + "check.txt");
         AnsiString Check = CheckList->Text;
         Check = ShieldingString(Check);
         strcpy(sPay.Check, Check.c_str());
      }
   }

   // заполняем структуру информацией по акционным подаркам
   PresentItemData.clear();
   for(int i = 1; i < PresentGrid->RowCount;i++)
   {
      if(PresentGrid->Cells[1][i] == "") continue;
      PresentItemData.push_back(PresentsData(PresentGrid->Cells[1][i], PresentGrid->Cells[2][i].Trim()));
   }

   if(onReceipt)
   {
      //закоментировано для теста
      res = Star->BillComplete();
      // ждем события завершения закрытия чека
      dw = WaitForSingleObject(hEvent1, 10000);
         switch (dw)
         {
            case WAIT_FAILED:
            res = false;
            log("WAIT_FAILED");
            break;

            case WAIT_TIMEOUT:
            res = false;
            log("WAIT_TIMEOUT");
            break;

            case WAIT_OBJECT_0:
            log("WAIT_OBJECT_0");
//         if(mfBill.Status == 0) {res = true;}
//         else {res = false;}
            break;
         }
//res = true;
      if(!res)
      {
         GetKKMError();
         log("Ошибка, чек не пробит. Результат последней операции " + IntToHex(frStatus.OperationResult, 2) + "h");
         Star->DeleteDoc();
         PayType = NO_PAYMENT;
         return false;
      }
   }
return res;
}
//------------------------------------------------------------
// для отладки
/*
AnsiString __fastcall TMainWindow::ftc(AnsiString fn)
{
char an[2048];
int i =0;
size_t size;
DWORD dw;
   ifstream data;
   bool res=false;
   data.open(fn.c_str(), ios::in|ios::binary|ios::ate);
//   while(!data.eof())
   if(data.is_open())
   {
//      data.get(an[i]);
//      data >> an[i];
      data.seekg(0, ios::end);
      size = data.tellg();
      data.seekg(0, ios::beg);
      data.read(an, size);
      an[size] = '\0';
      i++;
   }
   data.close();
//      }
  return an;
} */
//-----------------------------------------------------------------
bool __fastcall TMainWindow::FRInit()
{
  Star->GetStatus();
  int q = 0;
  switch(Star->Mode){
   case 1:
      log("Выдача данных");
   break;
   case 2:
     switch(Star->SubMode)
     {
         case 0:
            return true;
         case 1:
         case 2:
            while(Star->SubMode == 1 || Star->SubMode == 2)
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
               Star->GetStatus();
            }
           if(Star->SubMode == 0)
           {
               Star->ResumePrint();
               return FRInit();
           }
            else if(Star->SubMode == 3)
            {
               return FRInit();
            }
            else
            {
               return false;
            }
         case 3:
            Star->ResumePrint();
            return FRInit();
         case 5:
            int k = 0;
            while(Star->GetSubMode() == 5 && k < 60) {Sleep(1000); k++;}
            if(Star->GetSubMode() == 0) return FRInit();
            else return false;
     }
   case 3:
      Name->Caption = "Смена не закрыта, 24 часа закончились. Закройте смену!!!";
      return false;
   case 8:
      if(Star->SubMode == 3)
      {
         Star->ResumePrint();
         return FRInit();
      }
//      Star->DeleteDoc();
      if(Star->KKMStatus == "0000") return true;
      else return false;
   case 9:
      Name->Caption = "Необходимо технологическое обнуление ККМ!!!";
      return false;
  }
 return false;
}
//--------------------------------------------------------------------------
/*void __fastcall TMainWindow::SetLine(sLine *Obj,int line, AnsiString ScanCode, AnsiString Name, AnsiString Measure, hyper Price, hyper Quantity, hyper Sum, int NDS)
{
   Obj[line-1].ScanCode = ScanCode;
   Obj[line-1].Name = Name;
   Obj[line-1].Measure = Measure;
   Obj[line-1].Price = Price;
   Obj[line-1].Quantity = Quantity;
   Obj[line-1].Sum = Sum;
   Obj[line-1].NDS = NDS;
}  */
//----------------------------------------------------------------------------
void __fastcall TMainWindow::ClearBillData()
{
    mfBill.BillNumber="";
    mfBill.Sale = false;
    mfBill.CasName="";
    mfBill.CasNumber=0;
    mfBill.PayType="";
    mfBill.Recived="";
    mfBill.Round=1;
    mfBill.Status = -1;
}
//---------------------------------------------------------------------------
bool __fastcall TMainWindow::CheckSumEAN(AnsiString Code)
{
//   int num[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
   if(!ScanCodeCheckDigit) return true;
   int even=0, noteven = 0;
   int len = Code.Length();
   int * num = new int[len];
   if(len == 10 || len == 26) {delete[] num; return true;}
   if(len != 13) {delete[] num; return false;}
   for (int i=1; i<=len;i++)
   {
      num[i-1] = Code.SubString(i, 1).ToIntDef(-1);
      if (num[i-1] == -1)
      {
         log("Ошибка преобразования ШК");
         delete[] num;
         return false;
      }
   }
   for(int i=len - 2; i >= 0; i--)
   {
       if((len - i -1)%2 == 0)
       {
         even += num[i];
       }
       else
       {
         noteven += num[i];
       }
   }
    int res = noteven*3+even;
    int cs = (10 - res%10)%10;
    if(cs == num[len -1])
    {
      delete[] num;
      return true;
    }
    else
    {
      delete[] num;
      log("Ошибка контрольной суммы");
      return false;
    }
}
//---------------------------------------------------------------------
int __fastcall TMainWindow::CheckSum(AnsiString Code)
{
   int even=0, noteven = 0;
   int len = Code.Length();
   if(len == 13)
   {
      Code.Delete(len, 1);
      len = Code.Length();
   }
   if(len != 12) return -1;
   int * num = new int[len];
   for (int i=1; i<=len;i++)
   {
      num[i-1] = Code.SubString(i, 1).ToIntDef(-1);
      if (num[i-1] == -1)
      {
         log("Ошибка преобразования ШК");
         delete[] num;
         return false;
      }
   }
   for(int i=len - 1; i >= 0; i--)
   {
       if((len - i)%2 == 0)
       {
         even += num[i];
       }
       else
       {
         noteven += num[i];
       }
   }
    int res = noteven*3+even;
    int cs = (10 - res%10)%10;
    delete[] num;
return cs;
}

void __fastcall TMainWindow::ComplexPayKeyClick(TObject *Sender)
{
   ComboPayClick();
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::ComboPayClick()
{
   char RRN[MAX_REFNUM];
   AnsiString str;
   char Scard[10];
   int RC;
   double qt,pr,cost,err,sum;
   long st;
   bool res = false;
   ComboPay sPay;
   memset(&sPay, 0, sizeof(sPay));
   vector<ComboPay>::iterator it;

// Выходим при несоблюдении условий
    if(SetNum||SetPrice||NowSearching||CheckBool(Printing)||Grid->Cells[0][1].IsEmpty()||MoneyAshyper(TSum->Text) == 0) return;
    if(!Session){
     Name->Caption = "Не открыта смена";
     PlaySound("oy.wav",0, SND_ASYNC);
     return;
    }
    SetBool(Printing);
    // очищаем старые (отмененные) платежи
    ComboPayOldClear();


//    PayType = COMPLEX_PAYMENT;
    //PayFlag = 1;
// Проверка наличия акции, если акция есть, то выходим из функции расчета для ввода подарков
    if(!StocksProcessing()) { DownBool(Printing); return; }
    if(CheckSale()) Sale = true;
    else Sale = false;
// проверка уценки
    if(Sale)
    {
        PlaySound("warning.wav",0,SND_ASYNC);
        if
        (
            MessageBox (GetActiveWindow(), "Внимание! Уценненный товар! Продолжить?",
                "Уценка", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {DownBool(Printing);PayType = NO_PAYMENT; return;}
    }
// проверка полномочий кассира
    if(!NoCard && CardType != KASSIR_CARD && CardType != MANAGER_CARD)
        {
        DownBool(Printing);
        PayType = NO_PAYMENT;
        return;
        }
      PlaySound("cash.wav",0,SND_ASYNC);
// проверка нулевой цены
    if(CheckSale0())
    {
      PlaySound("warning.wav",0,SND_ASYNC);
      MessageBox (GetActiveWindow(),"ВНИМАНИЕ!",
              "Товар с нулевой ценой или количеством, укажите или удалите строку", MB_OK);
      DownBool(Printing); PayType = NO_PAYMENT; return;
    }
//Подменим текст на табло
    TextToShow = TextToShow.SubString(1,40-TSum->Text.Length())+TSum->Text;
    ShowOnDisplay(TextToShow.c_str());

//   RecivedCalc();
//Подменим текст на табло
    TextToShow = TextToShow.SubString(1,40-TSum->Text.Length())+TSum->Text;
    ShowOnDisplay(TextToShow.c_str());

// вызываем окно с принятой суммой
// при этом контроллируем нулевую сумму в остатке
// такое может произойти при сбое
    if(MoneyAshyper(TSum->Text) - MoneyAshyper(Recived->Text) > 0)
    {
      if(CashForm->ShowModal() != mrOk)
      {
         DownBool(Printing);
         PayType = NO_PAYMENT;
         PresentLabel->Visible = false;
         ClearGrid(PresentGrid);
         return;
      }

      if(PayType == INTERNATIONAL_CARD_PAYMENT)
      {
//       PaySB(CashForm->Cash->Text, Scard);
         sPay.Sum = 0;
         if(GetPayStatus(ComboPayGrid, ComboPayGrid->RowCount-1) == CP_NEW_STATUS)
            sPay.Sum = MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][ComboPayGrid->RowCount-1]);
         strcpy(sPay.RRN, "\0");
         DWORD result;
         if(!ManualSB)
         {
            result = preauth(&sPay);
         }
         else
         {
            result = 0;
         }
         if(result)
         {
            PlaySound("oy.wav",0,SND_ASYNC);
            Name->Caption = "Ошибка платежа по карте " + AnsiString(result);
            log("Ошибка платежа по карте" + AnsiString(sPay.FullErrorText));
            // подчищаем последнюю строку
            DeleteLastRow(ComboPayGrid);
            DownBool(Printing);
            return;
         }
         //if(!ManualSB)
         AddToFile((ProgramPath + "Check.txt").c_str(),"c:\\data\\checks.txt");
         ComboPayGrid->Cells[CP_RRN_COL][ComboPayGrid->RowCount-1] = sPay.RRN;
         ComboPayGrid->Cells[CP_CARD_ID_COL][ComboPayGrid->RowCount-1] = sPay.CardID;
         ComboPayGrid->Cells[CP_HASH_COL][ComboPayGrid->RowCount-1] = sPay.Hash;
         ComboPayGrid->Cells[CP_CARD_NAME_COL][ComboPayGrid->RowCount-1] = sPay.CardName;
         ComboPayGrid->Cells[CP_CHECK_COL][ComboPayGrid->RowCount-1] = sPay.Check;
         ComboPayGrid->Cells[CP_CARD_TYPE_COL][ComboPayGrid->RowCount-1] = sPay.CType;
         ComboPayGrid->Cells[CP_SBOWN_COL][ComboPayGrid->RowCount-1] = sPay.SberOwnCard;
         AnsiString CardId = sPay.CardID;
//         CardId = CardId.Trim();
         AnsiString Name = AnsiString(sPay.CardName) + " " + CardId.SubString(CardId.Length() - 5, 6);
         ComboPayGrid->Cells[CP_NAME_COL][ComboPayGrid->RowCount-1] = Name;
         SetPayStatus(ComboPayGrid, ComboPayGrid->RowCount - 1, CP_COMPLETE_STATUS);
         SaveGridToFile(ComboPayGrid, ProgramPath + "paygrid.txt"); // сохраняем данные для восстановления после выхода
      }
    }
    else if(MoneyAshyper(TSum->Text) - MoneyAshyper(Recived->Text) == 0 && MoneyAshyper(TOff->Text) > 0)
    { // возникает при нажатии наличного расчета, когда остаток меньше рубля - остаток округляется
      AddComboPay(NAL_PAYMENT,MoneyAshyper(0),ComboPayGrid);
      int row = CheckNalPayment();
      ComboPayGrid->Cells[CP_ROUND_COL][row] = MoneyAshyper(TOff->Text);
      SetPayStatus(ComboPayGrid, row, CP_COMPLETE_STATUS);
    }
    Payments = true;
    PresentLabel->Visible = false;
    RecivedCalc();
    TextToShow = "Получено:";
    while( TextToShow.Length()+Recived->Text.Length() < 20) TextToShow += " ";
    TextToShow += Recived->Text;
    TextToShow += "Сдача:";
    while( TextToShow.Length() + Change->Text.Length() < 40) TextToShow += " ";
    TextToShow += Change->Text;
    ShowOnDisplay(TextToShow.c_str());
// если сумма комбинированных платежей меньше суммы чека, то продолжаем ожидать ввода очередного платежа
    GroupComboPayGrid();
    SaveGridToFile(ComboPayGrid, ProgramPath + "paygrid.txt"); // сохраняем данные для восстановления после выхода
//    hyper a = GridSum(ComboPayGrid, CP_SUM_COL);
//    int b = RoundSum();
    if(GridSum(ComboPayGrid, CP_SUM_COL) + RoundSum() < MoneyAshyper(TSum->Text))
    {
      DownBool(Printing);
      PayType = NO_PAYMENT;
      return;
    }
// Идем дальше после сбора всех платежей
    if(Star->Serial == "" || Star->SerialID == "") //Если серийный номер не определен, работать не можем!
    {
      MessageBox(NULL,"Нет номера ФР или ID","Печать чека невозможна",MB_OK);
      DownBool(Printing);
      PayType = NO_PAYMENT;
      return;
    }
/*    if(ManualSB)
    {
        if
        (
            MessageBox (GetActiveWindow(), "Авторизация произведена?",
                "Терминал СБ", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES
        ) {DownBool(Printing); return;}
        strcpy(Scard,"ПК");
        SelectPayType();
    }
    else
    {
      TOff->Text = MoneyAsString(0); // сбросим скидку, если она есть
      bool psb = PaySB(TSum->Text, Scard);
    // для отладки
//    psb = true;
//    mfBill.BankCheck = ftc("check.txt");  //
      if(!psb)
      {
          PlaySound("oy.wav",0,SND_ASYNC);
          Name->Caption = String(PayType);
          DownBool(Printing);
          return;
      }

    } */

//  FRInit();

//  std:vector<ComboPay> CompletePayInfo;
//  complete_all_auth(&CompletePayInfo);
  res = CreateBillBody(true);
if(!res)  // если что-то пошло не удачно
  { // необходимо сделать расширенную обработку ошибок
   log("Ошибка пробития чека " + IntToHex(frStatus.OperationResult, 2) + "h");
   GetSverka(true);
   Star->FRInit();
   DownBool(Printing);
   PayType = NO_PAYMENT;
   PlaySound("oy.wav",0,SND_ASYNC);
   return;
  }
 PlaySound("cash.wav",0,SND_ASYNC);

 WritePayment(PayType,TSum->Text);
 WritePayment("11",TOff->Text);
 WritePayment("12",MoneyAsString(GiftCardCalcPayments()));
 if(!WriteRetail(PayType))
 {
   Name->Caption = "Ошибка SQL! Немедленно прекратите работу.";
   return;
 }
 BulkSetPayStatus(ComboPayGrid, CP_APPROVE_STATUS);
 ClearForm();
 DownBool(Printing);
 PayType = NO_PAYMENT;
 BulkSetPayStatus(ComboPayGrid, CP_OLD_STATUS);
 Payments = false;
 SaveGridToFile(ComboPayGrid, ProgramPath + "paygrid.txt");
}
//-----------------------------------------------------------------------------
// добавление платежа
void __fastcall TMainWindow::AddComboPay(AnsiString PayType, AnsiString Summ, TStringGrid *Grid)
{
   int Row = Grid->RowCount;
   if(!Grid->Cells[0][Grid->RowCount - 1].IsEmpty())Grid->RowCount = Grid->RowCount + 1;  // Если строка не пронумерована (пустая), то добавляем строку
   if(Grid->RowCount == 2 && Grid->Cells[0][1].IsEmpty()) Row = 1; // обходим вторую пустую строку на начальном этапе
   Grid->Cells[0][Row] = AnsiString(Row);
   Grid->Cells[CP_NAME_COL][Row] = PayName[PayType];
   Grid->Cells[CP_SUM_COL][Row] = Summ;
   Grid->Cells[CP_DEL_COL][Row] = "-";
   Grid->Cells[CP_PAY_TYPE_COL][Row] = PayType;

}
//-----------------------------------------------------------------------------
// считаем сколько осталось доплатить
hyper __fastcall TMainWindow::RestPayment()
{

   return MoneyAshyper(TSum->Text) - GridSum(ComboPayGrid, CP_SUM_COL);
}
//----------------------------------------------------------------------------
// считаем сумму по столбцу грида
unsigned hyper __fastcall TMainWindow::GridSum(TStringGrid *Grid, int colNum)
{
   unsigned hyper sum = 0;
   for(int i = 1; i<=Grid->RowCount;i++)
   {
      if(GetPayStatus(ComboPayGrid, i) == CP_COMPLETE_STATUS || GetPayStatus(ComboPayGrid, i) == CP_APPROVE_STATUS)      // только завершенный статус
         sum += MainWindow->MoneyAshyper(Grid->Cells[colNum][i]);
   }
return sum;
}
//-----------------------------------------------------------------------------
//Устанавливаем статус строки платежа в таблице платежей
void __fastcall TMainWindow::SetPayStatus(TStringGrid *Grid, int Row, int Status)
{
   Grid->Cells[CP_STATUS_COL][Row] = AnsiString(Status);
   Grid->Repaint();
}
//-------------------------------------------------------------------------------
//читает статус строки
int __fastcall TMainWindow::GetPayStatus(TStringGrid *Grid, int Row)
{
   int status = StrToIntDef(Grid->Cells[CP_STATUS_COL][Row],0);
   return status;
}

//------------------------------------------------------------------------------
// массово ставим указанный статус в таблице платежей
void __fastcall TMainWindow::BulkSetPayStatus(TStringGrid *Grid, int Status)
{
   for(int i = 1; i < Grid->RowCount; i++)
   {
      SetPayStatus(Grid, i, Status);
   }
}
//------------------------------------------------------------------------------
// расскрашиваем строки таблицы
void __fastcall TMainWindow::ComboPayGridDrawCell(TObject *Sender,
      int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
   TCanvas *Canvas = this->ComboPayGrid->Canvas;
   Canvas->Font->Color = clWindowText;
   if(ACol > 0 && ARow > 0 && !ComboPayGrid->Cells[CP_STATUS_COL][ARow].IsEmpty())
   {
       switch (ComboPayGrid->Cells[CP_STATUS_COL][ARow].ToInt())
       {
         case 1: // вводим сумму
            Canvas->Brush->Color = clYellow;
            break;
         case 2: // платеж погашен
            Canvas->Brush->Color = clLime;
            break;
         case 3: // после закрытия платежа
            Canvas->Brush->Color = clTeal;
            break;
         case 4: // после закрытия платежа
            Canvas->Brush->Color = clSilver;
            break;
         default:
            Canvas->Brush->Color = clBackground;
            break;
       }
       Canvas->FillRect(Rect);
       TRect r = Rect;
       r.left +=2;
       r.top+=2;
       DrawText(Canvas->Handle,this->ComboPayGrid->Cells[ACol][ARow].c_str(),-1,(TRect*)&r,DT_LEFT);
   }

}
//---------------------------------------------------------------------------
// отменяем все платежи
void __fastcall TMainWindow::ComboPayCancelClick(TObject *Sender)
{
   if(CardType != MANAGER_CARD) return;
   ClearGrid(ComboPayGrid);
   RecivedCalc();
   Payments = false;
}
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// обновляем и раскрашиваем поля "Получено" и "Сдача"/"Осталось"
void __fastcall TMainWindow::RecivedCalc()
{

    hyper sum = MoneyAshyper(TotalSum);
    int RoundSum = 0;
    if(sum == 0)return;
    int nalcol = CheckNalPayment(); // есть ли наличный платеж?
    if(nalcol) ComboPayGrid->Cells[CP_ROUND_COL][nalcol] = "";
    hyper recived = GridSum(ComboPayGrid, CP_SUM_COL);
//      + StrToIntDef(ComboPayGrid->Cells[CP_ROUND_COL][nalcol], 0);
    Recived->Text = MoneyAsString(recived); //суммируем все, что есть в таблице расчетов

    if(recived > 0) Payments = true;   //ставим признак расчетов, ввод новых позиций блокируется
    // считаем остаток
    hyper change = sum - recived;
    // если остаток меньше округления и есть нал в платежах - пишем округление в него
    if(PayType == NAL_PAYMENT || (nalcol && int(Round) - abs(change) < Round))   // если есть наличный платеж и остаток меньше округления
    {
      if (change%int(Round) < 0)
      {
         RoundSum = 100 + change%int(Round);
         if(nalcol)ComboPayGrid->Cells[CP_ROUND_COL][nalcol] = RoundSum;
         change -= RoundSum;
      }
      else
      {
         RoundSum = change%int(Round);
         if(nalcol)ComboPayGrid->Cells[CP_ROUND_COL][nalcol] = RoundSum;
         change -= RoundSum;
      }
      TSum->Text = MoneyAsString(sum - RoundSum);
      TOff->Text = MoneyAsString(RoundSum);
    }
//    change -= StrToIntDef(ComboPayGrid->Cells[CP_ROUND_COL][nalcol], 0);
/*    if(PayType == NAL_PAYMENT)
    {
      TSum->Text = MoneyAsString(sum - sum%Round);
      ComboPayGrid->Cells[CP_ROUND_COL][nalcol] = abs(change%int(Round));
      change -= change%int(Round);
    }
    else */
    if(PayType != NAL_PAYMENT)
    {
      TSum->Text = TotalSum;
    }
    if( change < 0) //в зависимости от знака, меняем цвет и описание поля
    {
      Label9->Caption = "Сдача:";
      Label9->Font->Color = clWindowText;
      Change->Text = MoneyAsString(abs(change));
      Change->Font->Color = clWindowText;
    }
    else
    {
      Label9->Caption = "Доплата:";
      Label9->Font->Color = clRed;
      Change->Text = MoneyAsString(change);   // сдача
      Change->Font->Color = clRed;
    }
//    Label9->Repaint();
    MainWindow->Repaint();
//    Recived->Repaint();
//    Change->Repaint();
//    TSum->Repaint();

}
//----------------------------------------------------------------------------

DWORD __fastcall TMainWindow::preauth(ComboPay *sPay)
{
  // для теста
/*  MessageBox(GetActiveWindow(),"Оплата с БК","Оплата произведена!", MB_OK);
  strcpy(sPay->CardID, "421644******5549");
  strcpy(sPay->Hash, "010000009C06030044FA18008F645100E0EB55001E7B51000E64510300000000000000");
  strcpy(sPay->RRN, "123456789012");
  strcpy(sPay->CardName, "VISA");
  strcpy(sPay->FullErrorText, "Никакой ошибки не было");
  sPay->ErrorCode = 0;
  sPay->CType = 10;
  return 0;
  */
//   log("Авторизация карты на сумму " + AnsiString(sPay->Sum));
  auth_answer13 argument;

  memset(&argument,0,sizeof(argument));
  memset(&argument.ans,0,sizeof(argument.ans));
  argument.ans.TType = OP_PURCHASE;
  argument.ans.Amount = sPay->Sum;
  DWORD result = card_authorize13(NULL,&argument);
  log("Результат авторизации " + AnsiString(result));
  sPay->CType = argument.ans.CType;
  if(argument.ans.Check != NULL)
  {
      log("Чек не пуст! RRN: " + AnsiString(argument.RRN));
      CheckFile(argument.ans.Check, (ProgramPath + "Check.txt").c_str());
      strcpy(sPay->Check, argument.ans.Check);
      ::GlobalFree((HGLOBAL)argument.ans.Check);
  }
  strcpy(sPay->CardID, argument.CardID);
  strcpy(sPay->Hash, argument.Hash);
  strcpy(sPay->RRN, argument.RRN);
  strcpy(sPay->CardName, argument.CardName);
  strcpy(sPay->FullErrorText, argument.FullErrorText);
  sPay->ErrorCode = argument.ErrorCode;
  sPay->SberOwnCard = argument.SberOwnCard;

  return result;
}

DWORD TMainWindow::add_auth(Kopeyki amount, char* RRN)
{
  auth_answer13 argument;

  memset(&argument,0,sizeof(argument));
  argument.ans.TType = OP_ADD_AUTH;
  argument.ans.Amount = amount;
  strcpy(argument.RRN, RRN);
  std::cout << "Second authorization. Amount = " << amount << ". RRN:" << RRN <<std::endl;
  DWORD result = card_authorize13(NULL,&argument);
  if(strcmp(RRN, argument.RRN) != 0)
    std::cerr << "Wrong RRN!" << std::endl;

  std::cout << "Operation completed with code '" << result << "'!" << std::endl;
  return result;
}

DWORD TMainWindow::complete_auth( Kopeyki amount, const char* RRN, ComboPay* sPay)
{
  auth_answer13 argument;

  memset(&argument,0,sizeof(argument));
  argument.ans.TType = OP_COMPLETION;
  argument.ans.Amount = amount;
  strcpy(argument.RRN, RRN);
  DWORD result = card_authorize13(NULL,&argument);

  sPay->CType = argument.ans.CType;
  if(argument.ans.Check != NULL) strcpy(sPay->Check, argument.ans.Check);
  strcpy(sPay->CardID, argument.CardID);
  strcpy(sPay->Hash, argument.Hash);
  strcpy(sPay->RRN, argument.RRN);
//  strcpy(sPay->CardName, argument.CardName);
//  strcpy(sPay->FullErrorText, argument.FullErrorText);
  sPay->ErrorCode = argument.ErrorCode;
  GlobalFree((HGLOBAL)argument.ans.Check);

  return result;
}

DWORD TMainWindow::cancel_auth(const char* RRN)
{
// для теста
//  MessageBox(GetActiveWindow(),"Возврат с БК","Возврат произведен!", MB_OK);
//  return 0;

  auth_answer13 argument;

  memset(&argument,0,sizeof(argument));
  argument.ans.TType = OP_COMPLETION;
  argument.ans.Amount = 0;
  strcpy(argument.RRN, RRN);
  std::cout << "Lets cancel authorization" << " RRN:" << RRN << std::endl;
  DWORD result = card_authorize13(NULL,&argument);

  std::cout << "Operation completed with code '" << result << "'!" << std::endl;
  return result;
}
// отмена операции по карте или возврат
DWORD TMainWindow::return_auth(ComboPay *sPay)
{
  auth_answer13 argument;

  memset(&argument,0,sizeof(argument));
  argument.ans.TType = OP_RETURN;
  argument.ans.Amount = sPay->Sum;
  strcpy(argument.RRN, sPay->RRN);
  DWORD result = card_authorize13(NULL,&argument);
  sPay->CType = argument.ans.CType;
  if(argument.ans.Check != NULL) strcpy(sPay->Check, argument.ans.Check);
  strcpy(sPay->CardID, argument.CardID);
  strcpy(sPay->Hash, argument.Hash);
  strcpy(sPay->CardName, argument.CardName);
  strcpy(sPay->FullErrorText, argument.FullErrorText);
  sPay->ErrorCode = argument.ErrorCode;
  sPay->SberOwnCard = argument.SberOwnCard;
  GlobalFree((HGLOBAL)argument.ans.Check);

return result;
}
//-----------------------------------------------------------------------------
bool __fastcall TMainWindow::complete_all_auth(vector<ComboPay> *ComletePayInfo)
{
// для теста
   MessageBox(GetActiveWindow(),"Платежи подтверждены","Платежи подтверждены!", MB_OK);
   return true;

   bool res = true;
   ComboPay Pay;
   memset(&Pay,0,sizeof(Pay));

   for(int i=1; i < ComboPayGrid->RowCount; ++i)
   {
      if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == INTERNATIONAL_CARD_PAYMENT)
      {
//         Pay.RRN = ComboPayGrid->Cells[CP_RRN_COL][i].c_str()
         DWORD result = complete_auth(
            MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][i]),
            ComboPayGrid->Cells[CP_RRN_COL][i].c_str(), &Pay);

//int result = 0;
         if(!result)
         {
            ComletePayInfo->push_back(Pay);
            SetPayStatus(ComboPayGrid, i, CP_COMPLETE_STATUS);
            log("Ошибка: " + AnsiString(Pay.FullErrorText));
//            Name->Text = AnsiString(Pay.FullErrorText);
            res = false;
         }
      }
   }
res = true;
return res;
}

//-----------------------------------------------------------------------------
// обрабатываем двойное нажатие на платеж
void __fastcall TMainWindow::ComboPayGridDblClick(TObject *Sender)
{
// закоментировано для теста
   if(GetPayStatus(ComboPayGrid, ComboPayGrid->Row) > CP_APPROVE_STATUS) return;
   if(GetPayStatus(ComboPayGrid, ComboPayGrid->Row) == CP_NEW_STATUS)
   {
      MessageBox(GetActiveWindow(), "Сбой платежа! Повторите расчет покупателя", "Платеж не произведен!", MB_OK);
      return;
   }
   if(CardType != MANAGER_CARD && GridSum(ComboPayGrid, CP_SUM_COL) == 0)
   {
     return;
   }
   ComboPay sPay;

   sPay.Sum = MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][ComboPayGrid->Row]);
   AnsiString a = ComboPayGrid->Cells[CP_PAY_TYPE_COL][ComboPayGrid->Row];
// при отмене наличного платежа
   if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][ComboPayGrid->Row] == NAL_PAYMENT)
   {
      if(MessageBox(GetActiveWindow(), ("Покупателю нужно отдать " + MoneyAsString(sPay.Sum) + " руб. наличными.").c_str(), "Возврат наличных",
         MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDOK)
      {
         SetPayStatus(ComboPayGrid, ComboPayGrid->Row, CP_OLD_STATUS);
         ComboPayGrid->Repaint();
      }
   }

// при отмене платежа по БК
   else if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][ComboPayGrid->Row] == INTERNATIONAL_CARD_PAYMENT)
   {
      strcpy(sPay.RRN, ComboPayGrid->Cells[CP_RRN_COL][ComboPayGrid->Row].c_str());
      if(sPay.RRN == "") return;
      if(MessageBox(GetActiveWindow(),
         ("Удаляем платеж карты суммой " + MoneyAsString(sPay.Sum)).c_str(),
            "Удаление платежа",  MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) != IDOK)
      {
         MessageBox(GetActiveWindow(),"Отмены платежа не произошло", "Нет отмены платежа", MB_OK);
         log("Отмены платежа не произошло\n RRN: " + AnsiString(sPay.RRN));
         return;
      }

      DWORD result = return_auth(&sPay);
      log("Отмена платежа по карте. Сумма: " + MoneyAsString(sPay.Sum) + "Результат: " + AnsiString(result));
      if(!result)
      {
         // ставим статус и печатаем чек
         SetPayStatus(ComboPayGrid, ComboPayGrid->Row, CP_OLD_STATUS);
         Star->Print(sPay.Check, true);
         Star->CRLF();
      }
   }
   else if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][ComboPayGrid->Row] == GIFT_CARD_PAYMENT)
   {
      if(MessageBox(GetActiveWindow(), "Покупателю нужно вернуть подарочную карту.", "Возврат подарочной карты",
         MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2) == IDOK)
      {
          SetPayStatus(ComboPayGrid, ComboPayGrid->Row, CP_OLD_STATUS);
          ComboPayGrid->Repaint();
      }

   }
            
   // если сумма равна нулю, то сбрасываем признак платежа
      if(GridSum(ComboPayGrid, CP_SUM_COL) == 0) Payments = false;
      RecivedCalc();
      SaveGridToFile(ComboPayGrid, ProgramPath + "paygrid.txt");
}
//---------------------------------------------------------------------------

void __fastcall TMainWindow::ComboPayGridKeyPress(TObject *Sender,
      char &Key)
{
//   if(Key == '\b') ShowMessage("Delete\n" "строка: "+IntToStr(ComboPayGrid->Row)+
//    "\nстолбец: "+IntToStr(ComboPayGrid->Col));

}
//---------------------------------------------------------------------------
// удаляем последнюю строку грида
void __fastcall TMainWindow::DeleteLastRow(TStringGrid *Grid)
{
   Grid->Rows[Grid->RowCount - 1]->Clear();
   if(Grid->RowCount > 2) --Grid->RowCount;
}
//---------------------------------------------------------------------------
//удаляем произвольную строку в таблице
void __fastcall TMainWindow::RemoveRow(TStringGrid *Grid, int Row)
{
   //отключаем на время  перерисовку компоненты
   SendMessage(Grid->Handle, WM_SETREDRAW, false,0);
   try
   {
      const int row_count=Grid->RowCount;
      for(int row = Row; row < row_count-1; ++row)
      {
         Grid->Rows[row] = Grid->Rows[row+1];
      }
      if(Grid->RowCount > 2)
      {
         Grid->Rows[row_count-1]->Clear();
         Grid->RowCount = row_count - 1;
      }
      else Grid->Rows[1]->Clear();
    }
    catch(...)
    {
      SendMessage(Grid->Handle, WM_SETREDRAW, true,0);
    }
    SendMessage(Grid->Handle, WM_SETREDRAW, true,0);
    // изменяется область, которая находится ниже удаляемой строки поэтому эту область нужно обновить
    RECT R = Grid->CellRect(0, Row); //  координаты первой ячейки в удаляемой строке
    InflateRect(&R, Grid->Width, Grid->Height);  // расширяем прямоугольник до крайнего нижнего угла
    InvalidateRect(Grid->Handle, &R, false);  // обновляем измененную область
}
//---------------------------------------------------------------------------
void __fastcall TMainWindow::RenumRow(TStringGrid *Grid)
{
   if(Grid->Cells[0][1].IsEmpty()) return;
   for(int i=1; i<= Grid->RowCount; ++i)
   {
      Grid->Cells[0][i] = i;
   }
}

//---------------------------------------------------------------------------------
void __fastcall TMainWindow::ComboPayOldClear()
{
   if(ComboPayGrid->Cells[0][1].IsEmpty()) return;
   for (int i = 1; i <= ComboPayGrid->RowCount; ++i)
   {
      if(GetPayStatus(ComboPayGrid, i) == CP_OLD_STATUS) RemoveRow(ComboPayGrid, i);
   }
   RenumRow(ComboPayGrid);
}
//----------------------------------------------------------------------------
// группировка по наличным платежам
void __fastcall TMainWindow::GroupComboPayGrid()
{
 AnsiString group, status;
 AnsiString group_next, status_next;
 std::vector<int> delRow;

   if(GridSum(ComboPayGrid, CP_SUM_COL) == 0) return;
   for(int i = 1; i < ComboPayGrid->RowCount - 1; i++)
   {
      group = ComboPayGrid->Cells[CP_PAY_TYPE_COL][i];
      status = ComboPayGrid->Cells[CP_STATUS_COL][i];
      if(group != NAL_PAYMENT || status != CP_COMPLETE_STATUS) continue;
      for(int k = i + 1; k < ComboPayGrid->RowCount; k++)
      {
         group_next = ComboPayGrid->Cells[CP_PAY_TYPE_COL][k];
         status_next = ComboPayGrid->Cells[CP_STATUS_COL][k];
         if(group_next != NAL_PAYMENT || status_next != CP_COMPLETE_STATUS) continue;
         hyper Sum = MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][i]) + MoneyAshyper(ComboPayGrid->Cells[CP_SUM_COL][k]);
         ComboPayGrid->Cells[CP_SUM_COL][k] = MoneyAsString(Sum);
         int Off = StrToIntDef(ComboPayGrid->Cells[CP_ROUND_COL][i],0) + StrToIntDef(ComboPayGrid->Cells[CP_ROUND_COL][k],0);
         ComboPayGrid->Cells[CP_ROUND_COL][k] = Off;
         delRow.push_back(i);
         break;
      }
   }
   std::vector<int>::iterator it;
   for(it = delRow.begin(); it < delRow.end(); ++it) RemoveRow(ComboPayGrid, *it);
   RenumRow(ComboPayGrid);
}
//----------------------------------------------------------------------------

int _fastcall TMainWindow::CheckNalPayment()
{
   for(int i = 0; i < ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == NAL_PAYMENT) return i;
   }
return 0;
}
//----------------------------------------------------------------------------
int __fastcall TMainWindow::RoundSum()
{
   int sum = 0;
   for(int i = 1; i < ComboPayGrid->RowCount; i++)
   {
      if(ComboPayGrid->Cells[CP_PAY_TYPE_COL][i] == NAL_PAYMENT)
      {
         sum += StrToIntDef(ComboPayGrid->Cells[CP_ROUND_COL][i],0);
      }
   }
return sum;
}

//----------------------------------------------------------------------------
void __fastcall TMainWindow::GiftCardSumRenew(AnsiString Code,AnsiString Balance, AnsiString Residual, AnsiString Sum)
{
// На данный момент карта найдена в таблице и информация по ней в рекордсете

unsigned hyper Summ,Bal,CardPay,Rem;
AnsiString CP,Remain;

// CalcTotal();
Summ = MoneyAshyper(Sum);
Bal = MoneyAshyper(Balance) + MoneyAshyper(Residual);
if(Bal > Summ) CardPay = Summ; else CardPay = Bal;   // условие
if(CardPay <= 0) return;
Rem = Bal-CardPay;
CP = MoneyAsString(CardPay);
Remain = MoneyAsString(Rem);
//while(CP.Length()<10) CP = " "+CP;
//while(Remain.Length()<10) Remain = " "+Remain;
// GiftCardM->Lines->Add(Code+" на сумму "+CP+"  остаток "+Remain);
for(int i = 1; i < ComboPayGrid->RowCount; i++)
{
   if(ComboPayGrid->Cells[CP_CARD_ID_COL][i++] == Code)
   {
      ComboPayGrid->Cells[CP_SUM_COL][i] = CP;
      ComboPayGrid->Cells[CP_RESIDUE_COL][i] = Remain;
      break;
   }
}
return;
}
//------------------------------------------------------------------------
AnsiString __fastcall TMainWindow::ShieldingString(AnsiString str)
{
   return StringReplace(str,"'","''",TReplaceFlags()<< rfReplaceAll << rfIgnoreCase);
}
/*
void __fastcall TMainWindow::ReturnBCClick(TObject *Sender)
{
     ComboPay sPay;

   if(BankCardReturnForm->ShowModal() != mrOk)
   {
      return;
   }
   sPay.Sum = MoneyAshyper(BankCardReturnForm->Sum->Text);
   strcpy(sPay.RRN, BankCardReturnForm->RRN->Text.c_str());
   return_auth(&sPay);
   Star->Print(sPay.Check, true);
   return;
} */
//---------------------------------------------------------------------------

void __fastcall TMainWindow::frReport()
{
   FormSverka->Update();
   AnsiString str = "";
   hyper sum = 0;
   for(std::vector<stSverka>::iterator it = FormSverka->vSverka.begin(); it != FormSverka->vSverka.end(); ++it)
   {
      str = FormSverka->PayTypeName[it->Type];
      str = str.SubString(1,20);
      str += ": ";
      str += str.StringOfChar(' ', 21 - str.Length());
      str += FormSverka->MoneyAsString(it->frSum);
      Star->Print(str.c_str(), true);
      sum += it->frSum;
   }
   str = "ИТОГ:";
   str += str.StringOfChar(' ', 21 - str.Length());
   str += MoneyAsString(sum);
   Star->Print(str.c_str(), true);
   Star->Print("----------------------------------------", true);
}
void __fastcall TMainWindow::ReturnBCClick(TObject *Sender)
{
  /*   ComboPay sPay;

   if(BankCardReturnForm->ShowModal() != mrOk)
   {
      return;
   }
   sPay.Sum = MoneyAshyper(BankCardReturnForm->Sum->Text);
   strcpy(sPay.RRN, BankCardReturnForm->RRN->Text.c_str());
   return_auth(&sPay);
   Star->Print(sPay.Check, true);
   return;  */
}
//---------------------------------------------------------------------------

bool __fastcall TMainWindow::SeekBill(AnsiString Code)
{
   if(Code.Length() != 12) return false;
   PriceQuery->SQL->Clear();
   PriceQuery->SQL->Add("DECLARE @ean nvarchar(12)");
   PriceQuery->SQL->Add("DECLARE @t nvarchar(20)");
   PriceQuery->SQL->Add("DECLARE @billnumber nvarchar(18)");
   PriceQuery->SQL->Add("SET @ean = '" + Code + "'");
   PriceQuery->SQL->Add("SET @t = CONVERT(nvarchar(20), DATEADD(second, CAST(RIGHT(@ean, 9) as int), '20050101'), 20)");
   PriceQuery->SQL->Add("SET @t = substring(@t,3,2) + substring(@t,6,2) + substring(@t,9,2) + substring(@t,12,2) + substring(@t,15,2) + substring(@t,18,2)");
   PriceQuery->SQL->Add("SET @billnumber = (SELECT RTRIM(SCash) FROM SCash WHERE Id = SUBSTRING(@ean,1,3))");
   PriceQuery->SQL->Add("SET @billnumber = REPLICATE('0',6-len(@billnumber))+@billnumber+@t");
//   PriceQuery->SQL->Add("SELECT pr.Scancode, rt.Billnumber, pr.Name, pr.Meas, rt.Price, rt.Quantity, pr.NDS, sys.fn_varbintohexstr(rt.IDNom) as IDNom FROM retail rt");
   PriceQuery->SQL->Add("SELECT pr.Scancode, rt.Billnumber, pr.Name, pr.Meas, rt.Price, rt.Quantity - ISNULL(di.Quantity,0) as Quantity, pr.NDS, sys.fn_varbintohexstr(rt.IDNom) as IDNom, di.* FROM retail rt");
   PriceQuery->SQL->Add("CROSS APPLY (SELECT TOP 1 * FROM price pr WHERE rt.IDnom = pr.IDnom) pr");
   PriceQuery->SQL->Add("LEFT JOIN DeliveryItems di ON di.IDNom = rt.IDnom AND di.DocID IN (select DocID from Delivery where BillNumber = @billnumber)");
   PriceQuery->SQL->Add("WHERE BillNumber=@billnumber AND flag > 100 AND rt.Sklad=0x" + Department);
   PriceQuery->SQL->Add("AND rt.Quantity - ISNULL(di.Quantity,0) > 0");
   try
   {
      PriceQuery->Active = true;
   }
   catch (EOleException &eException)
   {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return false;
   }
   if(PriceQuery->RecordCount == 0) {PriceQuery->Active = false; return false;}
   ClearGrid(PickupGrid);
   ClearGrid(DeliveryGrid);
   PickupGrid->RowCount = PriceQuery->RecordCount + 1;
   AnsiString bn = PriceQuery->FieldByName("Billnumber")->AsString;
   SeekBillNumber = bn;
   for(int i = 1;i <= PriceQuery->RecordCount; i++)
   {
      PickupGrid->Cells[PIG_ID_COL][i] = i;
      PickupGrid->Cells[PIG_NAME_COL][i] = PriceQuery->FieldByName("Name")->AsString;
      PickupGrid->Cells[PIG_MEASURE_COL][i] = PriceQuery->FieldByName("Meas")->AsString;
      PickupGrid->Cells[PIG_QUANTITY_COL][i] = QuantityAsString(PriceQuery->FieldByName("Quantity")->AsFloat * 1000 + 0.5);
      PickupGrid->Cells[PIG_PRICE_COL][i] = MoneyAsString(PriceQuery->FieldByName("Price")->AsFloat * 100 + 0.5);
      PickupGrid->Cells[PIG_IDNOM_COL][i] = PriceQuery->FieldByName("IDNom")->AsString;
      PickupGrid->Cells[PIG_CODE_COL][i] = PriceQuery->FieldByName("ScanCode")->AsString;

      PriceQuery->Next();
   }
   Name->Caption = "Чек " +bn.SubString(1,2)+"-"+bn.SubString(3,4)+"-"+bn.SubString(7,4)+"-"+bn.SubString(11,4)+"-"+bn.SubString(15,4);
   PriceQuery->Active = false;
   return true;
}

//--------------------------------------------------------------------------
/*vector<Delivery> __fastcall TMainWindow::SeekDeliveryDoc(AnsiString Scancode)
{
   vector<Delivery> ret;

} */

//----------------------------------------------------------------------------
/* AnsiString __fastcall TMainWindow::EANtoBillNumber(AnsiString Code)
{
   Code = "0924508746249";
   TDateTime StartDate = DecodeDate(2005,1,1);
   TDateTime
}  */
//----------------------------------------------------------------------------
void __fastcall TMainWindow::ShowDeliveryPanel(bool enable)
{
   if(enable)
   {
      panelDelivery->Visible = true;
      BillPickup = true;
   }
   else
   {
      panelDelivery->Visible = false;
      BillPickup = false;
   }
}
//----------------------------------------------------------------------------

void __fastcall TMainWindow::InitDeliveryPanel()
{
   int Margin = 5;
   int panelTop = Name->Top + Name->Height + Margin;
   int panelBottom = StatusBar->Top - panelTop;
   panelDelivery->Top = panelTop;
   panelDelivery->Height = panelBottom - Margin;
   panelDelivery->Left = Margin;
   panelDelivery->Width = ClientWidth - 2*Margin;

   lbPickup->Top = Margin;
   lbDelivery->Top = lbPickup->Top;
   lbPickup->Left = 2*Margin;

   PickupGrid->Top = lbPickup->Top + lbPickup->Height + Margin;
   PickupGrid->Left = Margin;
   PickupGrid->Width = (panelDelivery->Width - 3*Margin)/2;

   DeliveryGrid->Top = PickupGrid->Top;
   DeliveryGrid->Left = (panelDelivery->Width + Margin)/2;
   DeliveryGrid->Width = PickupGrid->Width;

   lbDelivery->Left = DeliveryGrid->Left + Margin;

   Bevel11->Top = panelBottom - PickupSum->Height - 8*Margin;
   PickupSum->Top = Bevel11->Top + Margin;
   lbItog1->Top = PickupSum->Top;
   Bevel12->Top = PickupSum->Top + PickupSum->Height + Margin;

   Bevel11->Left = 12*Margin;
   Bevel12->Left = Bevel11->Left;
   Bevel11->Width = PickupGrid->Width - Bevel11->Left;
   Bevel12->Width = Bevel11->Width;

   PickupSum->Left = PickupGrid->Width + Margin - PickupSum->Width;
   lbItog1->Left = PickupSum->Left - lbItog1->Width - Margin;

   PickupGrid->Height = Bevel11->Top - 2*Margin - PickupGrid->Top;

   Bevel21->Top = panelBottom - DeliverySum->Height - 8*Margin;
   DeliverySum->Top = Bevel21->Top + Margin;
   lbItog2->Top = DeliverySum->Top;
   Bevel22->Top = DeliverySum->Top + DeliverySum->Height + Margin;

   Bevel21->Left = panelDelivery->Width/2 + 12*Margin;
   Bevel22->Left = Bevel21->Left;
   Bevel21->Width = DeliveryGrid->Width - 8*Margin;
   Bevel22->Width = Bevel21->Width;

   DeliverySum->Left = DeliveryGrid->Left + DeliveryGrid->Width - DeliverySum->Width;
   lbItog2->Left = DeliverySum->Left - lbItog2->Width - Margin;

   DeliveryGrid->Height = Bevel21->Top - 2*Margin - DeliveryGrid->Top;
   PickupGrid->ColCount = 8;
//   PickupGrid->RowCount = 2;
   PickupGrid->Cells[PIG_ID_COL][0]="№";
   PickupGrid->Cells[PIG_NAME_COL][0] = "Название";
   PickupGrid->Cells[PIG_MEASURE_COL][0] = "Ед.измер.";
   PickupGrid->Cells[PIG_QUANTITY_COL][0] = "Кол-во";
   PickupGrid->Cells[PIG_PRICE_COL][0] = "Цена";
   PickupGrid->Cells[PIG_IDNOM_COL][0] = "IdNom";
   PickupGrid->Cells[PIG_CODE_COL][0] = "Код";
   PickupGrid->Cells[PIG_STATUS_COL][0] = "Статус";
   PickupGrid->ColWidths[PIG_ID_COL] = 30;
   PickupGrid->ColWidths[PIG_MEASURE_COL] = 60;
   PickupGrid->ColWidths[PIG_QUANTITY_COL] = 80;
   PickupGrid->ColWidths[PIG_PRICE_COL] = 80;
   PickupGrid->ColWidths[PIG_NAME_COL] = PickupGrid->Width
                                    - PickupGrid->ColWidths[PIG_ID_COL]
                                    - PickupGrid->ColWidths[PIG_MEASURE_COL]
                                    - PickupGrid->ColWidths[PIG_QUANTITY_COL]
                                    - PickupGrid->ColWidths[PIG_PRICE_COL];

   DeliveryGrid->ColCount = 7;
//   PickupGrid->RowCount = 2;
   DeliveryGrid->Cells[DG_ID_COL][0]="№";
   DeliveryGrid->Cells[DG_CODE_COL][0]="Код";
   DeliveryGrid->Cells[DG_NAME_COL][0]="Название";
   DeliveryGrid->Cells[DG_MEASURE_COL][0]="Ед.измер.";
   DeliveryGrid->Cells[DG_QUANTITY_COL][0]="Кол-во";
   DeliveryGrid->Cells[DG_PRICE_COL][0]="Цена";
   DeliveryGrid->Cells[DG_IDNOM_COL][0] = "IdNom";
   DeliveryGrid->ColWidths[DG_ID_COL] = 30;
   DeliveryGrid->ColWidths[DG_CODE_COL] = 140;
   DeliveryGrid->ColWidths[DG_MEASURE_COL] = 80;
   DeliveryGrid->ColWidths[DG_QUANTITY_COL] = 80;
   DeliveryGrid->ColWidths[DG_PRICE_COL] = 90;
   DeliveryGrid->ColWidths[DG_NAME_COL] = DeliveryGrid->Width
                                    - DeliveryGrid->ColWidths[DG_ID_COL]
                                    - DeliveryGrid->ColWidths[DG_CODE_COL]
                                    - DeliveryGrid->ColWidths[DG_MEASURE_COL]
                                    - DeliveryGrid->ColWidths[DG_QUANTITY_COL]
                                    - DeliveryGrid->ColWidths[DG_PRICE_COL];


}
//----------------------------------------------------------------------
// перенос позиций между таблицами
bool __fastcall TMainWindow::MoveItemBetweenTable(AnsiString IdNom, TStringGrid *FromGrid, TStringGrid *ToGrid)
{
   int rowFrom = SearchInGrid(IdNom, FromGrid, PIG_IDNOM_COL);
   int rowTo = SearchInGrid(IdNom, ToGrid, DG_IDNOM_COL);

   if(rowFrom == 0) return false;
 // контроль введенного количества
   hyper fq = QuantityAshyper(FromGrid->Cells[PIG_QUANTITY_COL][rowFrom]); // количество из исходной таблицы
   hyper sq = QuantityAshyper(Qnty->Text); // введенное количество
   if(sq > fq) Qnty->Text = QuantityAsString(fq);
   if(rowTo > 0)
   {
      hyper tq = QuantityAshyper(ToGrid->Cells[DG_QUANTITY_COL][rowTo]); // количество в целевой таблицы
      tq += QuantityAshyper(Qnty->Text);
      ToGrid->Cells[DG_QUANTITY_COL][rowTo] = QuantityAsString(tq);
   }
   else
   {
      int Row = ToGrid->RowCount;
      if(!ToGrid->Cells[0][ToGrid->RowCount - 1].IsEmpty())ToGrid->RowCount++;  // Если строка пронумерована (заполненная), то добавляем строку
      if(ToGrid->RowCount == 2 && ToGrid->Cells[0][1].IsEmpty()) Row = 1; // обходим вторую пустую строку на начальном этапе
      ToGrid->Cells[DG_ID_COL][Row] = Row;
      ToGrid->Cells[DG_CODE_COL][Row] = LastScancode;
      ToGrid->Cells[DG_NAME_COL][Row] = FromGrid->Cells[PIG_NAME_COL][rowFrom];
      ToGrid->Cells[DG_MEASURE_COL][Row] = FromGrid->Cells[PIG_MEASURE_COL][rowFrom];
      ToGrid->Cells[DG_QUANTITY_COL][Row] = QuantityAsString(QuantityAshyper(Qnty->Text));
      ToGrid->Cells[DG_PRICE_COL][Row] = FromGrid->Cells[PIG_PRICE_COL][rowFrom];
      ToGrid->Cells[DG_IDNOM_COL][Row] = FromGrid->Cells[PIG_IDNOM_COL][rowFrom];
   }
   FromGrid->Cells[PIG_QUANTITY_COL][rowFrom] = QuantityAsString(QuantityAshyper(FromGrid->Cells[PIG_QUANTITY_COL][rowFrom])
            - QuantityAshyper(Qnty->Text));

   if(QuantityAshyper(FromGrid->Cells[PIG_QUANTITY_COL][rowFrom]) == 0)
   {
      RemoveRow(FromGrid, rowFrom);
      RenumRow(FromGrid);
   }

}
//--------------------------------------------------------------------------
int __fastcall TMainWindow::SearchInGrid(AnsiString string, TStringGrid *Grid, int colNum)
{
   for(int i = 1; i < Grid->RowCount; i++)
   {
      if(Grid->Cells[colNum][i] == string)
      {
         return i;
      }
   }
   return 0;
}
//-------------------------------------------------------------------------
void __fastcall TMainWindow::InvertGridsClick(TObject *Sender)
{
   std::vector<Item> ItemList;
   for(int i = 1; i < PickupGrid->RowCount; i++)
   {
       if(PickupGrid->Cells[PIG_ID_COL][1].IsEmpty()) break;
       ItemList.push_back(Item(
         PickupGrid->Cells[PIG_CODE_COL][i],
         PickupGrid->Cells[PIG_NAME_COL][i],
         PickupGrid->Cells[PIG_MEASURE_COL][i],
         QuantityAshyper(PickupGrid->Cells[PIG_QUANTITY_COL][i]),
         MoneyAshyper(PickupGrid->Cells[PIG_PRICE_COL][i]),
         PickupGrid->Cells[PIG_IDNOM_COL][i]
       ));
   }
   ClearGrid(PickupGrid);
   for(int i = 1; i < DeliveryGrid->RowCount; i++)
   {
      if(DeliveryGrid->Cells[DG_ID_COL][1].IsEmpty()) break;
      if(i > 1) PickupGrid->RowCount++;
      PickupGrid->Cells[PIG_ID_COL][i] = i;
      PickupGrid->Cells[PIG_NAME_COL][i] = DeliveryGrid->Cells[DG_NAME_COL][i];
      PickupGrid->Cells[PIG_MEASURE_COL][i] = DeliveryGrid->Cells[DG_MEASURE_COL][i];
      PickupGrid->Cells[PIG_QUANTITY_COL][i] = DeliveryGrid->Cells[DG_QUANTITY_COL][i];
      PickupGrid->Cells[PIG_PRICE_COL][i] = DeliveryGrid->Cells[DG_PRICE_COL][i];
      PickupGrid->Cells[PIG_IDNOM_COL][i] = DeliveryGrid->Cells[DG_IDNOM_COL][i];
      PickupGrid->Cells[PIG_CODE_COL][i] = DeliveryGrid->Cells[DG_CODE_COL][i];
   }
   ClearGrid(DeliveryGrid);
   int k = 1;
   for(std::vector<Item>::iterator it = ItemList.begin(); it < ItemList.end(); ++it)
   {
      if(k > 1) DeliveryGrid->RowCount++;
      DeliveryGrid->Cells[DG_ID_COL][k] = k;
      DeliveryGrid->Cells[DG_CODE_COL][k] = it->ScanCode;
      DeliveryGrid->Cells[DG_NAME_COL][k] = it->Name;
      DeliveryGrid->Cells[DG_MEASURE_COL][k] = it->Measure;
      DeliveryGrid->Cells[DG_QUANTITY_COL][k] = QuantityAsString(it->Quantity);
      DeliveryGrid->Cells[DG_PRICE_COL][k] = MoneyAsString(it->Price);
      DeliveryGrid->Cells[DG_IDNOM_COL][k] = it->IDNom;
      k++;
   }

}
//---------------------------------------------------------------------------

// формирование документа по доставке

void __fastcall TMainWindow::DeliveryDocClick(TObject *Sender)
{
   if(DeliveryGrid->Cells[DG_ID_COL][0].IsEmpty()) return;
   AnsiString ScanCode = PushDeliveryDoc(DELIVERY_DOC_TYPE_DELIVERY);


}
//---------------------------------------------------------------------------

AnsiString __fastcall TMainWindow::PushDeliveryDoc(int Type)
{
   PriceQuery->SQL->Clear();
   PriceQuery->SQL->Add("BEGIN TRY");
   PriceQuery->SQL->Add("BEGIN TRAN");
   PriceQuery->SQL->Add("DECLARE @sc nvarchar(12) = (SELECT RIGHT(CONVERT(BIGINT, CONVERT(BINARY(6), NEWID())), 12))");
   PriceQuery->SQL->Add("DECLARE @now datetime = CURRENT_TIMESTAMP");
   PriceQuery->SQL->Add("DECLARE @ident as int");
   PriceQuery->SQL->Add("INSERT INTO Delivery (ScanCode, BillNumber, [Date], DeliveryDate, [Type])  VALUES ");
   PriceQuery->SQL->Add("(@sc, '" + SeekBillNumber + "', @now, ''," + Type + ")");
   PriceQuery->SQL->Add("SET @ident = SCOPE_IDENTITY()");
   PriceQuery->SQL->Add("INSERT INTO DeliveryItems (DocID, IDNom, Quantity, Price) VALUES");
   for(int i = 1; i < DeliveryGrid->RowCount; i++)
   {
      PriceQuery->SQL->Add("(@ident," + DeliveryGrid->Cells[DG_IDNOM_COL][i] + ",");
      PriceQuery->SQL->Add(DeliveryGrid->Cells[DG_QUANTITY_COL][i] + ",");
      PriceQuery->SQL->Add(DeliveryGrid->Cells[DG_PRICE_COL][i] + ")");
      if(i < DeliveryGrid->RowCount - 1) PriceQuery->SQL->Add(",");
   }
   PriceQuery->SQL->Add("INSERT INTO DeliveryStatus (DocID, [Date], [Status], SCash, Operator) VALUES");
   PriceQuery->SQL->Add("(@ident, @now, 1, ' " + Star->Serial + " ', ' " + CasName + " ')");
   PriceQuery->SQL->Add("SELECT @sc as ScanCode");
   PriceQuery->SQL->Add("COMMIT TRAN");
   PriceQuery->SQL->Add("END TRY");
   PriceQuery->SQL->Add("BEGIN CATCH");
   PriceQuery->SQL->Add("IF @@TRANCOUNT > 0 ROLLBACK TRAN;");
   PriceQuery->SQL->Add("DECLARE @ErrorMessage NVARCHAR(4000);");
   PriceQuery->SQL->Add("DECLARE @ErrorSeverity INT;");
   PriceQuery->SQL->Add("DECLARE @ErrorState INT;");
   PriceQuery->SQL->Add("SELECT @ErrorMessage = ERROR_MESSAGE(), @ErrorSeverity = ERROR_SEVERITY(), @ErrorState = ERROR_STATE();");
   PriceQuery->SQL->Add("RAISERROR (@ErrorMessage, @ErrorSeverity, @ErrorState);");
   PriceQuery->SQL->Add("END CATCH");

   _di_Errors        errCollection;
   _di_Error        errSingle;
   int iCount = 0;
   int SQL_RAISERROR_CODE =0;
   try
   {
      PriceQuery->Active = true;
      errCollection = CashConnection->Errors;
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
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + PriceQuery->SQL->Text;
      log(errormsg);
      PriceQuery->Active = false;
      return 0;
   }
   catch (_di_Errors &errCollection)
   {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg, Desc;
      int errorCode = 0;
      int iCount = errCollection->Count;
      _di_Error        errSingle;
      for(int i = 0; i < iCount; i++)
      {
         errCollection->Get_Item(i,errSingle);
         errSingle->Get_NativeError(errorCode);
         errormsg = " Ошибка SQL: Code = " + IntToStr(errSingle->NativeError) + " Description: " + errSingle->Description + " TSQL: " + PriceQuery->SQL->Text;
         log(errormsg);
      }
      PriceQuery->Active = false;
      return 0;
   }
   AnsiString sc = PriceQuery->FieldByName("ScanCode")->AsString;
   PriceQuery->Active = false;
   return sc;
}
//--------------------------------------------------------------------------
// печать данных на чековой ленте
void __fastcall TMainWindow::DeliveryPrint(Delivery *Doc)
{
   std::vector<AnsiString> ItemString;

   Star->PrintF("      Доставка", 2);
   Star->PrintF("Дата: " + FormatDateTime("dd.mm.yyyy hh:mm:ss",Doc->DateTime), 7);
   Star->PrintF("Кассир: " + AnsiString(Doc->Operator), 7);
   Star->PrintEAN(_atoi64(Doc->ScanCode.SubString(1,12).c_str()));

   for(std::vector<DeliveryItems>::iterator it = Doc->Items.begin(); it < Doc->Items.end(); ++it)
   {
      ItemString.clear();
      ItemString = GenerateItemString(it->Name, it->Quantity, it->Price, 50);
      for(std::vector<AnsiString>::iterator j = ItemString.begin(); j < ItemString.end(); ++j)
      {
         Star->PrintF(*j, 5);
      }
      Star->PrintF(AnsiString::StringOfChar('-', 50), 5);
   }
}
//----------------------------------------------------------------------
// запрос документа из базы данных, вывод в виде структуры

Delivery __fastcall TMainWindow::GetDeliveryDoc(AnsiString ScanCode, bool local)
{
   TADOQuery *Query;
   Delivery Ret;
   // переключатель откуда брать информацию из локальной базы или из общей
   if(local)  Query = PriceQuery;
   else Query = CentralQuery;

   Query->SQL->Clear();
   Query->SQL->Add("SELECT d.*, ds.* FROM Delivery d");
   Query->SQL->Add("CROSS APPLY");
   Query->SQL->Add("(SELECT TOP 1 Status, Date as StatusDate, [Operator], SCash FROM DeliveryStatus WHERE DeliveryStatus.DocID = d.DocID ORDER BY Date DESC) ds");
   Query->SQL->Add("WHERE Scancode='" + ScanCode + "'");
   try
   {
     Query->Active = true;
   }
   catch (EOleException &eException)
   {
     Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
     PlaySound("oy.wav",0,SND_ASYNC);
     AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + Query->SQL->Text;
     log(errormsg);
     Query->Active = false;
     return Delivery(); // возврат пустой структуры
   }
   if(Query->RecordCount == 0) return Delivery();

   Ret = Delivery(Query->FieldByName("ScanCode")->AsString,
         Query->FieldByName("BillNumber")->AsString,
         Query->FieldByName("Date")->AsDateTime,
         Query->FieldByName("DocID")->AsInteger,
         Query->FieldByName("Type")->AsInteger,
         Query->FieldByName("status")->AsInteger,
         Query->FieldByName("StatusDate")->AsDateTime,
         Query->FieldByName("SCash")->AsString,
         Query->FieldByName("Operator")->AsString
         );

   Query->Active = false;

   Query->SQL->Clear();
   Query->SQL->Add("SELECT * FROM DeliveryItems di");
   Query->SQL->Add("CROSS APPLY (SELECT TOP 1 Name, ScanCode as ItemScanCode, Meas FROM price WHERE price.IDnom = di.IDNom ORDER BY ScanCode DESC) pr");
   Query->SQL->Add("WHERE di.DocID = '" + AnsiString(Ret.DocID) + "'");
   try
   {
     Query->Active = true;
   }
   catch (EOleException &eException)
   {
      Name->Caption = "Ошибка SQL. Немедленно прекратить работу!";
      PlaySound("oy.wav",0,SND_ASYNC);
      AnsiString errormsg = "EOleException: Source=\""+eException.Source+"\" ErrorCode="+IntToStr(eException.ErrorCode)+" Message=\""+eException.Message+"\"" + Query->SQL->Text;
      log(errormsg);
      Query->Active = false;
      return Delivery();
    }

    if(Query->RecordCount == 0) return Delivery();
    for(int i = 0; i < Query->RecordCount; i++)
    {
       AnsiString a = Query->FieldByName("Name")->AsString.Trim();
       Ret.Items.push_back(DeliveryItems(Query->FieldByName("IDNom")->AsString.Trim(),
         Query->FieldByName("Name")->AsString.Trim(),
         Query->FieldByName("ItemScanCode")->AsString.Trim(),
         QuantityAshyper(Query->FieldByName("Quantity")->AsString),
         MoneyAshyper(Query->FieldByName("Price")->AsString),
         MoneyAshyper(Query->FieldByName("Meas")->AsString)
         ));
       Query->Next();
    }
    Query->Active = false;
    return Ret;
}
//----------------------------------------------------------------------
// генерируем строки для печати (разбитие, слияние с ценой и количеством)
std::vector<AnsiString> __fastcall TMainWindow::GenerateItemString(AnsiString Str,unsigned hyper qnty, unsigned hyper price, int wide)
{
//   TStringList Ret;
   std::vector<AnsiString> Ret;
   AnsiString str;
   AnsiString qxp = " " + QuantityAsString(qnty) + " X " + MoneyAsString(price);
   int lenSum = Str.Length() + qxp.Length();
   int remain = lenSum%wide;
   str = Str.Trim() + AnsiString::StringOfChar(' ', wide - remain) + qxp;

   for(int i = 0; i < str.Length()/wide + 1; i++)
   {
      Ret.push_back(str.SubString(1 + wide * i,wide*i + wide));
   }

return Ret;
}


