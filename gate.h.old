/**
 * @file public/gate.h
 * @author A. Brik, S. Papazov
 * @date 12 June 2016
 * @brief Public API for gate.dll.
 */
 
#ifndef _PUBLIC_GATE_
#define _PUBLIC_GATE_

/** @mainpage Основные сведения
 * Для обслуживания банковских, топливных карт и карт лояльности Сбербанк предоставляет динамическую библиотеку GATE.DLL,
 * обеспечивающую проведение авторизаций по картам, а также формирование чеков и итоговых отчетов за день.
 * Для работы с картами кассовый аппарат оснащается дополнительным внешним устройством – ПИН-клавиатурой.
 * Это устройство используется для считывания карт, для ввода ПИН-кода клиентом, а также выполняет функции
 * криптографической защиты при обмене данными с процессинговой системой Сбербанка. ПИН-клавиатура может
 * подключаться к СОМ-порту, порту USB или по протоколу ТТК2 через Ethernet Tcp/Ip. Сбербанк предоставляет
 * это устройство бесплатно на период действия договора эквайринга с торговым предприятием.
 * Если кассовый аппарат (ККМ) оснащен собственным ридером для магнитной полосы, то в период первоначальной
 * разработки/отладки кассового ПО можно обойтись без ПИН-клавиатуры. Для окончательного тестирования (а также
 * для реальной работы в торговом предприятии) ПИН-клавиатура понадобится обязательно.
 */

/** @page page2 Принцип работы с библиотекой
 * Функция ::call_sb_kernel является основной для выполнения всех операций.
 @code{.cpp}
 GATE_API DWORD call_sb_kernel(DWORD  func, void   *in_arg,  void   *out_arg);
 @endcode
 @warning В решении Windows x86 функция линкуется по имени с подчеркиванием _call_sb_kernel, в решении для Windows CE без подчеркивания.
 
 * Первый параметр func типа DWORD (целочисленное положительное) принимает значения из списка ::KernelOperation и задает основное действие.
 * Входные аргументы передаются вторым параметром в структуре ::InArg. Основная информация передается с помощью поля InArg::in_struct, а дополнительное поле InArg::Reserved.
 * Не используемые поля ::InArg должны быть установлены в нулевые значения.
 * Выходные аргументы передаются третьим параметром в структуре ::OutArg. Для получения расширенной информации следует проинициализоровать поля OutArg::out_struct и OutArg::Reserved.
 * 
 * В случае успешного вызова функции ядра функция ::call_sb_kernel возвращает 0 (соответствует константе ::ERR_OK – см. описание кодов ошибок ниже), в противном случае – ненулевой код ошибки. Вызывающая программа должна проанализировать значение, воз-вращаемое функцией  ::call_sb_kernel. Если функция вернула ::ERR_OK, следует проанализировать поле out_arg.ErrorCode.
 * В поле ErrorCode возвращается код ошибки или 0, если операция была успешно выполнена.
 * Перед вызовом функции вызывающая программа должна установить значение поля Flags в 0. После вызова функции вызывающая программа должна проанализировать содержимое поля Flags и выполнить действия в соответствии с установленными флагами.  
 * Если установлен флаг F_HAVE_DOCUMENT, вызывающая программа должна выполнить печать документа с помощью специальных функций ::KO_SIZE_OF_CHEQUE и ::KO_GET_LAST_CHEQUE.
 * Если установлен флаг F_HAVE_MONMSG, вызывающая программа должна выполнить чтение сообщения системы мониторинга с помощью специальной функций ::KO_GET_MONITORING_STATE.
 * Если установлен флаг F_CRITICAL (может устанавливаться только совместно с F_HAVE_DOCUMENT), вызывающая программа после печати документа должна убедиться, что он напечатан успешно (при необходимости запросив подтверждение у пользователя).
 * Параметр Reserved должен иметь значение NULL, если противное специально не оговари-вается в описании конкретной функции.
 * Параметр out_struct – указатель на буфер типа struct_out_clX, где X – номер класса, для функций ненулевого класа, или указатель на буфер типа struct_outX, где X – номер функции класса 0, или NULL, если функция не возвращает результатов. Этот буфер за-полняется данными только в случае успешного выполнения функции.
 * Примеры работы с библиотекой входят в состав дистрибутива и расположены в папке "Docs\Для разработчиков ПО ККМ\examples\gate.dll\cpp"

В общем случае код вызова может выглядеться так (последовательность действий рекомендатальная):
-# Включить заголовочные файлы
* @code{.cpp}
#include "..\..\..\..\errors.h"
#include "..\..\..\..\gate.h"
#include "..\..\..\..\sbkernel.h"
  @endcode
-# Объявить аргументы
* @code{.cpp}
  InArg in;
  OutArg out;
  struct_in_cl4 InDetail;
  struct_out_cl4 OutDetail;
  struct_in_reserved4 inExtra;
  struct_out_reserved4 outExtra;
  @endcode
  
-# Связать аргументы
* @code{.cpp}
  in.in_struct = &InDetail;
  out.out_struct = &OutDetail;
  out.Reserved = &outExtra;
  outExtra.size = sizeof(outExtra);
  inExtra.size = sizeof(inExtra);
  in.Reserved = &inExtra;
  @endcode
 
-# Задать значения
* @code{.cpp}
  InDetail.Amount = amount;
  strcpy_s(inExtra.RRN, rrn.c_str());
  @endcode

-# Загрузить библиотеку и основную функцию.
* @code{.cpp}
  HMODULE dll = LoadLibrary("gate.dll");
  Gate_Call_Sb_Kernel gate_Call_Sb_Kernel = (Gate_Call_Sb_Kernel)GetProcAddress(dll, "_call_sb_kernel");
  @endcode
  @warning Обязательно инициализировать библиотеку функцией KO_INITIALIZATION
-# Вызвать динамически загруженную функцию
* @code{.cpp}
  DWORD error = gate_Call_Sb_Kernel(KO_PREAUTH,&in,&out);
  @endcode
  
-# Проверить результат вызова
* @code{.cpp}
  throw_if_error(error, "perform_preauth");
  throw_if_error(out.ErrorCode, "perform_preauth");
  std::cout << "Customer used card:" << OutDetail.CardName << std::endl;
  @endcode

-# При необходимости выгрузить библиотеку.
  @warning Обязательно завершить работу библиотеки KO_FINALIZATION
* @code{.cpp}
  FreeLibrary(dll);
  @endcode
*/

