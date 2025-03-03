#ifndef SETTINGS_H
#define SETTINGS_H

void loadSettings();
void saveSettings();
void saveLastRecvd();
void loadLastRecvd();

extern float SetPoint;  // Глобальная установочная температура

#endif // SETTINGS_H
