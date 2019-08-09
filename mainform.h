//---------------------------------------------------------------------------

#ifndef mainformH
#define mainformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <Menus.hpp>


#include "SelectPayTypeFormUnit.h"
#include "clock.h"
#include "smartcardreader.h"
#include "CopyRes.h"
#include "scanner.h"
#include "mstar.h"
#include "Struct.h"
#include "frReturnForm.h"
#include "fSverka.h"
#include "receipt.h"

#include <ComCtrls.hpp>
#include <MPlayer.hpp>
#include <Db.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <ScktComp.hpp>
#include <Qrctrls.hpp>
#include <QuickRpt.hpp>
#include <ADODB.hpp>
#include <DBGrids.hpp>
#include <IniFiles.hpp>
#include <vector>
#include <map>

#define NO_ROUND        1
#define NO_CARD         0x00
#define MANAGER_CARD    0x01
#define KASSIR_CARD     0x02
#define ILLEGAL_CARD    0x04

#define NO_PAYMENT                      ""
#define NAL_PAYMENT                     " 1"
#define SBERCARD_PAYMENT                " 6"
#define INTERNATIONAL_CARD_PAYMENT      "10"
#define COMPLEX_PAYMENT                 "16"
#define GIFT_CARD_PAYMENT               " 3"
// колонки таблицы товаров Prodict Grid
#define PG_ID_COL                0
#define PG_CODE_COL              1
#define PG_NAME_COL              2
#define PG_MEASURE_COL           3
#define PG_PRICE_COL             4
#define PG_QUANTITY_COL          5
#define PG_COST_COL              6
#define PG_KASSIR_COL            8
#define PG_HANDINPUT_COL         9
#define PG_SETNUM_COL            10
#define PG_SETPRICE_COL          11
#define PG_NDS_COL               12
#define PG_IDNOM_COL             13
#define PG_ESTIMATED_PRICE_COL   14
#define PG_CORRECTION_COL        15
// колонки таблицы платежей Combo Pay
#define CP_ID_COL          0
#define CP_NAME_COL        1
#define CP_SUM_COL         2
#define CP_DEL_COL         3
#define CP_STATUS_COL      4
#define CP_PAY_TYPE_COL    5
#define CP_RRN_COL         6
#define CP_ROUND_COL       7
#define CP_RESIDUE_COL     7  // остаток по ПК
#define CP_CARD_ID_COL     8
#define CP_CARD_TYPE_COL   9
#define CP_CARD_NAME_COL   10
#define CP_HASH_COL        11
#define CP_CHECK_COL       12
#define CP_SBOWN_COL       13
// статусы платежей
#define CP_NEW_STATUS      1
#define CP_COMPLETE_STATUS 2
#define CP_APPROVE_STATUS  3
#define CP_OLD_STATUS      4

#define MR_GIFTCARD_REG          4180
#define MR_RETURN_GIFTCARD_REG   4182
#define MR_NAL_REG               193
#define MR_RETURN_NAL_REG        195
#define MR_SALE_REG              121
#define MR_RETURN_SALE_REG       123
#define MR_BC_MIR_REG            197
#define MR_RETURN_BC_MIR_REG     199
#define MR_BC_VMC_REG            201
#define MR_RETURN_BC_VMC_REG     203
#define MR_BC_OTHER_REG          205
#define MR_RETURN_BC_OTHER_REG   207

#define PIG_ID_COL         0
#define PIG_CODE_COL       1
#define PIG_NAME_COL       2
#define PIG_MEASURE_COL    3
#define PIG_QUANTITY_COL   4
#define PIG_PRICE_COL      5
#define PIG_IDNOM_COL      6
#define PIG_STATUS_COL     7

#define DG_ID_COL          0
#define DG_CODE_COL        1
#define DG_NAME_COL        2
#define DG_MEASURE_COL     3
#define DG_QUANTITY_COL    4
#define DG_PRICE_COL       5
#define DG_IDNOM_COL       6