/** @page page3 Оплата покупки с использованием баллов СПАСИБО
  Существует возможность организовать оплату покупки с использованием бонусов СПАСИБО. Для этого кассовая программа должна получить информацию, что данная банковская карта выпущена Сбербанком и, следовательно, имеет бонусный счет СПАСИБО. При этом хотелось бы избежать ситуации, когда клиенту придется вставлять/прокатывать/прикладывать карту повторно для выполнения собственно оплаты.
  Чтобы реализовать такой режим работы, необходимо в TLV-файле включить параметр «Дополнительные опции \ Возвращать промежуточный код ответа \ Возвращать промежуточный код ответа по картам Сбербанка». 

Ниже описан пример операции со стороны ККМ:
-# Включить заголовочные файлы
* @code{.cpp}
  #include "..\..\..\..\errors.h"
  #include "..\..\..\..\gate.h"
  #include "..\..\..\..\sbkernel.h"
  @endcode
-# Объявить аргументы
* @code{.cpp}
  InArg in;
  OutArg out;
  struct_in_cl4 InDetail;
  struct_out_cl4 OutDetail;
  struct_in_reserved4 inExtra;
  struct_out_reserved4 outExtra;
  @endcode
  
-# Связать аргументы
* @code{.cpp}
  in.in_struct = &InDetail;
  out.out_struct = &OutDetail;
  out.Reserved = &outExtra;
  outExtra.size = sizeof(outExtra);
  inExtra.size = sizeof(inExtra);
  in.Reserved = &inExtra;
  @endcode
 
-# Задать значения
* @code{.cpp}
  outExtra.Department = DEPARTMENT_NOT_SPECIFIED;
  InDetail.Amount = 100000; // тысяча рублей - сумма к оплате
  @endcode

-# Загрузить библиотеку и основную функцию.
* @code{.cpp}
  HMODULE dll = LoadLibrary("gate.dll");
  Gate_Call_Sb_Kernel gate_Call_Sb_Kernel = (Gate_Call_Sb_Kernel)GetProcAddress(dll, "_call_sb_kernel");
  @endcode
  @warning Обязательно инициализировать библиотеку функцией KO_INITIALIZATION
  
-# Вызвать динамически загруженную функцию на оплату
* @code{.cpp}
  DWORD error = function(4000,&in,&out); // вызов функции _call_sb_kernel библиотеки sb_kernel.dll
  @endcode

-# Если клиент использует для оплаты соответствующую карту, то функция _call_sb_kernel вернет промежуточный код ответа ::ERR_THIS_IS_SBER (4353).
    В поле outExtra.Cert вернется хеш карты для взаимодействия с системой ЦФТ.
    Следует обратиться к программе компании ЦФТ (списать баллы спасибо), которая занимается списанием/начислением бонусов СПАСИБО. По итогам взаимодействия с программой ЦФТ часть покупки, возможно, будет оплачена бонусами СПАСИБО. 
    Если списание бонусов "Спасибо" прошло успешно, то следует уменьшить сумму операции на это значение.
    После этого в inExtra.Track2 записать значение хеша карты от первой операции.
    Повторно вызвать оплату с измененными данными.
    Карту повторно прокатывать / прикладывать не нужно.
* @code{.cpp}
  if (out.ErrorCode == ERR_THIS_IS_SBER) // 4353
  {
    std::cout << "Sberbank Spasibo detected!" << std::endl;
    inExtra.Amount = 90000;                               // по Спасибо списано 10 рублей, итоговая сумма 900 рублей.
    strcpy_s(inExtra.Track2, (const char*)outExtra.Cert); // передача хеша карты для второй оплаты
    error = function(4000,&in,&out);                      // повторный вызов оплаты
  }
  @endcode
  
-# Проверить результат вызова
* @code{.cpp}
  out.ErrorCode;
  @endcode

-# Оплата с баллами спасибо выполнена успешно. При необходимости выгрузить библиотеку.
  @warning Обязательно завершить работу библиотеки KO_FINALIZATION
* @code{.cpp}
  FreeLibrary(dll);
  @endcode
  @see Пример реализации находится в папке Docs\Для разработчиков ПО ККМ\examples\gate.dll\cpp\spasibo
  @warning При повторном вызове кассовая программа должна изменить только сумму операции и/или количество товара. Изменения других параметров операции, таких как номер отдела, код валюты и т.п. запрещено.
 */
/** @page page5 Уникальный идентификатор операции RequestID
  Значение RequestID может потребоваться для выполнения отправки торгового (кассового) чека в банк.
  
  Если это необходимо, то для финансовых операций следует использовать функцию _call_sb_kernel(4xxx, ...).
  Значение будет возвращено в поле RequestID структуры struct_out_reserved4.
  
  Например:
* @code{.cpp}
  InArg in;
  OutArg out;
  struct_in_cl4 InDetail;
  struct_out_cl4 OutDetail;
  struct_in_reserved4 inExtra;
  struct_out_reserved4 outExtra;
  @endcode
  
-# Связать аргументы
* @code{.cpp}
  in.in_struct = &InDetail;
  out.out_struct = &OutDetail;
  out.Reserved = &outExtra;
  outExtra.size = sizeof(outExtra);
  inExtra.size = sizeof(inExtra);
  in.Reserved = &inExtra;
  @endcode
 
-# Задать значения
* @code{.cpp}
  outExtra.Department = DEPARTMENT_NOT_SPECIFIED;
  InDetail.Amount = 100000; // тысяча рублей - сумма к оплате
  @endcode

-# Загрузить библиотеку и основную функцию.
* @code{.cpp}
  HMODULE dll = LoadLibrary("gate.dll");
  Gate_Call_Sb_Kernel gate_Call_Sb_Kernel = (Gate_Call_Sb_Kernel)GetProcAddress(dll, "_call_sb_kernel");
  @endcode
  @warning Обязательно инициализировать библиотеку функцией KO_INITIALIZATION
  
-# Вызвать динамически загруженную функцию на оплату
* @code{.cpp}
  DWORD error = function(4000,&in,&out);
  //outExtra.RequestID содержит идентификатор операции
  @endcode

*/
/** @page page6 Отправка торгового чека в банк
  Для отправки торгового чека в банк, необходимо включить эту функцию в настройках терминала.
  Опция доступная в секции "Дополнительные настройки" и называется "Передавать чеки" (значение Да).
  Кроме того, терминал должен содержать "Вариант связи" с параметрами сервера чеков.
  По умолчанию опция выключена (аналогично значению "Передавать чеки" - Нет).
  После этого для каждой операции будет сохранена экранная подпись и банковский чек, а торговый можно будет отправить в терминал
  функцией ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...). Она же используется для отправки собранной формации в банк.
  Торговый чек должен быть передан в кодировке CP866.
  Алгоритм следующий:
  1. Выполнить финансовую операцию с получением идентификатора операции \ref page5 "RequestID".
  2. Запустить функцию ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...)
     В структуре ::tag_struct_in_reserved6 заполнить поля ::tag_struct_in_reserved6::dwQueryRequestID и ::tag_struct_in_reserved6::MerchantCheque.
     В поле ::tag_struct_in_reserved6::dwQueryRequestID следует записать значение, получное при соответствующей финансовой операции в поле ::struct_out_reserved4::RequestID.
     Вызов этой функции обязателен, если торговый чек отсутствует или не требуется, то в поле ::tag_struct_in_reserved6::MerchantCheque следует указывать 0.
     При этом торговый чек, банковский чек и подпись клиента (при наличии) будут сохранены в терминале, но не переданы в банк.
  3. Рекомендуется отправлять чеки в банк каждую десятую операцию.
     Для этого нужно вызвать функцию ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) со значениями ::tag_struct_in_reserved6::dwQueryRequestID и ::tag_struct_in_reserved6::MerchantCheque равными 0.

  Если в терминале заканчивается свободное место, то финансовая операция, либо функция ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) вернут ошибку ::ERR_SEND_CHEQUES_TO_SERVER.
  Это значит, что следует немедленно отправить сохраненные в терминале чеки в банк.
  При большом количестве операций и торговых чеков, может быть сформировано несколько файлов для отправки в банк.
  Если после отправки в банк функция ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) вернула ошибку ::ERR_MORE_DATA, значит в терминале присутствует ещё один/несколько файлов для отправки, и операцию можно повторить пока не вернется 0.
  Если торговый чек по операции уже передан в терминал, то повторный вызов ::call_sb_kernel(::KO_ADD_MERCHANT_CHEQUE,...) с тем же \ref page5 "RequestID" вернет ошибку ::ERR_CHEQUE_ALREADY_PRESENT.
  Например: @see Docs\\Для разработчиков ПО ККМ\\examples\\gate.dll\\cpp\\merh-cheque-to-host
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
 * @defgroup KERNEL_DESCRIPTOR_0 Служебные функции библиотеки
 * @brief Описание функций 0-22 - инициализация и финализация, получение чека, перечня поддерживаемых валют и функций, вызов функций мониторинга.

 *  @see  Примеры вызовов функций данного раздела находятся в Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
/**@{*/

