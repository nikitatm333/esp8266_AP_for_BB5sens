#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

// Глобальная установочная температура хранится в settings
extern float SetPoint;      // Определена в settings.cpp

// Текущая температура с центрального датчика
extern float CurrentTemp;

// Последняя принятая команда
extern String LastRecvd;

// Структура для хранения данных от АЧТ
typedef struct {
  float T[6];       // Температуры
  int16_t CV[4];    // Управляющие значения (PID-регуляторы)
  uint8_t status[4];// Статус
} MyDATA_36;
extern MyDATA_36 D36;

// Глобальные переменные для графика
#define L 100      // Размер буфера истории
#define HScale 3   // Горизонтальный масштаб графика
#define Scale 4    // Вертикальный масштаб графика

extern int TShead;
extern int TStail;
extern float TStorage[L];
extern int LLL;

#endif // GLOBALS_H
