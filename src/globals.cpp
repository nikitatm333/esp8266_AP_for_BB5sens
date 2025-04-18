/**
 * @file globals.cpp
 * @brief Глобальные переменные, используемые в проекте.
 * 
 * @details Этот файл содержит определения глобальных переменных, которые используются в различных частях программы.
 */

 #include "globals.h"

 float CurrentTemp = 0.0;  ///< Текущая температура, полученная с датчика.
 
 String LastRecvd = "none yet";  ///< Последняя полученная команда.
 
 MyDATA_36 D36;  ///< Структура данных для хранения информации (описание структуры в `globals.h`).
 
 int TShead = 0;  ///< Указатель на начало данных в буфере хранения температуры.
 int TStail = 0;  ///< Указатель на конец данных в буфере хранения температуры.
 
 float TStorage[L];  ///< Буфер хранения значений температуры (размер L определяется в `globals.h`).
 
 int LLL = 0;  ///< Дополнительная переменная (назначение не уточнено).
 
 char ap_ssid[32] = "ESP_AP";  ///< SSID точки доступа (максимальная длина 32 символа).
 char ap_password[32] = "12345678";  ///< Пароль точки доступа (максимальная длина 32 символа).