/**
 * @defgroup FUNC_0 Функция 0 (KO_INITIALIZATION)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief Инициализация и подготовка библиотеки к работе.
          
	При вызове функции 0 внешняя программа должна в поле struct_in0::AppVersion установить минимальный номер версии библиотеки, с которой она поддерживает работу.
	Рекомендованное значение ::KERNEL_MINIMAL_VERSION. В случае, если затребованный номер версии превышает текущую версию библиотеки, функция 0 вернет код ошибки ::ERR_INCORRECT_VERSION. В противном случае могут быть возвращены другие коды ошибок (например, связанные с неготовностью оборудова-ния, используемого библиотекой, нарушением целостности файлов и т.д.) или ::ERR_OK, если инициализация прошла успешно.
	Если используемая библиотека пользовательского интерфейса требует дополнительных параметров инициализации, то они передаются в поле UIData.
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

#define KERNEL_MINIMAL_VERSION 0x00030101 ///< Минимально поддерживаемая версия интерфейса gate.dll

/** 
 *  @brief Структура инициализации библиотеки gate.dll. Используется для функции 0 (::KO_INITIALIZATION).
 */
typedef struct tag_struct_in0{
  DWORD  AppVersion;  ///< [in] Минимальная требуемая версия ядра. Номер версии задается в шестнадцатеричном формате 0x00VVRRBB, где VV – версия, RR – релиз, BB – билд. Например, версия 2.19.12 кодируется как 0x0002130C. Рекомендованное значение ::KERNEL_MINIMAL_VERSION.
  void   *UIData;     ///< Не используется.
  void   *Reserved;   ///< [in] Может быть задано как указатель на структуру struct_in_reserved0
} struct_in0;

/** 
 *  @brief Дополнительные параметры для Функции 0. Не используются.
 */
typedef struct tag_struct_in_reserved0{
  DWORD size;          ///< Размер структуры в байтах = sizeof(struct_in_reserved0)
  void  *Reserved1;    ///< Зарезервировано. Должно быть установлено в NULL.
  ::BYTE  KeyVersion;  ///< Устеревшее поле. Не используется.
} struct_in_reserved0;

/** 
 *  @brief Информация об инициализированной библиотеке.
 */
typedef struct tag_struct_out0{
  DWORD  LibVersion;  ///< [out] Реальная установленная версия ядра. Номер версии задается в шестнадцатеричном формате 0x00VVRRBB, где VV – версия, RR – релиз, BB – билд.
  void   *Reserved;   ///< Не используется. Должно быть установлено в значение 0 (NULL)
}struct_out0;
/**	@}*/


/**
 * @defgroup FUNC_1 Функция 1 (KO_FINALIZATION)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief Завершение работы с библиотекой.
          
	Функция 1 (KO_FINALIZATION) должна вызываться перед выгрузкой библиотеки из памяти.
 */

/** 
 * @defgroup FUNC_2 Функция 2 (KO_FUNCTIONS_COUNT)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief Получить количество поддерживаемых библиотекой операций
	
	С помощью функции 2 внешняя программа может узнать количество карточных опера-ций, 
	поддерживаемых библиотекой.  Это необходимо для последующего получения спис-ка названий и номеров этих операций вызовом функции 3.
	@see  FUNC_3

 */
/**	@{*/
/** 
 *  @brief Количество поддерживаемых библиотекой операций.
 */
typedef struct tag_struct_out2{
  DWORD  Count;       ///<  [out] кол-во поддерживаемых операций. Значение используется для инициализации буфера описаний операций в функции 3. @see  FUNC_3
}struct_out2;
/**	@}*/

/** 
 * @defgroup FUNC_3 Функция 3 (KO_FUNCTIONS_LIST)
 * \ingroup KERNEL_DESCRIPTOR_0
 * @brief Получить описание поддерживаемых операций

	При вызове функции 3 структура struct_out3 должна быть инициализирована вызываю-щей программой. 
	Буфер Buffer также должен быть выделен вызывающей программой и иметь достаточный размер 
	(для его определения необходимо предварительно вызвать функцию 2). 
	Возвращенный список карточных операций может быть использован внешней програм-мой при построении 
	пользовательского интерфейса (меню, диалоговых окон и т.д.). 
	Но-мер функции позволяет приблизительно определить ее содержание (см. раздел «Классы функций») 
	и расположить ее в соответствующей ветви меню. Кроме того, дополнительная информация о содержании операции 
	может быть получена из поля Options. В нем могут быть установлены биты ::GateOperationOptions.
	\code
	  //предполагается что библиотека предварительно инициализирована вызовом функции 0 
  	  //
	  InArg in;
	  OutArg out;
	  struct_out2 out2;
	  struct_out3 out3;
	  
	  //Названия функций указаны в кодировке 1251, поэтому устаналиваем локаль для корректного вывода текста в консольном режиме
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
	Результат работы примера
	\code{.unparsed}
Function: 'Оплата покупки' has number 1000
Function: 'Выдача наличных' has number 1001
Function: 'Возврат покупки' has number 1002
Function: 'Оплата ГСМ' has number 1003
Function: 'Проверить наличие суммы' has number 1004
Function: 'Прочитать ФИО клиента и номер карты' has number 1005
Function: 'Баланс карты' has number 2000
Function: 'Справка об операциях' has number 2001
Function: 'Перевод средств на ОТОК' has number 2002
Function: 'Изменение ПИНа' has number 2003
Function: 'Загрузка средств' has number 2004
Function: 'Баланс счета' has number 2005
Function: 'Авторизация' has number 2006
Function: 'Получить идентификатор карты' has number 2007
Function: 'Справка о карте терминала' has number 3000
Function: 'Инкассация карты терминала' has number 3001
Function: 'Закрытие дня по СБЕРКАРТ' has number 3002
Function: 'Оплата покупки' has number 4000
Function: 'Выдача наличных' has number 4001
Function: 'Возврат покупки' has number 4002
Function: 'Отмена операции' has number 4003
Function: 'Безналичный перевод при наличии карты клиента' has number 4004
Function: 'Безналичный перевод при отсутствии карты клиента' has number 4005
Function: 'Слип по картам American Express' has number 4006
Function: 'Слип оплаты' has number 4007
Function: 'Слип выдачи наличных' has number 4008
Function: 'Предавторизация' has number 4009
Function: 'Завершение расчета' has number 4010
Function: 'Взнос наличных (предв. зачисление)' has number 4011
Function: 'Взнос наличных (подтверждение)' has number 4012
Function: 'Погашение кредита' has number 4013
Function: 'Перевод с карты на карту' has number 4014
Function: 'Арест средств по решению суда' has number 4016
Function: 'Списание средств по аресту' has number 4017
Function: 'Снятие ареста' has number 4018
Function: 'Выдача наличных по заявлению клиента без карты' has number 4019
Function: 'Списание по требованию' has number 4020
Function: 'Установка ПИН-кода' has number 4021
Function: 'Смена ПИН-кода' has number 4022
Function: 'Списание средств в оплату услуг Банка' has number 4023
Function: 'Запись отпечатков пальцев' has number 4024
Function: 'Установка оффлайн-лимита' has number 4025
Function: 'Распечатка истории карты' has number 4026
Function: 'Оплата услуг банка' has number 4023
Function: 'Запрос баланса' has number 5000
Function: 'Разблокировка карт с чипом' has number 5001
Function: 'Получить идентификатор карты' has number 5002
Function: 'Блокировка карты' has number 5003
Function: 'Проверка ПИНа' has number 5004
Function: 'Итоги дня по картам с магнитной полосой' has number 6000
Function: 'Подтверждение операции по картам с магнитной полосой' has number 6001
Function: 'Формирование текущего отчета по всем типам карт' has number 7000
Function: 'Повторное формирование последнего чека' has number 7001
Function: 'Печать старой контрольной ленты' has number 7002
	\endcode
 */
