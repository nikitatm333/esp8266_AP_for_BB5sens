/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    graph.cpp
  * @brief   Этот файл содержит функции для работы с OLED-дисплеем и
  *          обработки нажатий кнопок в системе управления температурой.
  ******************************************************************************
  */
/* USER CODE END Header */


#include "../inc/graph.h"
#include "../inc/globals.h"
#include "../inc/graph_page.h"
#include <ESP8266WebServer.h>

// Объявляем внешний веб-сервер
extern ESP8266WebServer server;

void drawGraph() {
  float minv, maxv, v, y, y2;
  int i, n, n2;
  String out = "";
  char temp[100];
  
  int graphWidth = L * HScale;
  int graphHeight = 50 * Scale;
  
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"auto\" viewBox=\"0 0 ";
  out += String(graphWidth);
  out += " ";
  out += String(graphHeight);
  out += "\" preserveAspectRatio=\"xMidYMid meet\">";
  
  out += "<rect width=\"" + String(graphWidth) + "\" height=\"" + String(graphHeight) + "\" fill=\"#ffffff\" stroke-width=\"1\" stroke=\"#000088\" />\n";
  out += "<g stroke=\"#000088\">\n";
  
  minv = 20.0;
  maxv = 40.0;
  i = TStail;
  n = 0;
  while (i != TShead) {
    v = TStorage[i];
    if (v < minv) minv = v;
    if (v > maxv) maxv = v;
    i = (i + 1) % L;
    n++;
    if (n >= L) break;
  }
  
  out += "<text x=\"5\" y=\"20\" font-size=\"16px\" fill=\"#000088\">" + String(maxv, 2) + " C</text>\n";
  out += "<text x=\"5\" y=\"" + String(graphHeight - 10) + "\" font-size=\"16px\" fill=\"#000088\">" + String(minv, 2) + " C</text>\n";
  if (TShead > 0)
    out += "<text x=\"30\" y=\"100\" font-size=\"32px\" fill=\"#000088\">" + String(TStorage[(TShead - 1 + L) % L], 2) + " C</text>\n";
  
  n2 = 0;
  i = TStail;
  if (n > 0) {
    v = TStorage[i];
    y = (v - minv) * 50.0 / (maxv - minv + 0.01);
  } else {
    y = 0;
  }
  
  while (i != TShead) {
    v = TStorage[i];
    y2 = (v - minv) * 50.0 / (maxv - minv + 0.01);
    snprintf(temp, 100, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000088\" stroke-width=\"1\"/>\n", 
             (int)(n2 * HScale), (int)((50.0 - y) * Scale), (int)((n2 + 1) * HScale), (int)((50.0 - y2) * Scale));
    out += temp;
    y = y2;
    i = (i + 1) % L;
    n2++;
    if (n2 >= n) break;
  }
  
  out += "</g>\n</svg>\n";
  LLL = out.length();
  server.send(200, "image/svg+xml; charset=utf-8", out);
}
