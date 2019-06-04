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
 * @defgroup CARDS Типы обслуживаемых карт
 * @brief Тип карты используется во входных или выходных аргументах некоторых функций библиотек pilot_nt.dll и gate.dll.
 *	 
 * Если функция требует тип карты в качестве входного аргумента, рекомендуется передавать ::CT_USER. 
 * В этом случае, в зависимости от настройки терминала, тип карты будет определен автоматически или кассиру будет предложено 
 * выбрать тип карты из меню. Библиотеки pilot_nt.dll или gate.dll могут вернуть тип карты ::CT_USER, если предъявленная карта 
 * не соответсвует перечислению, например, в настройках терминала могут быть дополнительно определены бины и типы карт для 
 * топливных или подарочных карт и предъявлена такая карта.
 */
/**@{*/
/** Типы обслуживаемых карт.*/
typedef enum {
CT_USER    = 0,  ///< Выбор типа карты из меню, либо автоматически
CT_VISA    = 1,  ///< Карта Visa. Карты Visa Electron, считанные через магнитный ридер тоже определяются как Visa.
CT_EUROCARD= 2,  ///< Карта Eurocard/Mastercard
CT_CIRRUS  = 3,  ///< Карта Cirrus/Maestro
CT_AMEX    = 4,  ///< Карта Amex
CT_DINERS  = 5,  ///< Карта DinersCLub
CT_ELECTRON= 6,  ///< Карта VisaElectron.
CT_PRO100  = 7,  ///< Карта PRO100
CT_CASHIER = 8,  ///< Карта кассира. Используется в пунктах выдачи наличных Банка.
CT_SBERCARD= 9,  ///< Карта Сберкарт
CT_MIR     = 10, ///< Карта MIR Credit
CT_MIR_DEBIT = 11,  ///< Карта MIR Debit
CT_UPI_CREDIT= 12, ///< Карта China unionpay Credit
CT_UPI_DEBIT = 13  ///< Карта China unionpay Debit
} CardTypes;

/**@}*/

#pragma pack()

#endif // _PUBLIC_CARD_TYPES_