/**	@{*/
/** Флаги поля Options */
typedef enum {
O_DEBIT      = 0x00000001, ///< Операция предполагает списание средств с карты (счета) клиента
O_CREDIT     = 0x00000002, ///< Операция предполагает зачисление средств на карту (счет) клиента
O_CASH       = 0x00000004, ///< Операция предполагает выдачу наличных денежных средств из кассы или внесение их в кассу
O_TRANSFER   = 0x00000008, ///< Операция предполагает перемещение средств в пределах карты (карточных счетов) клиента
O_SERVICE    = 0x00000010, ///< Операция является сервисной; она не влияет на финансовое состояние карты (счета), но может влиять на административное состояние (блокировка, разблокировка, смена ПИНа и т.д.)
O_INFO       = 0x00000020, ///< Операция является только информационной и не влияет на финансовое или административное состояние карты
O_BANKTERM   = 0x00000040, ///< Операция разрешена на банковском терминале
O_TRADETERM  = 0x00000080, ///< Операция разрешена на торговом терминале
O_SPECIAL    = 0x00000100, ///< Специальная операция, предназаначенная для использовании в ПО, в котором специально реализована поддержка такой операции.
} GateOperationOptions;

/** 
 *  @brief Описание поддерживаемой операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_EnumStruct{
  DWORD  FuncID;    	 ///<  [out] номер функции библиотеки @see KernelOperation
  DWORD  Options;        ///<  [out] флаги, описывающие содержание операции, @see GateOperationOptions
  char   Name[64];       ///<  [out] название операции. Кодовая страница Windows 1251.
}EnumStruct;

/** 
 *  @brief Контейнер для массива поддерживаемых операции
 *  @note  Buffer должен быть инициализирован. Возвращенный список операций
 *  может быть использован внешней программой при построении пользовательского
 *  интерфейса (меню, диалоговых окон и т.д.).
 *  Для торгового кассового ПО могут использоваться операции с опцией O_TRADETERM
 *  Для банковского кассового ПО могут использоваться операции с опцией O_BANKTERM
 *  Если кассовое предполагает доступ к сервисным функция терминала, то необходимо предоставить доступ к фугкциям с опцией O_SERVICE
 *  Для банковского ПО может быть предоставлен доступ в информационным функциям с опцией O_INFO
 *  Кассовое ПО может требовать наличия специальных функций, реализованных "под заказчика" такие функции имеют опцию O_SPECIAL
 */
typedef struct tag_struct_out3{
  EnumStruct  *Buffer;   ///< [in,out] буфер размера tag_struct_out2::Count * sizeof(EnumStruct)
} struct_out3;
/**	@}*/

/** 
 *  @brief Информация о последнем чеке в терминале
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out4{
  DWORD  Size;          ///< [out] размер образа документа в байтах
}struct_out4;

/** 
 *  @brief Контейнер для буфера с последним чеком в терминале
 *  @note  Buffer должен быть инициализирован.
 *  После завершения функции 5 в буфере будет содержаться текстовая строка в кодировке 1251,
 *   содержащая образ документа, который следует вывести на печать. При выводе на печать не-
 *   обходимо использовать шрифт с фиксированной шириной символов.
 *   Внешняя программа должна обеспечить вывод указанного документа на печать. При обнаружении
 *   ошибки печати она должна инициировать повторную печать после устранения ошибки.
 */
typedef struct tag_struct_out5{
  void  *Buffer;         ///< [in,out] буфер, в который помещается образ документа
} struct_out5;

/** 
 *  @brief Информация о валютах поддерживаемых в терминалом
 *  @note Не поддерживается в PCI-DSS
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out6{
  DWORD  Count;          ///< [out] число используемых валют
} struct_out6;

/** 
 *  @brief Информация о валюте
 */
typedef struct tagCurrDesc{
  DWORD Currency;     ///< [out] Код валюты: 810 - рубли, 840 - доллары США
  char  Name[4];      ///< [out] Сокращенное название валюты, напр. "Руб", "USD"
  char  FullName[64]; ///< [out] Полное название валюты
} CurrDesc;

/** 
 *  @brief Контейнер для буфера с массивом валют
 *  @note  Buffer должен быть инициализирован
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out7{
  CurrDesc  *Buffer; ///< [in,out] Буфер, заполняемый структурами CurrDescr
}struct_out7;

/** Флаги поля Options */
typedef enum {
GATE_WINDOW_HANDLES = 1, ///< Тип передаваемых параметров
} GateSpecificSettings;

/** 
 *  @brief Специфичные параметры
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in8{
  void *Settings; ///<Указатель на буфер с настроечными параметрами
  int   Version;   ///<Тип передаваемых параметров @see GateSpecificSettings
} struct_in8;

/** @brief Результат установки параметров
 */
#ifdef __cplusplus
typedef struct tag_struct_out8{
} struct_out8;
#endif /*__cplusplus*/

/** 
 *  @brief Установка имени пользователя
 *  @note  Не используется в PCI-DSS решениях
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in9{
  char UserName[MAX_OPER_NAME+1]; ///< Имя пользователя
} struct_in9;

/** @brief Результат установки имени пользователя
 */
#ifdef __cplusplus
typedef struct tag_struct_out9{
} struct_out9;
#endif /*__cplusplus*/

/** 
 *  @brief Подготовить гибридный считыватель к работе
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in11{
  DWORD hWindow;    ///< Handle окна для получения windows-сообщений из очереди
  DWORD Message;    ///< Идентификатор сообщений для получения windows-сообщений из очереди
} struct_in11;

/** 
 *  @brief Проверка наличия пинпада. Получение информации о терминале.
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in13{
  DWORD ScrId;   ///< Устаревшее поле. Не используется. Долен быть равен 0.
  int   DlgNum;  ///< Устаревшее поле. Не используется. Долен быть равен 0.
  BYTE  RFU[56]; ///< Устаревшее поле. Не используется. Долен быть равен 0.
} struct_in13;

/** 
 *  @brief Информация о терминале
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out13{
  BYTE  Model;                   ///< Модель пинпада
  BYTE  Version;                 ///< Номер версии планового прелиза.
  BYTE  Release;                 ///< Номер версии срочного релиза.
  BYTE  Build;                   ///< Номер версии сборки.
  BYTE  HasCtls;                 ///< Признак "бесконтактный считыватель в наличии и включен".
  BYTE  bPPScreenWidth;          ///< Ширина экрана пинпада
  BYTE  bPPScreenHeight;         ///< Высота экрана пинпада
  char  SN[12+1];                ///< Серийный номер пинпада
} struct_out13;

/** 
 *  @brief Получение текста ошибки по коду ошибки
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in14{
  DWORD dwErrorCode;          ///< Код ошибки
} struct_in14;

/** 
 *  @brief Контейнер для текста ошибки
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out14{
  char ErrorDescription[256]; ///< Текст ошибки, соответствующий коду в запросе
} struct_out14;

/** 
 *  @brief Ответ на запрос номера терминала и мерчанта
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out15{
  char TerminalID[ 9];
} struct_out15;

/** 
 *  @brief Опциональный ответ на запрос номера терминала и мерчанта
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out_reserved15{
  DWORD size;            ///< Размер структуры в байтах = sizeof(struct_in_reserved15)
  char MerchantID[16];   ///< Номер мерчанта
} struct_out_reserved15;

/**@brief Значения аргумента amount для запуска запросов (команда 47) к серверу мониторинга через функцию KO_CALL_PILOT_COMMAND
 * @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 * */
