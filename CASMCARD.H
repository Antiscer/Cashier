#ifndef _CASMCARD_H
#define _CASMCARD_H

//=================================================================//
//   
//    PC/SC SmartCard Application Programming Interface
//
//=================================================================//

#include <wtypes.h>
#include <winioctl.h>
#include <winsmcrd.h>
#ifndef SCARD_S_SUCCESS
#include "cascerr.h"
//#include <SCardErr.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LPCBYTE_DEFINED
#define _LPCBYTE_DEFINED
typedef const BYTE *LPCBYTE;
#endif
#ifndef _LPCVOID_DEFINED
#define _LPCVOID_DEFINED
typedef const VOID *LPCVOID;
#endif
#ifndef _LPCGUID_DEFINED
#define _LPCGUID_DEFINED
typedef const GUID *LPCGUID;
#endif

#ifndef WINSCARDAPI
#define WINSCARDAPI
#endif
#ifndef WINSCARDDATA
#define WINSCARDDATA __declspec(dllimport)
#endif

WINSCARDDATA extern const SCARD_IO_REQUEST
    g_rgSCardT0Pci,
    g_rgSCardT1Pci,
    g_rgSCardRawPci;
#define SCARD_PCI_T0  (&g_rgSCardT0Pci)
#define SCARD_PCI_T1  (&g_rgSCardT1Pci)
#define SCARD_PCI_RAW (&g_rgSCardRawPci)


//
////////////////////////////////////////////////////////////////////////////////
//
//  Service Manager Access Services
//
//      The following services are used to manage user and terminal contexts for
//      Smart Cards.
//

typedef ULONG SCARDCONTEXT;
typedef SCARDCONTEXT *PSCARDCONTEXT, *LPSCARDCONTEXT;

typedef ULONG SCARDHANDLE;
typedef SCARDHANDLE *PSCARDHANDLE, *LPSCARDHANDLE;

#define SCARD_AUTOALLOCATE (DWORD)(-1)

#define SCARD_SCOPE_USER     0  // The context is a user context, and any
                                // database operations are performed within the
                                // domain of the user.
#define SCARD_SCOPE_TERMINAL 1  // The context is that of the current terminal,
                                // and any database operations are performed
                                // within the domain of that terminal.  (The
                                // calling application must have appropriate
                                // access permissions for any database actions.)
#define SCARD_SCOPE_SYSTEM    2 // The context is the system context, and any
                                // database operations are performed within the
                                // domain of the system.  (The calling
                                // application must have appropriate access
                                // permissions for any database actions.)

extern WINSCARDAPI LONG WINAPI
CasEstablishContext(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext);

extern WINSCARDAPI LONG WINAPI
CasReleaseContext(
    IN      SCARDCONTEXT hContext);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Smart Card Database Management Services
//
//      The following services provide for managing the Smart Card Database.
//

static const TCHAR
    SCARD_ALL_READERS0[]     = TEXT("SCard$AllReaders\000"),
    SCARD_DEFAULT_READERS0[] = TEXT("SCard$DefaultReaders\000"),
    SCARD_LOCAL_READERS0[]   = TEXT("SCard$LocalReaders\000"),
    SCARD_SYSTEM_READERS0[]  = TEXT("SCard$SystemReaders\000");

//
// Database Reader routines
//

extern WINSCARDAPI LONG WINAPI
CasListReaderGroupsA(
    IN      SCARDCONTEXT hContext,
    OUT     LPSTR mszGroups,
    IN OUT  LPDWORD pcchGroups);
extern WINSCARDAPI LONG WINAPI
CasListReaderGroupsW(
    IN      SCARDCONTEXT hContext,
    OUT     LPWSTR mszGroups,
    IN OUT  LPDWORD pcchGroups);
#ifdef UNICODE
#define CasListReaderGroups  CasListReaderGroupsW
#else
#define CasListReaderGroups  CasListReaderGroupsA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasListReadersA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR mszGroups,
    OUT     LPSTR mszReaders,
    IN OUT  LPDWORD pcchReaders);
extern WINSCARDAPI LONG WINAPI
CasListReadersW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszGroups,
    OUT     LPWSTR mszReaders,
    IN OUT  LPDWORD pcchReaders);
