/**
 * @file serial_processing.cpp
 * @brief Обработка данных, поступающих через UART.
 * 
 * @details Этот файл содержит функции обработки команд, полученных через последовательный порт (UART).
 * Реализовано распознавание команды `RESET`, а также обработка температурных данных.
 */

 #include "serial_processing.h"
 #include "globals.h"
 #include "settings.h"
 #include <Arduino.h>
 #include <stdio.h>
 #include <string.h>  // для strcmp
 #include <ESP8266WiFi.h>
 
 /**
  * @brief Обрабатывает строку, полученную по UART.
  * 
  * @param[in] s Указатель на строку с данными.
  * 
  * @details Если строка содержит команду `"RESET"`, выполняется очистка EEPROM и переход в режим точки доступа (AP).
  * В противном случае производится разбор температурных данных и обновление глобальных переменных.
  */
 static void Process_pline(char *s) {
    // Если получена команда "RESET"
    if (strcmp(s, "RESET") == 0) {
        Serial.println("RESET command received. Clearing EEPROM and switching to AP mode.");
        clearEEPROM();

        // Устанавливаем новые параметры точки доступа:
        strncpy(ap_ssid, "ESP_RESET", sizeof(ap_ssid) - 1);
        ap_ssid[sizeof(ap_ssid) - 1] = '\0';
        ap_password[0] = '\0';  // Устанавливаем пустой пароль.
        saveAPSettings();       // Сохраняем новые настройки в EEPROM.

        Serial.println("AP mode set with SSID: ESP_RESET, no password");
        return;
    }

    // Разбор строки с температурными данными:
    int n = sscanf(s, "%f %f %f %f %f %f %hd %hd %hd %hd %c%c%c%c",
                   &D36.T[0], &D36.T[1], &D36.T[2], &D36.T[3], &D36.T[4], &D36.T[5],
                   &D36.CV[0], &D36.CV[1], &D36.CV[2], &D36.CV[3],
                   &D36.status[0], &D36.status[1], &D36.status[2], &D36.status[3]);

    // Если количество успешно прочитанных элементов меньше 14 – игнорируем строку.
    if (n < 14) {
        Serial.println("Invalid UART data received, ignoring.");
        return;
    }

    CurrentTemp = D36.T[4];  // Обновляем текущую температуру.

    // Сохраняем значение температуры в историю для графика
    TStorage[TShead] = CurrentTemp;
    TShead = (TShead + 1) % L;
    if (TShead == TStail) {
        TStail = (TStail + 1) % L;
    }
}

 
 /**
  * @brief Читает и обрабатывает входные данные из UART.
  * 
  * @details Функция проверяет наличие новых данных в UART и накапливает их в буфер.
  * Когда получен символ `'\n'` или `'\r'`, буфер передаётся в `Process_pline()` для обработки.
  */
 void processSerialData() {
     static char in_s[256];  ///< Буфер для входящей строки.
     static uint8_t in_p = 0;  ///< Индекс текущей позиции в буфере.
 
     while (Serial.available() > 0) {
         char c = Serial.read();
         if (c == '\n' || c == '\r') {
             if (in_p > 0) {
                 in_s[in_p] = '\0';  ///< Завершаем строку нулевым символом.
                 Process_pline(in_s);
                 in_p = 0;
             }
         } else {
             if (in_p < sizeof(in_s) - 1) {
                 in_s[in_p++] = c;  ///< Добавляем символ в буфер.
             }
         }
     }
 }
 