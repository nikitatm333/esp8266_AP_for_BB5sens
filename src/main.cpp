#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Настройки точки доступа
const char* ap_ssid = "ESP8266_AP";  // SSID точки доступа
const char* ap_password = "12345678"; // Пароль точки доступа

// Веб-сервер
ESP8266WebServer server(80);

// Переменные для управления температурой
float SetPoint = 36.6; // Начальная температура
String LastRecvd = "none yet"; // Последняя полученная команда

// HTML страница для управления температурой
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
 <head>
  <meta charset="utf-8">
  <title>Управление АЧТ</title>
  <style>
    body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }
    input[type='number'] { width: 100px; }
  </style>
 </head>
 <body>
  <h1>Управление температурой АЧТ</h1>
  <form method="post" action="/set_temp">
    Температура: <input type="number" name="temp" value="36.6" step="0.1" min="-20" max="100"><br><br>
    <input type="submit" value="Установить">
  </form>
  <p>Последняя команда: %LASTCMD%</p>
 </body>
</html>
)=====";

// Обработчик корневой страницы
void handleRoot() {
  String page = MAIN_page;
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

// Настройка
void setup() {
  Serial.begin(115200); // Инициализация UART

  // Запуск Wi-Fi в режиме точки доступа (AP)
  WiFi.softAP(ap_ssid, ap_password);
  
  // Получение IP-адреса точки доступа
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("Wi-Fi AP запущен");
  Serial.print("IP-адрес: ");
  Serial.println(myIP);

  // Запуск веб-сервера
  server.on("/", handleRoot);
  server.on("/set_temp", handleSetTemp);
  server.begin();
  Serial.println("Веб-сервер запущен");
}

// Основной цикл
void loop() {
  server.handleClient(); // Обработка запросов клиентов
}
