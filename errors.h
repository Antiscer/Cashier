#ifndef _INCLUDE_ERRORS_
#define _INCLUDE_ERRORS_

/**
 * @defgroup ERROR_CODES Коды ошибок
 */
 /**@{*/
 
#define ERR_OK                  0 /**< Успешно                                                                */
#define SPS_OK             ERR_OK /**< Успешно                                                                */
#define SPS_WAITING             1 /**< Задача поставлена в очередь                                            */
#define SPS_PROCESSING          3 /**< Задача в процессе выполнения                                           */
#define SPS_BADLRC              4 /**< На терминал передана команда с неверной контрольной суммой             */
#define SPS_SUPPRESS_REPLY      5 /**< Подождать с ответом. Скорее всего терминал выполняет перезагрузку      */

#define SPS_WRONGCMD           12 /**< На терминал передана не корректная команда                             */
#define SPS_WRONGPAR           13 /**< На терминал передана команда не содержащая обязательные параметры      */
#define SPS_UEPSERR            14 /**< Ответ терминала содержит код ошибки                                    */
#define SPS_BAD_REQUEST        15 /**< Задача с указанным номером не найдена в очереди задач                  */

#define SPS_NOCARD             21 /**< Ответ на команду CMD_CARD_TEST при отсутствии карты                    */
#define SPS_BADFRAME           23 /**< На терминал передана команда не полная команда                         */
#define SPS_READFAIL           34 /**< Ошибка чтения магнитной полосы карты                                   */
#define SPS_CMFAIL             35 /**< Ошибка чтения ключей пинпада                                           */
#define SPS_PINERR             36 /**< Ошибка ввод пин-кода                                                   */
#define SPS_MACERR             37 /**< Ошибка создания MAC-ключа                                              */
#define SPS_INCOMPL            38 /**< Локальный протокол обмена запрашивает следующий блок данных            */
//#define SPS_ACK                39
#define SPS_NULLPOINTER        40 /**< Передан нулевой указатель */
#define SPS_FILE_NOT_FOUND     41 /**< Файл tlv для передачи на терминал не найден.*/
#define SPS_DATA_NOT_TRANSF    42 /**< Данные на терминал не доставлены [в полном объеме]*/
#define SPS_CTLS_NOT_SUPP      43 /**< Пинпад не поддерживает бесконтактный режим*/

#define SPS_NEED_EDITOR        96 /**< Для выполнения операции требуется запуск утилиты "Редактор Параметров" */
//#define SPS_NEED_LOADER        97
#define SPS_NEED_RESTART       98 /**< Для выполнения операции требуется перезапуск кассы                     */
#define SPS_NOLINK             99 /**< Нет связи с пинпадом или терминалом                                    */

#define ERR_NOT_SUPPORTED            101 /**< Операция не поддерживается на текущем решении                   */
//#define ERR_NO_MAC_KEY_LOADED        102
//#define ERR_PP_CANCELLED             103
//#define ERR_NO_PIN_KEY_LOADED        104
//#define ERR_PP_NO_CARD               105
//#define ERR_PP_WRONG_CARD            106
//#define ERR_PP_PROTOCOL_ERROR        107
//#define ERR_PP_OPER_NOT_ALLOWED      108
//#define ERR_PP_MAGN_READER_BUSY      109
//#define ERR_PP_MAGN_READER_ERROR     110

//#define SPS_HASH                     111

#define ERR_SUPPRESSED               112 /**< Требуется подавить ответ на комманду                                   */

#define ERR_LIBRARY_LOAD             113 /**< Ошибка загрузки динамической библиотеки                                */
#define ERR_FUNCTION_NOT_FOUND       114 /**< Указанная функция не найдена в динамической библиотеке                 */
#define ERR_LIBRARY_BUSY             115 /**< Библиотека занята другим процессом, требуется подождать его завершения */
#define ERR_CTX_GET                  116 /**< Ошибка чтения параметра */
#define ERR_CTX_SET                  117 /**< Ошибка установки параметра */

#define ERR_RFU_A0                   160 /**< Зарезервированно и не должно использоваться. CMD_MASTERCALL */
#define ERR_RFU_C0                   192 /**< Зарезервированно и не должно использоваться. CMD_RUNSCREEN */

