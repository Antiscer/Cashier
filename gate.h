/**
 * @file public/gate.h
 * @author A. Brik, S. Papazov
 * @date 12 June 2016
 * @brief Public API for gate.dll.
 */
 
#ifndef _PUBLIC_GATE_
#define _PUBLIC_GATE_

/** @mainpage �������� ��������
 * ��� ������������ ����������, ��������� ���� � ���� ���������� �������� ������������� ������������ ���������� GATE.DLL,
 * �������������� ���������� ����������� �� ������, � ����� ������������ ����� � �������� ������� �� ����.
 * ��� ������ � ������� �������� ������� ���������� �������������� ������� ����������� � ���-�����������.
 * ��� ���������� ������������ ��� ���������� ����, ��� ����� ���-���� ��������, � ����� ��������� �������
 * ����������������� ������ ��� ������ ������� � �������������� �������� ���������. ���-���������� �����
 * ������������ � ���-�����, ����� USB ��� �� ��������� ���2 ����� Ethernet Tcp/Ip. �������� �������������
 * ��� ���������� ��������� �� ������ �������� �������� ���������� � �������� ������������.
 * ���� �������� ������� (���) ������� ����������� ������� ��� ��������� ������, �� � ������ ��������������
 * ����������/������� ��������� �� ����� �������� ��� ���-����������. ��� �������������� ������������ (� �����
 * ��� �������� ������ � �������� �����������) ���-���������� ����������� �����������.
 */

/** @page page2 ������� ������ � �����������
 * ������� ::call_sb_kernel �������� �������� ��� ���������� ���� ��������.
 @code{.cpp}
 GATE_API DWORD call_sb_kernel(DWORD  func, void   *in_arg,  void   *out_arg);
 @endcode
 @warning � ������� Windows x86 ������� ��������� �� ����� � �������������� _call_sb_kernel, � ������� ��� Windows CE ��� �������������.
 
 * ������ �������� func ���� DWORD (������������� �������������) ��������� �������� �� ������ ::KernelOperation � ������ �������� ��������.
 * ������� ��������� ���������� ������ ���������� � ��������� ::InArg. �������� ���������� ���������� � ������� ���� InArg::in_struct, � �������������� ���� InArg::Reserved.
 * �� ������������ ���� ::InArg ������ ���� ����������� � ������� ��������.
 * �������� ��������� ���������� ������� ���������� � ��������� ::OutArg. ��� ��������� ����������� ���������� ������� ������������������� ���� OutArg::out_struct � OutArg::Reserved.
 * 
 * � ������ ��������� ������ ������� ���� ������� ::call_sb_kernel ���������� 0 (������������� ��������� ::ERR_OK � ��. �������� ����� ������ ����), � ��������� ������ � ��������� ��� ������. ���������� ��������� ������ ���������������� ��������, ���-��������� ��������  ::call_sb_kernel. ���� ������� ������� ::ERR_OK, ������� ���������������� ���� out_arg.ErrorCode.
 * � ���� ErrorCode ������������ ��� ������ ��� 0, ���� �������� ���� ������� ���������.
 * ����� ������� ������� ���������� ��������� ������ ���������� �������� ���� Flags � 0. ����� ������ ������� ���������� ��������� ������ ���������������� ���������� ���� Flags � ��������� �������� � ������������ � �������������� �������.  
 * ���� ���������� ���� F_HAVE_DOCUMENT, ���������� ��������� ������ ��������� ������ ��������� � ������� ����������� ������� ::KO_SIZE_OF_CHEQUE � ::KO_GET_LAST_CHEQUE.
 * ���� ���������� ���� F_HAVE_MONMSG, ���������� ��������� ������ ��������� ������ ��������� ������� ����������� � ������� ����������� ������� ::KO_GET_MONITORING_STATE.
 * ���� ���������� ���� F_CRITICAL (����� ��������������� ������ ��������� � F_HAVE_DOCUMENT), ���������� ��������� ����� ������ ��������� ������ ���������, ��� �� ��������� ������� (��� ������������� �������� ������������� � ������������).
 * �������� Reserved ������ ����� �������� NULL, ���� ��������� ���������� �� �������-������ � �������� ���������� �������.
 * �������� out_struct � ��������� �� ����� ���� struct_out_clX, ��� X � ����� ������, ��� ������� ���������� �����, ��� ��������� �� ����� ���� struct_outX, ��� X � ����� ������� ������ 0, ��� NULL, ���� ������� �� ���������� �����������. ���� ����� ��-��������� ������� ������ � ������ ��������� ���������� �������.
 * ������� ������ � ����������� ������ � ������ ������������ � ����������� � ����� "Docs\��� ������������� �� ���\examples\gate.dll\cpp"

� ����� ������ ��� ������ ����� ����������� ��� (������������������ �������� ����������������):
-# �������� ������������ �����
* @code{.cpp}
#include "..\..\..\..\errors.h"
#include "..\..\..\..\gate.h"
#include "..\..\..\..\sbkernel.h"
  @endcode
-# �������� ���������
* @code{.cpp}
  InArg in;
  OutArg out;
  struct_in_cl4 InDetail;
  struct_out_cl4 OutDetail;
  struct_in_reserved4 inExtra;
  struct_out_reserved4 outExtra;
  @endcode
  
-# ������� ���������
* @code{.cpp}
  in.in_struct = &InDetail;
  out.out_struct = &OutDetail;
  out.Reserved = &outExtra;
  outExtra.size = sizeof(outExtra);
  inExtra.size = sizeof(inExtra);
  in.Reserved = &inExtra;
  @endcode
 
-# ������ ��������
* @code{.cpp}
  InDetail.Amount = amount;
  strcpy_s(inExtra.RRN, rrn.c_str());
  @endcode

-# ��������� ���������� � �������� �������.
* @code{.cpp}
  HMODULE dll = LoadLibrary("gate.dll");
  Gate_Call_Sb_Kernel gate_Call_Sb_Kernel = (Gate_Call_Sb_Kernel)GetProcAddress(dll, "_call_sb_kernel");
  @endcode
  @warning ����������� ���������������� ���������� �������� KO_INITIALIZATION
-# ������� ����������� ����������� �������
* @code{.cpp}
  DWORD error = gate_Call_Sb_Kernel(KO_PREAUTH,&in,&out);
  @endcode
  
-# ��������� ��������� ������
* @code{.cpp}
  throw_if_error(error, "perform_preauth");
  throw_if_error(out.ErrorCode, "perform_preauth");
  std::cout << "Customer used card:" << OutDetail.CardName << std::endl;
  @endcode

-# ��� ������������� ��������� ����������.
  @warning ����������� ��������� ������ ���������� KO_FINALIZATION
* @code{.cpp}
  FreeLibrary(dll);
  @endcode
*/

