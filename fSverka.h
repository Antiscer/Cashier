//---------------------------------------------------------------------------

#ifndef fSverkaH
#define fSverkaH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <map>
#include <vector>

#include "mainform.h"

#define NAL_TYPE              1
#define ELECTRON_TYPE         2
#define GIFTCARD_TYPE         3
#define RETURN_NAL_TYPE       11
#define RETURN_ELECTRON_TYPE  12
#define RETURN_GIFTCARD_TYPE  13

#define SV_PAYTYPE_COL     0
#define SV_FR_SUM_COL      1
#define SV_KASS_SUM_COL    2
#define SV_DIFF_SUM_COL    3

#define ROUND_PART_TYPE      88

//---------------------------------------------------------------------------
class TFormSverka : public TForm
{
__published:	// IDE-managed Components
   TStringGrid *sgSverka;
   TButton *svCloseBtn;
   TLabel *svLabel;
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormCreate(TObject *Sender);
   void __fastcall svCloseBtnClick(TObject *Sender);
   void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
   TMStarF * Star;
public:		// User declarations
   __fastcall TFormSverka(TComponent* Owner);
   void __fastcall TFormSverka::Update();
//   std::vector<int, > vSverka;
// ???????? ???????? ????? ???????
// ?????????????? ??????????? ?? ????, ???????? ????? ??? ? VISA - ??? ??????????? ??? ???????
   struct stPayType
   {
      int numType;
      int Type;
      AnsiString PayType;
      short int code;
      stPayType(){};
      stPayType(int numType, int Type, AnsiString PayType, short int code)
      {
         this->numType = numType;
         this->Type = Type;
         this->PayType = PayType;
         this->code = code;
      };
   };

   std::vector<stPayType> payType;
//   std::map<int, int> regNum;
   std::vector<stSverka> vSverka;

   std::map<int, AnsiString> PayTypeName;
   std::map<int, int> PayTypeSign;

   struct DBPay
   {
      AnsiString PayType;
      unsigned hyper Sum;
      DBPay(){};
      DBPay(AnsiString pt, unsigned hyper sum): PayType(pt), Sum(sum){};
   };
   void __fastcall TFormSverka::GetDBPay(std::map<AnsiString, unsigned hyper> &bp);
   void __fastcall TFormSverka::SetGrid(std::vector<stSverka> &vs);
   bool __fastcall st_cmp(const stSverka& a, const stSverka& b);
   bool __fastcall gr_cmp(const stSverka& a, const stSverka& b);
   AnsiString __fastcall MoneyAsString(hyper Money);
   int __fastcall TFormSverka::ShowModal(bool silent);
   bool __fastcall TFormSverka::checkSverka();
   void __fastcall TFormSverka::ClearGrid(TStringGrid *Grid);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSverka *FormSverka;
//---------------------------------------------------------------------------
#endif