typedef enum {
  CMD_MONIT_H = 1, ///< отправка запроса на мониторинг helios
  CMD_MONIT_P = 2  ///< отправка запроса на мониторинг pserverDB
} GateMonitoringRequestType;

/** 
 *  @brief Ответ чтения сообщения мониторинга
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_in22{
  int bErase;   ///< Удалить полученное сообщение
} struct_in22;

/** 
 *  @brief Ответ чтения сообщения мониторинга
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-0-999
 */
typedef struct tag_struct_out22{
  int mesType;     ///< тип сообщения. 0 - обычное. должно выводиться только если текст был изменен, 1 - срочное. выводится всегда
  int mesState;    ///< состояние процессинга. 0 - стабильное, 1 - предупреждение, возможны отдельные отказы и замедление проведения операции, 2 - все плохо.
  int mesStateEx;  ///< RFU
  char msg[196];   ///< Текст полученного сообщения
} struct_out22;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_1 Финансовые операции по СБЕРКАРТ
 * @brief Устаревший функционал. Не используется.
 */
/**@{*/

typedef struct tag_struct_in_cl1{
  DWORD  Amount;                                   ///< сумма операции; если 0 - запрашивается библиотекой
} struct_in_cl1;

#define MAX_SBERCARD_CLIENT_NAME 38 ///< Длина номера карта СБЕРКАРТ 

typedef struct tag_struct_out_cl1{
  DWORD    AmountClear;                            ///< сумма операции без комиссии/скидки
  DWORD    Amount;                                 ///< сумма операции с учетом комиссии
  char     TrxDate[MAX_DATE+1];                    ///< дата операции  (ДД.ММ.ГГГГ)
  char     TrxTime[MAX_TIME+1];                    ///< время операции (ЧЧ:ММ:СС)
  char     MerchantCard[MAX_CARD_NUM+1];           ///< номер карты терминала
  char     MerchantName[MAX_NAME+1];               ///< название карты терминала
  char     MerchantCert[MAX_CERT+1];               ///< терминальный сертификат транзакции
  ::WORD     MerchantTSN;                          ///< номер транзакции в пакете терминала
  ::WORD     MerchantBatchNum;                     ///< номер пакета терминала
  char     MerchantBatchDate[MAX_DATE+1];          ///< дата пакета терминала (ДД.ММ.ГГГГ)
  char     ClientCard[MAX_CARD_NUM+1];             ///< номер карты клиента
  char     ClientBranch[MAX_BRANCH_NUM+1];         ///< код банка клиента
  char     ClientAccount[MAX_ACCOUNT_NUM+1];       ///< номер счета клиента
  char     ClientName[MAX_SBERCARD_CLIENT_NAME+1]; ///< фамилия клиента
  char     ClientCert[MAX_CERT+1];                 ///< клиентский сертификат транзакции
  ::WORD     ClientTSN;                            ///< номер транзакции по клиентской карте
  char     ClientExpiryDate[MAX_DATE+1];           ///< срок действия карты клиента
} struct_out_cl1;


/** @brief  Структура для паспортных данных
 */
typedef struct tagTPassportData{
  char sFIO[MAX_FIO_N];    ///< ФИО
  char sAddr[58];          ///< Адрес
  short  sRes;             ///< Резидент/нерезидент
  char sDocType[21];       ///< Тип документа
  char sSer[11];           ///< Серия
  char sNum[16];           ///< Номер
  char sIssuer[58];        ///< Кем выдан
  char sWhen[13];          ///< Когда выдан
  char sValid[13];         ///< Срок действия
  char sEmit[42];          ///< Банк - эмитент карты
  char Agent[42];          ///< Банк, выдавший карту
} TPassportData;

/** @brief  Дополнительные данные для финансовой операции по Сберкарт
 */
typedef struct tag_struct_in_reserved1{
  DWORD size;                 ///< Размер структуры в байтах = sizeof(struct_in_reserved1)
  void  *Reserved1;           ///< Зарезервировано.
  DWORD Rand;                 ///< Случайное число для включения в сертификат (0<=Rand<100000)
  ::WORD  ColNum;             ///< Номер колонки при оплате бензина
  TPassportData PassportData; ///< Паспортные данные
  DWORD Department;           ///< Номер отдела (DEPARTMENT_NOT_SPECIFIED - выбор из меню)
} struct_in_reserved1;

/** @brief  Дополнительные результаты финансовой операции по Сберкарт
 */

typedef struct tag_struct_out_reserved1{
  DWORD size;                 ///< Размер структуры в байтах = sizeof(struct_out_reserved1)
  void  *Reserved1;           ///< Зарезервировано.
  ::BYTE  Cert[128];          ///< Сертификат операции.
  WORD  PurchCode;            ///< Код завершения операции
  DWORD PurchTrace;           ///< Трассировка завершения операции
  TPassportData PassportData; ///< Паспортные данные
} struct_out_reserved1;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_2 Информационные операции по СБЕРКАРТ
 * @brief Устаревший функционал. Не используется.
 */
/**@{*/
#ifdef __cplusplus
typedef struct tag_struct_in_cl2{
}struct_in_cl2;
#endif /*__cplusplus*/

typedef struct tag_struct_out_cl2{
  char  TrxDate[MAX_DATE+1];                    ///< дата операции  (ДД.ММ.ГГГГ)
  char  TrxTime[MAX_TIME+1];                    ///< время операции (ЧЧ:ММ:СС)
  char  ClientCard[MAX_CARD_NUM+1];             ///< номер карты клиента
  char  ClientBranch[MAX_BRANCH_NUM+1];         ///< код банка клиента
  char  ClientAccount[MAX_ACCOUNT_NUM+1];       ///< номер счета клиента
  char  ClientName[MAX_SBERCARD_CLIENT_NAME+1]; ///< фамилия клиента
  char  ClientCert[MAX_CERT+1];                 ///< клиентский сертификат транзакции
  char  ServerCert[MAX_CERT+1];                 ///< серверный сертификат транзакции
  ::WORD  ClientTSN;                            ///< номер транзакции по клиентской карте
  char  ClientExpiryDate [MAX_DATE+1];          ///< срок действия карты клиента
} struct_out_cl2;


/**
 * Структура для хранения данных о транзакциях на карте клиента (СБЕРКАРТ)
 * */
