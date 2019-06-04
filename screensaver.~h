//---------------------------------------------------------------------------

#ifndef screensaverH
#define screensaverH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSaver : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label;
    TTimer *Timer1;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
private:	// User declarations
    int dx;
    int dy;
    int x;
    int y;
public:		// User declarations
    AnsiString Text;
    int Speed;
    __fastcall TSaver(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSaver *Saver;
//---------------------------------------------------------------------------
#endif
