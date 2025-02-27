#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "../inc/index.h" 


// Настройки точки доступа
const char* ap_ssid = "ESP_AP";        // SSID вашей точки доступа
const char* ap_password = "12345678";  // Пароль от точки доступа

// Веб-сервер
ESP8266WebServer server(80);

// Переменные для управления температурой
float SetPoint = 36.6;    // Начальная температура
float CurrentTemp = 0.0;  // Текущая температура с АЧТ
String LastRecvd = "none yet"; // Последняя полученная команда

// Структура для хранения данных от АЧТ
typedef struct {
  float T[6];    // Температуры
  int16_t CV[4]; // Управляющие значения
  uint8_t status[4]; // Статус
} MyDATA_36;

MyDATA_36 D36; // Экземпляр структуры


// Обработчик корневой страницы
void handleRoot() {
  String page = MAIN_page;
  page.replace("%SETPOINT%", String(SetPoint, 1));
  page.replace("%CURRENTTEMP%", String(CurrentTemp, 1));
  page.replace("%LASTCMD%", LastRecvd);
  server.send(200, "text/html", page);
}

// Отправка температуры на устройство через UART
void sendTemperatureToDevice(float temp) {
  String command = "T=" + String(temp, 1); // Формат команды: "T=36.6"
  Serial.println(command); // Отправка команды на устройство
}

// Обработчик установки температуры
void handleSetTemp() {
  if (server.hasArg("temp")) {
    SetPoint = server.arg("temp").toFloat();
    LastRecvd = "Установлена температура: " + String(SetPoint);
    sendTemperatureToDevice(SetPoint); // Отправка температуры на устройство
  }
  server.sendHeader("Location", "/");
  server.send(303);
}


// Обработка строки с данными от АЧТ
void Process_pline(char *s) {
  // Пример строки: "25.0 26.0 27.0 28.0 29.0 30.0 0 0 0 0 ABCD"
  sscanf(s, "%f %f %f %f %f %f %hd %hd %hd %hd %c%c%c%c",
         &D36.T[0], &D36.T[1], &D36.T[2], &D36.T[3], &D36.T[4], &D36.T[5],
         &D36.CV[0], &D36.CV[1], &D36.CV[2], &D36.CV[3],
         &D36.status[0], &D36.status[1], &D36.status[2], &D36.status[3]);

  // Обновляем текущую температуру 
  CurrentTemp = D36.T[4]; // Выводим значение центрального датчика
}

// Обработка входящих данных с UART
void processSerialData() {
  static char in_s[256]; // Буфер для входящих данных
  static uint8_t in_p = 0; // Указатель на текущую позицию в буфере

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (in_p > 0) {
        in_s[in_p] = 0; // Завершаем строку
        Process_pline(in_s); // Обрабатываем строку
        in_p = 0; // Сбрасываем указатель
      }
    } else {
      if (in_p < sizeof(in_s) - 1) {
        in_s[in_p++] = c; // Добавляем символ в буфер
      }
    }
  }
}

// Настройка
void setup() {
  Serial.begin(115200); // Инициализация UART

  // Запуск точки доступа
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Точка доступа запущена");

  // Запуск веб-сервера
  server.on("/", handleRoot);
  server.on("/set_temp", handleSetTemp);
  server.begin();
  Serial.println("Веб-сервер запущен");

  // Вывод IP-адреса точки доступа
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());
}

// Основной цикл
void loop() {
  server.handleClient(); // Обработка запросов клиентов
  processSerialData(); // Обработка входящих данных с UART
}