#define ERR_MCL_ERROR                230 /**< Ошибка чтения карты. TODO: Check error text.*/
//#define ERR_PP_WRONG_APDU            231
//#define ERR_PP_ABORTED               232
//#define ERR_PP_BAD_FRAME             234

#define ERR_ABORT_STUPID             238 /**< Отправка команды отменена. TODO: Check error text.*/
#define DRV_NO_MEMORY                248 /**< Не хватает динамической памяти */
#define DRV_WRONGPAR                 249 /**< Внутренняя ошибка: На терминал передана команда не содержащая обязательные параметры */
#define DRV_UCANCEL                  250 /**< Внутренняя ошибка: Операция отменена                                   */
#define DRV_BADMEDIA                 251 /**< Внутренняя ошибка: Ошибка записи данных на диск                        */
#define DRV_NOTSUPP                  252 /**< Внутренняя ошибка: Операция не поддерживается                          */
#define DRV_FAILURE                  253 /**< Внутренняя ошибка: Что-то пошло не так...                              */
#define DRV_TIMEOUT                  254 /**< Внутренняя ошибка: Истекло время ожидания                              */


/**************************************************/
/* range 260...3000 - reserved for BGS errors !!! */
/**************************************************/

/* Do not localize - range of 2000...4000 - are codes returned from server */
#define ERR_CANCEL                2000 /**< Операция отменена клиентом или кассиром                              */
//#define ERR_CARD_BLOCKED          2001
#define ERR_TIMEOUT               2002 /**< Истекло время ожидания                                               */
#define ERR_CANCEL_BY_OWNER       2003 /**< Операция прервана по требованию кассового ПО                         */
#define ERR_HOT_CLIENT            2004 /**< Obsolete duet error                                                  */
#define ERR_HOT_REGION            2005 /**< Obsolete duet error                                                  */
#define ERR_HOT_STAFF             2006 /**< Obsolete duet error                                                  */
#define ERR_HOT_BANK              2007 /**< Obsolete duet error                                                  */
#define ERR_BAD_OPER_MASK         2008 /**< Для этой карты операция запрещена                                    */
//#define ERR_OLD_HOTLIST           2018
#define ERR_BAD_HOTLIST           2020
//#define ERR_CANT_WRITE_HOTLIST    2021 /**<  */
#define ERR_BLOCK_PIN1            2404 /**<  */
#define ERR_BLOCK_PIN2            2405 /**<  */
#define ERR_BLOCK_BOTH_PINS       2406 /**<  */
#define ERR_WRONG_LOY_PWD         2601 /**<  */
#define ERR_CLIENT_NOT_IN_LOY     2606 /**<  */
#define ERR_LOW_FUNDS             3001 /**<  */
#define ERR_BIG_TROUBLES          3002 /**<  */
#define ERR_OPERDAY_NOT_OPEN      3019 /**<  */
#define ERR_NO_ACCOUNT            3138 /**<  */
#define ERR_NO_FUNDS_TO_LOAD      3168 /**<  */
/* end of server's range */

/* 4000...MAX_WORD is our range */
#define ERR_IV_FAIL               4046
#define ERR_MK_LOAD_FAIL_0        4060
#define ERR_MK_LOAD_FAIL_1        4061
#define ERR_MK_LOAD_FAIL_2        4062
#define ERR_MAC_FAIL              4063

//#define ERR_FINGER_NOT_FOUND      4070
//#define ERR_FINGER_FAILURE        4071
//#define ERR_BIO_FAILED_SESSION    4072
#define ERR_BIO_NO_SCANNER        4073 /**< Биосканер не подключен*/
#define ERR_BIO_CANCEL            4074 /**< Работа с биосканером прервана клиентом*/
#define ERR_BIO_NOT_SUPPORTED     4075 /**< Работа с биосканером не поддерживается*/
#define ERR_BIO_NO_FINGERS        4076 /**< Отпечатков не получено*/
//#define ERR_BIOLINK_ERR           4077
//#define ERR_BAD_SCAN              4078
//#define ERR_BAD_MATCHER           4079