#define DELIVERY_DOC_TYPE_DELIVERY  1
#define DELIVERY_DOC_TYPE_PICKUP    2



//typedef int (*dynamic_card_auth)(char *, struct auth_answer13);
//---------------------------------------------------------------------------
class TMainWindow : public TForm
{
__published:	// IDE-managed Components
    TEdit *TSum;
    TEdit *Recived;
    TEdit *Change;
    TStringGrid *Grid;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TMainMenu *Menu;
    TStatusBar *StatusBar;
    TEdit *Nnum;
    TLabel *Label4;
    TLabel *Label5;
    TEdit *Price;
    TLabel *Label6;
    TEdit *Qnty;
    TLabel *Unit;
    TLabel *Name;
    TMenuItem *Bill;
    TMenuItem *Cash;
//    TServerSocket *CashierServer;
    TMenuItem *SB;
    TMenuItem *SBManual;
    TMenuItem *N2;
    TMenuItem *SBDayClose;
    TMenuItem *CheckRepeat;
   TMenuItem *Return;
        TADOQuery *PriceQuery;
        TADOConnection *CashConnection;
        TMenuItem *N4;
        TMenuItem *N1;
        TMenuItem *N5;
        TMenuItem *N6;
        TMenuItem *N7;
        TMenuItem *N8;
        TMenuItem *N9;
        TMenuItem *N10;
        TMenuItem *N11;
        TMenuItem *N12;
        TMenuItem *N13;
        TMenuItem *N14;
        TMenuItem *N15;
        TMenuItem *COL;
        TMenuItem *DelLastBill;
        TMenuItem *L1;
        TMenuItem *L2;
        TMenuItem *N16;
        TEdit *TOff;
        TLabel *Label7;
        TADOQuery *CentralQuery;
        TMenuItem *FreeCheck;
        TMenuItem *N17;
        TMenuItem *N18;
        TLabel *IDNom;
        TLabel *Label8;
        TMemo *GiftCardM;
        TMenuItem *N19;
        TADOConnection *CentralConnection;
        TLabel *Label9;
        TLabel *PresentLabel;
        TStringGrid *PresentGrid;
   TMenuItem *ClearPresent;
   TStringGrid *ComboPayGrid;
   TMenuItem *ComboPayCancel;
   TMenuItem *ReturnBC;
   TPanel *panelDelivery;
   TLabel *lbPickup;
   TLabel *lbDelivery;
   TBevel *Bevel11;
   TBevel *Bevel21;
   TEdit *PickupSum;
   TLabel *Label12;
   TLabel *lbItog1;
   TLabel *Label13;
   TLabel *lbItog2;
   TBevel *Bevel12;
   TBevel *Bevel22;
   TEdit *DeliverySum;
   TStringGrid *PickupGrid;
   TStringGrid *DeliveryGrid;
   TMenuItem *N3;
   TMenuItem *DeliveryInit;
   TMenuItem *InvertGrids;
   TMenuItem *DeliveryDoc;
   TMenuItem *DeliveryDocRepeat;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyPress(TObject *Sender, char &Key);
    void __fastcall MeasureClick(TObject *Sender);
    void __fastcall PriceChangeClick(TObject *Sender);
    void __fastcall DeleteAllClick(TObject *Sender);
    void __fastcall DeleteNnumClick(TObject *Sender);
    void __fastcall X_ReportClick(TObject *Sender);
    void __fastcall S_CloseClick(TObject *Sender);
    void __fastcall S_OpenClick(TObject *Sender);
    void __fastcall CashClick(TObject *Sender);
    void __fastcall BoxClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
//    void __fastcall CashierServerClientError(TObject *Sender,
//          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
//          int &ErrorCode);
    void __fastcall SBClick(TObject *Sender);
    void __fastcall SBManualClick(TObject *Sender);
    void __fastcall SBDayCloseClick(TObject *Sender);
    void __fastcall CheckRepeatClick(TObject *Sender);
    void __fastcall CancelClick(TObject *Sender);
    void __fastcall CopyCheckClick(TObject *Sender);
    void __fastcall N1Click(TObject *Sender);
    void __fastcall ReturnClick(TObject *Sender);
    void __fastcall N4Click(TObject *Sender);
    void __fastcall COLClick(TObject *Sender);
    void __fastcall DelLastBillClick(TObject *Sender);
        void __fastcall L1Click(TObject *Sender);
        void __fastcall L2Click(TObject *Sender);
        void __fastcall FormDblClick(TObject *Sender);
        void __fastcall N16Click(TObject *Sender);
        void __fastcall FreeCheckClick(TObject *Sender);
        void __fastcall N17Click(TObject *Sender);
        void __fastcall N18Click(TObject *Sender);
        void __fastcall N19Click(TObject *Sender);
   void __fastcall ClearPresentClick(TObject *Sender);
   void __fastcall ComplexPayKeyClick(TObject *Sender);
   void __fastcall ComboPayGridDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall ComboPayCancelClick(TObject *Sender);
   void __fastcall ComboPayGridDblClick(TObject *Sender);
   void __fastcall ComboPayGridKeyPress(TObject *Sender, char &Key);
   void __fastcall ReturnBCClick(TObject *Sender);
   void __fastcall InvertGridsClick(TObject *Sender);
   void __fastcall DeliveryDocClick(TObject *Sender);
   void __fastcall DeliveryInitClick(TObject *Sender);
   void __fastcall DeliveryDocRepeatClick(TObject *Sender);
   void __fastcall PickupGridDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
   void __fastcall DeliveryGridDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
//   void __fastcall ReturnBCClick(TObject *Sender);
private:	// User declarations
    CRITICAL_SECTION CS;
    int __fastcall TMainWindow::CheckSum(AnsiString Code);
    TClockThread *Clock;
    TSmartCardReader *Reader;
    TCopyRes *CopyRes;
    TScanner *Scan;
    AnsiString OutputDir;
    AnsiString Department;
    AnsiString DepartmentId;
    AnsiString DepartmentName;
    AnsiString ServerDirectory;
    AnsiString SupportEMail;
    AnsiString Psw;
    AnsiString NDS;
    AnsiString GoodName;
    AnsiString GoodPrice;
    AnsiString TotalSum;
    AnsiString TextToShow;
    AnsiString LastScancode;
    AnsiString CardNominal;

