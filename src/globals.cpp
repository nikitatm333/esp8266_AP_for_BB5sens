#include "../inc/globals.h"

float CurrentTemp = 0.0;
String LastRecvd = "none yet";
MyDATA_36 D36;
int TShead = 0;
int TStail = 0;
float TStorage[L];
int LLL = 0;

char ap_ssid[32] = "ESP_AP";         // Максимальная длина SSID = 32 символа
char ap_password[32] = "12345678";   // Максимальная длина пароля = 32 символа
