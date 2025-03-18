/**
 * @file settings.cpp
 * @brief Управление настройками системы, включая работу с EEPROM.
 *
 * @details Файл содержит функции для сохранения и загрузки параметров работы системы,
 * включая уставку температуры, последние полученные данные и параметры Wi-Fi AP.
 */

 #include "settings.h"
 #include <EEPROM.h>
 #include "globals.h"
 
 #define EEPROM_SIZE 512   ///< Размер энергонезависимой памяти EEPROM.
 #define SETPOINT_ADDR 0   ///< Адрес хранения уставки температуры.
 #define LASTRECVD_ADDR 32 ///< Адрес хранения последнего полученного сообщения.
 #define AP_SSID_ADDR 100  ///< Адрес хранения SSID точки доступа.
 #define AP_PASS_ADDR 132  ///< Адрес хранения пароля точки доступа.
 
 /// Значение температуры по умолчанию.
 float SetPoint = 36.6;
 
 /**
  * @brief Сохраняет уставку температуры в EEPROM.
  */
 bool saveSettingsTemp() {
     EEPROM.begin(EEPROM_SIZE);
     EEPROM.put(SETPOINT_ADDR, SetPoint);
     bool res = EEPROM.commit();
     EEPROM.end();
     return res;
 }
 
 /**
  * @brief Загружает уставку температуры из EEPROM.
  * 
  * @details Проверяет корректность загруженного значения, если оно выходит за
  * допустимые границы (-20°C...100°C), то устанавливает значение по умолчанию.
  */
 void loadSettingsTemp() {
     EEPROM.begin(EEPROM_SIZE);
     EEPROM.get(SETPOINT_ADDR, SetPoint);
     EEPROM.end();
     if (SetPoint < -20.0 || SetPoint > 100.0) {
         SetPoint = 36.6;
     }
 }
 
 /**
  * @brief Сохраняет последнее полученное сообщение в EEPROM.
  */
 bool saveLastRecvd() {
     EEPROM.begin(EEPROM_SIZE);
     for (size_t i = 0; i < LastRecvd.length(); i++) {
         EEPROM.write(LASTRECVD_ADDR + i, LastRecvd[i]);
     }
     EEPROM.write(LASTRECVD_ADDR + LastRecvd.length(), '\0'); ///< Завершающий символ.
     bool res = EEPROM.commit();
     EEPROM.end();
     return res;
 }
 
 /**
  * @brief Загружает последнее полученное сообщение из EEPROM.
  * 
  * @details Если в памяти обнаружено значение `0xFF`, то данные считаются несохранёнными,
  * и устанавливается значение "none yet".
  */
 void loadLastRecvd() {
     EEPROM.begin(EEPROM_SIZE);
     char buffer[64];
     for (int i = 0; i < 64; i++) {
         buffer[i] = EEPROM.read(LASTRECVD_ADDR + i);
         if (buffer[i] == '\0') break;
     }
     EEPROM.end();
 
     // Проверяем, есть ли данные в EEPROM
     if ((unsigned char)buffer[0] == 0xFF) {
         LastRecvd = "none yet";
     } else {
         LastRecvd = String(buffer);
     }
 }
 
 /**
  * @brief Сохраняет настройки точки доступа Wi-Fi в EEPROM.
  */
 void saveAPSettings() {
     EEPROM.begin(EEPROM_SIZE);
     EEPROM.put(AP_SSID_ADDR, ap_ssid);
     EEPROM.put(AP_PASS_ADDR, ap_password);
     EEPROM.commit();
     EEPROM.end();
 }
 
 /**
  * @brief Загружает настройки точки доступа Wi-Fi из EEPROM.
  * 
  * @details Если SSID не установлен или содержит `0xFF`, устанавливается значение по умолчанию.
  */
 void loadAPSettings() {
     EEPROM.begin(EEPROM_SIZE);
     EEPROM.get(AP_SSID_ADDR, ap_ssid);
     EEPROM.get(AP_PASS_ADDR, ap_password);
     EEPROM.end();
 
     // Проверяем корректность загруженных данных
     if (ap_ssid[0] == 0xFF || ap_ssid[0] == '\0') {
         strncpy(ap_ssid, "ESP_AP", sizeof(ap_ssid) - 1);
     }
     ap_ssid[sizeof(ap_ssid) - 1] = '\0';
     ap_password[sizeof(ap_password) - 1] = '\0';
 }
 
 /**
  * @brief Очищает EEPROM, записывая во всю память значение `0xFF`.
  * 
  * @details Использует `yield()` или `delay(1)` для предотвращения зависания системы во время записи.
  */
 void clearEEPROM() {
     EEPROM.begin(EEPROM_SIZE);
     for (int i = 0; i < EEPROM_SIZE; i++) {
         EEPROM.write(i, 0xFF);
         if (i % 32 == 0) {
             yield(); ///< Позволяет системе выполнять фоновые задачи.
         }
     }
     EEPROM.commit();
     EEPROM.end();
     Serial.println("EEPROM cleared!");
 }
 