#ifdef UNICODE
#define CasListReaders  CasListReadersW
#else
#define CasListReaders  CasListReadersA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasListCardsA(
    IN      SCARDCONTEXT hContext,
    IN      LPCBYTE pbAtr,
    IN      LPCGUID rgquidInterfaces,
    IN      DWORD cguidInterfaceCount,
    OUT     LPSTR mszCards,
    IN OUT  LPDWORD pcchCards);
extern WINSCARDAPI LONG WINAPI
CasListCardsW(
    IN      SCARDCONTEXT hContext,
    IN      LPCBYTE pbAtr,
    IN      LPCGUID rgquidInterfaces,
    IN      DWORD cguidInterfaceCount,
    OUT     LPWSTR mszCards,
    IN OUT  LPDWORD pcchCards);
#ifdef UNICODE
#define CasListCards  CasListCardsW
#else
#define CasListCards  CasListCardsA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasListInterfacesA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szCard,
    OUT     LPGUID pguidInterfaces,
    IN OUT  LPDWORD pcguidInterfaces);
extern WINSCARDAPI LONG WINAPI
CasListInterfacesW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szCard,
    OUT     LPGUID pguidInterfaces,
    IN OUT  LPDWORD pcguidInterfaces);
#ifdef UNICODE
#define CasListInterfaces  CasListInterfacesW
#else
#define CasListInterfaces  CasListInterfacesA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasGetProviderIdA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szCard,
    OUT     LPGUID pguidProviderId);
extern WINSCARDAPI LONG WINAPI
CasGetProviderIdW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szCard,
    OUT     LPGUID pguidProviderId);
#ifdef UNICODE
#define CasGetProviderId  CasGetProviderIdW
#else
#define CasGetProviderId  CasGetProviderIdA
#endif // !UNICODE


//
// Database Writer routines
//

extern WINSCARDAPI LONG WINAPI
CasIntroduceReaderGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
CasIntroduceReaderGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define CasIntroduceReaderGroup  CasIntroduceReaderGroupW
#else
#define CasIntroduceReaderGroup  CasIntroduceReaderGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasForgetReaderGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
CasForgetReaderGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define CasForgetReaderGroup  CasForgetReaderGroupW
#else
#define CasForgetReaderGroup  CasForgetReaderGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasIntroduceReaderA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szDeviceName);
extern WINSCARDAPI LONG WINAPI
CasIntroduceReaderW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szDeviceName);
#ifdef UNICODE
#define CasIntroduceReader  CasIntroduceReaderW
#else
#define CasIntroduceReader  CasIntroduceReaderA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasForgetReaderA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName);
extern WINSCARDAPI LONG WINAPI
CasForgetReaderW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName);
#ifdef UNICODE
#define CasForgetReader  CasForgetReaderW
#else
#define CasForgetReader  CasForgetReaderA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasAddReaderToGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
CasAddReaderToGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define CasAddReaderToGroup  CasAddReaderToGroupW
#else
#define CasAddReaderToGroup  CasAddReaderToGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasRemoveReaderFromGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
CasRemoveReaderFromGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define CasRemoveReaderFromGroup  CasRemoveReaderFromGroupW
#else
#define CasRemoveReaderFromGroup  CasRemoveReaderFromGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasIntroduceCardTypeA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName,
    IN LPGUID pguidPrimaryProvider,
    IN LPGUID rgguidInterfaces,
    IN DWORD dwInterfaceCount,
    IN LPCBYTE pbAtr,
    IN LPCBYTE pbAtrMask,
    IN DWORD cbAtrLen);
extern WINSCARDAPI LONG WINAPI
CasIntroduceCardTypeW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName,
    IN LPGUID pguidPrimaryProvider,
    IN LPGUID rgguidInterfaces,
    IN DWORD dwInterfaceCount,
    IN LPCBYTE pbAtr,
    IN LPCBYTE pbAtrMask,
    IN DWORD cbAtrLen);
#ifdef UNICODE
#define CasIntroduceCardType  CasIntroduceCardTypeW
#else
#define CasIntroduceCardType  CasIntroduceCardTypeA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasForgetCardTypeA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName);
extern WINSCARDAPI LONG WINAPI
CasForgetCardTypeW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName);
#ifdef UNICODE
#define CasForgetCardType  CasForgetCardTypeW
#else
#define CasForgetCardType  CasForgetCardTypeA
#endif // !UNICODE


