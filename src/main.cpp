#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "../inc/index.h" 
#include "../inc/graph_page.h"  // Подключаем страницу графика
#include <EEPROM.h>
#include <math.h>

// Настройки точки доступа
const char* ap_ssid = "ESP_AP";        // SSID вашей точки доступа
const char* ap_password = "12345678";  // Пароль от точки доступа

// Веб-сервер
ESP8266WebServer server(80);

// Переменные для управления температурой
float SetPoint = 36.6;          // Начальная температура
float CurrentTemp = 0.0;        // Текущая температура с АЧТ
String LastRecvd = "none yet";  // Последняя полученная команда

// Структура для хранения данных от АЧТ
typedef struct {
  float T[6];       // Температуры
  int16_t CV[4];    // Управляющие значения (PID-регуляторы)
  uint8_t status[4];// Статус
} MyDATA_36;

MyDATA_36 D36; // Экземпляр структуры

// Глобальные переменные для графика
#define L 100      // Размер буфера истории
#define HScale 3   // Горизонтальный масштаб графика
#define Scale 4    // Вертикальный масштаб графика
int TShead = 0;
int TStail = 0;
float TStorage[L];
int LLL = 0; // Длина SVG-строки

// Константы для EEPROM
#define EEPROM_SIZE 512
#define SETPOINT_ADDR 0

// --- Новые переменные для отслеживания статуса температуры ---
unsigned long targetAchievedStart = 0; // время начала нахождения в пределах допуска ±0.05 (для достижения)
unsigned long targetLostStart = 0;     // время начала отклонения от допустимого диапазона после достижения
bool targetReached = false;            // флаг: достигнута ли целевая температура

// Сохранение настроек в EEPROM
void saveSettings() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(SETPOINT_ADDR, SetPoint);
  EEPROM.commit();
  EEPROM.end();
}

// Загрузка настроек из EEPROM
void loadSettings() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(SETPOINT_ADDR, SetPoint);
  EEPROM.end();

  // Проверка на корректность загруженного значения
  if (SetPoint < -20.0 || SetPoint > 100.0) {
    SetPoint = 36.6;  // Значение по умолчанию
  }
}

// Функция формирования SVG-графика (код оставлен без изменений)
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
    if(n >= L) break;
  }
  
  out += "<text x=\"5\" y=\"20\" font-size=\"16px\" fill=\"#000088\">" + String(maxv, 2) + " C</text>\n";
  out += "<text x=\"5\" y=\"" + String(graphHeight - 10) + "\" font-size=\"16px\" fill=\"#000088\">" + String(minv, 2) + " C</text>\n";
  if (TShead > 0)
    out += "<text x=\"30\" y=\"100\" font-size=\"32px\" fill=\"#000088\">" + String(TStorage[(TShead - 1 + L) % L], 2) + " C</text>\n";
  
  n2 = 0;
  i = TStail;
  if(n > 0) {
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
    if(n2 >= n) break;
  }
  
  out += "</g>\n</svg>\n";
  LLL = out.length();
  server.send(200, "image/svg+xml; charset=utf-8", out);
}

// Обновление статуса достижения температуры
void updateTargetStatus() {
  float error = fabs(SetPoint - CurrentTemp);
  unsigned long now = millis();
  
  if (error <= 0.05) {
    // Если температура в пределах допуска
    if (!targetReached) {
      if (targetAchievedStart == 0) {
        targetAchievedStart = now;
      }
      if (now - targetAchievedStart >= 5000) { // 5 секунд в пределах допуска
        targetReached = true;
        targetLostStart = 0; // сброс таймера потери достижения
      }
    } else {
      // Если уже достигли, сбрасываем таймер потери
      targetLostStart = 0;
    }
  } else {
    // Температура вне допуска
    targetAchievedStart = 0; // сброс таймера достижения
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

// Обработчик корневой страницы
void handleRoot() {
  String page = MAIN_page;
  page.replace("%SETPOINT%", String(SetPoint, 1));
  page.replace("%CURRENTTEMP%", String(CurrentTemp, 1));
  page.replace("%LASTCMD%", LastRecvd);
  server.send(200, "text/html; charset=utf-8", page);
}

// Обработчик получения температуры (JSON) – добавлен флаг reached
void handleGetTemp() {
  String json = "{\"temp\": " + String(CurrentTemp, 1) + ", \"reached\": " + String(targetReached ? "true" : "false") + "}";
  server.send(200, "application/json; charset=utf-8", json);
}

// Отправка температуры на устройство через UART
void sendTemperatureToDevice(float temp) {
  String command = "T=" + String(temp, 1);
  Serial.println(command);
}

// Обработчик установки температуры (с автоматическим сохранением)
void handleSetTemp() {
  if (server.hasArg("temp")) {
    SetPoint = server.arg("temp").toFloat();
    LastRecvd = "Установлена температура: " + String(SetPoint);
    sendTemperatureToDevice(SetPoint);
    saveSettings();  // Автоматическое сохранение установленной температуры
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// Обработчик для возвращения значений датчиков в JSON
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

// Обработчик для возвращения значений PID-регуляторов в JSON
void handlePids() {
  String json = "{";
  json += "\"pid1\": " + String(D36.CV[0]) + ",";
  json += "\"pid2\": " + String(D36.CV[1]) + ",";
  json += "\"pid3\": " + String(D36.CV[2]) + ",";
  json += "\"pid4\": " + String(D36.CV[3]);
  json += "}";
  server.send(200, "application/json; charset=utf-8", json);
}

// Обработка строки с данными от АЧТ
void Process_pline(char *s) {
  sscanf(s, "%f %f %f %f %f %f %hd %hd %hd %hd %c%c%c%c",
         &D36.T[0], &D36.T[1], &D36.T[2], &D36.T[3], &D36.T[4], &D36.T[5],
         &D36.CV[0], &D36.CV[1], &D36.CV[2], &D36.CV[3],
         &D36.status[0], &D36.status[1], &D36.status[2], &D36.status[3]);

  CurrentTemp = D36.T[4];
  
  // Сохранение значения в историю графика
  TStorage[TShead] = CurrentTemp;
  TShead = (TShead + 1) % L;
  if(TShead == TStail) {
    TStail = (TStail + 1) % L;
  }
}

// Обработка входящих данных с UART
void processSerialData() {
  static char in_s[256];
  static uint8_t in_p = 0;
  
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (in_p > 0) {
        in_s[in_p] = 0;
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

// Обработчик, возвращающий только SVG-график
void handleGraphSVG() {
  drawGraph();
}

// Обработчик, возвращающий HTML-страницу с графиком (из graph_page.h)
void handleGraphHTML() {
  server.send_P(200, "text/html; charset=utf-8", GRAPH_PAGE);
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Точка доступа запущена");
  loadSettings();  // Загружаем сохраненные настройки
  
  // Инициализация буфера графика (начальные данные)
  for (int i = 0; i < L; i++) {
    TStorage[i] = 36.0;
  }
  TShead = 0;
  TStail = 0;
  
  server.on("/", handleRoot);
  server.on("/set_temp", handleSetTemp);
  server.on("/get_temp", handleGetTemp);
  server.on("/sensors", handleSensors);
  server.on("/pids", handlePids);
  server.on("/graph_svg", handleGraphSVG);
  server.on("/graph", handleGraphHTML);
  server.begin();
  Serial.println("Веб-сервер запущен");
  Serial.print("IP-адрес точки доступа: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  processSerialData();
  updateTargetStatus();  // Обновляем статус достижения целевой температуры
}
