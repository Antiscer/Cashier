//---------------------------------------------------------------------------

#ifndef spyformunitH
#define spyformunitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include "Trayicon.h"
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TSpyForm : public TForm
{
__published:	// IDE-managed Components
    TTimer *ReconnectTimer;
    TLabel *Label1;
    TLabel *Label2;
    TRichEdit *Deleted;
    TRichEdit *Bill;
    TGroupBox *Desks;
    TMainMenu *MainMenu;
    TMenuItem *N1;
    TMenuItem *Exit;
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
    TGroupBox *Sum;
    TEdit *Edit13;
    TEdit *Edit14;
    TEdit *Edit15;
    TEdit *Edit16;
    TEdit *Edit17;
    TEdit *Edit18;
    TEdit *Edit19;
    TEdit *Edit20;
    TEdit *Edit21;
    TEdit *Edit22;
    TEdit *Edit23;
    TEdit *Edit24;
    TTrayIcon *Tray;
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
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall ReconnectTimerTimer(TObject *Sender);
    void __fastcall TrayMinimize(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
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
