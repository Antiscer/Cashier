//---------------------------------------------------------------------------

#ifndef billprintcH
#define billprintcH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <QuickRpt.hpp>
#include <Qrctrls.hpp>
//---------------------------------------------------------------------------
class TBillPrint : public TForm
{
__published:	// IDE-managed Components
        TQuickRep *QuickRep1;
        TQRMemo *QRMemo1;
        TQRLabel *QRLabel1;
        TQRBand *QRBand3;
        TQRLabel *QRLabel6;
        TQRLabel *QRLabel7;
        TQRLabel *QRLabel8;
        TQRLabel *QRLabel9;
        TQRBand *TitleBand1;
        TQRBand *PageFooterBand1;
        TQRMemo *QRMemo2;
        TQRLabel *QRLabel2;
        TQRLabel *QRLabel3;
        TQRLabel *QRLabel4;
        TQRLabel *QRLabel5;
        void __fastcall QuickRep1NeedData(TObject *Sender, bool &MoreData);
        void __fastcall TitleBand1BeforePrint(TQRCustomBand *Sender,
          bool &PrintBand);
        void __fastcall QuickRep1BeforePrint(TCustomQuickRep *Sender,
          bool &PrintReport);
private:	// User declarations
        double Itog;
public:		// User declarations
        __fastcall TBillPrint(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBillPrint *BillPrint;
//---------------------------------------------------------------------------
#endif
