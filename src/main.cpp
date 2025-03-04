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
const char* password = "^I={test}.1206";  // Пароль от Wi-Fi

// Веб-сервер
ESP8266WebServer server(80);

// Функция запуска точки доступа (AP)
void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  loadAPSettings(); // Загружаем SSID из EEPROM

  // Запускаем точку доступа без пароля (открытая сеть)
  WiFi.softAP(ap_ssid);
  Serial.println("Точка доступа запущена");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());
}

void connectToWiFi() {
  WiFi.mode(WIFI_STA);  // Устанавливаем режим станции
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi");

  unsigned long startAttemptTime = millis(); // Засекаем время старта
  const unsigned long wifiTimeout = 10000;   // 10 секунд на подключение

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nПодключено к Wi-Fi");
    Serial.print("IP-адрес: ");
    Serial.println(WiFi.localIP());
  }else {
    Serial.println("\nНе удалось подключиться к Wi-Fi. Запуск режима AP...");
    startAccessPoint(); // Переход в режим AP
  }
}



void setup() {
  Serial.begin(115200);
  connectToWiFi(); // Попытка подключения к Wi-Fi
  
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