/** @page page3 ������ ������� � �������������� ������ �������
  ���������� ����������� ������������ ������ ������� � �������������� ������� �������. ��� ����� �������� ��������� ������ �������� ����������, ��� ������ ���������� ����� �������� ���������� �, �������������, ����� �������� ���� �������. ��� ���� �������� �� �������� ��������, ����� ������� �������� ���������/�����������/������������ ����� �������� ��� ���������� ���������� ������.
  ����� ����������� ����� ����� ������, ���������� � TLV-����� �������� �������� ��������������� ����� \ ���������� ������������� ��� ������ \ ���������� ������������� ��� ������ �� ������ ���������. 

���� ������ ������ �������� �� ������� ���:
-# �������� ������������ �����
* @code{.cpp}
  #include "..\..\..\..\errors.h"
  #include "..\..\..\..\gate.h"
  #include "..\..\..\..\sbkernel.h"
  @endcode
-# �������� ���������
* @code{.cpp}
  InArg in;
  OutArg out;
  struct_in_cl4 InDetail;
  struct_out_cl4 OutDetail;
  struct_in_reserved4 inExtra;
  struct_out_reserved4 outExtra;
  @endcode
  
-# ������� ���������
* @code{.cpp}
  in.in_struct = &InDetail;
  out.out_struct = &OutDetail;
  out.Reserved = &outExtra;
  outExtra.size = sizeof(outExtra);
  inExtra.size = sizeof(inExtra);
  in.Reserved = &inExtra;
  @endcode
 
-# ������ ��������
* @code{.cpp}
  outExtra.Department = DEPARTMENT_NOT_SPECIFIED;
  InDetail.Amount = 100000; // ������ ������ - ����� � ������
  @endcode

-# ��������� ���������� � �������� �������.
* @code{.cpp}
  HMODULE dll = LoadLibrary("gate.dll");
  Gate_Call_Sb_Kernel gate_Call_Sb_Kernel = (Gate_Call_Sb_Kernel)GetProcAddress(dll, "_call_sb_kernel");
  @endcode
  @warning ����������� ���������������� ���������� �������� KO_INITIALIZATION
  
-# ������� ����������� ����������� ������� �� ������
* @code{.cpp}
  DWORD error = function(4000,&in,&out); // ����� ������� _call_sb_kernel ���������� sb_kernel.dll
  @endcode

-# ���� ������ ���������� ��� ������ ��������������� �����, �� ������� _call_sb_kernel ������ ������������� ��� ������ ::ERR_THIS_IS_SBER (4353).
    � ���� outExtra.Cert �������� ��� ����� ��� �������������� � �������� ���.
    ������� ���������� � ��������� �������� ��� (������� ����� �������), ������� ���������� ���������/����������� ������� �������. �� ������ �������������� � ���������� ��� ����� �������, ��������, ����� �������� �������� �������. 
    ���� �������� ������� "�������" ������ �������, �� ������� ��������� ����� �������� �� ��� ��������.
    ����� ����� � inExtra.Track2 �������� �������� ���� ����� �� ������ ��������.
    �������� ������� ������ � ����������� �������.
    ����� �������� ����������� / ������������ �� �����.
* @code{.cpp}
  if (out.ErrorCode == ERR_THIS_IS_SBER) // 4353
  {
    std::cout << "Sberbank Spasibo detected!" << std::endl;
    inExtra.Amount = 90000;                               // �� ������� ������� 10 ������, �������� ����� 900 ������.
    strcpy_s(inExtra.Track2, (const char*)outExtra.Cert); // �������� ���� ����� ��� ������ ������
    error = function(4000,&in,&out);                      // ��������� ����� ������
  }
  @endcode
  
-# ��������� ��������� ������
* @code{.cpp}
  out.ErrorCode;
  @endcode

-# ������ � ������� ������� ��������� �������. ��� ������������� ��������� ����������.
  @warning ����������� ��������� ������ ���������� KO_FINALIZATION
* @code{.cpp}
  FreeLibrary(dll);
  @endcode
  @see ������ ���������� ��������� � ����� Docs\��� ������������� �� ���\examples\gate.dll\cpp\spasibo
  @warning ��� ��������� ������ �������� ��������� ������ �������� ������ ����� �������� �/��� ���������� ������. ��������� ������ ���������� ��������, ����� ��� ����� ������, ��� ������ � �.�. ���������.
 */
/** @page page5 ���������� ������������� �������� RequestID
  �������� RequestID ����� ������������� ��� ���������� �������� ��������� (���������) ���� � ����.
  
  ���� ��� ����������, �� ��� ���������� �������� ������� ������������ ������� _call_sb_kernel(4xxx, ...).
  �������� ����� ���������� � ���� RequestID ��������� struct_out_reserved4.
  
  ��������:
* @code{.cpp}
  InArg in;
  OutArg out;
  struct_in_cl4 InDetail;
  struct_out_cl4 OutDetail;
  struct_in_reserved4 inExtra;
  struct_out_reserved4 outExtra;
  @endcode
  
-# ������� ���������
* @code{.cpp}
  in.in_struct = &InDetail;
  out.out_struct = &OutDetail;
  out.Reserved = &outExtra;
  outExtra.size = sizeof(outExtra);
  inExtra.size = sizeof(inExtra);
  in.Reserved = &inExtra;
  @endcode
 
-# ������ ��������
* @code{.cpp}
  outExtra.Department = DEPARTMENT_NOT_SPECIFIED;
  InDetail.Amount = 100000; // ������ ������ - ����� � ������
  @endcode

-# ��������� ���������� � �������� �������.
* @code{.cpp}
  HMODULE dll = LoadLibrary("gate.dll");
  Gate_Call_Sb_Kernel gate_Call_Sb_Kernel = (Gate_Call_Sb_Kernel)GetProcAddress(dll, "_call_sb_kernel");
  @endcode
  @warning ����������� ���������������� ���������� �������� KO_INITIALIZATION
  
-# ������� ����������� ����������� ������� �� ������
* @code{.cpp}
  DWORD error = function(4000,&in,&out);
  //outExtra.RequestID �������� ������������� ��������
  @endcode

*/
/** @page page6 �������� ��������� ���� � ����
  ��� �������� ��������� ���� � ����, ���������� �������� ��� ������� � ���������� ���������.
  ����� ��������� � ������ "�������������� ���������" � ���������� "���������� ����" (�������� ��).
  ����� ����, �������� ������ ��������� "������� �����" � ����������� ������� �����.
  �� ��������� ����� ��������� (���������� �������� "���������� ����" - ���).
  ����� ����� ��� ������ �������� ����� ��������� �������� ������� � ���������� ���, � �������� ����� ����� ��������� � ��������
  �������� ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...). ��� �� ������������ ��� �������� ��������� �������� � ����.
  �������� ��� ������ ���� ������� � ��������� CP866.
  �������� ���������:
  1. ��������� ���������� �������� � ���������� �������������� �������� \ref page5 "RequestID".
  2. ��������� ������� ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...)
     � ��������� ::tag_struct_in_reserved6 ��������� ���� ::tag_struct_in_reserved6::dwQueryRequestID � ::tag_struct_in_reserved6::MerchantCheque.
     � ���� ::tag_struct_in_reserved6::dwQueryRequestID ������� �������� ��������, �������� ��� ��������������� ���������� �������� � ���� ::struct_out_reserved4::RequestID.
     ����� ���� ������� ����������, ���� �������� ��� ����������� ��� �� ���������, �� � ���� ::tag_struct_in_reserved6::MerchantCheque ������� ��������� 0.
     ��� ���� �������� ���, ���������� ��� � ������� ������� (��� �������) ����� ��������� � ���������, �� �� �������� � ����.
  3. ������������� ���������� ���� � ���� ������ ������� ��������.
     ��� ����� ����� ������� ������� ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) �� ���������� ::tag_struct_in_reserved6::dwQueryRequestID � ::tag_struct_in_reserved6::MerchantCheque ������� 0.

  ���� � ��������� ������������� ��������� �����, �� ���������� ��������, ���� ������� ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) ������ ������ ::ERR_SEND_CHEQUES_TO_SERVER.
  ��� ������, ��� ������� ���������� ��������� ����������� � ��������� ���� � ����.
  ��� ������� ���������� �������� � �������� �����, ����� ���� ������������ ��������� ������ ��� �������� � ����.
  ���� ����� �������� � ���� ������� ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) ������� ������ ::ERR_MORE_DATA, ������ � ��������� ������������ ��� ����/��������� ������ ��� ��������, � �������� ����� ��������� ���� �� �������� 0.
  ���� �������� ��� �� �������� ��� ������� � ��������, �� ��������� ����� ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) � ��� �� \ref page5 "RequestID" ������ ������ ::ERR_CHEQUE_ALREADY_PRESENT.
  ��������: @see Docs\\��� ������������� �� ���\\examples\\gate.dll\\cpp\\merh-cheque-to-host
*/


#include <windows.h>

