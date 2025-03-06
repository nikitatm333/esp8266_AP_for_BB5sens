#include "../inc/settings.h"
#include <EEPROM.h>
#include "../inc/globals.h"

#define EEPROM_SIZE 512 // Размер энергонезависимой памяти
#define SETPOINT_ADDR 0
#define LASTRECVD_ADDR 32  // Отступ от `SetPoint`, строка начнется с 32 байта
#define AP_SSID_ADDR 100    // Адрес для хранения SSID
#define AP_PASS_ADDR 132    // Адрес для хранения пароля (на 32 байта дальше)

// Значение по умолчанию
float SetPoint = 36.6;


void saveSettingsTemp() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(SETPOINT_ADDR, SetPoint);
  EEPROM.commit();
  EEPROM.end();
}

void loadSettingsTemp() {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(SETPOINT_ADDR, SetPoint);
    EEPROM.end();
    if (SetPoint < -20.0 || SetPoint > 100.0) {
      SetPoint = 36.6;
    }
  }
  
void saveLastRecvd() {
  EEPROM.begin(EEPROM_SIZE);
  for (size_t i = 0; i < LastRecvd.length(); i++) {  
    EEPROM.write(LASTRECVD_ADDR + i, LastRecvd[i]);
  }
  EEPROM.write(LASTRECVD_ADDR + LastRecvd.length(), '\0');  // Завершающий символ
  EEPROM.commit();
  EEPROM.end();
}


void loadLastRecvd() {
  EEPROM.begin(EEPROM_SIZE);
  char buffer[64];
  for (int i = 0; i < 64; i++) {
    buffer[i] = EEPROM.read(LASTRECVD_ADDR + i);
    if (buffer[i] == '\0') break;
  }
  EEPROM.end();
  
  // Если первый символ равен 0xFF, считаем, что данные не сохранены
  if ((unsigned char)buffer[0] == 0xFF) {
    LastRecvd = "none yet";
  } else {
    LastRecvd = String(buffer);
  }
}

// Сохранение данных для режима AP
void saveAPSettings() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(AP_SSID_ADDR, ap_ssid);
  EEPROM.put(AP_PASS_ADDR, ap_password);
  EEPROM.commit();
  EEPROM.end();
}

void loadAPSettings() {
  EEPROM.begin(512);
  EEPROM.get(AP_SSID_ADDR, ap_ssid);
  EEPROM.get(AP_PASS_ADDR, ap_password);
  EEPROM.end();

  // Проверка и установка значений по умолчанию
  if(ap_ssid[0] == 0xFF || ap_ssid[0] == '\0') {
    strncpy(ap_ssid, "ESP_AP", sizeof(ap_ssid)-1);
  }
  ap_ssid[sizeof(ap_ssid)-1] = '\0';
  ap_password[sizeof(ap_password)-1] = '\0';
}