#define ERR_NO_LINK               4100 /**< Нет связи с банком*/
#define ERR_SBER_NO_HOTLIST       4101 /**< На терминале нет стоп-листа. Выполните инкассацию*/
#define ERR_SBER_NO_PERCENT       4102 /**< Нет данных для расчета комиссии*/
#define ERR_EMV_BAD_AC2           4103
#define ERR_WRONG_REPLY           4104
#define ERR_PIN_ABSENT            4105
#define ERR_PIN_WRONG             4106
#define ERR_PIN_LOCKED            4107
#define ERR_BAD_CHECK_DIGIT       4108
#define ERR_MERCH_BATCH_FULL      4110
#define ERR_MERCH_BATCH_EXPIRED   4111
#define ERR_WRONG_HOTLIST         4112
#define ERR_FIXED_LIMIT_EXCEEDED  4113
#define ERR_ONLINE_FAILED         4114
#define ERR_NO_MANUAL_ENTRY       4115
#define ERR_WRONG_4DIGITS         4116
#define ERR_PIN_REFUSED           4117
#define ERR_UNABLE_FIND_REC       4118
#define ERR_V2R_NOLINK            4119
#define ERR_V2R_UNABLE_MAC        4120
#define ERR_V2R_INT_ERR           4121
#define ERR_V2R_KEY_ERROR         4122
#define ERR_NO_SKEYS              4123
#define ERR_NO_MKEYS              4124
#define ERR_NEED_CHIP             4125
#define ERR_BAD_TRX_LIST          4126
#define ERR_NEED_SPLIT_TRX        4127
#define ERR_BAD_CONFIG_PARAMS     4128
#define ERR_NO_LOG_FILE           4129
#define ERR_DISK_FULL             4130
#define ERR_NEED_PARAMS           4131
#define ERR_REFUSED_BY_CARD       4132
#define ERR_INVALID_RC            4133
#define ERR_TIME_FOR_TOTALS       4134
#define ERR_BAD_DEPT_CFG          4135
#define ERR_NEED_NEW_PINPAD       4136
#define ERR_WRONG_NEW_PWD         4137
#define ERR_CARDS_ARE_SAME        4138
#define ERR_NO_SUITABLE_CONN      4139
#define ERR_INVALID_TRX_DATA      4140
#define ERR_SH_FILE_NOT_FOUND     4141
#define ERR_ONLINE_CANC_FAILED    4142

#define ERR_OLD_M_HOTLIST         4143
#define ERR_BAD_M_HOTLIST_3       4144
#define ERR_BAD_M_HOTLIST_2       4145
#define ERR_BAD_M_HOTLIST_1       4146
#define ERR_BAD_M_HOTLIST         4147
#define ERR_CARD_IN_HL            4148
#define ERR_NAME_NOT_FOUND        4149
#define ERR_FLOOR_LIMIT_EXCEEDED  4150
#define ERR_CARD_EXPIRED          4151

#define ERR_NO_MPAD_HISTORY       4152
#define ERR_BAD_MPAD_HISTORY_FMT  4153
#define ERR_CHIP_REQUIRED         4154
#define ERR_EMPTY_HISTORY         4155

#define ERR_UNABLE_TO_CANCEL      4156
#define ERR_OPER_LIMIT_EXCEEDED   4157
#define ERR_PAYPASS_NO_PIN        4158
#define ERR_MULTI_CUR_NOT_SUPP    4159


#define ERR_APDU_ERR              4160
#define ERR_NO_CERT_EMUL_FILE     4161  
#define ERR_RSA_PRIVATE_FAIL1     4162
#define ERR_RSA_PRIVATE_FAIL2     4163
#define ERR_BSC_BAD_SKEY          4164
#define ERR_BIO_BAD_EA1           4165
#define ERR_BIO_BAD_EA2           4166
#define ERR_BIO_BAD_EA3           4167
#define ERR_BIO_BAD_TEMPLATE1     4168
#define ERR_BIO_BAD_TEMPLATE2     4169
#define ERR_BIO_NO_IAPPD          4171

#define ERR_BAD_OPER_TYPE         4172
#define ERR_CLASS_NOT_SUPP        4173
#define ERR_FILE_NOT_FOUND        4174
#define ERR_FILE_TOO_BIG          4175
#define ERR_VIVO_NO_VERINFO       4176