#ifdef GATE_EXPORTS
  #include "public/paramsln.h"
  #define GATE_API __declspec(dllexport)
#else
  #include "paramsln.h"
  #define GATE_API __declspec(dllimport)
#endif


#pragma pack(1)

/**
 * @defgroup KERNEL_DESCRIPTOR_0 ��������� ������� ����������
 * @brief �������� ������� 0-22 - ������������� � �����������, ��������� ����, ������� �������������� ����� � �������, ����� ������� �����������.

 *  @see  ������� ������� ������� ������� ������� ��������� � Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
/**@{*/

/**
 * @defgroup FUNC_0 ������� 0 (KO_INITIALIZATION)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief ������������� � ���������� ���������� � ������.
          
	��� ������ ������� 0 ������� ��������� ������ � ���� struct_in0::AppVersion ���������� ����������� ����� ������ ����������, � ������� ��� ������������ ������.
	��������������� �������� ::KERNEL_MINIMAL_VERSION. � ������, ���� ������������� ����� ������ ��������� ������� ������ ����������, ������� 0 ������ ��� ������ ::ERR_INCORRECT_VERSION. � ��������� ������ ����� ���� ���������� ������ ���� ������ (��������, ��������� � ������������� ���������-���, ������������� �����������, ���������� ����������� ������ � �.�.) ��� ::ERR_OK, ���� ������������� ������ �������.
	���� ������������ ���������� ����������������� ���������� ������� �������������� ���������� �������������, �� ��� ���������� � ���� UIData.
	\code
		InArg   inArg;
		OutArg  outArg;
		struct_in0  in0;
		struct_out0 out0;
		
		memset(&inArg, 0, sizeof(inArg));
		memset(&outArg, 0, sizeof(outArg));
		memset(&in0, 0, sizeof(in0));
		memset(&out0, 0, sizeof(out0));
		
		in0.AppVersion=KERNEL_MINIMAL_VERSION;
		inArg.in_struct=&in0;
		outArg.out_struct=&out0;
		DWORD ErrorCode=call_sb_kernel(KO_INITIALIZATION, &inArg, &outArg);
	 	if (ErrorCode==::ERR_OK){
			//do something...
		}
		//Finalize library
		call_sb_kernel(KO_FINALIZATION, NULL, NULL);
	\endcode
 */
/**	@{*/

#define KERNEL_MINIMAL_VERSION 0x00030101 ///< ���������� �������������� ������ ���������� gate.dll

/** 
 *  @brief ��������� ������������� ���������� gate.dll. ������������ ��� ������� 0 (::KO_INITIALIZATION).
 */
typedef struct tag_struct_in0{
  DWORD  AppVersion;  ///< [in] ����������� ��������� ������ ����. ����� ������ �������� � ����������������� ������� 0x00VVRRBB, ��� VV � ������, RR � �����, BB � ����. ��������, ������ 2.19.12 ���������� ��� 0x0002130C. ��������������� �������� ::KERNEL_MINIMAL_VERSION.
  void   *UIData;     ///< �� ������������.
  void   *Reserved;   ///< [in] ����� ���� ������ ��� ��������� �� ��������� struct_in_reserved0
} struct_in0;

/** 
 *  @brief �������������� ��������� ��� ������� 0. �� ������������.
 */
typedef struct tag_struct_in_reserved0{
  DWORD size;          ///< ������ ��������� � ������ = sizeof(struct_in_reserved0)
  void  *Reserved1;    ///< ���������������. ������ ���� ����������� � NULL.
  ::BYTE  KeyVersion;  ///< ���������� ����. �� ������������.
} struct_in_reserved0;

/** 
 *  @brief ���������� �� ������������������ ����������.
 */
typedef struct tag_struct_out0{
  DWORD  LibVersion;  ///< [out] �������� ������������� ������ ����. ����� ������ �������� � ����������������� ������� 0x00VVRRBB, ��� VV � ������, RR � �����, BB � ����.
  void   *Reserved;   ///< �� ������������. ������ ���� ����������� � �������� 0 (NULL)
}struct_out0;
/**	@}*/


/**
 * @defgroup FUNC_1 ������� 1 (KO_FINALIZATION)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief ���������� ������ � �����������.
          
	������� 1 (KO_FINALIZATION) ������ ���������� ����� ��������� ���������� �� ������.
 */

/** 
 * @defgroup FUNC_2 ������� 2 (KO_FUNCTIONS_COUNT)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief �������� ���������� �������������� ����������� ��������
	
	� ������� ������� 2 ������� ��������� ����� ������ ���������� ��������� �����-���, 
	�������������� �����������.  ��� ���������� ��� ������������ ��������� ����-�� �������� � ������� ���� �������� ������� ������� 3.
	@see  FUNC_3

 */
/**	@{*/
/** 
 *  @brief ���������� �������������� ����������� ��������.
 */
typedef struct tag_struct_out2{
  DWORD  Count;       ///<  [out] ���-�� �������������� ��������. �������� ������������ ��� ������������� ������ �������� �������� � ������� 3. @see  FUNC_3
}struct_out2;
/**	@}*/

/** 
 * @defgroup FUNC_3 ������� 3 (KO_FUNCTIONS_LIST)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief �������� �������� �������������� ��������

	��� ������ ������� 3 ��������� struct_out3 ������ ���� ���������������� �������-��� ����������. 
	����� Buffer ����� ������ ���� ������� ���������� ���������� � ����� ����������� ������ 
	(��� ��� ����������� ���������� �������������� ������� ������� 2). 
	������������ ������ ��������� �������� ����� ���� ����������� ������� �������-��� ��� ���������� 
	����������������� ���������� (����, ���������� ���� � �.�.). 
	��-��� ������� ��������� �������������� ���������� �� ���������� (��. ������ ������� �������) 
	� ����������� �� � ��������������� ����� ����. ����� ����, �������������� ���������� � ���������� �������� 
	����� ���� �������� �� ���� Options. � ��� ����� ���� ����������� ���� ::GateOperationOptions.
	\code
	  //�������������� ��� ���������� �������������� ���������������� ������� ������� 0 
  	  //
	  InArg in;
	  OutArg out;
	  struct_out2 out2;
	  struct_out3 out3;
	  
	  //�������� ������� ������� � ��������� 1251, ������� ������������ ������ ��� ����������� ������ ������ � ���������� ������
	  setlocale(LC_ALL, "russian_Russia.1251");

	  memset(&in,0,sizeof(in));
		
	  memset(&out,0,sizeof(out));
	  memset(&out2,0,sizeof(out2));
	  out.out_struct = &out2;
		
	  DWORD error = call_sb_kernel(2,&in,&out);
		
	  if (error==::ERR_OK){
	    memset(&in,0,sizeof(in));
	    memset(&out,0,sizeof(out));
	    memset(&out3,0,sizeof(out3));
	    out.out_struct = &out3;
	    out3.Buffer = new EnumStruct[out2.Count];
	
	    if ((error = call_sb_kernel(3,&in,&out))==::ERR_OK){
		
	      for(unsigned n = 0; n < out2.Count; n++)
	      {
	        std::cout << "Function: '"<< out3.Buffer[n].Name << "' has number " << out3.Buffer[n].FuncID << std::endl;
	      }
	    }
	
	    delete[] out3.Buffer;
	  }	
	\endcode	
	��������� ������ �������
	\code{.unparsed}
Function: '������ �������' has number 1000
Function: '������ ��������' has number 1001
Function: '������� �������' has number 1002
Function: '������ ���' has number 1003
Function: '��������� ������� �����' has number 1004
Function: '��������� ��� ������� � ����� �����' has number 1005
Function: '������ �����' has number 2000
Function: '������� �� ���������' has number 2001
Function: '������� ������� �� ����' has number 2002
Function: '��������� ����' has number 2003
Function: '�������� �������' has number 2004
Function: '������ �����' has number 2005
Function: '�����������' has number 2006
Function: '�������� ������������� �����' has number 2007
Function: '������� � ����� ���������' has number 3000
Function: '���������� ����� ���������' has number 3001
Function: '�������� ��� �� ��������' has number 3002
Function: '������ �������' has number 4000
Function: '������ ��������' has number 4001
Function: '������� �������' has number 4002
Function: '������ ��������' has number 4003
Function: '����������� ������� ��� ������� ����� �������' has number 4004
Function: '����������� ������� ��� ���������� ����� �������' has number 4005
Function: '���� �� ������ American Express' has number 4006
Function: '���� ������' has number 4007
Function: '���� ������ ��������' has number 4008
Function: '���������������' has number 4009
Function: '���������� �������' has number 4010
Function: '����� �������� (�����. ����������)' has number 4011
Function: '����� �������� (�������������)' has number 4012
Function: '��������� �������' has number 4013
Function: '������� � ����� �� �����' has number 4014
Function: '����� ������� �� ������� ����' has number 4016
Function: '�������� ������� �� ������' has number 4017
Function: '������ ������' has number 4018
Function: '������ �������� �� ��������� ������� ��� �����' has number 4019
Function: '�������� �� ����������' has number 4020
Function: '��������� ���-����' has number 4021
Function: '����� ���-����' has number 4022
Function: '�������� ������� � ������ ����� �����' has number 4023
Function: '������ ���������� �������' has number 4024
Function: '��������� �������-������' has number 4025
Function: '���������� ������� �����' has number 4026
Function: '������ ����� �����' has number 4023
Function: '������ �������' has number 5000
Function: '������������� ���� � �����' has number 5001
Function: '�������� ������������� �����' has number 5002
Function: '���������� �����' has number 5003
Function: '�������� ����' has number 5004
Function: '����� ��� �� ������ � ��������� �������' has number 6000
Function: '������������� �������� �� ������ � ��������� �������' has number 6001
Function: '������������ �������� ������ �� ���� ����� ����' has number 7000
Function: '��������� ������������ ���������� ����' has number 7001
Function: '������ ������ ����������� �����' has number 7002
	\endcode
 */