typedef struct tagTClientTrxData{
  short TSN;              ///< Номер операции на карте клиента
  char  Date[6];          ///< Дата операции
  char  Description[17];  ///< Описание
  DWORD Amount;           ///< Сумма операции
  WORD  Batch;            ///< Номер пакета карты терминала. Заполняется для операци списания средств.
  BYTE  BatchRSN;         ///< Номер транзакции в пакете карты терминала. Заполняется для операци списания средств.
  DWORD BalanceBefore;    ///< Баланс ЗСК карты до операции. Заполняется для операций перевода средств.
} TClientTrxData;

typedef struct tag_struct_out_reserved2{
  DWORD size;                ///< Размер структуры в байтах = sizeof(struct_out_reserved2)
  void  *Reserved1;          ///< Зарезервировано.
  DWORD SCABefore;           ///< SCA до операции
  DWORD HSSSBefore;          ///< HSSS до операции
  DWORD SCAAfter;            ///< SCA после операции
  DWORD HSSSAfter;           ///< HSSS после операции
  TClientTrxData TrxData[20];///< Данные об операциях с карты клиента
} struct_out_reserved2;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_3 Информационные операции по СБЕРКАРТ
 * @brief Устаревший функционал. Не используется.
 */
/**@{*/
#ifdef __cplusplus
typedef struct tag_struct_in_cl3{
} struct_in_cl3;
#endif /*__cplusplus*/

/** 
 *  @brief Запрос на проведение информационной операции
 */
typedef struct tag_struct_out_cl3{
  char  TrxDate[MAX_DATE+1];          ///< дата операции  (ДД.ММ.ГГГГ)
  char  TrxTime[MAX_TIME+1];          ///< время операции (ЧЧ:ММ:СС)
  char  MerchantCard[MAX_CARD_NUM+1]; ///< номер карты терминала
  char  MerchantName[MAX_NAME+1];     ///< название карты терминала
  char  MerchantCert[MAX_CERT+1];     ///< терминальный сертификат транзакции
  ::WORD  MerchantBatchNum;           ///< номер пакета терминала
  char  MerchantBatchDate[MAX_DATE+1];///< дата пакета терминала (ДД.ММ.ГГГГ)
  DWORD BatchAmount;                  ///< сумма пакета
  ::WORD  BatchTrx;                   ///< число операций в пакете
  char  ServerCert[MAX_CERT+1];       ///< серверный сертификат транзакции
}struct_out_cl3;

/** 
 *  @brief Ответ на проведение информационной операции
 */
typedef struct tag_struct_out_reserved3{
  DWORD size;             ///< Размер структуры в байтах = sizeof(struct_out_reserved3)
  void  *Reserved1;       ///< Зарезервировано.
  DWORD ClearTotal;       ///< SCA до операции
} struct_out_reserved3;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_4 Финансовые операции по картам
 */
/**@{*/

/** 
 *  @brief Запрос на проведение финансовой операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_cl4{
  DWORD Amount;                      ///<сумма операции, если 0 - запрашивается библиотекой
  char  Track2[MAX_TRACK2+1];        ///<Должен быть пуст, то есть содержать 0 в первом байте, в крайнем случае может содержать 2-ю дорожка карты
  ::BYTE  CardType;                  ///<тип карты. @see CardTypes
} struct_in_cl4;

#define SBER_OWN_CARD     'S' ///< Признак принадлежности карты Сбербанку. Присутствует в последнем байте поля struct_out_cl4::CardName.

/**
 *  @brief Ответ на проведение финансовой операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_cl4{
  DWORD AmountClear;                  ///< сумма операции без учета комиссии / скидки
  DWORD Amount;                       ///< сумма операции с учетом комиссии / скидки
  char  CardName[MAX_NAME+1];         ///< название карты (Visa, Maestro и т.д.)
  ::BYTE  CardType;                   ///< тип карты. @see CardTypes
  char  TrxDate[MAX_DATE+1];          ///< дата операции  (ДД.ММ.ГГГГ)
  char  TrxTime[MAX_TIME+1];          ///< время операции (ЧЧ:ММ:СС)
  char  TermNum[MAX_TERM+1];          ///< номер терминала
  char  MerchNum[MAX_MERCHANT_LN+1];  ///< номер мерчанта
  char  AuthCode[MAX_AUTH_CODE+1];    ///< код авторизации
  char  RRN[MAX_RRN+1];               ///< номер ссылки
  ::WORD  MerchantTSN;                ///< номер транзакции в пакете терминала
  ::WORD  MerchantBatchNum;           ///< номер пакета терминала по картам
  char  ClientCard[MAX_CARD_NUM+1];   ///< номер карты клиента
  char  ClientExpiryDate[MAX_DATE+1]; ///< срок действия карты клиента
} struct_out_cl4;

/** 
 *  @brief Структура для передачи товарной информации
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tagTGoodsData{
  DWORD   Price;                     ///< Цена за единицу, exp 2
  DWORD   Volume;                    ///< Количество exp 3
  char    Name[MAX_GOODS_NAME+1];    ///< Наименование товара
  char    Code[MAX_GOODS_CODE+1];    ///< Внутренний код учетной системы вызывающей программы
}TGoodsData;

/** 
 *  @brief Дополнительные параметры запроса на проведение финансовой операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_reserved4{
  DWORD size;                              ///< Размер структуры в байтах = sizeof(struct_in_reserved4)
  void  *Reserved1;                        ///< Зарезервировано
  char  RRN[MAX_RRN+1];                    ///< RRN
  DWORD RequestID;                         ///< Случайное число для включения в сертификат (0<=Rand<100000)
  DWORD Currency;                          ///< Код валюты. Если 0, выбирается из меню
  char  RecvCard[MAX_PAN_N+1];             ///< Номер карты получателя платежа
  BYTE  BinHash[20];                       ///< Хеш карты бинарный
  BYTE  HashFlags[5];                      ///< see HashFlagsMask
  TPassportData PassportData;              //</ Паспортные данные
  char  AuthCode[MAX_AUTH_CODE+1];         ///< Код авторизации для операции ввода слипа по Amex
  char  RecvValidDate[MAX_CARD_DATE+1];    ///< Срок действия карты получателя
  DWORD Department;                        ///< Номер отдела (DEPARTMENT_NOT_SPECIFIED - выбор из меню)
  char  PaymentTag[MAX_PAYMENT_TAG+1];     ///< Тег платежной системы
  char  TagValue  [MAX_PAYMENT_VALUE+1];   ///< Значение тега платежной системы
  char  CashierFIO[MAX_FIO_N+1];           ///< ФИО кассира
  char  TextMessage[MAX_TEXT_MESSAGE];     ///< Текстовое сообщение. Используется для команд для 5005/5006 (на дисплей) и 4011/4012 (на чек) операций
  TGoodsData GoodsData;                    ///< ИНформация о товаре. @see TGoodsData
} struct_in_reserved4;

/** @brief Тип считывания карты
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef enum {
 CARD_ENTRY_DEVICE             =  'D',
 CARD_ENTRY_MANUAL             =  'M', ///< Ручной ввод номера карты
 CARD_ENTRY_CHIP               =  'C', ///< Считан чип карты
 CARD_ENTRY_FALLBACK           =  'F', ///< Карта считана в режиме Fallback
 CARD_ENTRY_VIVO_MS            =  'R', ///< Карта считана бесконтактно
 CARD_ENTRY_VIVO_EMV           =  'E', ///< Карта считана бесконтактно
 CARD_ENTRY_BY_ID              =  'I', ///< Выполнен рекурентный платеж
} CARD_ENTRY_MODE;

/** 
 *  @brief Опциональные параметры ответа на проведение финансовой операции
 * 
 * Поле struct_out_reserved4::TrxFlags принимает следующие значения.
 *	 
 * Три младших бита младшего байта содержат способ чтения карты.
 *  
 * - 0 – магнитный ридер;
 * - 1 – ручной ввод номера карты;        
 * - 2 - чиповый ридер;                 
 * - 3 - на карте есть чип, но она считана через магнитный ридер;                 
 * - 4 – бесконтактная карта с эмуляцией магнитной полосы;                      
 * - 5 – бесконтактная карта с эмуляцией чипа;        
 * - 6 – введен идентификатор клиента.
 *
 * остальные биты:        
 * - 0x00008000L - Была проведена биоверификация держателя карты;
 * - 0x00010000L - Введен online pin;         
 * - 0x00020000L - Введен offline pin;         
 * - 0x00040000L - Операция без верификации держателя.
 *            
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_reserved4{
  DWORD size;                  ///< Размер структуры в байтах = sizeof(struct_out_reserved4)
  void  *Reserved1;            ///<Зарезервировано.
  ::BYTE  Cert[128];           ///<Сертификат операции или Track2.
  TPassportData PassportData;  ///<Паспортные данные
  BYTE  IsOwn;                 ///<Признак Карта Сбербанка
  DWORD Currency;              ///<Код валюты: 643 - рубли, 840 - доллары США
  DWORD TrxFlags;              ///<Флаги проведеной операции. Для PCI-DSS версии поле заполняется только если RequestID<0.
  DWORD RequestID;             ///<Идентификатор операции, Заполняется для PCI-DSS версии. @see \ref page5 "RequestID".
  char  CardEntryMode;         ///<Тип чтения карты @see CARD_ENTRY_MODE
  char  AID[33];               ///<AID карты. Не используется для PCI-DSS версии.
  BYTE  LltID;                 ///<Идентификатор программы лояльности
} struct_out_reserved4;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_5 Информационные операции по банковским картам и картам лояльности
 */
