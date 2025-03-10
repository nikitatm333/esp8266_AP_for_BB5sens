#include "../inc/serial_processing.h"
#include "../inc/globals.h"
#include "../inc/settings.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>  // для strcmp
#include <ESP8266WiFi.h>



// Функция, вызываемая при получении строки по UART
static void Process_pline(char *s) {
  // Если получена команда "RESET"
  if (strcmp(s, "RESET") == 0) {
    Serial.println("Получена команда RESET. Выполняется очистка EEPROM и переход в режим AP.");
    clearEEPROM();
    // Устанавливаем новые параметры точки доступа:
    // ap_ssid и ap_password определяются в settings.h/globals.h
    strncpy(ap_ssid, "ESP_RESET", sizeof(ap_ssid) - 1);
    ap_ssid[sizeof(ap_ssid) - 1] = '\0';
    ap_password[0] = '\0';  // пароль пустой
    saveAPSettings();       // сохраняем новые настройки в EEPROM

    // Переключаем режим работы WiFi на точку доступа (AP)
    // WiFi.mode(WIFI_AP);
    // WiFi.softAP(ap_ssid);
    Serial.println("Режим AP установлен с SSID: ESP_RESET, без пароля");
    return;
  }
  // if (strncmp(s, "T=", 2) == 0) {
  //   float t;
  //   if (sscanf(s + 2, "%f", &t) == 1) {
  //     // Обновляем уставку температуры и LastRecvd
  //     SetPoint = t;
  //     LastRecvd = "Установить " + String(t) + " °C";
  //     Serial.println("Получена команда установки температуры: " + String(t));
  //     // Можно дополнительно сохранить настройки в EEPROM, если требуется:
  //     saveSettingsTemp();
  //     saveLastRecvd();
  //     return;
  //   }
  // }

  // Если строка не содержит команду RESET, продолжаем разбор строки с температурными данными:
  sscanf(s, "%f %f %f %f %f %f %hd %hd %hd %hd %c%c%c%c",
         &D36.T[0], &D36.T[1], &D36.T[2], &D36.T[3], &D36.T[4], &D36.T[5],
         &D36.CV[0], &D36.CV[1], &D36.CV[2], &D36.CV[3],
         &D36.status[0], &D36.status[1], &D36.status[2], &D36.status[3]);

  CurrentTemp = D36.T[4];

  // Сохраняем значение температуры в историю для графика
  TStorage[TShead] = CurrentTemp;
  TShead = (TShead + 1) % L;
  if (TShead == TStail) {
    TStail = (TStail + 1) % L;
  }
}


void processSerialData() {
  static char in_s[256];
  static uint8_t in_p = 0;
  
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (in_p > 0) {
        in_s[in_p] = '\0';
        Process_pline(in_s);
        in_p = 0;
      }
    } else {
      if (in_p < sizeof(in_s) - 1) {
        in_s[in_p++] = c;
      }
    }
  }
}