/**	@{*/
/** ����� ���� Options */
typedef enum {
O_DEBIT      = 0x00000001, ///< �������� ������������ �������� ������� � ����� (�����) �������
O_CREDIT     = 0x00000002, ///< �������� ������������ ���������� ������� �� ����� (����) �������
O_CASH       = 0x00000004, ///< �������� ������������ ������ �������� �������� ������� �� ����� ��� �������� �� � �����
O_TRANSFER   = 0x00000008, ///< �������� ������������ ����������� ������� � �������� ����� (��������� ������) �������
O_SERVICE    = 0x00000010, ///< �������� �������� ���������; ��� �� ������ �� ���������� ��������� ����� (�����), �� ����� ������ �� ���������������� ��������� (����������, �������������, ����� ���� � �.�.)
O_INFO       = 0x00000020, ///< �������� �������� ������ �������������� � �� ������ �� ���������� ��� ���������������� ��������� �����
O_BANKTERM   = 0x00000040, ///< �������� ��������� �� ���������� ���������
O_TRADETERM  = 0x00000080, ///< �������� ��������� �� �������� ���������
O_SPECIAL    = 0x00000100, ///< ����������� ��������, ���������������� ��� ������������� � ��, � ������� ���������� ����������� ��������� ����� ��������.
} GateOperationOptions;

/** 
 *  @brief �������� �������������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_EnumStruct{
  DWORD  FuncID;    	 ///<  [out] ����� ������� ���������� @see KernelOperation
  DWORD  Options;        ///<  [out] �����, ����������� ���������� ��������, @see GateOperationOptions
  char   Name[64];       ///<  [out] �������� ��������. ������� �������� Windows 1251.
}EnumStruct;

/** 
 *  @brief ��������� ��� ������� �������������� ��������
 *  @note  Buffer ������ ���� ���������������. ������������ ������ ��������
 *  ����� ���� ����������� ������� ���������� ��� ���������� �����������������
 *  ���������� (����, ���������� ���� � �.�.).
 *  ��� ��������� ��������� �� ����� �������������� �������� � ������ O_TRADETERM
 *  ��� ����������� ��������� �� ����� �������������� �������� � ������ O_BANKTERM
 *  ���� �������� ������������ ������ � ��������� ������� ���������, �� ���������� ������������ ������ � �������� � ������ O_SERVICE
 *  ��� ����������� �� ����� ���� ������������ ������ � �������������� �������� � ������ O_INFO
 *  �������� �� ����� ��������� ������� ����������� �������, ������������� "��� ���������" ����� ������� ����� ����� O_SPECIAL
 */
typedef struct tag_struct_out3{
  EnumStruct  *Buffer;   ///< [in,out] ����� ������� tag_struct_out2::Count * sizeof(EnumStruct)
} struct_out3;
/**	@}*/

/** 
 *  @brief ���������� � ��������� ���� � ���������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out4{
  DWORD  Size;          ///< [out] ������ ������ ��������� � ������
}struct_out4;

/** 
 *  @brief ��������� ��� ������ � ��������� ����� � ���������
 *  @note  Buffer ������ ���� ���������������.
 *  ����� ���������� ������� 5 � ������ ����� ����������� ��������� ������ � ��������� 1251,
 *   ���������� ����� ���������, ������� ������� ������� �� ������. ��� ������ �� ������ ��-
 *   �������� ������������ ����� � ������������� ������� ��������.
 *   ������� ��������� ������ ���������� ����� ���������� ��������� �� ������. ��� �����������
 *   ������ ������ ��� ������ ������������ ��������� ������ ����� ���������� ������.
 */
typedef struct tag_struct_out5{
  void  *Buffer;         ///< [in,out] �����, � ������� ���������� ����� ���������
} struct_out5;

/** 
 *  @brief ���������� � ������� �������������� � ����������
 *  @note �� �������������� � PCI-DSS
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out6{
  DWORD  Count;          ///< [out] ����� ������������ �����
} struct_out6;

/** 
 *  @brief ���������� � ������
 */
typedef struct tagCurrDesc{
  DWORD Currency;     ///< [out] ��� ������: 810 - �����, 840 - ������� ���
  char  Name[4];      ///< [out] ����������� �������� ������, ����. "���", "USD"
  char  FullName[64]; ///< [out] ������ �������� ������
} CurrDesc;

/** 
 *  @brief ��������� ��� ������ � �������� �����
 *  @note  Buffer ������ ���� ���������������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out7{
  CurrDesc  *Buffer; ///< [in,out] �����, ����������� ����������� CurrDescr
}struct_out7;

/** ����� ���� Options */
typedef enum {
GATE_WINDOW_HANDLES = 1, ///< ��� ������������ ����������
} GateSpecificSettings;

/** 
 *  @brief ����������� ���������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in8{
  void *Settings; ///<��������� �� ����� � ������������ �����������
  int   Version;   ///<��� ������������ ���������� @see GateSpecificSettings
} struct_in8;

/** @brief ��������� ��������� ����������
 */
#ifdef __cplusplus
typedef struct tag_struct_out8{
} struct_out8;
#endif /*__cplusplus*/

/** 
 *  @brief ��������� ����� ������������
 *  @note  �� ������������ � PCI-DSS ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in9{
  char UserName[MAX_OPER_NAME+1]; ///< ��� ������������
} struct_in9;

/** @brief ��������� ��������� ����� ������������
 */
#ifdef __cplusplus
typedef struct tag_struct_out9{
} struct_out9;
#endif /*__cplusplus*/

