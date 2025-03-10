/**
 * @file temp_control.cpp
 * @brief Обработка достижения установочной температуры.
 *
 * @details Данный модуль отслеживает, когда текущая температура стабилизируется 
 * в пределах допустимого отклонения от заданной уставки (SetPoint). 
 * Если температура удерживается в пределах допуска ±0.05°C в течение 20 секунд, 
 * считается, что цель достигнута. Если температура выходит за пределы допуска 
 * более чем на 10 секунд, цель считается потерянной.
 */

 #include "../inc/temp_control.h"
 #include "../inc/globals.h"
 #include "../inc/settings.h"
 #include <Arduino.h>
 #include <math.h>
 
 /// Время начала нахождения температуры в пределах допуска.
 unsigned long targetAchievedStart = 0;
 /// Время начала отклонения температуры после достижения уставки.
 unsigned long targetLostStart = 0;
 /// Флаг, указывающий, достигнута ли уставка температуры.
 bool targetReached = false;
 
 /**
  * @brief Проверяет, удерживается ли температура в пределах уставки.
  * 
  * @details Алгоритм работает следующим образом:
  * - Если температура находится в диапазоне ±0.05°C от уставки 20 секунд, цель считается достигнутой.
  * - Если температура выходит за пределы допуска и остаётся там более 10 секунд, цель считается потерянной.
  */
 void updateTargetStatus() {
     float error = fabs(SetPoint - CurrentTemp);
     unsigned long now = millis();
 
     if (error <= 0.05) {
         if (!targetReached) {
             if (targetAchievedStart == 0) {
                 targetAchievedStart = now; ///< Фиксация начала стабильного состояния.
             }
             if (now - targetAchievedStart >= 20000) { ///< 20 секунд стабильности.
                 targetReached = true;
                 targetLostStart = 0; ///< Сброс таймера потери достижения.
             }
         } else {
             targetLostStart = 0; ///< Обнуление, если цель уже достигнута.
         }
     } else {
         targetAchievedStart = 0; ///< Сброс таймера достижения.
         if (targetReached) {
             if (targetLostStart == 0) {
                 targetLostStart = now; ///< Фиксация начала выхода из допуска.
             }
             if (now - targetLostStart >= 10000) { ///< 10 секунд вне допуска.
                 targetReached = false;
             }
         }
     }
 }
 