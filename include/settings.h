#ifndef SETTINGS_H
#define SETTINGS_H

void loadSettingsTemp();
bool saveSettingsTemp();
bool saveLastRecvd();
void loadLastRecvd();
void saveAPSettings();
void loadAPSettings();
void clearEEPROM();


#define EEPROM_SIZE 512 
#define SETPOINT_ADDR 0
#define LASTRECVD_ADDR 32  // Отступ от `SetPoint`, строка начнется с 32 байта
#define AP_SSID_ADDR 100    // Адрес для хранения SSID
#define AP_PASS_ADDR 132    // Адрес для хранения пароля (на 32 байта дальше)


extern float SetPoint;  // Глобальная установочная температура

#endif // SETTINGS_H
