#ifndef _PARAMSLN_H
#define _PARAMSLN_H

/**
 * @defgroup KERNEL_CONST ���������, ������������ ��� ����������� ��������
 */
 /**@{*/


///���������, ������������ ��� ����������� ��������

#define MAX_TRACK1     79  ///< ������ Track1
#define MAX_TRACK2     40  ///< ������ Track2
#define MAX_TRACK3    107  ///< ������ Track3
#define CARD_TRACK3_LEN   (MAX_TRACK3-3) ///< ����� ������ ������� ������� ����� � ���������� ���������� pilot_nt.dll

#define MAX_AUTH_CODE      6  ///< ����� ���� �����������
#define MAX_AUTHCODE     ( MAX_AUTH_CODE+1) ///< ����� ���� ����������� � ������ ������������ ����
#define MAX_RRN           12  ///< ����� ������ ������/�������������� �����
#define MAX_REFNUM       (MAX_RRN+1) ///< ����� ������ ������ � ������ ������������ ����
#define MAX_TERM           8 ///< ����� ������ ���������
#define MAX_TERMID       ( MAX_TERM+1)  ///< ����� ������ ��������� � ������ ������������ ����
#define MAX_PAN_N         19  ///< ������ PAN
#define MAX_PAN          (MAX_PAN_N+1) ///< ������ PAN � ������ ������������ ����
#define MAX_VALTHRU      ( 5+1)
#define MAX_CURRCODE     ( 3+1)
#define MAX_REPLY        ( 2+1)

#define MAX_MERCHANT     (12+1)
#define MAX_MCCCODE      ( 4+1)
#define MAX_STORENUM     ( 4+1)
#define MAX_TERMNUM      ( 4+1)
#define MAX_SOFTVER      ( 3+1)
#define MAX_TSN          ( 4+1)
#define MAX_OPDATE       ( 6+1)
#define MAX_MESSAGE      (16+1)
#define MAX_EXT_MESSAGE  (24+1)
#define MAX_EXTRA       (249+1)
#define MAX_EXTRA_VISA  (120+1)
#define MAX_CHIPDATA        256
#define MAX_FIO          (40+1)
#define MAX_KEYLEN      (120+1)           /* for GISKE keys */
#define MAX_CVV           (3+1)
#define MAX_USER_NAME_FULL (20+1)
#define MAX_USER_PWD_FULL  (20+1)
#define MAX_ENT_CODE       (24+1)

//���������, ������������ ��� ����������� ��������
#define MAX_CARD_NUM      30  ///< ����� ������ �����
#define MAX_ACCOUNT_NUM   11  ///< ����� ������ ����� (��������)
#define MAX_BRANCH_NUM    11  ///< ����� ���� ����� (��������)
#define MAX_CERT          16  ///< ����� ����������� ����������
#define MAX_NAME          38  ///< ����� �������� ����� (Visa, Maestro, ...)
#define MAX_DATE          10  ///< ����� ������ ���� (��.��.����)
#define MAX_TIME           8  ///< ����� ������� �������� (��:��:��)


#define MAX_MERCHANT_LN   15  ///< ����� ������ ��������
#define MAX_OPER_NAME     64  ///< ����� ����� ������������

#define MAX_HASH          20  ///< ������ ���� ������ �����
#define MAX_CARD_DATE     4   ///< ������ ���� ������� ����� MMYY
#define MAX_PASSPORT      512 ///< ������ ���������� ������
#define MAX_PAYMENT_TAG   32  ///< ��� �������������� ���������� �������� ��� �������� � ����
#define MAX_PAYMENT_VALUE 200 ///< �������� �������������� ���������� �������� ��� �������� � ����
#define MAX_CARD_LS_DATA  800 ///< ������ ������ ������ ��� �������� �������
#define MAX_TEXT_MESSAGE  700 ///< ������ ������������� ���������� ���������
#define MAX_CLIENT_NAME    80 ///< ��� ��������� �����
#define MAX_GOODS_NAME     25 ///< ������������ ����� ����� ������
#define MAX_GOODS_CODE     24 ///< ������������ ����� ���� ������
#define MAX_CASH_STATS     24 ///< ������ ����� ��� �������� ���������� � �������� �������
#define MAX_FIO_N          58 ///< ������ ����� �������
#define CARD_HOLDER_ANSWER_LEN 256 ///< ����� ������ ��������� �������� (v 26)
#define CARD_ID_LEN        25 ///< ����� ������ ������ ����� � ���������� ���������� pilot_nt.dll
#define CARD_HASH_LEN      41 ///< ����� ������ ���� ����� � ���������� ���������� pilot_nt.dll
#define TRANSDATE_LEN      20 ///< ������ ���� � ������� �������� � ���������� ���������� pilot_nt.dll


#define MAX_PAYMENT_ITEM  128      ///< ������������ ������ �������� ���� ��������� �������.
#define MAX_ENCR_DATA  32          ///< ������ ������ ����������� ������ ����� � ����� ��������
#define MAX_AID_ASCII_LEN   (32+1) ///< ������������ ������ ������ AID ������� ����� (� ���� ASCIIZ-������)
#define MAX_CARD_NAME_LEN   (32+1) ///< ������������ ������ �������� ��������� ������� �����

#define MAX_FULL_ERROR_TEXT 128 ///<������������ ������ ������ ������, ������������ UPOS

#define MAX_BARCODE_DATA 2048   ///<������������ ����� ��������� ������ �� ������� ������� !!!��������, ������������ ������ ������ �� ������� 1093 �����

#define MAX_MON_MSG   196      //max psdb message about current host health

/**@}*/

#endif