/** 
 *  @brief ����������� ��������� ����������� � ������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in11{
  DWORD hWindow;    ///< Handle ���� ��� ��������� windows-��������� �� �������
  DWORD Message;    ///< ������������� ��������� ��� ��������� windows-��������� �� �������
} struct_in11;

/** 
 *  @brief �������� ������� �������. ��������� ���������� � ���������.
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in13{
  DWORD ScrId;   ///< ���������� ����. �� ������������. ����� ���� ����� 0.
  int   DlgNum;  ///< ���������� ����. �� ������������. ����� ���� ����� 0.
  BYTE  RFU[56]; ///< ���������� ����. �� ������������. ����� ���� ����� 0.
} struct_in13;

/** 
 *  @brief ���������� � ���������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out13{
  BYTE  Model;                   ///< ������ �������
  BYTE  Version;                 ///< ����� ������ ��������� �������.
  BYTE  Release;                 ///< ����� ������ �������� ������.
  BYTE  Build;                   ///< ����� ������ ������.
  BYTE  HasCtls;                 ///< ������� "������������� ����������� � ������� � �������".
  BYTE  bPPScreenWidth;          ///< ������ ������ �������
  BYTE  bPPScreenHeight;         ///< ������ ������ �������
  char  SN[12+1];                ///< �������� ����� �������
} struct_out13;

/** 
 *  @brief ��������� ������ ������ �� ���� ������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in14{
  DWORD dwErrorCode;          ///< ��� ������
} struct_in14;

/** 
 *  @brief ��������� ��� ������ ������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out14{
  char ErrorDescription[256]; ///< ����� ������, ��������������� ���� � �������
} struct_out14;

/** 
 *  @brief ����� �� ������ ������ ��������� � ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out15{
  char TerminalID[ 9];
} struct_out15;

/** 
 *  @brief ������������ ����� �� ������ ������ ��������� � ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out_reserved15{
  DWORD size;            ///< ������ ��������� � ������ = sizeof(struct_in_reserved15)
  char MerchantID[16];   ///< ����� ��������
} struct_out_reserved15;

/**@brief �������� ��������� amount ��� ������� �������� (������� 47) � ������� ����������� ����� ������� KO_CALL_PILOT_COMMAND
 * @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 * */
typedef enum {
  CMD_MONIT_H = 1, ///< �������� ������� �� ���������� helios
  CMD_MONIT_P = 2  ///< �������� ������� �� ���������� pserverDB
} GateMonitoringRequestType;

/** 
 *  @brief ����� ������ ��������� �����������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in22{
  int bErase;   ///< ������� ���������� ���������
} struct_in22;

/** 
 *  @brief ����� ������ ��������� �����������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out22{
  int mesType;     ///< ��� ���������. 0 - �������. ������ ���������� ������ ���� ����� ��� �������, 1 - �������. ��������� ������
  int mesState;    ///< ��������� �����������. 0 - ����������, 1 - ��������������, �������� ��������� ������ � ���������� ���������� ��������, 2 - ��� �����.
  int mesStateEx;  ///< RFU
  char msg[196];   ///< ����� ����������� ���������
} struct_out22;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_1 ���������� �������� �� ��������
 * @brief ���������� ����������. �� ������������.
 */
/**@{*/

typedef struct tag_struct_in_cl1{
  DWORD  Amount;                                   ///< ����� ��������; ���� 0 - ������������� �����������
} struct_in_cl1;

#define MAX_SBERCARD_CLIENT_NAME 38 ///< ����� ������ ����� �������� 

typedef struct tag_struct_out_cl1{
  DWORD    AmountClear;                            ///< ����� �������� ��� ��������/������
  DWORD    Amount;                                 ///< ����� �������� � ������ ��������
  char     TrxDate[MAX_DATE+1];                    ///< ���� ��������  (��.��.����)
  char     TrxTime[MAX_TIME+1];                    ///< ����� �������� (��:��:��)
  char     MerchantCard[MAX_CARD_NUM+1];           ///< ����� ����� ���������
  char     MerchantName[MAX_NAME+1];               ///< �������� ����� ���������
  char     MerchantCert[MAX_CERT+1];               ///< ������������ ���������� ����������
  ::WORD     MerchantTSN;                          ///< ����� ���������� � ������ ���������
  ::WORD     MerchantBatchNum;                     ///< ����� ������ ���������
  char     MerchantBatchDate[MAX_DATE+1];          ///< ���� ������ ��������� (��.��.����)
  char     ClientCard[MAX_CARD_NUM+1];             ///< ����� ����� �������
  char     ClientBranch[MAX_BRANCH_NUM+1];         ///< ��� ����� �������
  char     ClientAccount[MAX_ACCOUNT_NUM+1];       ///< ����� ����� �������
  char     ClientName[MAX_SBERCARD_CLIENT_NAME+1]; ///< ������� �������
  char     ClientCert[MAX_CERT+1];                 ///< ���������� ���������� ����������
  ::WORD     ClientTSN;                            ///< ����� ���������� �� ���������� �����
  char     ClientExpiryDate[MAX_DATE+1];           ///< ���� �������� ����� �������
} struct_out_cl1;


/** @brief  ��������� ��� ���������� ������
 */
typedef struct tagTPassportData{
  char sFIO[MAX_FIO_N];    ///< ���
  char sAddr[58];          ///< �����
  short  sRes;             ///< ��������/����������
  char sDocType[21];       ///< ��� ���������
  char sSer[11];           ///< �����
  char sNum[16];           ///< �����
  char sIssuer[58];        ///< ��� �����
  char sWhen[13];          ///< ����� �����
  char sValid[13];         ///< ���� ��������
  char sEmit[42];          ///< ���� - ������� �����
  char Agent[42];          ///< ����, �������� �����
} TPassportData;

/** @brief  �������������� ������ ��� ���������� �������� �� ��������
 */
typedef struct tag_struct_in_reserved1{
  DWORD size;                 ///< ������ ��������� � ������ = sizeof(struct_in_reserved1)
  void  *Reserved1;           ///< ���������������.
  DWORD Rand;                 ///< ��������� ����� ��� ��������� � ���������� (0<=Rand<100000)
  ::WORD  ColNum;             ///< ����� ������� ��� ������ �������
  TPassportData PassportData; ///< ���������� ������
  DWORD Department;           ///< ����� ������ (DEPARTMENT_NOT_SPECIFIED - ����� �� ����)
} struct_in_reserved1;

/** @brief  �������������� ���������� ���������� �������� �� ��������
 */

typedef struct tag_struct_out_reserved1{
  DWORD size;                 ///< ������ ��������� � ������ = sizeof(struct_out_reserved1)
  void  *Reserved1;           ///< ���������������.
  ::BYTE  Cert[128];          ///< ���������� ��������.
  WORD  PurchCode;            ///< ��� ���������� ��������
  DWORD PurchTrace;           ///< ����������� ���������� ��������
  TPassportData PassportData; ///< ���������� ������
} struct_out_reserved1;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_2 �������������� �������� �� ��������
 * @brief ���������� ����������. �� ������������.
 */
/**@{*/
#ifdef __cplusplus
typedef struct tag_struct_in_cl2{
}struct_in_cl2;
#endif /*__cplusplus*/

typedef struct tag_struct_out_cl2{
  char  TrxDate[MAX_DATE+1];                    ///< ���� ��������  (��.��.����)
  char  TrxTime[MAX_TIME+1];                    ///< ����� �������� (��:��:��)
  char  ClientCard[MAX_CARD_NUM+1];             ///< ����� ����� �������
  char  ClientBranch[MAX_BRANCH_NUM+1];         ///< ��� ����� �������
  char  ClientAccount[MAX_ACCOUNT_NUM+1];       ///< ����� ����� �������
  char  ClientName[MAX_SBERCARD_CLIENT_NAME+1]; ///< ������� �������
  char  ClientCert[MAX_CERT+1];                 ///< ���������� ���������� ����������
  char  ServerCert[MAX_CERT+1];                 ///< ��������� ���������� ����������
  ::WORD  ClientTSN;                            ///< ����� ���������� �� ���������� �����
  char  ClientExpiryDate [MAX_DATE+1];          ///< ���� �������� ����� �������
} struct_out_cl2;