    int cansetnum_flag;
    int CommPort;
    int CommPortD;
    int ZKPort;
    int MstarCom;
    hyper BaudMStar;
    bool NewCode;
    bool InputQnty;
    bool InputMoney;
    bool NewPrice;
    bool Delete;
    int DeleteMode;
    bool ManualZK;
    bool ManualSB;
    bool Sale;
    bool HandInput;
    bool ScannerEnter;
    bool PresentEnter;
    AnsiString SoundDir;
    bool ScanCodeCheckDigit;

    char  cName[40000];
    char cPrice[4950];
    char  cQnty[4950];
    char  cMeas[8250];
    char  cCode[6600];
    char   cNDS[600];

    HANDLE hCommD;          // the handle of the opened Comm Device Дисплей.

    void __fastcall CodeEnter(AnsiString Code);
    void __fastcall AddToTable(TStringGrid *Grid);
    void __fastcall AddToPresentTable();
    void __fastcall AlterLastRow(TStringGrid *Grid);
    void __fastcall ArrangeTable();
    void __fastcall CalcTotal();
    void __fastcall KeyPressDelete(char Key);
    bool __fastcall Seek(AnsiString, bool);

    bool __fastcall GiftCardIsInCheck(AnsiString Code);
    bool __fastcall SeekGiftCard(AnsiString Code);
    bool __fastcall ActivateGiftCard(AnsiString Code);
    void __fastcall ActivateGiftCards(TStringGrid *Grid,int RowNum);
    bool __fastcall ShowGiftCard(AnsiString Code,AnsiString CapMsg,int ButtonType);