//
////////////////////////////////////////////////////////////////////////////////
//
//  Service Manager Support Routines
//
//      The following services are supplied to simplify the use of the Service
//      Manager API.
//

extern WINSCARDAPI LONG WINAPI
CasFreeMemory(
    IN SCARDCONTEXT hContext,
    IN LPVOID pvMem);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Reader Services
//
//      The following services supply means for tracking cards within readers.
//

#ifndef SCARD_READERSTATE_STRUCTURE
#define SCARD_READERSTATE_STRUCTURE

typedef struct {
    LPCSTR      szReader;       // reader name
    LPVOID      pvUserData;     // user defined data
    DWORD       dwCurrentState; // current state of reader at time of call
    DWORD       dwEventState;   // state of reader after state change
    DWORD       cbAtr;          // Number of bytes in the returned ATR.
    BYTE        rgbAtr[36];     // Atr of inserted card, (extra alignment bytes)
} SCARD_READERSTATE_A;
typedef SCARD_READERSTATE_A *PSCARD_READERSTATE_A, *LPSCARD_READERSTATE_A;
typedef struct {
    LPCWSTR     szReader;       // reader name
    LPVOID      pvUserData;     // user defined data
    DWORD       dwCurrentState; // current state of reader at time of call
    DWORD       dwEventState;   // state of reader after state change
    DWORD       cbAtr;          // Number of bytes in the returned ATR.
    BYTE        rgbAtr[36];     // Atr of inserted card, (extra alignment bytes)
} SCARD_READERSTATE_W;
typedef SCARD_READERSTATE_W *PSCARD_READERSTATE_W, *LPSCARD_READERSTATE_W;
#ifdef UNICODE
#define SCARD_READERSTATE   SCARD_READERSTATE_W
#define PSCARD_READERSTATE  PSCARD_READERSTATE_W
#define LPSCARD_READERSTATE LPSCARD_READERSTATE_W
#else
#define SCARD_READERSTATE  SCARD_READERSTATE_A
#define PSCARD_READERSTATE  PSCARD_READERSTATE_A
#define LPSCARD_READERSTATE LPSCARD_READERSTATE_A
#endif // !UNICODE

#endif 

#define SCARD_STATE_UNAWARE     0x00000000  // The application is unaware of the
                                            // current state, and would like to
                                            // know.  The use of this value
                                            // results in an immediate return
                                            // from state transition monitoring
                                            // services.  This is represented by
                                            // all bits set to zero.
#define SCARD_STATE_IGNORE      0x00000001  // The application requested that
                                            // this reader be ignored.  No other
                                            // bits will be set.
#define SCARD_STATE_CHANGED     0x00000002  // This implies that there is a
                                            // difference between the state
                                            // believed by the application, and
                                            // the state known by the Service
                                            // Manager.  When this bit is set,
                                            // the application may assume a
                                            // significant state change has
                                            // occurred on this reader.
#define SCARD_STATE_UNKNOWN     0x00000004  // This implies that the given
                                            // reader name is not recognized by
                                            // the Service Manager.  If this bit
                                            // is set, then SCARD_STATE_CHANGED
                                            // and SCARD_STATE_IGNORE will also
                                            // be set.
#define SCARD_STATE_UNAVAILABLE 0x00000008  // This implies that the actual
                                            // state of this reader is not
                                            // available.  If this bit is set,
                                            // then all the following bits are
                                            // clear.
#define SCARD_STATE_EMPTY       0x00000010  // This implies that there is not
                                            // card in the reader.  If this bit
                                            // is set, all the following bits
                                            // will be clear.
#define SCARD_STATE_PRESENT     0x00000020  // This implies that there is a card
                                            // in the reader.
#define SCARD_STATE_ATRMATCH    0x00000040  // This implies that there is a card
                                            // in the reader with an ATR
                                            // matching one of the target cards.
                                            // If this bit is set,
                                            // SCARD_STATE_PRESENT will also be
                                            // set.  This bit is only returned
                                            // on the SCardLocateCard() service.
