#include "../inc/temp_control.h"
#include "../inc/globals.h"
#include "../inc/settings.h"
#include <Arduino.h>
#include <math.h>

unsigned long targetAchievedStart = 0; // время начала нахождения в пределах допуска ±0.05
unsigned long targetLostStart = 0;     // время начала отклонения от допуска после достижения
bool targetReached = false;

void updateTargetStatus() {
  float error = fabs(SetPoint - CurrentTemp);
  unsigned long now = millis();
  
  if (error <= 0.05) {
    if (!targetReached) {
      if (targetAchievedStart == 0) {
        targetAchievedStart = now;
      }
      if (now - targetAchievedStart >= 20000) { // 20 секунд в пределах допуска
        targetReached = true;
        targetLostStart = 0; // сброс таймера потери достижения
      }
    } else {
      targetLostStart = 0;
    }
  } else {
    targetAchievedStart = 0;
    if (targetReached) {
      if (targetLostStart == 0) {
        targetLostStart = now;
      }
      if (now - targetLostStart >= 10000) { // 10 секунд вне допуска
        targetReached = false;
      }
    }
  }
}