    bool __fastcall GiftCardAddPayment(AnsiString Code,AnsiString Balance);
    bool __fastcall GiftCardAddPayment1(AnsiString Code,AnsiString Balance, AnsiString Residual, AnsiString Summ);
    bool __fastcall GiftCardClearPayments();
    unsigned hyper __fastcall GiftCardCalcPayments();
    bool __fastcall GiftCardDoPayments();
//    bool __fastcall GiftCardDoPayment(AnsiString Code,AnsiString Summ);
//    bool __fastcall GiftCardPrintPayments();

    void __fastcall SendMessageAB();
    void __fastcall SendDeletedLine();
//    void __fastcall SendDeletedBill();
//    void __fastcall SendCloseSession();
//    void __fastcall SendOpenSession();
    bool __fastcall PaySB(AnsiString Sum, char* Scard);
    int __fastcall CheckFile(char* Check, char* FileName);
    bool __fastcall CancelMC(AnsiString Sum);
    bool __fastcall CheckSale();
    bool __fastcall CheckSale0();
    bool __fastcall WriteRetail(AnsiString PayType);
    bool __fastcall WriteCloseRetail(void);
    void __fastcall SelectPayType(void);
    void __fastcall ShowOnDisplay(char * Message);
    void __fastcall Present(AnsiString s,int Gift);
    AnsiString __fastcall GetLastCardCheckSumm(void);
    void __fastcall DiscountCalc(int Rnd);

    AnsiString __fastcall FormatN(AnsiString N);
    AnsiString __fastcall FormatD(AnsiString D);
    AnsiString __fastcall FormatDV(AnsiString D,AnsiString V);
    bool __fastcall WritePayment(AnsiString PayType,AnsiString Summ);
    bool __fastcall StocksInCheck();
    int __fastcall Explode(String sContent, String sSeparator, TStringList *slTokens, bool bTrim);
   struct Stock
   {
      AnsiString IDStock;
      AnsiString Priority;
      AnsiString Condition;
      AnsiString Description;
      AnsiString WavFileName;
      AnsiString Remain;
      Stock(AnsiString IDStock, AnsiString Priority, AnsiString Condition,
            AnsiString Description,AnsiString WavFileName,AnsiString Remain)
      {
         this->IDStock = IDStock;
         this->Priority = Priority;
         this->Condition = Condition;
         this->Description = Description;
         this->WavFileName = WavFileName;
         this->Remain = Remain;
      }
   };
   AnsiString ProgramPath;
   
public:		// User declarations
    TMStarF * Star;
    AnsiString CasName;
    int CasNumber;
    unsigned hyper Round;
    bool ScreenSaverStarted;
    bool Session;
    bool Log;
    AnsiString BillHead[3];
    int BillHeadCount;
    AnsiString InputFile;
    AnsiString SQLServer;
    AnsiString SQLServerCentral;
    AnsiString PayType;
    bool FROnline;  // Онлайн-касса или нет
    int PayFlag; // флаг наличной (1) или безналичной (10) оплаты
    __fastcall TMainWindow(TComponent* Owner);
    void __fastcall ReaderAnswer(hyper result);
    void __fastcall ScannerAnswer(AnsiString code);
//    void __fastcall PlayWav(AnsiString FileName);
    bool __fastcall SendFiles();
    bool __fastcall SendError();
    bool __fastcall SynchronizeServer();
    hyper __fastcall MoneyAshyper(AnsiString String);
    unsigned hyper __fastcall QuantityAshyper(AnsiString String);
    unsigned hyper __fastcall X(unsigned hyper Price,unsigned hyper Quantity);
    AnsiString __fastcall MoneyAsString(unsigned hyper Money);
    AnsiString __fastcall QuantityAsString(unsigned hyper Quantity);
    bool __fastcall GetSverka(bool Silent);
    bool __fastcall AddToFile(char * Input,char * Output);
    bool __fastcall PrintFileOnFR(char* Input);
    bool __fastcall GetSerialID(void);
    bool __fastcall isrus(AnsiString s);
    void __fastcall GetKKMError();
    void __fastcall CheckStockDataModel();
//    void __fastcall StockInfoShow(bool Visible);
    AnsiString __fastcall SeekName(AnsiString IDNom);
    void __fastcall InfoShow(bool PrEnter, bool Visible);
    bool __fastcall VerifyPresent(AnsiString IDNom, TStringGrid *StrGrid, int colNum);
    bool __fastcall VerifyAllPresent(TStringGrid *StrGrid, int colNum);
    bool __fastcall InsertToBill(TStringGrid *Grid);
    bool __fastcall InsertToBillPresent(TStringGrid *Grid);
    void __fastcall AddToPresentGrid(TStringGrid *Grid);
    void __fastcall ClearForm();
    void __fastcall WritePresent(TStringGrid *Grid);
    void __fastcall ClearGrid(TStringGrid *Grid);
    bool __fastcall PresentPrintBill(TStringGrid *Grid);
    bool __fastcall UpdateLocalCounts();
    bool __fastcall UpdateCentralCounts(TStringGrid *Grid);
    bool __fastcall CheckCentralCounts(TStringGrid *Grid);
    void __fastcall SaveGridToFile(TStringGrid *Grid, AnsiString FileName);
    void __fastcall LoadGridFromFile(TStringGrid *Grid, AnsiString FileName);
    void __fastcall ReadParams();
    bool __fastcall StocksProcessing();
    void __fastcall PlayStockSound();
    void __fastcall log(AnsiString s);
    double __fastcall RoundTo(double val, int Digit);
    hyper __fastcall SumExcept(int Rows, bool N);
//    void __fastcall KKMErrHand();
   AnsiString __fastcall ftc(AnsiString fn);
   bool __fastcall FRInit();
   void __fastcall ClearBillData();
   BillData mfBill;
   FRStatus frStatus;
   bool __fastcall CreateBillBody(bool onReceipt);

