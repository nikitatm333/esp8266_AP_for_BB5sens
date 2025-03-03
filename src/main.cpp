#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "../inc/index.h" 
#include "../inc/graph_page.h"
#include "../inc/web_handlers.h"
#include "../inc/settings.h"
#include "../inc/serial_processing.h"
#include "../inc/temp_control.h"
#include "../inc/globals.h"
#include <EEPROM.h>
#include <math.h>

// Настройки точки доступа
const char* ap_ssid = "ESP_AP";        // SSID вашей точки доступа
const char* ap_password = "12345678";  // Пароль от точки доступа

// Веб-сервер
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Точка доступа запущена");

  loadSettings();  // Загружаем сохранённую установочную температуру
  loadLastRecvd();
  // Инициализация буфера графика (начальные данные)
  for (int i = 0; i < L; i++) {
    TStorage[i] = 36.0;
  }
  TShead = 0;
  TStail = 0;

  // Регистрация веб-обработчиков
  initWebHandlers(server);

  server.begin();
  Serial.println("Веб-сервер запущен");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  processSerialData();
  updateTargetStatus();  // Обновляем статус достижения температуры
}
