//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FreeCheck.h"
#include "mainform.h"
#include "mstar.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFreeCheckForm *FreeCheckForm;
//---------------------------------------------------------------------------
__fastcall TFreeCheckForm::TFreeCheckForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFreeCheckForm::CancelClick(TObject *Sender)
{
  FreeCheckForm->Close();
}
//---------------------------------------------------------------------------

void __fastcall TFreeCheckForm::OkClick(TObject *Sender)
{
unsigned hyper summ;
char NDS[2];
AnsiString Nal,Card;

//��������� ������������ ������ (��� �����������)�
if( (summ = MainWindow->MoneyAshyper( FreeCheckForm->Summ->Text )) <= 0 )
  {
  summ = 0;
  if( MessageBox(NULL,"����� ���� ����� 0.00. ��������?","������ ���������� ����",MB_YESNO) == IDNO )return;
  }
if(summ > 2000000000 )
  {
  MessageBox(NULL,"����� ���� �� ����� ���� ������ 20 ��������� ������, ��������� �� �� ��������� �����","������ ���������� ����",MB_OK);
  return;
  }

  //��� � ����������� �������

switch( FreeCheckForm->RadioGroupNDS->ItemIndex )
  {
  case 0: NDS[0] = '1'; break;
  case 1: NDS[0] = '2'; break;
  case 2:
  default: NDS[0] = '3'; break;
  }
NDS[1] = 0;

//��� ������ ��������
switch( FreeCheckForm->RadioGroupPayType->ItemIndex )
  {
  default:
  case 0: Card = "0.00";
          Nal = FreeCheckForm->Summ->Text;
          break;
  case 1: Nal = "0.00";
          Card = FreeCheckForm->Summ->Text;
          break;
  }

//�������
    if(MainWindow->Star->Serial == "" || MainWindow->Star->SerialID == "") //���� �������� ����� �� ���������, �������� �� �����!
      {
      MessageBox(NULL,"��� ������ �� ��� ID","������ ���� ����������",MB_OK);
      return;
      }

//��������� ���
    MainWindow->Star->BillOpen(0,MainWindow->CasName,MainWindow->CasNumber);
    if(MainWindow->Star->KKMResult != "0000")
    {
        MainWindow->GetKKMError();
        if(MainWindow->Star->KKMResult != "0000") return;
    }

// ������������ � ���� ������ ������� � ���������� ����������
    MainWindow->Star->BillLine(FreeCheckForm->Text->Text.c_str(),FreeCheckForm->Summ->Text.c_str(),"1",1,"��","",NDS);


// ������� ���
    MainWindow->Star->BillPayment(Nal,"0.00","0.00",Card);
    if(MainWindow->Star->KKMResult != "0000")
    {
        MainWindow->GetKKMError();
        MainWindow->Star->GetVersion();
        MainWindow->GetSverka(true);
        if(MainWindow->Star->KKMResult != "0000") return;
    }

  FreeCheckForm->Close();
}
//---------------------------------------------------------------------------
