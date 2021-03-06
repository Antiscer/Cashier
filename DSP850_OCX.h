// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// C++ TLBWRTR : $Revision:   1.134.1.39  $
// File generated on 07.08.2007 11:28:06 from Type Library described below.

// ************************************************************************ //
// Type Lib: dsp850.dll (1)
// IID\LCID: {3EC18DA5-A4F2-11D4-8B68-008048B6BB00}\0
// Helpfile: 
// DepndLst: 
//   (1) v2.0 stdole, (C:\WINDOWS\system32\STDOLE2.TLB)
//   (2) v4.0 StdVCL, (C:\WINDOWS\system32\STDVCL40.DLL)
// ************************************************************************ //
#ifndef   __DSP850_OCX_h__
#define   __DSP850_OCX_h__

#pragma option push -b -w-inl

#include <utilcls.h>
#if !defined(__UTILCLS_H_VERSION) || (__UTILCLS_H_VERSION < 0x0500)
//
// The code generated by the TLIBIMP utility or the Import|TypeLibrary 
// and Import|ActiveX feature of C++Builder rely on specific versions of
// the header file UTILCLS.H found in the INCLUDE\VCL directory. If an 
// older version of the file is detected, you probably need an update/patch.
//
#error "This file requires a newer version of the header UTILCLS.H" \
       "You need to apply an update/patch to your copy of C++Builder"
#endif
#include <olectl.h>
#include <ocidl.h>
#if !defined(_NO_VCL)
#include <stdvcl.hpp>
#endif  //   _NO_VCL
#include <ocxproxy.h>

#include "DSP850_TLB.h"
namespace Dsp850_tlb
{

// *********************************************************************//
// HelpString: 
// Version:    1.0
// *********************************************************************//


// *********************************************************************//
// COM Component Proxy Class Declaration
// Component Name   : TDSP850_
// Help String      : 
// Default Interface: IDSP850I
// Def. Intf. Object: IDSP850IPtr
// Def. Intf. DISP? : Yes
// Event   Interface: 
// TypeFlags        : (2) CanCreate
// *********************************************************************//
class PACKAGE TDSP850_ : public Oleserver::TOleServer
{
  IDSP850IPtr m_DefaultIntf;
  _di_IUnknown __fastcall GetDunk();
public:
  __fastcall TDSP850_(TComponent* owner) : Oleserver::TOleServer(owner)
  {}

  IDSP850IPtr& GetDefaultInterface();
  void __fastcall InitServerData();
  void __fastcall Connect();
  void __fastcall Disconnect();
  void __fastcall BeforeDestruction();
  void __fastcall ConnectTo(IDSP850IPtr intf);


  short __fastcall Get_wPort()
  {
    return GetDefaultInterface()->get_wPort();
  }

  void __fastcall Set_wPort(short param)
  {
    GetDefaultInterface()->set_wPort(param);
  }

  long __fastcall Get_dwSpeed()
  {
    return GetDefaultInterface()->get_dwSpeed();
  }

  void __fastcall Set_dwSpeed(long param)
  {
    GetDefaultInterface()->set_dwSpeed(param);
  }

  short __fastcall Get_wError()
  {
    return GetDefaultInterface()->get_wError();
  }

  void __fastcall Set_wError(short param)
  {
    GetDefaultInterface()->set_wError(param);
  }

  void __fastcall Disconnect1(void)
  {
    GetDefaultInterface()->Disconnect();
  }

  void __fastcall SelectLanguage(short wLanguage)
  {
    GetDefaultInterface()->SelectLanguage(wLanguage);
  }

  void __fastcall SaveView(short wLayer)
  {
    GetDefaultInterface()->SaveView(wLayer);
  }

  void __fastcall GotoXY(short wX, short wY)
  {
    GetDefaultInterface()->GotoXY(wX, wY);
  }

  void __fastcall ClrScr(void)
  {
    GetDefaultInterface()->ClrScr();
  }

  void __fastcall RunDemo(short wLayer, short wMode)
  {
    GetDefaultInterface()->RunDemo(wLayer, wMode);
  }

  void __fastcall PutStr(BSTR sStr)
  {
    GetDefaultInterface()->PutStr(sStr);
  }

  void __fastcall Connect1(void)
  {
    GetDefaultInterface()->Connect();
  }

  __property short wPort={ read=Get_wPort, write=Set_wPort, stored=false };
  __property long dwSpeed={ read=Get_dwSpeed, write=Set_dwSpeed, stored=false };
  __property short wError={ read=Get_wError, write=Set_wError, stored=false };
};

};     // namespace Dsp850_tlb

#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using  namespace Dsp850_tlb;
#endif

#pragma option pop

#endif // __DSP850_OCX_h__