/**
 * ��������� ��� �������� ������ � ����������� �� ����� ������� (��������)
 * */
typedef struct tagTClientTrxData{
  short TSN;              ///< ����� �������� �� ����� �������
  char  Date[6];          ///< ���� ��������
  char  Description[17];  ///< ��������
  DWORD Amount;           ///< ����� ��������
  WORD  Batch;            ///< ����� ������ ����� ���������. ����������� ��� ������� �������� �������.
  BYTE  BatchRSN;         ///< ����� ���������� � ������ ����� ���������. ����������� ��� ������� �������� �������.
  DWORD BalanceBefore;    ///< ������ ��� ����� �� ��������. ����������� ��� �������� �������� �������.
} TClientTrxData;

typedef struct tag_struct_out_reserved2{
  DWORD size;                ///< ������ ��������� � ������ = sizeof(struct_out_reserved2)
  void  *Reserved1;          ///< ���������������.
  DWORD SCABefore;           ///< SCA �� ��������
  DWORD HSSSBefore;          ///< HSSS �� ��������
  DWORD SCAAfter;            ///< SCA ����� ��������
  DWORD HSSSAfter;           ///< HSSS ����� ��������
  TClientTrxData TrxData[20];///< ������ �� ��������� � ����� �������
} struct_out_reserved2;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_3 �������������� �������� �� ��������
 * @brief ���������� ����������. �� ������������.
 */
/**@{*/
#ifdef __cplusplus
typedef struct tag_struct_in_cl3{
} struct_in_cl3;
#endif /*__cplusplus*/

/** 
 *  @brief ������ �� ���������� �������������� ��������
 */
typedef struct tag_struct_out_cl3{
  char  TrxDate[MAX_DATE+1];          ///< ���� ��������  (��.��.����)
  char  TrxTime[MAX_TIME+1];          ///< ����� �������� (��:��:��)
  char  MerchantCard[MAX_CARD_NUM+1]; ///< ����� ����� ���������
  char  MerchantName[MAX_NAME+1];     ///< �������� ����� ���������
  char  MerchantCert[MAX_CERT+1];     ///< ������������ ���������� ����������
  ::WORD  MerchantBatchNum;           ///< ����� ������ ���������
  char  MerchantBatchDate[MAX_DATE+1];///< ���� ������ ��������� (��.��.����)
  DWORD BatchAmount;                  ///< ����� ������
  ::WORD  BatchTrx;                   ///< ����� �������� � ������
  char  ServerCert[MAX_CERT+1];       ///< ��������� ���������� ����������
}struct_out_cl3;

/** 
 *  @brief ����� �� ���������� �������������� ��������
 */
typedef struct tag_struct_out_reserved3{
  DWORD size;             ///< ������ ��������� � ������ = sizeof(struct_out_reserved3)
  void  *Reserved1;       ///< ���������������.
  DWORD ClearTotal;       ///< SCA �� ��������
} struct_out_reserved3;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_4 ���������� �������� �� ������
 */
/**@{*/

/** 
 *  @brief ������ �� ���������� ���������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_cl4{
  DWORD Amount;                      ///<����� ��������, ���� 0 - ������������� �����������
  char  Track2[MAX_TRACK2+1];        ///<������ ���� ����, �� ���� ��������� 0 � ������ �����, � ������� ������ ����� ��������� 2-� ������� �����
  ::BYTE  CardType;                  ///<��� �����. @see CardTypes
} struct_in_cl4;

#define SBER_OWN_CARD     'S' ///< ������� �������������� ����� ���������. ������������ � ��������� ����� ���� struct_out_cl4::CardName.

/**
 *  @brief ����� �� ���������� ���������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_cl4{
  DWORD AmountClear;                  ///< ����� �������� ��� ����� �������� / ������
  DWORD Amount;                       ///< ����� �������� � ������ �������� / ������
  char  CardName[MAX_NAME+1];         ///< �������� ����� (Visa, Maestro � �.�.)
  ::BYTE  CardType;                   ///< ��� �����. @see CardTypes
  char  TrxDate[MAX_DATE+1];          ///< ���� ��������  (��.��.����)
  char  TrxTime[MAX_TIME+1];          ///< ����� �������� (��:��:��)
  char  TermNum[MAX_TERM+1];          ///< ����� ���������
  char  MerchNum[MAX_MERCHANT_LN+1];  ///< ����� ��������
  char  AuthCode[MAX_AUTH_CODE+1];    ///< ��� �����������
  char  RRN[MAX_RRN+1];               ///< ����� ������
  ::WORD  MerchantTSN;                ///< ����� ���������� � ������ ���������
  ::WORD  MerchantBatchNum;           ///< ����� ������ ��������� �� ������
  char  ClientCard[MAX_CARD_NUM+1];   ///< ����� ����� �������
  char  ClientExpiryDate[MAX_DATE+1]; ///< ���� �������� ����� �������
} struct_out_cl4;

/** 
 *  @brief ��������� ��� �������� �������� ����������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tagTGoodsData{
  DWORD   Price;                     ///< ���� �� �������, exp 2
  DWORD   Volume;                    ///< ���������� exp 3
  char    Name[MAX_GOODS_NAME+1];    ///< ������������ ������
  char    Code[MAX_GOODS_CODE+1];    ///< ���������� ��� ������� ������� ���������� ���������
}TGoodsData;

/** 
 *  @brief �������������� ��������� ������� �� ���������� ���������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_reserved4{
  DWORD size;                              ///< ������ ��������� � ������ = sizeof(struct_in_reserved4)
  void  *Reserved1;                        ///< ���������������
  char  RRN[MAX_RRN+1];                    ///< RRN
  DWORD RequestID;                         ///< ��������� ����� ��� ��������� � ���������� (0<=Rand<100000)
  DWORD Currency;                          ///< ��� ������. ���� 0, ���������� �� ����
  char  RecvCard[MAX_PAN_N+1];             ///< ����� ����� ���������� �������
  BYTE  BinHash[20];                       ///< ��� ����� ��������
  BYTE  HashFlags[5];                      ///< see HashFlagsMask
  TPassportData PassportData;              //</ ���������� ������
  char  AuthCode[MAX_AUTH_CODE+1];         ///< ��� ����������� ��� �������� ����� ����� �� Amex
  char  RecvValidDate[MAX_CARD_DATE+1];    ///< ���� �������� ����� ����������
  DWORD Department;                        ///< ����� ������ (DEPARTMENT_NOT_SPECIFIED - ����� �� ����)
  char  PaymentTag[MAX_PAYMENT_TAG+1];     ///< ��� ��������� �������
  char  TagValue  [MAX_PAYMENT_VALUE+1];   ///< �������� ���� ��������� �������
  char  CashierFIO[MAX_FIO_N+1];           ///< ��� �������
  char  TextMessage[MAX_TEXT_MESSAGE];     ///< ��������� ���������. ������������ ��� ������ ��� 5005/5006 (�� �������) � 4011/4012 (�� ���) ��������
  TGoodsData GoodsData;                    ///< ���������� � ������. @see TGoodsData
} struct_in_reserved4;

/** @brief ��� ���������� �����
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef enum {
 CARD_ENTRY_DEVICE             =  'D',
 CARD_ENTRY_MANUAL             =  'M', ///< ������ ���� ������ �����
 CARD_ENTRY_CHIP               =  'C', ///< ������ ��� �����
 CARD_ENTRY_FALLBACK           =  'F', ///< ����� ������� � ������ Fallback
 CARD_ENTRY_VIVO_MS            =  'R', ///< ����� ������� ������������
 CARD_ENTRY_VIVO_EMV           =  'E', ///< ����� ������� ������������
 CARD_ENTRY_BY_ID              =  'I', ///< �������� ����������� ������
} CARD_ENTRY_MODE;

/** 
 *  @brief ������������ ��������� ������ �� ���������� ���������� ��������
 * 
 * ���� struct_out_reserved4::TrxFlags ��������� ��������� ��������.
 *	 
 * ��� ������� ���� �������� ����� �������� ������ ������ �����.
 *  
 * - 0 � ��������� �����;
 * - 1 � ������ ���� ������ �����;        
 * - 2 - ������� �����;                 
 * - 3 - �� ����� ���� ���, �� ��� ������� ����� ��������� �����;                 
 * - 4 � ������������� ����� � ��������� ��������� ������;                      
 * - 5 � ������������� ����� � ��������� ����;        
 * - 6 � ������ ������������� �������.
 *
 * ��������� ����:        
 * - 0x00008000L - ���� ��������� �������������� ��������� �����;
 * - 0x00010000L - ������ online pin;         
 * - 0x00020000L - ������ offline pin;         
 * - 0x00040000L - �������� ��� ����������� ���������.
 *            
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_reserved4{
  DWORD size;                  ///< ������ ��������� � ������ = sizeof(struct_out_reserved4)
  void  *Reserved1;            ///<���������������.
  ::BYTE  Cert[128];           ///<���������� �������� ��� Track2.
  TPassportData PassportData;  ///<���������� ������
  BYTE  IsOwn;                 ///<������� ����� ���������
  DWORD Currency;              ///<��� ������: 643 - �����, 840 - ������� ���
  DWORD TrxFlags;              ///<����� ���������� ��������. ��� PCI-DSS ������ ���� ����������� ������ ���� RequestID<0.
  DWORD RequestID;             ///<������������� ��������, ����������� ��� PCI-DSS ������. @see \ref page5 "RequestID".
  char  CardEntryMode;         ///<��� ������ ����� @see CARD_ENTRY_MODE
  char  AID[33];               ///<AID �����. �� ������������ ��� PCI-DSS ������.
  BYTE  LltID;                 ///<������������� ��������� ����������
} struct_out_reserved4;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_5 �������������� �������� �� ���������� ������ � ������ ����������
 */
