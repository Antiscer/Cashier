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

/** Типы операций ядра sb_kernel */
typedef enum {
  KO_INITIALIZATION          =    0, ///< Подготовка библиотеки к работе
  KO_FINALIZATION            =    1, ///< Завершение работы библиотеки
  KO_FUNCTIONS_COUNT         =    2, ///< Получение числа поддерживаемых функций начиная с KO_SB_SALE
  KO_FUNCTIONS_LIST          =    3, ///< Получение списка поддерживаемых функций начиная с KO_SB_SALE
  KO_SIZE_OF_CHEQUE          =    4, ///< Получение размера документа для печати
  KO_GET_LAST_CHEQUE         =    5, ///< Получение образа документа для печати
  KO_GET_DEPARTMENTS_COUNT   =    6, ///< Получение числа настроенных терминалов (валют)
  KO_GET_DEPARTMENTS_LIST    =    7, ///< Получение списка настроенных терминалов (валют)
  KO_SET_USER_INTERFACE      =    8, ///< Настройка пользовательского интерфейса
  KO_SET_USER_NAME           =    9, ///< Установка имени пользователя
  KO_GET_TERMINAL_TECH_MENU  =   10, ///< Вызов меню тех. обслуживания с ожиданием завершения операции
  KO_ENABLE_READER           =   11, ///< Подготовить гибридный считыватель к работе
  KO_DISABLE_READER          =   12, ///< Завершить работу с гибридным считывателем
  KO_TEST_READER             =   13, ///< Проверить наличие карты в гибридном считывателе
  KO_GET_ERROR_TEXT          =   14, ///< Преобразовать кода ошибки в текст
  KO_GET_TERMINAL_ID         =   15, ///< Получить номер терминала (и мерчанта, опционально)
  KO_ABORT_OPERATION         =   16, ///< Прервать операцию аварийно
  KO_CALL_PILOT_COMMAND      =   17, ///< Запуск команды с аргументами sb_pilot
  KO_CALL_STANDALONE_APP     =   18, ///< Запуск как отдельное приложение
  KO_GET_MONITORING_STATE    =   22, ///< Получение состояния ГПЦ банка или сообщения системы мониторинга

  KO_SB_SALE                 = 1000, ///< Оплата СБЕРКАРТ
  KO_SB_CASH                 = 1001, ///< Выдача наличных СБЕРКАРТ
  KO_SB_REFUND               = 1002, ///< Возврат СБЕРКАРТ
  KO_SB_PETROL_FOR_PETROL    = 1003, ///< Оплата ГСМ СБЕРКАРТ
  KO_SB_CHECK_AMOUNT         = 1004, ///< Проверить наличие суммы СБЕРКАРТ
  KO_SB_GET_CARD_HOLDER_INFO = 1005, ///< Прочитать ФИО клиента и номер карты СБЕРКАРТ

  KO_SB_CARD_BALANCE         = 2000, ///< Баланс карты СБЕРКАРТ
  KO_SB_OPERATION_INFO       = 2001, ///< Справка об операциях СБЕРКАРТ
  KO_SB_TRANSFER             = 2002, ///< Перевод средств на ОТОК СБЕРКАРТ
  KO_SB_CHANGE_PIN           = 2003, ///< Изменение ПИНа СБЕРКАРТ
  KO_SB_LOAD                 = 2004, ///< Баланс счета СБЕРКАРТ
  KO_SB_CHECK_ACCOUNT        = 2005, ///< Загрузка средств СБЕРКАРТ
  KO_SB_AUTHORIZATION        = 2006, ///< Авторизация СБЕРКАРТ
  KO_SB_GET_CARD_ID          = 2007, ///< Получить идентификатор карты СБЕРКАРТ
  
  KO_SB_MERCHANT_INFO        = 3000, ///< Справка о карте терминала СБЕРКАРТ
  KO_SB_SETTLMENT            = 3001, ///< Инкассация карты терминала СБЕРКАРТ
  KO_SB_CLOSE_DAY            = 3002, ///< Закрытие дня СБЕРКАРТ

  KO_SALE                    = 4000, ///< Оплата покупки
  KO_CASH                    = 4001, ///< Выдача наличных
  KO_REFUND                  = 4002, ///< Возврат покупки
  KO_CANCEL                  = 4003, ///< Отмена операции
  KO_TRANSFER                = 4004, ///< Безналичный перевод при наличии карты клиента
  KO_TRANSFER_NO_CARD        = 4005, ///< Безналичный перевод при отсутствии карты клиента
  KO_ENTER_AMEX_SLIP         = 4006, ///< Слип по картам American Express
  KO_ENTER_SALE_SLIP         = 4007, ///< Слип оплаты
  KO_ENTER_CASH_SLIP         = 4008, ///< Слип выдачи наличных
  KO_PREAUTH                 = 4009, ///< Предавторизация
  KO_COMPLETE_AUTH           = 4010, ///< Завершение расчета
  KO_CASH_IN_BEGIN           = 4011, ///< Взнос наличных. Предварительное зачисление
  KO_CASH_IN_COMPLETE        = 4012, ///< Взнос наличных. Подтверждение
  KO_CREDIT_PAYMENT          = 4013, ///< Погашение кредита
  KO_CARD_TO_CARD_TRANSFER   = 4014, ///< Перевод с карты на карту
  KO_COMMUNAL_PAYMENT        = 4015, ///< Коммунальные платежи
  KO_AREST                   = 4016, ///< Арест средств по решению суда
  KO_AREST_COMPLETION        = 4017, ///< Списание средств по аресту
  KO_AREST_CANCEL            = 4018, ///< Снятие ареста
  KO_CASH_NO_CARD            = 4019, ///< Выдача наличных по заявлению клиента без карты
  KO_TRANSFER_NO_CLIENT      = 4020, ///< Списание по требованию
  KO_SET_PIN                 = 4021, ///< Установка ПИН-кода
  KO_CHANGE_PIN              = 4022, ///< Смена ПИН-кода
  KO_FUNDS_TRANSFER          = 4023, ///< Списание средств в оплату услуг Банка
  KO_CAPTURE_BIO_DATA        = 4024, ///< Запись отпечатков пальцев
  KO_SET_OFFLINE_LIMIT       = 4025, ///< Установка оффлайн-лимита
  KO_PRINT_HISTORY           = 4026, ///< Распечатка истории карты

  KO_OPEN_CARD_READER        = 4040, ///< Только PCI DSS. Начать ожидание ввода карты.
  KO_TEST_CARD_READ          = 4041, ///< Только PCI DSS. Проверить наличие карты при ожидании ввода карты.
  KO_CLOSE_CARD_READER       = 4042, ///< Только PCI DSS. Закончить ожидание ввода карты.

  KO_CARD_BALANCE            = 5000, ///< Запрос баланса
  KO_UNBLOCK_CARD            = 5001, ///< Разблокировка карт с чипом
  KO_GET_CARD_ID             = 5002, ///< Получить идентификатор карты
  KO_BLOCK_CARD              = 5003, ///< Блокировка карты
  KO_CHECK_PIN               = 5004, ///< Проверка ПИНа
  KO_GET_TOOCH_SIGNATURE     = 5005, ///< Получение подписи клиента на экране терминала
  KO_SHOW_LONG_TEXT          = 5006, ///< Вывод текста на экран терминала/пинпада для подтверждения
  KO_SHOW_LONG_TEXT_INPUT    = 5007, ///< Вывод текста на экран терминала/пинпада для подтверждения с поле ввода текста
  KO_EJECT_CARD              = 5020, ///< Извлечь карту
  KO_CAPTURE_CARD            = 5021, ///< Захватить карту
  KO_TEST_CARD               = 5022, ///< Извлечь карту
 
  KO_TOTALS                  = 6000, ///< Сверка итогов. Итоги дня.
  KO_COMMIT_TRANSACTION      = 6001, ///< Подтверждение операции по карте
  KO_X_TOTALS                = 6002, ///< Х-отчет. Сверка итогов всегда с полным отчетом.
  KO_SUSPEND_TRANSACTION     = 6003, ///< Приостановить операцию (для подтверждения или отмены)
  KO_ROLLBACK_TRANSACTION    = 6004, ///< Аварийная отмена операции
  KO_ADD_MERCHANT_CHEQUE     = 6005, ///< Отправка торгового чека в терминал. Только PCI DSS.
  
  KO_FULL_REPORT             = 7000, ///< Формирование полного отчета. Контрольная лента.
  KO_LAST_CHEQUE             = 7001, ///< Формирование последнего чека.
  KO_SHOW_REPORT_MENU        = 7002, ///< Печать старой контрольной ленты
  KO_TEST_PINPAD             = 7003, ///< Проверка наличия пинпада
  KO_SHORT_REPORT            = 7004, ///< Формирование краткого отчета. Сводный чек.
  KO_GET_OPERATION_INFO      = 7006, ///< Получить операцию из контрольной ленты
  KO_GET_TERMINAL_AND_MERCHENT_ID = 7007, ///< Получить номер терминала и мерчанта
  } KernelOperation;
#ifdef __cplusplus
} // extern "C"
#endif //__cplusplus
/**@}*/


#endif //_PUBLIC_SBKERNEL_
