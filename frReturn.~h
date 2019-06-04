//---------------------------------------------------------------------------

#ifndef frReturnFormH
#define frReturnFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>

#include "mainform.h"
#include "Struct.h"
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <ComCtrls.hpp>


#define CL_NUMBER_COL      0
#define CL_DATE_COL        1
#define CL_BILLNUM_COL     2
#define CL_SUM_COL         3
#define CL_PAYTYPE_COL     4

#define OC_NUMBER_COL      0
#define OC_NAME_COL        1
#define OC_QUANTITY_COL    2
#define OC_PRICE_COL       3
#define OC_SUM_COL         4

//---------------------------------------------------------------------------
class TfrReturnForm : public TForm
{
__published:	// IDE-managed Components
   TGroupBox *gbReturnCheck;
   TRadioButton *rbReturnCheck;
   TRadioButton *rbReturnFree;
   TGroupBox *gbReturnFree;
   TStringGrid *sgCheckList;
   TEdit *fldFilter;
   TButton *ReturnBtn;
   TButton *CancelBtn;
   TStringGrid *sgOneCheck;
   TPanel *plConfirm;
   TLabel *lbSum;
   TButton *btnFRandDB;
   TButton *btnFRonly;
   TButton *btnCancelP;
   TMaskEdit *edBillNumber;
   TStringGrid *sgThingsList;
   TStringGrid *sgPays;
   TEdit *edSum;
   TDateTimePicker *DateTimePicker;
   void __fastcall ReturnBtnClick(TObject *Sender);
   void __fastcall CancelBtnClick(TObject *Sender);
   void __fastcall sgCheckListDblClick(TObject *Sender);
   void __fastcall sgCheckListDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall btnCancelPClick(TObject *Sender);
   void __fastcall btnFRandDBClick(TObject *Sender);
   void __fastcall btnFRonlyClick(TObject *Sender);
   void __fastcall FormResize(TObject *Sender);
   void __fastcall DateTimePickerChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TfrReturnForm(TComponent* Owner);
   AnsiString delBillNumber;
   TMStarF * Star;
   BillData *frRetBill;
   bool __fastcall retBillDelete(AnsiString BillNumber);
   void Multiline(TWinControl *control);
   void __fastcall GetBillItem(std::vector<BillItemLine> &biLine);
   void __fastcall BillReturn(bool delBase);
   void __fastcall PanelEnable(bool enable);
   void __fastcall GetPay(std::vector<ComboPay> &cPay);
   void __fastcall pkReturn(AnsiString ScanCode, hyper Sum);
   void __fastcall pkDeactivate(AnsiString ScanCode);
   void __fastcall GetBillList(TDate dt);
   void __fastcall ClearGrid(TStringGrid *Grid);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrReturnForm *frReturnForm;
//---------------------------------------------------------------------------
#endif
