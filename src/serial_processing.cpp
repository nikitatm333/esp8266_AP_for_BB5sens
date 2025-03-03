#include "../inc/serial_processing.h"
#include "../inc/globals.h"
#include <Arduino.h>
#include <stdio.h>

static void Process_pline(char *s) {
  sscanf(s, "%f %f %f %f %f %f %hd %hd %hd %hd %c%c%c%c",
         &D36.T[0], &D36.T[1], &D36.T[2], &D36.T[3], &D36.T[4], &D36.T[5],
         &D36.CV[0], &D36.CV[1], &D36.CV[2], &D36.CV[3],
         &D36.status[0], &D36.status[1], &D36.status[2], &D36.status[3]);

  CurrentTemp = D36.T[4];

  // Сохраняем значение в историю графика
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
