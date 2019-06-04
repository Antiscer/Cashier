//---------------------------------------------------------------------------

#include <vcl.h>
#include "mainform.h"
#pragma hdrstop

#include "screensaver.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSaver *Saver;
//---------------------------------------------------------------------------
__fastcall TSaver::TSaver(TComponent* Owner)
    : TForm(Owner)
{
    Text = "";
    Speed = 8;
    MainWindow->ScreenSaverStarted = true;
}
//---------------------------------------------------------------------------
void __fastcall TSaver::FormShow(TObject *Sender)
{
    dx = random(Speed);
    dy = Speed - dx;
    x = ClientWidth/2;
    y = ClientHeight/2;
    Label->Caption = Text;
    Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TSaver::Timer1Timer(TObject *Sender)
{
    TColor cl[6] = {
        clWhite, clYellow, clRed, clLime, clBlue, clFuchsia};
    x += dx;
    if(x + Label->Width > ClientWidth || x < 0)
    {
        dx = -dx;
        if(x + Label->Width > ClientWidth && dx>0) dx=-dx;
        x += dx;
        Label->Font->Color = cl[random(6)];
    }
    y += dy;
    if(y + Label->Height > ClientHeight || y < 0)
    {
        dy = -dy;
        y += dy;
        Label->Font->Color = cl[random(6)];
    }
    Label->Left = x;
    Label->Top = y;
    Sleep(100);
}
//---------------------------------------------------------------------------

void __fastcall TSaver::FormHide(TObject *Sender)
{
    Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

