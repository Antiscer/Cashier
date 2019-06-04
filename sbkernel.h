/**
 * @file public/gate.h
 * @author A. Brik, S. Papazov
 * @date 12 June 2016
 * @brief Public API for gate.dll.
 */
 
#ifndef _PUBLIC_SBKERNEL_
#define _PUBLIC_SBKERNEL_

#pragma pack(1)

/**
 * @addtogroup KERNEL_OPERATIONS
 */
/**@{*/

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

/** ���� �������� ���� sb_kernel */
typedef enum {
  KO_INITIALIZATION          =    0, ///< ���������� ���������� � ������
  KO_FINALIZATION            =    1, ///< ���������� ������ ����������
  KO_FUNCTIONS_COUNT         =    2, ///< ��������� ����� �������������� ������� ������� � KO_SB_SALE
  KO_FUNCTIONS_LIST          =    3, ///< ��������� ������ �������������� ������� ������� � KO_SB_SALE
  KO_SIZE_OF_CHEQUE          =    4, ///< ��������� ������� ��������� ��� ������
  KO_GET_LAST_CHEQUE         =    5, ///< ��������� ������ ��������� ��� ������
  KO_GET_DEPARTMENTS_COUNT   =    6, ///< ��������� ����� ����������� ���������� (�����)
  KO_GET_DEPARTMENTS_LIST    =    7, ///< ��������� ������ ����������� ���������� (�����)
  KO_SET_USER_INTERFACE      =    8, ///< ��������� ����������������� ����������
  KO_SET_USER_NAME           =    9, ///< ��������� ����� ������������
  KO_GET_TERMINAL_TECH_MENU  =   10, ///< ����� ���� ���. ������������ � ��������� ���������� ��������
  KO_ENABLE_READER           =   11, ///< ����������� ��������� ����������� � ������
  KO_DISABLE_READER          =   12, ///< ��������� ������ � ��������� ������������
  KO_TEST_READER             =   13, ///< ��������� ������� ����� � ��������� �����������
  KO_GET_ERROR_TEXT          =   14, ///< ������������� ���� ������ � �����
  KO_GET_TERMINAL_ID         =   15, ///< �������� ����� ��������� (� ��������, �����������)
  KO_ABORT_OPERATION         =   16, ///< �������� �������� ��������
  KO_CALL_PILOT_COMMAND      =   17, ///< ������ ������� � ����������� sb_pilot
  KO_CALL_STANDALONE_APP     =   18, ///< ������ ��� ��������� ����������
  KO_GET_MONITORING_STATE    =   22, ///< ��������� ��������� ��� ����� ��� ��������� ������� �����������

  KO_SB_SALE                 = 1000, ///< ������ ��������
  KO_SB_CASH                 = 1001, ///< ������ �������� ��������
  KO_SB_REFUND               = 1002, ///< ������� ��������
  KO_SB_PETROL_FOR_PETROL    = 1003, ///< ������ ��� ��������
  KO_SB_CHECK_AMOUNT         = 1004, ///< ��������� ������� ����� ��������
  KO_SB_GET_CARD_HOLDER_INFO = 1005, ///< ��������� ��� ������� � ����� ����� ��������

  KO_SB_CARD_BALANCE         = 2000, ///< ������ ����� ��������
  KO_SB_OPERATION_INFO       = 2001, ///< ������� �� ��������� ��������
  KO_SB_TRANSFER             = 2002, ///< ������� ������� �� ���� ��������
  KO_SB_CHANGE_PIN           = 2003, ///< ��������� ���� ��������
  KO_SB_LOAD                 = 2004, ///< ������ ����� ��������
  KO_SB_CHECK_ACCOUNT        = 2005, ///< �������� ������� ��������
  KO_SB_AUTHORIZATION        = 2006, ///< ����������� ��������
  KO_SB_GET_CARD_ID          = 2007, ///< �������� ������������� ����� ��������
  
  KO_SB_MERCHANT_INFO        = 3000, ///< ������� � ����� ��������� ��������
  KO_SB_SETTLMENT            = 3001, ///< ���������� ����� ��������� ��������
  KO_SB_CLOSE_DAY            = 3002, ///< �������� ��� ��������

  KO_SALE                    = 4000, ///< ������ �������
  KO_CASH                    = 4001, ///< ������ ��������
  KO_REFUND                  = 4002, ///< ������� �������
  KO_CANCEL                  = 4003, ///< ������ ��������
  KO_TRANSFER                = 4004, ///< ����������� ������� ��� ������� ����� �������
  KO_TRANSFER_NO_CARD        = 4005, ///< ����������� ������� ��� ���������� ����� �������
  KO_ENTER_AMEX_SLIP         = 4006, ///< ���� �� ������ American Express
  KO_ENTER_SALE_SLIP         = 4007, ///< ���� ������
  KO_ENTER_CASH_SLIP         = 4008, ///< ���� ������ ��������
  KO_PREAUTH                 = 4009, ///< ���������������
  KO_COMPLETE_AUTH           = 4010, ///< ���������� �������
  KO_CASH_IN_BEGIN           = 4011, ///< ����� ��������. ��������������� ����������
  KO_CASH_IN_COMPLETE        = 4012, ///< ����� ��������. �������������
  KO_CREDIT_PAYMENT          = 4013, ///< ��������� �������
  KO_CARD_TO_CARD_TRANSFER   = 4014, ///< ������� � ����� �� �����
  KO_COMMUNAL_PAYMENT        = 4015, ///< ������������ �������
  KO_AREST                   = 4016, ///< ����� ������� �� ������� ����
  KO_AREST_COMPLETION        = 4017, ///< �������� ������� �� ������
  KO_AREST_CANCEL            = 4018, ///< ������ ������
  KO_CASH_NO_CARD            = 4019, ///< ������ �������� �� ��������� ������� ��� �����
  KO_TRANSFER_NO_CLIENT      = 4020, ///< �������� �� ����������
  KO_SET_PIN                 = 4021, ///< ��������� ���-����
  KO_CHANGE_PIN              = 4022, ///< ����� ���-����
  KO_FUNDS_TRANSFER          = 4023, ///< �������� ������� � ������ ����� �����
  KO_CAPTURE_BIO_DATA        = 4024, ///< ������ ���������� �������
  KO_SET_OFFLINE_LIMIT       = 4025, ///< ��������� �������-������
  KO_PRINT_HISTORY           = 4026, ///< ���������� ������� �����

  KO_OPEN_CARD_READER        = 4040, ///< ������ PCI DSS. ������ �������� ����� �����.
  KO_TEST_CARD_READ          = 4041, ///< ������ PCI DSS. ��������� ������� ����� ��� �������� ����� �����.
  KO_CLOSE_CARD_READER       = 4042, ///< ������ PCI DSS. ��������� �������� ����� �����.

  KO_CARD_BALANCE            = 5000, ///< ������ �������
  KO_UNBLOCK_CARD            = 5001, ///< ������������� ���� � �����
  KO_GET_CARD_ID             = 5002, ///< �������� ������������� �����
  KO_BLOCK_CARD              = 5003, ///< ���������� �����
  KO_CHECK_PIN               = 5004, ///< �������� ����
  KO_GET_TOOCH_SIGNATURE     = 5005, ///< ��������� ������� ������� �� ������ ���������
  KO_SHOW_LONG_TEXT          = 5006, ///< ����� ������ �� ����� ���������/������� ��� �������������
  KO_SHOW_LONG_TEXT_INPUT    = 5007, ///< ����� ������ �� ����� ���������/������� ��� ������������� � ���� ����� ������
  KO_EJECT_CARD              = 5020, ///< ������� �����
  KO_CAPTURE_CARD            = 5021, ///< ��������� �����
  KO_TEST_CARD               = 5022, ///< ������� �����
 
  KO_TOTALS                  = 6000, ///< ������ ������. ����� ���.
  KO_COMMIT_TRANSACTION      = 6001, ///< ������������� �������� �� �����
  KO_X_TOTALS                = 6002, ///< �-�����. ������ ������ ������ � ������ �������.
  KO_SUSPEND_TRANSACTION     = 6003, ///< ������������� �������� (��� ������������� ��� ������)
  KO_ROLLBACK_TRANSACTION    = 6004, ///< ��������� ������ ��������
  KO_ADD_MERCHANT_CHEQUE     = 6005, ///< �������� ��������� ���� � ��������. ������ PCI DSS.
  
  KO_FULL_REPORT             = 7000, ///< ������������ ������� ������. ����������� �����.
  KO_LAST_CHEQUE             = 7001, ///< ������������ ���������� ����.
  KO_SHOW_REPORT_MENU        = 7002, ///< ������ ������ ����������� �����
  KO_TEST_PINPAD             = 7003, ///< �������� ������� �������
  KO_SHORT_REPORT            = 7004, ///< ������������ �������� ������. ������� ���.
  KO_GET_OPERATION_INFO      = 7006, ///< �������� �������� �� ����������� �����
  KO_GET_TERMINAL_AND_MERCHENT_ID = 7007, ///< �������� ����� ��������� � ��������
  } KernelOperation;
#ifdef __cplusplus
} // extern "C"
#endif //__cplusplus
/**@}*/


#endif //_PUBLIC_SBKERNEL_
