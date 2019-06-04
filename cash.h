//---------------------------------------------------------------------------

#ifndef cashH
#define cashH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TCashForm : public TForm
{
__published:	// IDE-managed Components
    TEdit *Cash;
    void __fastcall FormKeyPress(TObject *Sender, char &Key);
    void __fastcall FormShow(TObject *Sender);
   void __fastcall CashKeyPress(TObject *Sender, char &Key);
private:	// User declarations
public:		// User declarations
    __fastcall TCashForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCashForm *CashForm;
//---------------------------------------------------------------------------
#endif