#define SCARD_STATE_EXCLUSIVE   0x00000080  // This implies that the card in the
                                            // reader is allocated for exclusive
                                            // use by another application.  If
                                            // this bit is set,
                                            // SCARD_STATE_PRESENT will also be
                                            // set.
#define SCARD_STATE_INUSE       0x00000100  // This implies that the card in the
                                            // reader is in use by one or more
                                            // other applications, but may be
                                            // connected to in shared mode.  If
                                            // this bit is set,
                                            // SCARD_STATE_PRESENT will also be
                                            // set.
#define SCARD_STATE_MUTE        0x00000200  // This implies that the card in the
                                            // reader is unresponsive or not
                                            // supported by the reader or
                                            // software.
#define SCARD_STATE_UNPOWERED   0x00000400  // This implies that the card in the
                                            // reader has not been powered up.

extern WINSCARDAPI LONG WINAPI
CasLocateCardsA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR mszCards,
    IN OUT  LPSCARD_READERSTATE_A rgReaderStates,
    IN      DWORD cReaders);
extern WINSCARDAPI LONG WINAPI
CasLocateCardsW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszCards,
    IN OUT  LPSCARD_READERSTATE_W rgReaderStates,
    IN      DWORD cReaders);
#ifdef UNICODE
#define CasLocateCards  CasLocateCardsW
#else
#define CasLocateCards  CasLocateCardsA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasGetStatusChangeA(
    IN      SCARDCONTEXT hContext,
    IN      DWORD dwTimeout,
    IN OUT  LPSCARD_READERSTATE_A rgReaderStates,
    IN      DWORD cReaders);
extern WINSCARDAPI LONG WINAPI
CasGetStatusChangeW(
    IN      SCARDCONTEXT hContext,
    IN      DWORD dwTimeout,
    IN OUT  LPSCARD_READERSTATE_W rgReaderStates,
    IN      DWORD cReaders);
#ifdef UNICODE
#define CasGetStatusChange  CasGetStatusChangeW
#else
#define CasGetStatusChange  CasGetStatusChangeA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasCancel(
    IN      SCARDCONTEXT hContext);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Card/Reader Access Services
//
//      The following services provide means for establishing communication with
//      the card.
//

#define SCARD_SHARE_EXCLUSIVE 1 // This application is not willing to share this
                                // card with other applications.
#define SCARD_SHARE_SHARED    2 // This application is willing to share this
                                // card with other applications.
#define SCARD_SHARE_DIRECT    3 // This application demands direct control of
                                // the reader, so it is not available to other
                                // applications.

#define SCARD_LEAVE_CARD      0 // Don't do anything special on close
#define SCARD_RESET_CARD      1 // Reset the card on close
#define SCARD_UNPOWER_CARD    2 // Power down the card on close
#define SCARD_EJECT_CARD      3 // Eject the card on close

extern WINSCARDAPI LONG WINAPI
CasConnectA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol);
extern WINSCARDAPI LONG WINAPI
CasConnectW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol);
#ifdef UNICODE
#define CasConnect  CasConnectW
#else
#define CasConnect  CasConnectA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
CasReconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    IN      DWORD dwInitialization,
    OUT     LPDWORD pdwActiveProtocol);

extern WINSCARDAPI LONG WINAPI
CasDisconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition);

extern WINSCARDAPI LONG WINAPI
CasBeginTransaction(
    IN      SCARDHANDLE hCard);

extern WINSCARDAPI LONG WINAPI
CasEndTransaction(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition);

extern WINSCARDAPI LONG WINAPI
CasState(
    IN SCARDHANDLE hCard,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen);

extern WINSCARDAPI LONG WINAPI
CasStatusA(
    IN SCARDHANDLE hCard,
    OUT LPSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen);
extern WINSCARDAPI LONG WINAPI
CasStatusW(
    IN SCARDHANDLE hCard,
    OUT LPWSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    OUT LPDWORD pcbAtrLen);
#ifdef UNICODE
#define CasStatus  CasStatusW
#else
#define CasStatus  CasStatusA
#endif // !UNICODE


//
////////////////////////////////////////////////////////////////////////////////
//
//  I/O Services
//
//      The following services provide access to the I/O capabilities of the
//      reader drivers.  Services of the Smart Card are requested by placing the
//      following structure into the protocol buffer:
//

