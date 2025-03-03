#include "../inc/settings.h"
#include <EEPROM.h>
#include "../inc/globals.h"

#define EEPROM_SIZE 512
#define SETPOINT_ADDR 0
#define LASTRECVD_ADDR 32  // Отступ от `SetPoint`, строка начнется с 32 байта


// Значение по умолчанию
float SetPoint = 36.6;

void saveSettings() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(SETPOINT_ADDR, SetPoint);
  EEPROM.commit();
  EEPROM.end();
}

// void loadSettings() {
//   EEPROM.begin(EEPROM_SIZE);
//   EEPROM.get(SETPOINT_ADDR, SetPoint);
//   EEPROM.end();
//   if (SetPoint < -20.0 || SetPoint > 100.0) {
//     SetPoint = 36.6;
//   }
// }

void loadSettings() {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(SETPOINT_ADDR, SetPoint);
    EEPROM.end();
    if (SetPoint < -20.0 || SetPoint > 100.0) {
      SetPoint = 36.6;
    }
  }
  
  void saveLastRecvd() {
    EEPROM.begin(EEPROM_SIZE);
    for (size_t i = 0; i < LastRecvd.length(); i++) {  // <-- заменили int на size_t
      EEPROM.write(LASTRECVD_ADDR + i, LastRecvd[i]);
    }
    EEPROM.write(LASTRECVD_ADDR + LastRecvd.length(), '\0');  // Завершающий символ
    EEPROM.commit();
    EEPROM.end();
  }
  
  
  void loadLastRecvd() {
    EEPROM.begin(EEPROM_SIZE);
    char buffer[64];  // Буфер для строки
    for (int i = 0; i < 64; i++) {
      buffer[i] = EEPROM.read(LASTRECVD_ADDR + i);
      if (buffer[i] == '\0') break;  // Конец строки
    }
    EEPROM.end();
    
    LastRecvd = String(buffer);
    if (LastRecvd.length() == 0) {
      LastRecvd = "none yet";
    }
  }