/**@{*/
/** 
 *  @brief ������ �� ���������� �������������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_cl5{
  char  Track2[MAX_TRACK2+1];         ///< 2-� ������� �����, ���� ������ - ����������� �����������
  ::BYTE  CardType;                   ///<��� �����. @see CardTypes
} struct_in_cl5;

/** 
 *  @brief ����� �� ���������� �������������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_cl5{
  char  CardName[MAX_NAME+1];         ///< �������� �����
  ::BYTE  CardType;                   ///<��� �����. @see CardTypes
  char  TrxDate[MAX_DATE+1];          ///< ���� ��������  (��.��.����)
  char  TrxTime[MAX_TIME+1];          ///< ����� �������� (��:��:��)
  char  TermNum[MAX_TERM+1];          ///< ����� ���������
  char  ClientCard[MAX_CARD_NUM+1];   ///< ����� ����� �������
  char  ClientExpiryDate [MAX_DATE+1];///< ���� �������� ����� �������
} struct_out_cl5;

typedef enum {
 TVM_FULL        = 0x01, ///< �������� ����� ���������
 TVM_NEXT        = 0x02, ///< �������� ������ ������������� ������ "������"
 TVM_PREV        = 0x04, ///< �������� ������ ������������� ������ "�����"
 TVM_NEED_TRACK1 = 0x08, ///< �������� ������������ Track1
 TVM_NEED_PHONE  = 0x10, ///< ���� ����������� ������
 TVM_NEED_EMAIL  = 0x20, ///< ���� ������ ����������� �����
} EditModeMask;

typedef enum {
 INFOPF_NONE             = 0x00, ///< ����� �� ����������
 INFOPF_NEGATIVE_BALANCE = 0x01, ///< �������� ������� ����� ������������
} InfoOperationFlags;

/** 
 *  @brief �������������� ��������� ������� �� ���������� �������������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_reserved5{
  DWORD size;                          ///< ������ ��������� � ������ = sizeof(struct_in_reserved5)
  BYTE  bEditMode;                     ///< @see EditModeMask
  char  TextMessage[MAX_TEXT_MESSAGE]; ///< ��������� ��������� ��� ������ ����� ��� �� ��� (� ����������� �� ��������)
  char  CashierFIO[MAX_FIO_N+1];       ///< �.�.�. �������
} struct_in_reserved5;

/** 
 *  @brief ������������ ��������� ������ �� ���������� �������������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_reserved5{
  DWORD size;                          ///< ������ ��������� � ������ = sizeof(struct_out_reserved5)
  void  *Reserved1;                    ///< ���������������.
  BYTE  Hash    [MAX_HASH];            ///< ��� ������ �����, �������� ��������
  BYTE  CardData[MAX_TRACK2];          ///< Track2 �����, ���� ������������� ����� �����

  DWORD Balance;                       ///< ������ �����
  DWORD Currency;                      ///< ��� ������: 643 - �����, 840 - ������� ���

  DWORD CardLCDataLen;                 ///< ������ ������ ������� �������
  BYTE  CardLCData[MAX_CARD_LS_DATA];  ///< ������ ������� ������� [X0=8 ����][Y0=8 ����][X1...
  char  AuthCode[6+1];                 ///< ��� �����������
  char  ClientName[MAX_CLIENT_NAME];   ///< ��� ��������� �����
  BYTE  IsOwn;                         ///< ���� "����� �������� ����������"
  BYTE  LltID;                         ///< ������������� ��������� ����������
  BYTE  Flags;                         ///< ����� �������� 5000 @see 
} struct_out_reserved5;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_6 C�������� �������� �� ������
 */
/**@{*/
/** 
 *  @brief ������ �� ���������� ��������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-6000-6999
 */
typedef struct tag_struct_in_cl6{
  DWORD  Currency;                     ///<��� ������: 810 - �����, 840 - ������� ���
} struct_in_cl6;

/** 
 *  @brief ����� �� ���������� ��������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-6000-6999
 */
typedef struct tag_struct_out_cl6{
  char  TrxDate[MAX_DATE+1];         ///< ���� ��������  (��.��.����)
  char  TrxTime[MAX_TIME+1];         ///< ����� �������� (��:��:��)
  char  TermNum[MAX_TERM+1];         ///< ����� ���������
  char  MerchNum[MAX_MERCHANT_LN+1]; ///< ����� ��������
  ::WORD  MerchantBatchNum;          ///< ����� ������ ��������� �� ����.������
  DWORD DebitNumber;                 ///< ����� �������� ������ ��������/������ �������
  DWORD DebitTotal;                  ///< ����� �������� ������ ��������/������ �������
  DWORD DebitClearTotal;             ///< ����� �������� ������ ��������/������ ������� ��� ����� ��������/������
  ::WORD  ReturnNumber;              ///< ����� �������� ��������/������������ ��������
  DWORD ReturnTotal;                 ///< ����� ���������� ��������
  DWORD ReturnClearTotal;            ///< ����� ���������� �������� ��� ����� ��������
} struct_out_cl6;

/** 
 *  @brief �������������� ��������� ������� �� ���������� ��������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-6000-6999
 */
typedef struct tag_struct_in_reserved6{
  DWORD size;                       ///< ������ ��������� � ������ = sizeof(struct_in_reserved6)
  void  *Reserved1;                 ///< ���������������.
  char  CashierCard[MAX_TRACK2+1];  ///< ����� �������
  WORD  MerchantTSN;                ///< ����� ���������� � ������ ���������
  WORD  MerchantBatchNum;           ///< ����� ������ ��������� �� ����.������
  DWORD dwAmount;                   ///< ����� ��� suspend/commit/rollback
  char  AuthCode[MAX_AUTH_CODE+1];  ///< ��� ����������� ��� suspend/commit/rollback
  char  AmountCashTotal[MAX_CASH_STATS+1];///< ����� ����� ���������, �������� � ��������
  char  CountCashTotal[MAX_CASH_STATS+1]; ///< ���������� ����� ���������
  char  ShiftIndexTotal[MAX_RRN+1]; ///< ������������� �����
  DWORD dwQueryRequestID;           ///< ������������� ���������� ��������
  const char* MerchantCheque;       ///< ���������� ��������� ���� � ��������� CP866
} struct_in_reserved6;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_7 ����� ��������� ��������
   @brief ������� ����� ������ ��������� �������� ���������� � ����� ���������� ��������� � �������, ����� ��� ������� ��� � ����������� �����.
 */