/**@{*/
/** 
 *  @brief Запрос на проведение информационной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_cl5{
  char  Track2[MAX_TRACK2+1];         ///< 2-я дорожка карты, если пустая - считывается библиотекой
  ::BYTE  CardType;                   ///<тип карты. @see CardTypes
} struct_in_cl5;

/** 
 *  @brief Ответ на проведение информационной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_cl5{
  char  CardName[MAX_NAME+1];         ///< название карты
  ::BYTE  CardType;                   ///<тип карты. @see CardTypes
  char  TrxDate[MAX_DATE+1];          ///< дата операции  (ДД.ММ.ГГГГ)
  char  TrxTime[MAX_TIME+1];          ///< время операции (ЧЧ:ММ:СС)
  char  TermNum[MAX_TERM+1];          ///< номер терминала
  char  ClientCard[MAX_CARD_NUM+1];   ///< номер карты клиента
  char  ClientExpiryDate [MAX_DATE+1];///< срок действия карты клиента
} struct_out_cl5;

typedef enum {
 TVM_FULL        = 0x01, ///< Показать текст полностью
 TVM_NEXT        = 0x02, ///< Показать кнопку пролистывания текста "Вперед"
 TVM_PREV        = 0x04, ///< Показать кнопку пролистывания текста "Назад"
 TVM_NEED_TRACK1 = 0x08, ///< Проверка корректности Track1
 TVM_NEED_PHONE  = 0x10, ///< Ввод телефонного номера
 TVM_NEED_EMAIL  = 0x20, ///< Ввод адреса электронной почты
} EditModeMask;

typedef enum {
 INFOPF_NONE             = 0x00, ///< Флаги не выставлены
 INFOPF_NEGATIVE_BALANCE = 0x01, ///< Значение баланса карты отрицательно
} InfoOperationFlags;

/** 
 *  @brief Дополнительные параметры запроса на проведение информационной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_in_reserved5{
  DWORD size;                          ///< Размер структуры в байтах = sizeof(struct_in_reserved5)
  BYTE  bEditMode;                     ///< @see EditModeMask
  char  TextMessage[MAX_TEXT_MESSAGE]; ///< Текстовое сообщение для вывода экран или на чек (в зависимости от операции)
  char  CashierFIO[MAX_FIO_N+1];       ///< Ф.И.О. кассира
} struct_in_reserved5;

/** 
 *  @brief Опциональные параметры ответа на проведение информационной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_reserved5{
  DWORD size;                          ///< Размер структуры в байтах = sizeof(struct_out_reserved5)
  void  *Reserved1;                    ///< Зарезервировано.
  BYTE  Hash    [MAX_HASH];            ///< Хеш номера карты, бинарное значение
  BYTE  CardData[MAX_TRACK2];          ///< Track2 карты, либо маскированный номер карты

  DWORD Balance;                       ///< Баланс карты
  DWORD Currency;                      ///< Код валюты: 643 - рубли, 840 - доллары США

  DWORD CardLCDataLen;                 ///< Длинна данных подписи клиента
  BYTE  CardLCData[MAX_CARD_LS_DATA];  ///< Данные подписи клиента [X0=8 байт][Y0=8 байт][X1...
  char  AuthCode[6+1];                 ///< Код авторизации
  char  ClientName[MAX_CLIENT_NAME];   ///< Имя держателя карты
  BYTE  IsOwn;                         ///< Флаг "Карта выпущена Сбербанком"
  BYTE  LltID;                         ///< Идентификатор программы лояльности
  BYTE  Flags;                         ///< Флаги операций 5000 @see 
} struct_out_reserved5;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_6 Cлужебные операции по картам
 */
/**@{*/
/** 
 *  @brief Запрос на проведение служебной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-6000-6999
 */
typedef struct tag_struct_in_cl6{
  DWORD  Currency;                     ///<Код валюты: 810 - рубли, 840 - доллары США
} struct_in_cl6;

/** 
 *  @brief Ответ на проведение служебной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-6000-6999
 */
typedef struct tag_struct_out_cl6{
  char  TrxDate[MAX_DATE+1];         ///< дата операции  (ДД.ММ.ГГГГ)
  char  TrxTime[MAX_TIME+1];         ///< время операции (ЧЧ:ММ:СС)
  char  TermNum[MAX_TERM+1];         ///< номер терминала
  char  MerchNum[MAX_MERCHANT_LN+1]; ///< номер мерчанта
  ::WORD  MerchantBatchNum;          ///< номер пакета терминала по магн.картам
  DWORD DebitNumber;                 ///< число операций выдачи наличных/оплаты покупки
  DWORD DebitTotal;                  ///< сумма операций выдачи наличных/оплаты покупки
  DWORD DebitClearTotal;             ///< сумма операций выдачи наличных/оплаты покупки без учета комиссии/скидки
  ::WORD  ReturnNumber;              ///< число операций возврата/безналичного перевода
  DWORD ReturnTotal;                 ///< сумма кредитовых операций
  DWORD ReturnClearTotal;            ///< сумма кредитовых операций без учета комиссии
} struct_out_cl6;

/** 
 *  @brief Дополнительные параметры запроса на проведение служебной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-6000-6999
 */