extern WINSCARDAPI LONG WINAPI
CasTransmit(
    IN SCARDHANDLE hCard,
    IN LPCSCARD_IO_REQUEST pioSendPci,
    IN LPCBYTE pbSendBuffer,
    IN DWORD cbSendLength,
    IN OUT LPSCARD_IO_REQUEST pioRecvPci,
    OUT LPBYTE pbRecvBuffer,
    IN OUT LPDWORD pcbRecvLength);

////////////////////////////////////////////////////////////////////////////////
//
//  Reader Control Routines
//
//      The following services provide for direct, low-level manipulation of the
//      reader by the calling application allowing it control over the
//      attributes of the communications with the card.
//

extern WINSCARDAPI LONG WINAPI
CasControl(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwControlCode,
    IN      LPCVOID lpInBuffer,
    IN      DWORD nInBufferSize,
    OUT     LPVOID lpOutBuffer,
    IN      DWORD nOutBufferSize,
    OUT     LPDWORD lpBytesReturned);

extern WINSCARDAPI LONG WINAPI
CasGetAttrib(
    IN SCARDHANDLE hCard,
    IN DWORD dwAttrId,
    OUT LPBYTE pbAttr,
    IN OUT LPDWORD pcbAttrLen);

extern WINSCARDAPI LONG WINAPI
CasSetAttrib(
    IN SCARDHANDLE hCard,
    IN DWORD dwAttrId,
    IN LPCBYTE pbAttr,
    IN DWORD cbAttrLen);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Smart Card Dialog definitions
//
//      The following section contains structures and  exported function
//      declarations for the Smart Card Common Dialog dialog.
//

// Defined constants
// Flags
#define SC_DLG_MINIMAL_UI       0x01
#define SC_DLG_NO_UI            0x02
#define SC_DLG_FORCE_UI         0x04

#define SCERR_NOCARDNAME        0x4000
#define SCERR_NOGUIDS           0x8000

typedef SCARDHANDLE (WINAPI *LPOCNCONNPROCA) (IN SCARDCONTEXT, IN LPSTR, IN LPSTR, IN PVOID);
typedef SCARDHANDLE (WINAPI *LPOCNCONNPROCW) (IN SCARDCONTEXT, IN LPWSTR, IN LPWSTR, IN PVOID);

#ifdef UNICODE
#define LPOCNCONNPROC   LPOCNCONNPROCW
#else
#define LPOCNCONNPROC   LPOCNCONNPROCA
#endif

typedef BOOL (WINAPI *LPOCNCHKPROC) (IN SCARDCONTEXT, IN SCARDHANDLE, IN PVOID);
typedef void (WINAPI *LPOCNDSCPROC) (IN SCARDCONTEXT, IN SCARDHANDLE, IN PVOID);

#ifndef TAGOCNA_STRUCTURE
#define TAGOCNA_STRUCTURE
typedef struct tagOCNA {
    DWORD           dwStructSize;
    HWND            hwndOwner;
    SCARDCONTEXT    hSCardContext;
    LPSTR           lpstrGroupNames;
    DWORD           nMaxGroupNames;
    LPSTR           lpstrCardNames;
    DWORD           nMaxCardNames;
    LPGUID          rgguidInterfaces;
    DWORD           cguidInterfaces;
    LPSTR           lpstrRdr;
    DWORD           nMaxRdr;
    LPSTR           lpstrCard;
    DWORD           nMaxCard;
    LPCSTR          lpstrTitle;
    DWORD           dwFlags;
    LPVOID          pvUserData;
    DWORD           dwShareMode;
    DWORD           dwPreferredProtocols;
    DWORD           dwActiveProtocol;
    LPOCNCONNPROCA  lpfnConnect;
    LPOCNCHKPROC    lpfnCheck;
    LPOCNDSCPROC    lpfnDisconnect;
    SCARDHANDLE     hCardHandle;
} OPENCARDNAMEA, *LPOPENCARDNAMEA;
typedef struct tagOCNW {
    DWORD           dwStructSize;
    HWND            hwndOwner;
    SCARDCONTEXT    hSCardContext;
    LPWSTR          lpstrGroupNames;
    DWORD           nMaxGroupNames;
    LPWSTR          lpstrCardNames;
    DWORD           nMaxCardNames;
    LPGUID          rgguidInterfaces;
    DWORD           cguidInterfaces;
    LPWSTR          lpstrRdr;
    DWORD           nMaxRdr;
    LPWSTR          lpstrCard;
    DWORD           nMaxCard;
    LPCWSTR         lpstrTitle;
    DWORD           dwFlags;
    LPVOID          pvUserData;
    DWORD           dwShareMode;
    DWORD           dwPreferredProtocols;
    DWORD           dwActiveProtocol;
    LPOCNCONNPROCW  lpfnConnect;
    LPOCNCHKPROC    lpfnCheck;
    LPOCNDSCPROC    lpfnDisconnect;
    SCARDHANDLE     hCardHandle;
} OPENCARDNAMEW, *LPOPENCARDNAMEW;