#define ERR_NOT_OWN_CARD          4180
#define ERR_OLD_PP_VERSION        4181
#define ERR_CA_KEY_BAD            4182
#define ERR_SPASIBO_1             4183
#define ERR_SPASIBO_2             4184
#define ERR_WRONG_ADMIN_CARD      4185
#define ERR_KEY_ALREADY_PRESENT   4186
#define ERR_WRONG_PAN             4187
#define ERR_WRONG_VT              4188
#define ERR_WRONG_VALUE           4189
//                                4190 see bgs error CARD_WAS_REMOVED
#define ERR_WRONG_DEPARTMENT      4191

#define ERR_RDL_START_AGAIN       4200 /**< Повторите удаленную загрузку.*/
#define ERR_RDL_VERSION_OK        4201 /**< Удаленное обновление не требуется.*/
#define ERR_RDL_BAD_OFFSET        4202 /**< Нарушение формата сообщений удаленной загрузки.*/
#define ERR_RDL_UNKNOWN_DEV       4203 /**< Терминал не зарегистрирован на сервере удаленной загрузки.*/
#define ERR_RDL_ERROR             4204 /**< Внутренняя ошибка сервера удаленной загрузки.*/
#define ERR_RDL_BAD_HOST          4205 /**< Ошибка связи с хостом удаленной загрузки.*/
#define ERR_RDL_BAD_CLIENT        4206 /**< Нарушение протокола удаленной загрузки.*/
#define ERR_RDL_BAD_MESSAGE       4207 /**< Нарушение формата сообщений удаленной загрузки.*/
#define ERR_RDL_BAD_DATABASE      4208 /**< Ошибка базы данных удаленной загрузки.*/
#define ERR_RDL_BAD_DATA          4209 /**< Некорректные данные удаленной загрузки.*/
#define ERR_RDL_BAD_CRYPT         4210 /**< Ошибка шифрования данных удаленной загрузки.*/
#define ERR_RDL_KEY_MISSING       4211 /**< Ключ удаленной загрузки отсутствует.*/
#define ERR_RDL_BAD_PROC          4212
#define ERR_RDL_BUSY              4213 /**< Сервер PSDB слишком нагружен. Повторите позже.*/
#define ERR_RDL_NO_REGION         4220 /**< Не указан код региона для удаленной загрузки.*/
#define ERR_RDL_KKM_LINK_FAIL     4221 /**< Не удалось восстановить связь с ККМ после удаленной загрузки.*/

#define ERR_NEED_TRANS_CHEQUE     4222


#define ERR_PIL_PAR_MISSING       4300
#define ERR_PIL_OT_INVALID        4301
#define ERR_PIL_CT_INVALID        4302
#define ERR_PIL_CT_NOT_SERVICED   4303
#define ERR_PIL_WRONG_REPLY       4304

#define ERR_INCORRECT_VERSION	  4305
#define ERR_NOT_INITIALISED  	  4306
#define ERR_RESERVED              4307
#define ERR_GENERAL               4308
#define ERR_NO_DOCUMENT           4309

#define ERR_PIL_BAD_TRACK2        4310
#define ERR_PIL_NO_FILES          4311 /**< Операция не найдена*/
#define ERR_PIL_NO_MORE_TASKS     4312 /**< Все задачи выполнены. Список задач пуст.*/
#define ERR_PIL_CARD_NOT_SAME     4313 /**< Номер карты не соответствует исходному*/
#define ERR_PIL_NEED_SBER         4314
#define ERR_PIL_CARD_NOT_SAME_2   4315 /**< Разрешены только отмены в текущей смене*/
#define ERR_PIL_AUTOCANCEL        4316
#define ERR_PIL_NOT_CLOSED        4317
#define ERR_PIL_NOT_PROCESSED     4318
#define ERR_PIL_AMOUNT_TOO_BIG    4319
#define ERR_PIL_CARD_NOT_SAME_3   4320
#define ERR_PIL_REMOVE_OLD_CARD   4321
#define ERR_PIL_PRINT_ERROR       4322 /**< Печать чека операции завершилась ошибкой. Ошибка возвращается только для ИКР 
                                            PCI-DSS подключенных через COM/USB.
                                            При печати чека пинпад создает на ККМ файл p (имя может быть изменено 
                                            настройками) и построчно выводит в него чек. В случае, если при работе с этим
                                            файлом прервалась связь между пинпадом и ПК, или пинпад не смог 
                                            создать файл чека на ККМ или не смог в него записать строку чека. 
                                            Финансовая операция, завершившаяся такой ошибкой, будет отменена при 
                                            следующей операции.*/