typedef struct tag_struct_in_reserved6{
  DWORD size;                       ///< Размер структуры в байтах = sizeof(struct_in_reserved6)
  void  *Reserved1;                 ///< Зарезервировано.
  char  CashierCard[MAX_TRACK2+1];  ///< Карта кассира
  WORD  MerchantTSN;                ///< номер транзакции в пакете терминала
  WORD  MerchantBatchNum;           ///< номер пакета терминала по магн.картам
  DWORD dwAmount;                   ///< Сумма для suspend/commit/rollback
  char  AuthCode[MAX_AUTH_CODE+1];  ///< Код авторизации для suspend/commit/rollback
  char  AmountCashTotal[MAX_CASH_STATS+1];///< сумма оплат наличными, значение в копейках
  char  CountCashTotal[MAX_CASH_STATS+1]; ///< количество оплат наличными
  char  ShiftIndexTotal[MAX_RRN+1]; ///< идентификатор смены
  DWORD dwQueryRequestID;           ///< Идентификатор финансовой операции
  const char* MerchantCheque;       ///< Содержимое торгового чека в кодировке CP866
} struct_in_reserved6;
/**@}*/

/**
 * @defgroup KERNEL_DESCRIPTOR_7 Общие служебные операции
   @brief Функции этого класса позволяют получать информацию о ранее выполненых операциях и отчетах, таких как сводный чек и контрольная лента.
 */
/**@{*/

#ifdef __cplusplus
typedef struct tag_struct_in_cl7{
} struct_in_cl7;

typedef struct tag_struct_out_cl7{
} struct_out_cl7;
#endif /*__cplusplus*/

/** 
 *  @brief Дополнительные параметры запроса на проведение служебной операции
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-7000-7999
 */
typedef struct tag_struct_in_reserved7{
  DWORD  size;        ///< размер структуры sizeof(struct_in_reserved7)
  int    RecordID;    ///< номер записи в журнале. 0, struct_out_reserved7::TBatchRecord::NextRecId,... пока не вернется ошибка ::ERR_PIL_NO_FILES
} struct_in_reserved7;

/** 
 *  @brief Структура для хранения данных о транзакциях в пакете терминала
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-7000-7999
 */
typedef struct tagTBatchRecord{
  BYTE    TrxType;
  DWORD   AmountClear; // сумма операции без учета комиссии / скидки
  DWORD   Amount;      // сумма операции с учетом комиссии / скидки
  char    CardName[MAX_NAME+1];       // название карты (Visa, Maestro и т.д.)
  ::BYTE  CardType;                        ///<тип карты. @see CardTypes
  char    TrxDate[MAX_DATE+1];        // дата операции  (ДД.ММ.ГГГГ)
  char    TrxTime[MAX_TIME+1];        // время операции (ЧЧ:ММ:СС)
  char    AuthCode[MAX_AUTH_CODE+1];  // код авторизации
  char    RRN[MAX_RRN+1];             //номер ссылки
  ::WORD  MerchantTSN;                // номер транзакции в пакете терминала
  ::WORD  MerchantBatchNum;           // номер пакета терминала по магн.картам
  char    ClientCard[MAX_CARD_NUM+1];   // номер карты клиента
  char    ClientExpiryDate[MAX_DATE+1]; // срок действия карты клиента
  ::BYTE  Hash[MAX_HASH];
  int     NextRecId;
} TBatchRecord;

/** 
 *  @brief Опциональный ответ на проведение служебной операции
 *  @note Используется в операциях KO_GET_OPERATION_INFO и KO_GET_TERMINAL_AND_MERCHENT_ID
 *  @see  Docs/Для разработчиков ПО ККМ/examples/gate.dll/cpp/functions-4000-5999
 */
typedef struct tag_struct_out_reserved7{
  DWORD         size;                        ///< Размер структуры в байтах = sizeof(struct_out_reserved7)
  char          TermNum[MAX_TERM+1];         ///< номер терминала
  char          MerchNum[MAX_MERCHANT_LN+1]; ///< номер мерчанта
  TBatchRecord  Record;                      ///< операция из журнала.
} struct_out_reserved7;
/**@}*/

typedef enum {
  USE_BINARY_HASH   = 0x01,    ///< В запросе присутствует хеш карты в бинарном представлении
  NO_PIN_BYPASS     = 0x02,    ///< Запрет отказа от ввода пин кода.
  KILL_SECOND_COPY  = 0x04,    ///< Оставлять только одну копию чека в ответе (в файле p по прежнему останется заданое число копий чека)
  CLIENT_SKIPPED_PIN= 0x08,    ///< Не печатать на чека поле "Подпись клиента". Например, если клиент верифицирован иным образом.
} HashFlagsMask;

/** 
 *  @brief  Входной аргумент функции ::call_sb_kernel
 *  @note   Параметр Reserved должен иметь значение NULL, если противное специально не оговари-вается в описании конкретной функции. 
 */
typedef struct tagInArg{
  void *Reserved;        /**< [in]  Дополнительные параметры операции. Тип передаваемого параметра зависит от номера выполняемой операции functionNumber функции ::call_sb_kernel. Обычно содержит указатель на структуру типа struct_in_reservedX, где X – номер класса */
  void *in_struct;       /**< [in]  Параметр in_struct – указатель на буфер типа struct_in_clX, где X – номер класса , для функций ненулевого класа, или указатель на буфер типа struct_inX, где X – номер функции класса 0, или NULL, если функция не принимает параметров. */
} InArg;

/** 
 *  @brief  Выходной аргумент функции ::call_sb_kernel
 *  @note   Параметр Reserved должен иметь значение NULL, если противное специально не оговари-вается в описании конкретной функции. 
 */
typedef struct tagOutArg{
  DWORD ErrorCode;        /**< [in]  Код ошибки*/
  DWORD Flags;            /**< [in]  см. GateOutFlags*/
  void* Reserved;         /**< [in]  Дополнительные параметры операции. Тип передаваемого параметра зависит от номера выполняемой операции functionNumber функции ::call_sb_kernel*/
  void* out_struct;       /**< [in]  Указатель на буфер типа struct_in_clX, где X – номер класса , для функций ненулевого класа, или указатель на буфер типа struct_inX, где X – номер функции класса 0, или NULL, если функция не принимает параметров. */
} OutArg;

#pragma pack()

/**
 * @defgroup KERNEL_OPERATIONS Операции ядра sb_kernel
 */
/**@{*/

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

/** Поясняющие флаги для обработки выходных параметров*/
typedef enum {
  F_HAVE_DOCUMENT = 0x00000001,  ///< Вызывающая программа должна выполнить печать документа с помощью специальных функций KO_SIZE_OF_SLIP и KO_GET_SLIPS_LIST
  F_CRITICAL      = 0x00000002,  /**< Вызывающая программа после печати документа должна убедиться, что он напечатан успешно. При
                                       *   необходимости запросив подтверждение у пользователя. Устанавливается только совместно с F_HAVE_DOCUMENT.
                                       */
  F_HAVE_MONMSG   = 0x00000004,  ///< Вызывающая программа должна выполнить чтение сообщения системы мониторинга с помощью специальной функций 22
} GateOutFlags;

/** @brief Запуск операции UPOS
 *  @note 
 *  @param[in] functionNumber Тип/номер операции. См. ::KernelOperation.
 *  @param[in] in_arg         Указатель на входной аргумент. См. ::InArg
 *  @param[in/out] out_arg        Указатель на выходной аргумент. См. ::OutArg
 *  @return DWORD Код ошибки. Если функция вернула 0, необходимо проанализировать поле OutArg::ErrorCode.
*/
  GATE_API DWORD call_sb_kernel(DWORD  func, void   *in_arg,  void   *out_arg);

/** @brief Возвращает версию ядра sb_kernel.dll
 *  @return DWORD Версия как целое число в формате 0x00VVRRBB. VV - версия, RR - релиз, BB - сборка
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