#endif

#ifdef UNICODE
#define OPENCARDNAME OPENCARDNAMEW
#define LPOPENCARDNAME LPOPENCARDNAMEW
#else
#define OPENCARDNAME OPENCARDNAMEA
#define LPOPENCARDNAME LPOPENCARDNAMEA
#endif // UNICODE

extern WINSCARDAPI LONG WINAPI
GetOpenCardNameA(
    LPOPENCARDNAMEA);
extern WINSCARDAPI LONG WINAPI
GetOpenCardNameW(
    LPOPENCARDNAMEW);

#ifdef UNICODE
#define GetOpenCardName  GetOpenCardNameW
#else
#define GetOpenCardName  GetOpenCardNameA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardDlgExtendedError (void);


// Only For Castles Serial EZ-Reader
extern WINSCARDAPI LONG WINAPI
CasSetBaudRate(
    IN SCARDHANDLE hCard,
	IN BYTE BaudRate);

/* These are only for Castles Serial EZ-Reader */
#define CAS_SET_BAUD_RATE_38400 1
#define CAS_SET_BAUD_RATE_19200 2
#define CAS_SET_BAUD_RATE_9600 3
#define CAS_SET_BAUD_RATE_4800 4
#define CAS_SET_BAUD_RATE_2400 5
#define CAS_SET_BAUD_RATE_1200 6

///////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
CasGetRdrType(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN OUT LPDWORD pcbRecvLength);

//=================================================================//
//   
//    Memory Card Application Programming Interface
//
//=================================================================//

#ifndef _SCARD_M_STATUS
#define _SCARD_M_STATUS
#define SCARD_M_SUCCESS  NO_ERROR
#define SCARD_M_CARD_ABSENT     ((DWORD)0x80100078L)
#define SCARD_M_NO_RESPONSE     ((DWORD)0x80100079L)
#define SCARD_M_POWER_FAIL      ((DWORD)0x8010007AL)
#define SCARD_M_COMM_ERROR      ((DWORD)0x8010007BL)
#define SCARD_M_VERIFY_FAIL     ((DWORD)0x8010007CL)
#define SCARD_M_TYPE_ERROR      ((DWORD)0x8010007DL)
#define SCARD_M_COMMAND_ERROR   ((DWORD)0x8010007EL)
#define SCARD_M_COUNTER_EMPTY   ((DWORD)0x8010007FL)
#define SCARD_M_CARD_LOCKED     ((DWORD)0x80100080L)
#define SCARD_M_WRITE_ERROR     ((DWORD)0x80100081L)
#define SCARD_M_CHECK_ERROR     ((DWORD)0x80100082L)
#define SCARD_M_UNSUPPORTED     ((DWORD)0x80100083L)
#define SCARD_M_OTHER_FAIL      ((DWORD)0x80100084L)
#endif

/* Indicate the current protocol is SCARD_SYNCHRONIZE;
   that is, the card in the reader is a memory card.  
*/  
#define SCARD_PROTOCOL_SYNC 0x00000010

#ifndef MM_ADDRESS
#define MM_ADDRESS LONG
#endif
////////////////////////////////////////////////////////////////////////
//  SLE4442 Memory Card  API
////////////////////////////////////////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
SLE4442_Read_Main_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4442_Update_Main_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4442_Update_Main_MemoryA(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4442_Read_Protection_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer);

extern WINSCARDAPI LONG WINAPI
SLE4442_Write_Protection_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
	IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4442_Read_Security_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer);