#define ERR_PIL_CARD_NOT_SAME_4   4323
#define ERR_PIL_ENROLL_FAILED     4324
#define ERR_PIL_NO_AMOUNT         4325
#define ERR_PIL_BAD_TRACK1        4326
#define ERR_NO_GOODS_FOR_DISPLAY  4327
#define ERR_NO_GOODS_IN_ARGS      4328
#define ERR_PIL_TOO_MANY_GOODS    4329
#define ERR_NO_GOODS_FOUND        4330


#define ERR_GENERAL1              4331
#define ERR_PIL_INVALID_ARG       4332
#define ERR_GENERAL3              4333
#define ERR_READ_CARD             4334
#define ERR_GENERAL5              4335
#define ERR_INVALID_CURRENCY      4336
#define ERR_GENERAL7              4337
#define ERR_GENERAL8              4338
#define ERR_GENERAL9              4339
#define ERR_GENERAL_10            4340
#define ERR_GENERAL_11            4341
#define ERR_GENERAL_12            4342
#define ERR_GENERAL_13            4343

#define ERR_ADMIN_CARD            4350
#define ERR_NO_TLV_FILES          4351
#define ERR_CARD_READY            4352  /**< Карта вставлена. Ответ на команды ::OP_PILOT_STATUS и ::KO_TEST_CARD_READ */

#define ERR_THIS_IS_SBER          4353  /**< Промежуточный код ответа для использования баллов спасибо и программ лояльности */

#define ERR_SONDA_CHECK_FINGER    4354
#define ERR_SONDA_FINGER_DUP      4355
#define ERR_SONDA_USER_DUP        4356
#define ERR_SONDA_INSUFF_Q        4357
#define ERR_SONDA_FAILED          4358
#define ERR_SONDA_UNKNOWN_ERR     4359
#define ERR_SONDA_ENROLL_FAILED   4360
#define ERR_SONDA_NOT_FOUND       4361

#define ERR_PP_NOT_WORKING        4362
#define ERR_AMOUNT_GT_ORIGINAL    4363

#define ERR_SIGN_CAPTURE_NOT_SUPPORTED  4365
#define ERR_TOO_LOW_DISCOUNT_VALUE      4366

#define ERR_RKL_INVALID_REQ_FMT   4367 
#define ERR_RKL_NO_CA_KEY         4368
#define ERR_RKL_NO_HOST_CERT      4369
#define ERR_RKL_NO_CA_PK          4370
#define ERR_RKL_KEYGEN_FAIL       4371
#define ERR_RKL_B2B_FAIL          4372
#define ERR_RKL_LENGTH_SERT       4373

#define ERR_CASHINFO_NO_NUM_TRX   4375
#define ERR_CASHINFO_NO_AMOUNT    4376
#define ERR_CASHINFO_NO_SHIFT_ID  4377
#define ERR_CASHINFO_BAD_AMOUNT   4378
//#define ERR_USE_ME_PLEASE       4379
#define ERR_DAY_SHOULD_BE_CLOSED  4380
#define ERR_QR_BAD_FORMAT         4381
#define ERR_VOLUME_TOO_BIG        4382
#define ERR_CANT_OPEN_SCANER      4383
#define ERR_MORE_DATA             4384  /**< Не все данные считаны/получены */
#define ERR_WRONG_USER            4385
#define ERR_PUBLIC_KEY_HASH       4386
#define ERR_PUBLIC_KEY_EXPIRED    4387
#define ERR_SEND_CHEQUES_TO_SERVER 4388 /**< Место в терминале закончилось. Отправите чеки в банк. */
#define ERR_CHEQUE_ALREADY_PRESENT 4389 /**< Торговый чек передан в терминал */



#define ERR_V2R_AUTH_DECLINED     4400  /* reserved up to 4699 */
#define ERR_V2R_AUTH_CALL_ISS     4401

//#define SPS_CANCEL   ERR_PP_ABORTED

#define ERR_VV_ERROR              4700

/**@}*/

#define IsError(error_code) ((error_code != ERR_OK) && (error_code != ERR_THIS_IS_SBER))

#endif
