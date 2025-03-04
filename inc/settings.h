#ifndef SETTINGS_H
#define SETTINGS_H

void loadSettingsTemp();
void saveSettingsTemp();
void saveLastRecvd();
void loadLastRecvd();
void saveAPSettings();
void loadAPSettings();

extern float SetPoint;  // Глобальная установочная температура

#endif // SETTINGS_H