   HANDLE hEvent1, hPrnEvent, evConnStatus, evSQLConnStatusOK;

//   std::vector<BillData> mfBillData;
//   std::vector<BillItemLine> PrintLineData;
   std::vector<BillItemLine> ShadowLineData;
   std::vector<BillItemLine> PickupBillData;
   std::vector<GiftCardData> GiftItemData;
   std::vector<PresentsData> PresentItemData;
   AnsiString LogDir;
   bool __fastcall CheckSumEAN(AnsiString Code);
   void __fastcall ComboPayClick();
   void __fastcall AddComboPay(AnsiString PayType, AnsiString Summ, TStringGrid *Grid);
   hyper __fastcall RestPayment();
   unsigned hyper __fastcall GridSum(TStringGrid *Grid, int colNum);
   unsigned hyper __fastcall TMainWindow::GridSum(TStringGrid *Grid, int colPrice, int colQuantity);
   void __fastcall SetPayStatus(TStringGrid *Grid, int Row, int Status);
   bool Payments; // признак того, что есть платежи
   void __fastcall BulkSetPayStatus(TStringGrid *Grid, int Status);
   void __fastcall RecivedCalc();
   DWORD __fastcall preauth(ComboPay *sPay);
   DWORD add_auth(unsigned hyper amount, char* rrn);
   DWORD complete_auth(unsigned hyper amount, const char* rrn, ComboPay* Pay);
   DWORD cancel_auth(const char* rrn);
   void __fastcall DeleteLastRow(TStringGrid *Grid);
   void __fastcall RemoveRow(TStringGrid *Grid, int Row);
   int __fastcall GetPayStatus(TStringGrid *Grid, int Row);
   void __fastcall ComboPayOldClear();
   void __fastcall RenumRow(TStringGrid *Grid);
   bool __fastcall complete_all_auth(std::vector<ComboPay> *ComletePayInfo);
   DWORD return_auth(ComboPay *sPay);
   void __fastcall GroupComboPayGrid();
   int _fastcall CheckNalPayment();
   int __fastcall RoundSum();
   void __fastcall GiftCardSumRenew(AnsiString Code,AnsiString Balance, AnsiString Residual, AnsiString Sum);
   AnsiString __fastcall ShieldingString(AnsiString str);
   bool NoCard;
   unsigned char CardType;
   bool SetNum;
   bool SetPrice;
   bool Printing;
   bool __fastcall CheckBool(bool &);
   void __fastcall SetBool(bool &);
   void __fastcall DownBool(bool &);
   bool NowSearching;
   TSelectPayTypeForm * SelectPayTypeForm;
//   TFormSverka * Sverka;
   void __fastcall TMainWindow::frReport();
   Delivery __fastcall TMainWindow::SeekBill(AnsiString Code);
   bool BillPickup;
   void __fastcall ShowDeliveryPanel(bool enable, Delivery *data);
   void __fastcall InitDeliveryPanel();
   bool __fastcall TMainWindow::MoveItemBetweenTable(AnsiString IdNom, TStringGrid *FromGrid, TStringGrid *ToGrid);
   int __fastcall TMainWindow::SearchInGrid(AnsiString string, TStringGrid *Grid, int colNum);
   AnsiString SeekBillNumber;
   AnsiString __fastcall TMainWindow::PushDeliveryDoc(Delivery *data);
   void __fastcall TMainWindow::DeliveryPrint(Delivery *Doc, bool Long);
   Delivery __fastcall TMainWindow::GetDeliveryDoc(AnsiString ScanCode);
   std::vector<AnsiString> __fastcall TMainWindow::GenerateItemString(AnsiString Str,unsigned hyper qnty, unsigned hyper price, int wide);
   std::map<AnsiString,int> pickupCols;
   std::map<AnsiString,int> deliveryCols;
   void __fastcall TMainWindow::DeliveryPushGrid(std::vector<DeliveryItems> *data, TStringGrid *Grid, std::map<AnsiString,int> cols);
   AnsiString __fastcall TMainWindow::FormatBillNumber(AnsiString bn);
   std::vector<DeliveryItems> __fastcall TMainWindow::DeliveryPopGrid(TStringGrid *Grid, std::map<AnsiString,int> cols);
   void __fastcall TMainWindow::SetDeliveryStatus(Delivery *data);
   void __fastcall TMainWindow::DeliveryDiffDocPrint(Delivery *Doc, std::vector<DeliveryItems> *Items);
   void __fastcall TMainWindow::DeliveryItemsPrint(std::vector<DeliveryItems> *Items);
   bool SQLConnOK;
   void __fastcall TMainWindow::SetConnStatus(bool conn);
   bool __fastcall TMainWindow::GetConnStatus(bool silent);
   AnsiString TMainWindow::GetLastDeliveryDoc();
   hyper __fastcall TMainWindow::GetSumDeliveryDoc(std::vector<DeliveryItems> *items);
   std::vector<BillItemLine> __fastcall GetProdItems();
   Receipt *receipt;
   bool __fastcall TMainWindow::SetFoundStatus(Delivery *doc, int status);
   bool __fastcall TMainWindow::SetFoundStatus(Delivery *doc);
   bool __fastcall TMainWindow::DownFoundStatus(Delivery *doc);
   std::vector<DeliveryItems> __fastcall TMainWindow::DiffVector(std::vector<DeliveryItems> vec_1, std::vector<DeliveryItems> vec_2);
   std::vector<DeliveryItems> __fastcall TMainWindow::FilterVector(std::vector<DeliveryItems> vec, bool flag);
   void __fastcall TMainWindow::PushDeliveryLostItems(Delivery lostDoc);
   void __fastcall TMainWindow::ClearDeliveryLostItems(int docID);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainWindow *MainWindow;
//----------------------------------------------------------------------
#endif
