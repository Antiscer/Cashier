//---------------------------------------------------------------------------

#ifndef ComplexPayH
#define ComplexPayH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <AppEvnts.hpp>
#include <Mask.hpp>
#include "Struct.h"
#define NAL_PAYMENT                     " 1"
#define SBERCARD_PAYMENT                " 6"
#define INTERNATIONAL_CARD_PAYMENT      "10"
#define COMPLEX_PAYMENT                 "16"
//---------------------------------------------------------------------------
class TComplexPayForm : public TForm
{
__published:	// IDE-managed Components
   TStringGrid *ComplexPayGrid;
   TButton *ButtonNalPay;
   TButton *ButtonBezPay;
   TButton *ButtonPKPay;
   TGroupBox *PayGroupBox;
   TEdit *PayEdit;
   TApplicationEvents *ApplicationEvents1;
   TButton *PayButton;
   void __fastcall FormActivate(TObject *Sender);
   void __fastcall ApplicationEvents1Message(tagMSG &Msg, bool &Handled);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall PayEditKeyPress(TObject *Sender, char &Key);
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall ButtonNalPayClick(TObject *Sender);
   void __fastcall ButtonBezPayClick(TObject *Sender);
   void __fastcall PayButtonClick(TObject *Sender);
private:	// User declarations
   void __fastcall AddComplexPay(AnsiString PayType, AnsiString Summ, TStringGrid *Grid);
   AnsiString __fastcall RestPayment();
   unsigned hyper __fastcall GridSum(TStringGrid *Grid, int colNum);
   void __fastcall NalPayment();
   void __fastcall BeznalPayment();
   void __fastcall ClickReturn();
   void __fastcall ClickEscape();
public:		// User declarations
   __fastcall TComplexPayForm(TComponent* Owner);
//   BillData *cpBill;
};
//---------------------------------------------------------------------------
extern PACKAGE TComplexPayForm *ComplexPayForm;
//---------------------------------------------------------------------------
#endif
