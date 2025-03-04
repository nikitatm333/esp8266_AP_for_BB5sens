#include "../inc/web_handlers.h"
#include "../inc/index.h"
#include "../inc/graph_page.h"
#include "../inc/settings.h"
#include "../inc/globals.h"
#include "../inc/temp_control.h"
#include <ESP8266WebServer.h>
#include "../inc/graph.h"

// Объявляем внешний сервер
extern ESP8266WebServer server;

// Объявляем глобальные переменные, используемые в обработчиках
extern float CurrentTemp;
extern String LastRecvd;
extern float SetPoint;
extern MyDATA_36 D36;
extern int TShead, TStail, LLL;
extern float TStorage[L];

// Вспомогательная функция для отправки команды температуры через UART
void sendTemperatureToDevice(float temp) {
  String command = "T=" + String(temp, 1);
  Serial.println(command);
}

void handleRoot() {
  String page = MAIN_page;
  page.replace("%SETPOINT%", String(SetPoint, 1));
  page.replace("%CURRENTTEMP%", String(CurrentTemp, 1));
  page.replace("%LASTCMD%", LastRecvd);
  server.send(200, "text/html; charset=utf-8", page);
}


void handleSetTemp() {
    if (server.hasArg("temp")) {
      SetPoint = server.arg("temp").toFloat();
      LastRecvd = "Установить " + String(SetPoint) + " °C";
      sendTemperatureToDevice(SetPoint);
      saveLastRecvd();  // Сохраняем строку в EEPROM
      saveSettingsTemp();    // Сохраняем SetPoint в EEPROM
    }
    server.sendHeader("Location", "/");
    server.send(303);
  }
  

void handleGetTemp() {
  // Используем переменную targetReached из temp_control
  extern bool targetReached;
  String json = "{\"temp\": " + String(CurrentTemp, 1) + ", \"reached\": " + String(targetReached ? "true" : "false") + "}";
  server.send(200, "application/json; charset=utf-8", json);
}

void handleSensors() {
  String json = "{";
  json += "\"sensor1\": " + String(D36.T[0], 2) + ",";
  json += "\"sensor2\": " + String(D36.T[1], 2) + ",";
  json += "\"sensor3\": " + String(D36.T[2], 2) + ",";
  json += "\"sensor4\": " + String(D36.T[3], 2) + ",";
  json += "\"central\": " + String(D36.T[4], 2);
  json += "}";
  server.send(200, "application/json; charset=utf-8", json);
}

void handlePids() {
  String json = "{";
  json += "\"pid1\": " + String(D36.CV[0]) + ",";
  json += "\"pid2\": " + String(D36.CV[1]) + ",";
  json += "\"pid3\": " + String(D36.CV[2]) + ",";
  json += "\"pid4\": " + String(D36.CV[3]);
  json += "}";
  server.send(200, "application/json; charset=utf-8", json);
}

void handleGraphSVG() {
  // Функция drawGraph определена в модуле graph
  drawGraph();
}

void handleGraphHTML() {
  server.send_P(200, "text/html; charset=utf-8", GRAPH_PAGE);
}

void initWebHandlers(ESP8266WebServer &server) {
  server.on("/", handleRoot);
  server.on("/set_temp", handleSetTemp);
  server.on("/get_temp", handleGetTemp);
  server.on("/sensors", handleSensors);
  server.on("/pids", handlePids);
  server.on("/graph_svg", handleGraphSVG);
  server.on("/graph", handleGraphHTML);
}


