//---------------------------------------------------------------------------

#ifndef FreeCheckH
#define FreeCheckH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFreeCheckForm : public TForm
{
__published:	// IDE-managed Components
        TEdit *Text;
        TEdit *Summ;
        TButton *Cancel;
        TButton *Ok;
        TLabel *Label1;
        TLabel *Label2;
        TRadioGroup *RadioGroupNDS;
        TRadioGroup *RadioGroupPayType;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OkClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFreeCheckForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFreeCheckForm *FreeCheckForm;
//---------------------------------------------------------------------------
#endif
