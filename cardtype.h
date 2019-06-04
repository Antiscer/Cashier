/**
 * @file public/cardtype.h
 * @author A. Brik, S. Papazov
 * @date 12 June 2016
 * @brief Card types for public API
 */
#ifndef _PUBLIC_CARD_TYPES_
#define _PUBLIC_CARD_TYPES_

#pragma pack(1)

/**
 * @defgroup CARDS ���� ������������� ����
 * @brief ��� ����� ������������ �� ������� ��� �������� ���������� ��������� ������� ��������� pilot_nt.dll � gate.dll.
 *	 
 * ���� ������� ������� ��� ����� � �������� �������� ���������, ������������� ���������� ::CT_USER. 
 * � ���� ������, � ����������� �� ��������� ���������, ��� ����� ����� ��������� ������������� ��� ������� ����� ���������� 
 * ������� ��� ����� �� ����. ���������� pilot_nt.dll ��� gate.dll ����� ������� ��� ����� ::CT_USER, ���� ������������� ����� 
 * �� ������������ ������������, ��������, � ���������� ��������� ����� ���� ������������� ���������� ���� � ���� ���� ��� 
 * ��������� ��� ���������� ���� � ����������� ����� �����.
 */
/**@{*/
/** ���� ������������� ����.*/
typedef enum {
CT_USER    = 0,  ///< ����� ���� ����� �� ����, ���� �������������
CT_VISA    = 1,  ///< ����� Visa. ����� Visa Electron, ��������� ����� ��������� ����� ���� ������������ ��� Visa.
CT_EUROCARD= 2,  ///< ����� Eurocard/Mastercard
CT_CIRRUS  = 3,  ///< ����� Cirrus/Maestro
CT_AMEX    = 4,  ///< ����� Amex
CT_DINERS  = 5,  ///< ����� DinersCLub
CT_ELECTRON= 6,  ///< ����� VisaElectron.
CT_PRO100  = 7,  ///< ����� PRO100
CT_CASHIER = 8,  ///< ����� �������. ������������ � ������� ������ �������� �����.
CT_SBERCARD= 9,  ///< ����� ��������
CT_MIR     = 10, ///< ����� MIR Credit
CT_MIR_DEBIT = 11,  ///< ����� MIR Debit
CT_UPI_CREDIT= 12, ///< ����� China unionpay Credit
CT_UPI_DEBIT = 13  ///< ����� China unionpay Debit
} CardTypes;

/**@}*/

#pragma pack()

#endif // _PUBLIC_CARD_TYPES_







