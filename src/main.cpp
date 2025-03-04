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

// Настройки подключения к Wi-Fi
const char* ssid = "INFRATEST";          // SSID вашей сети Wi-Fi
const char* password = "^I={test}.1206   @";  // Пароль от Wi-Fi

// Веб-сервер
ESP8266WebServer server(80);

// Функция подключения к Wi-Fi в режиме станции (STA)
void connectToWiFi() {
  WiFi.mode(WIFI_STA);  // Устанавливаем режим станции
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi");
  
  // Ожидание подключения
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nПодключено к Wi-Fi");
  Serial.print("IP-адрес: ");
  Serial.println(WiFi.localIP());
}

// Функция запуска точки доступа (AP)
void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  loadAPSettings();
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());
}

void setup() {
  Serial.begin(115200);
  
  // connectToWiFi();  // Подключаемся к Wi-Fi
  startAccessPoint();
  loadSettingsTemp();  // Загружаем сохранённую установочную температуру
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
}

void loop() {
  server.handleClient();
  processSerialData();
  updateTargetStatus();  // Обновляем статус достижения температуры
}
