//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Sspyform.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Trayicon"
#pragma resource "*.dfm"
TSpyForm *SpyForm;
//---------------------------------------------------------------------------
__fastcall TSpyForm::TSpyForm(TComponent* Owner)
    : TForm(Owner)
{
    ClientList = new TList;
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::SocketError(System::TObject* Sender,
    TCustomWinSocket* Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    if(Socket->Connected)
    {
        Socket->Close();
    }
/*    switch(ErrorEvent)
    {
        case eeConnect:
//            StatusBar->SimpleText = "Ошибка подключения к "
//                + ((TClientSocket*) Sender)->Address;
            break;
//        default:
//            StatusBar->SimpleText = "Ошибка";
    }*/
//eeGeneral	The socket received an error message that does not fit into any of the following categories.
//eeSend	An error occurred when trying to write to the socket connection.
//eeReceive	An error occurred when trying to read from the socket connection.
//eeConnect	A connection request that was already accepted could not be completed.
//eeDisconnect	An error occurred when trying to close a connection.
//eeAccept	A problem occurred when trying to accept a client connection request.    }
    ErrorCode = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::SocketDisconnect(System::TObject* Sender, TCustomWinSocket* Socket)
{
    for(int i = 0; i < ClientList->Count; i++)
    {
        if(Socket->RemoteAddress == ((TClientSocket*) ClientList->Items[i])->Address)
        {
            ((TEdit*) Desks->Controls[i])->Enabled = false;
            ((TEdit*) LastDate->Controls[i])->Enabled = false;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::SocketConnect(System::TObject* Sender, TCustomWinSocket* Socket)
{
    for(int i = 0; i < ClientList->Count; i++)
    {
        if(Socket->RemoteAddress == ((TClientSocket*) ClientList->Items[i])->Address)
        {
            ((TEdit*) Desks->Controls[i])->Enabled = true;
            ((TEdit*) LastDate->Controls[i])->Enabled = true;
            break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::SocketRead(System::TObject* Sender,
        TCustomWinSocket* Socket)
{
    AnsiString str = Socket->ReceiveText();
    if(str.SubString(8,5) == "Смена")
    {
        for(int i = 0; i < ClientList->Count; i++)
        {
            if(Socket->RemoteAddress == ((TClientSocket*) ClientList->Items[i])->Address)
            {
                ((TEdit*) LastDate->Controls[i])->Text = str.SubString(8, str.Length()-8);
                break;
            }
        }
        if(str.AnsiPos("закрыта"))
        {
            Tray->Restore();
            Beep(1000,1000);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    TClientSocket* Socket;
    for(int i = 0; i < ClientList->Count; i++)
    {
        Socket = (TClientSocket*) ClientList->Items[i];
        if(Socket->Active)
        {
            Socket->Close();
        }
    }
    ClientList->Clear();
    delete ClientList;
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::FormCreate(TObject *Sender)
{
    TEdit *Box;
    TClientSocket* Socket;
    if(FileExists("cashspy.ini"))
    {
        TStringList *IniList =new TStringList;
        IniList->LoadFromFile("cashspy.ini");
        int count = IniList->Count;
        count /= 2;
        count *= 2;
        if(count > 24) count = 24;
        for(int i = 0; i < count; i += 2)
        {
            Socket = new TClientSocket(this);
            Socket->ClientType = ctNonBlocking;
            Socket->Address = IniList->Strings[i+1];
            Socket->Port = 1024;
            Socket->OnError = SocketError;
            Socket->OnConnect = SocketConnect;
            Socket->OnDisconnect = SocketDisconnect;
            Socket->OnRead = SocketRead;
            for(int i = 0; i < ClientList->Count; i++)
            {
                if(Socket->Address == ((TClientSocket*) ClientList->Items[i])->Address)
                {
                    delete Socket;
                    Socket = NULL;
                }
            }
            if(Socket)
            {
                ClientList->Add(Socket);
                if(!Socket->Active) Socket->Open();
                Box = (TEdit*) Desks->Controls[ClientList->Count - 1];
                Box->Text = IniList->Strings[i];
                Box->Visible = true;
                ((TEdit*) LastDate->Controls[ClientList->Count - 1])->Visible = true;
            }
        }
        IniList->Clear();
        delete IniList;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::ExitClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::ReconnectTimerTimer(TObject *Sender)
{
    TClientSocket* Socket;
    for(int i = 0; i < ClientList->Count; i++)
    {
        Socket = (TClientSocket*) ClientList->Items[i];
        if(!Socket->Socket->Connected)
        {
            if(Socket->Active) Socket->Close();
            Socket->Open();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSpyForm::FormActivate(TObject *Sender)
{
    Tray->Minimize();
}
//---------------------------------------------------------------------------