/**@{*/

#ifdef __cplusplus
typedef struct tag_struct_in_cl7{
} struct_in_cl7;

typedef struct tag_struct_out_cl7{
} struct_out_cl7;
#endif /*__cplusplus*/

/** 
 *  @brief �������������� ��������� ������� �� ���������� ��������� ��������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-7000-7999
 */
typedef struct tag_struct_in_reserved7{
  DWORD  size;        ///< ������ ��������� sizeof(struct_in_reserved7)
  int    RecordID;    ///< ����� ������ � �������. 0, struct_out_reserved7::TBatchRecord::NextRecId,... ���� �� �������� ������ ::ERR_PIL_NO_FILES
} struct_in_reserved7;

/** 
 *  @brief ��������� ��� �������� ������ � ����������� � ������ ���������
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-7000-7999
 */
typedef struct tagTBatchRecord{
  BYTE    TrxType;
  DWORD   AmountClear; // ����� �������� ��� ����� �������� / ������
  DWORD   Amount;      // ����� �������� � ������ �������� / ������
  char    CardName[MAX_NAME+1];       // �������� ����� (Visa, Maestro � �.�.)
  ::BYTE  CardType;                        ///<��� �����. @see CardTypes
  char    TrxDate[MAX_DATE+1];        // ���� ��������  (��.��.����)
  char    TrxTime[MAX_TIME+1];        // ����� �������� (��:��:��)
  char    AuthCode[MAX_AUTH_CODE+1];  // ��� �����������
  char    RRN[MAX_RRN+1];             //����� ������
  ::WORD  MerchantTSN;                // ����� ���������� � ������ ���������
  ::WORD  MerchantBatchNum;           // ����� ������ ��������� �� ����.������
  char    ClientCard[MAX_CARD_NUM+1];   // ����� ����� �������
  char    ClientExpiryDate[MAX_DATE+1]; // ���� �������� ����� �������
  ::BYTE  Hash[MAX_HASH];
  int     NextRecId;
} TBatchRecord;

/** 
 *  @brief ������������ ����� �� ���������� ��������� ��������
 *  @note ������������ � ��������� KO_GET_OPERATION_INFO � KO_GET_TERMINAL_AND_MERCHENT_ID
 *  @see  Docs/��� ������������� �� ���/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_reserved7{
  DWORD         size;                        ///< ������ ��������� � ������ = sizeof(struct_out_reserved7)
  char          TermNum[MAX_TERM+1];         ///< ����� ���������
  char          MerchNum[MAX_MERCHANT_LN+1]; ///< ����� ��������
  TBatchRecord  Record;                      ///< �������� �� �������.
} struct_out_reserved7;
/**@}*/

typedef enum {
  USE_BINARY_HASH   = 0x01,    ///< � ������� ������������ ��� ����� � �������� �������������
  NO_PIN_BYPASS     = 0x02,    ///< ������ ������ �� ����� ��� ����.
  KILL_SECOND_COPY  = 0x04,    ///< ��������� ������ ���� ����� ���� � ������ (� ����� p �� �������� ��������� ������� ����� ����� ����)
  CLIENT_SKIPPED_PIN= 0x08,    ///< �� �������� �� ���� ���� "������� �������". ��������, ���� ������ ������������� ���� �������.
} HashFlagsMask;

/** 
 *  @brief  ������� �������� ������� ::call_sb_kernel
 *  @note   �������� Reserved ������ ����� �������� NULL, ���� ��������� ���������� �� �������-������ � �������� ���������� �������. 
 */
typedef struct tagInArg{
  void *Reserved;        /**< [in]  �������������� ��������� ��������. ��� ������������� ��������� ������� �� ������ ����������� �������� functionNumber ������� ::call_sb_kernel. ������ �������� ��������� �� ��������� ���� struct_in_reservedX, ��� X � ����� ������ */
  void *in_struct;       /**< [in]  �������� in_struct � ��������� �� ����� ���� struct_in_clX, ��� X � ����� ������ , ��� ������� ���������� �����, ��� ��������� �� ����� ���� struct_inX, ��� X � ����� ������� ������ 0, ��� NULL, ���� ������� �� ��������� ����������. */
} InArg;

/** 
 *  @brief  �������� �������� ������� ::call_sb_kernel
 *  @note   �������� Reserved ������ ����� �������� NULL, ���� ��������� ���������� �� �������-������ � �������� ���������� �������. 
 */
typedef struct tagOutArg{
  DWORD ErrorCode;        /**< [in]  ��� ������*/
  DWORD Flags;            /**< [in]  ��. GateOutFlags*/
  void* Reserved;         /**< [in]  �������������� ��������� ��������. ��� ������������� ��������� ������� �� ������ ����������� �������� functionNumber ������� ::call_sb_kernel*/
  void* out_struct;       /**< [in]  ��������� �� ����� ���� struct_in_clX, ��� X � ����� ������ , ��� ������� ���������� �����, ��� ��������� �� ����� ���� struct_inX, ��� X � ����� ������� ������ 0, ��� NULL, ���� ������� �� ��������� ����������. */
} OutArg;

#pragma pack()

/**
 * @defgroup KERNEL_OPERATIONS �������� ���� sb_kernel
 */
/**@{*/

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

/** ���������� ����� ��� ��������� �������� ����������*/
typedef enum {
  F_HAVE_DOCUMENT = 0x00000001,  ///< ���������� ��������� ������ ��������� ������ ��������� � ������� ����������� ������� KO_SIZE_OF_SLIP � KO_GET_SLIPS_LIST
  F_CRITICAL      = 0x00000002,  /**< ���������� ��������� ����� ������ ��������� ������ ���������, ��� �� ��������� �������. ���
                                       *   ������������� �������� ������������� � ������������. ��������������� ������ ��������� � F_HAVE_DOCUMENT.
                                       */
  F_HAVE_MONMSG   = 0x00000004,  ///< ���������� ��������� ������ ��������� ������ ��������� ������� ����������� � ������� ����������� ������� 22
} GateOutFlags;

/** @brief ������ �������� UPOS
 *  @note 
 *  @param[in] functionNumber ���/����� ��������. ��. ::KernelOperation.
 *  @param[in] in_arg         ��������� �� ������� ��������. ��. ::InArg
 *  @param[in/out] out_arg        ��������� �� �������� ��������. ��. ::OutArg
 *  @return DWORD ��� ������. ���� ������� ������� 0, ���������� ���������������� ���� OutArg::ErrorCode.
*/
  GATE_API DWORD call_sb_kernel(DWORD  func, void   *in_arg,  void   *out_arg);

/** @brief ���������� ������ ���� sb_kernel.dll
 *  @return DWORD ������ ��� ����� ����� � ������� 0x00VVRRBB. VV - ������, RR - �����, BB - ������
 */
  GATE_API DWORD get_version();

  #ifndef WINCE
  GATE_API
    #ifdef __BORLANDC__
            __stdcall
    #endif //__BORLANDC__
                    DWORD call_sb_kernel_std(DWORD  func, void   *in_arg,  void   *out_arg);
  #endif // WINCE

#ifdef __cplusplus
}
#endif // __cplusplus

//#else
typedef DWORD (*Gate_Call_Sb_Kernel)(DWORD  func,void   *in_arg,void   *out_arg);
//#endif // UPOS
/**@}*/

#endif //_PUBLIC_GATE_
