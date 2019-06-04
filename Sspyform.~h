//---------------------------------------------------------------------------

#ifndef SspyformH
#define SspyformH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ScktComp.hpp>
#include <Forms.hpp>
#include "Trayicon.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TSpyForm : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *Desks;
    TEdit *Edit1;
    TEdit *Edit2;
    TEdit *Edit3;
    TEdit *Edit4;
    TEdit *Edit5;
    TEdit *Edit6;
    TEdit *Edit7;
    TEdit *Edit8;
    TEdit *Edit9;
    TEdit *Edit10;
    TEdit *Edit11;
    TEdit *Edit12;
    TGroupBox *LastDate;
    TEdit *Edit25;
    TEdit *Edit26;
    TEdit *Edit27;
    TEdit *Edit28;
    TEdit *Edit29;
    TEdit *Edit30;
    TEdit *Edit31;
    TEdit *Edit32;
    TEdit *Edit33;
    TEdit *Edit34;
    TEdit *Edit35;
    TEdit *Edit36;
    TMainMenu *MainMenu;
    TMenuItem *N1;
    TMenuItem *Exit;
    TTimer *ReconnectTimer;
    TTrayIcon *Tray;
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall ReconnectTimerTimer(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
    TList* ClientList;
    void __fastcall SocketError(System::TObject* Sender,
        TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode);
    void __fastcall SocketDisconnect(System::TObject* Sender,
        TCustomWinSocket* Socket);
    void __fastcall SocketConnect(System::TObject* Sender,
        TCustomWinSocket* Socket);
    void __fastcall SocketRead(System::TObject* Sender,
        TCustomWinSocket* Socket);
public:		// User declarations
    __fastcall TSpyForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSpyForm *SpyForm;
//---------------------------------------------------------------------------
#endif