//
extern WINSCARDAPI LONG WINAPI
SLE4442_Update_Security_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
	IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4442_Compare_Verification_Data(
    IN SCARDHANDLE hCard,
	IN BYTE PSC1,
	IN BYTE PSC2,
	IN BYTE PSC3);

////////////////////////////////////////////////////////////////////////
//  SLE4432 Memory Card  API
////////////////////////////////////////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
SLE4432_Read_Main_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4432_Update_Main_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4432_Update_Main_MemoryA(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4432_Read_Protection_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer);

extern WINSCARDAPI LONG WINAPI
SLE4432_Write_Protection_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
	IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);




////////////////////////////////////////////////////////////////////////
//  SLE4418 Memory Card  API
////////////////////////////////////////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
SLE4418_Write_Data_With_Protect_Bit(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4418_Write_Data_With_Protect_BitA(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength); 
   
extern WINSCARDAPI LONG WINAPI
SLE4418_Write_Data_Without_Protect_Bit(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4418_Write_Data_Without_Protect_BitA(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4418_Write_Protect_Bit(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
	IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4418_Read_Data_With_Protect_Bit(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	OUT LPBYTE pbProtectBuffer,
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4418_Read_Data_Without_Protect_Bit(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,	
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);


////////////////////////////////////////////////////////////////////////
//  SLE4428 Memory Card  API
////////////////////////////////////////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
SLE4428_Write_Data_With_Protect_Bit(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength); 

extern WINSCARDAPI LONG WINAPI
SLE4428_Write_Data_With_Protect_BitA(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);
   
extern WINSCARDAPI LONG WINAPI
SLE4428_Write_Data_Without_Protect_Bit(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4428_Write_Data_Without_Protect_BitA(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4428_Write_Protect_Bit(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
	IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4428_Read_Data_With_Protect_Bit(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	OUT LPBYTE pbProtectBuffer,
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4428_Read_Data_Without_Protect_Bit(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,	
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4428_PSC_Verification(
    IN SCARDHANDLE hCard,
	IN BYTE PSC1,
	IN BYTE PSC2);

////////////////////////////////////////////////////////////////////////
//  SLE4404 Memory Card  API
////////////////////////////////////////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
SLE4404_Read_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4404_Read_User_Memory(
    IN SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN MM_ADDRESS aStartAddr,
	IN OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI				
SLE4404_Compare_User_Code(
    IN SCARDHANDLE hCard,
	IN BYTE Code1,
	IN BYTE Code2);

extern WINSCARDAPI LONG WINAPI				
SLE4404_Compare_Memory_Code(
    IN SCARDHANDLE hCard,
	IN BYTE Code1,
	IN BYTE Code2,
	IN BYTE Code3,
	IN BYTE Code4);

extern WINSCARDAPI LONG WINAPI 
SLE4404_Erase_Memory(
    IN SCARDHANDLE hCard,	
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4404_Write_Memory(
    IN SCARDHANDLE hCard,
 	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4404_Write_MemoryA(
    IN SCARDHANDLE hCard,
 	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4404_Write_User_Memory(
    IN SCARDHANDLE hCard,
 	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4404_Write_User_MemoryA(
    IN SCARDHANDLE hCard,
 	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4404_Enter_Test_Mode(
	IN SCARDHANDLE hCard);

extern WINSCARDAPI LONG WINAPI
SLE4404_Exit_Test_Mode(
	IN SCARDHANDLE hCard);

extern WINSCARDAPI LONG WINAPI
SLE4404_Blow_Fuse(
    IN SCARDHANDLE hCard);

////////////////////////////////////////////////////////////////////////
//  SLE4406 Memory Card API
////////////////////////////////////////////////////////////////////////
extern WINSCARDAPI LONG WINAPI
SLE4406_Read_Memory(
    IN  SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN  MM_ADDRESS aStartAddr,
	IN  OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4406_Read_Counter_Stages(
    IN  SCARDHANDLE hCard,
	OUT BYTE Stage[5]);

extern WINSCARDAPI LONG WINAPI
SLE4406_Write_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4406_Write_Counter_Stage(
    IN SCARDHANDLE hCard,	
	IN BYTE Stage,
	IN BYTE Data);

extern WINSCARDAPI LONG WINAPI
SLE4406_Reload(
    IN SCARDHANDLE hCard,
    IN BYTE BitAddr);


////////////////////////////////////////////////////////////////////////
//  SLE4436 Memory Card API
////////////////////////////////////////////////////////////////////////
extern WINSCARDAPI LONG WINAPI
SLE4436_Read_Memory(
    IN  SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN  MM_ADDRESS aStartAddr,
	IN  OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE4436_Read_Counter_Stages(
    IN  SCARDHANDLE hCard,
	OUT BYTE Stage[5]);

extern WINSCARDAPI LONG WINAPI
SLE4436_Write_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE4436_Write_Counter_Stage(
    IN SCARDHANDLE hCard,	
	IN BYTE Stage,
	IN BYTE Data);

extern WINSCARDAPI LONG WINAPI
SLE4436_Reload(
    IN SCARDHANDLE hCard,
    IN BYTE BitAddr);

extern WINSCARDAPI LONG WINAPI
SLE4436_Verify_Transport_Code(
    IN SCARDHANDLE hCard,
	IN BYTE Code1,
	IN BYTE Code2,
	IN BYTE Code3);

extern WINSCARDAPI LONG WINAPI
SLE4436_Authentication(
    IN SCARDHANDLE hCard,
	OUT BYTE ReturnCode[2],	
	IN BYTE Key,
	IN BYTE ClockPulse,
	IN BYTE Code[6]);

////////////////////////////////////////////////////////////////////////
//  SLE5536 Memory Card API
////////////////////////////////////////////////////////////////////////
extern WINSCARDAPI LONG WINAPI
SLE5536_Read_Memory(
    IN  SCARDHANDLE hCard,
	OUT LPBYTE pbRecvBuffer,
	IN  MM_ADDRESS aStartAddr,
	IN  OUT LPDWORD pcbRecvLength);

extern WINSCARDAPI LONG WINAPI
SLE5536_Read_Counter_Stages(
    IN  SCARDHANDLE hCard,
	OUT BYTE Stage[5]);

extern WINSCARDAPI LONG WINAPI
SLE5536_Write_Memory(
    IN SCARDHANDLE hCard,
	IN LPBYTE pbTransmitBuffer,
    IN MM_ADDRESS aStartAddr,
	IN DWORD TransmitLength);

extern WINSCARDAPI LONG WINAPI
SLE5536_Write_Counter_Stage(
    IN SCARDHANDLE hCard,	
	IN BYTE Stage,
	IN BYTE Data);

extern WINSCARDAPI LONG WINAPI
SLE5536_Reload(
    IN SCARDHANDLE hCard,
    IN BYTE BitAddr);

extern WINSCARDAPI LONG WINAPI
SLE5536_Verify_Transport_Code(
    IN SCARDHANDLE hCard,
	IN BYTE Code1,
	IN BYTE Code2,
	IN BYTE Code3);

extern WINSCARDAPI LONG WINAPI
SLE5536_Authentication(
    IN SCARDHANDLE hCard,
	OUT BYTE ReturnCode[2],	
	IN BYTE Key,
	IN BYTE ClockPulse,
	IN BYTE Code[6]);

////////////////////////////////////////////////////////////////////////
//  I2C Memory Card  API
////////////////////////////////////////////////////////////////////////

extern WINSCARDAPI LONG WINAPI
I2C_Read_Memory(
    IN SCARDHANDLE hCard,    
	IN BYTE ReadCommand, 
	IN MM_ADDRESS aStartAddr,
	OUT LPBYTE pbRecvBuffer,
	IN OUT LPDWORD pcbRecvLength,
	IN BYTE Dummy,
	IN BYTE AddrByteNum);

extern WINSCARDAPI LONG WINAPI
I2C_Write_Memory(
    IN SCARDHANDLE hCard,    
	IN BYTE WriteCommand, 
	IN MM_ADDRESS aStartAddr,
	OUT LPBYTE pbTransmitBuffer,
	IN DWORD TransmitLength,
	IN BYTE AddrByteNum);

extern WINSCARDAPI LONG WINAPI
I2C_SetMode(
    IN SCARDHANDLE hCard);

extern WINSCARDAPI LONG WINAPI
I2C_ClearMode(
    IN SCARDHANDLE hCard);

#ifdef __cplusplus
}
#endif



#endif
