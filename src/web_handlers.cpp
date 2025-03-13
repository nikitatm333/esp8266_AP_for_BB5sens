/**
 * @file web_handlers.cpp
 * @brief Веб-обработчики для взаимодействия с веб-интерфейсом.
 *
 * @details Этот модуль содержит обработчики HTTP-запросов для управления системой, 
 * включая установку температуры, получение текущих данных и работу с графиками.
 */

 #include "web_handlers.h"
 #include "index.h"
 #include "graph_page.h"
 #include "settings.h"
 #include "globals.h"
 #include "temp_control.h"
 #include <ESP8266WebServer.h>
 #include "graph.h"
 #include <EEPROM.h>
 
 /// Объявляем внешний веб-сервер.
 extern ESP8266WebServer server;
 
 /// Объявляем глобальные переменные, используемые в обработчиках.
 extern float CurrentTemp;
 extern String LastRecvd;
 extern float SetPoint;
 extern MyDATA_36 D36;
 extern int TShead, TStail, LLL;
 extern float TStorage[L];
 
 /**
  * @brief Отправляет команду на установку температуры через UART.
  * @param temp Желаемая температура.
  */
 void sendTemperatureToDevice(float temp) {
     String command = "T=" + String(temp, 1);
     Serial.println(command);
 }
 
 /**
  * @brief Обработчик главной страницы.
  * 
  * @details Отправляет клиенту HTML-страницу, подставляя в неё актуальные данные.
  */
 void handleRoot() {
     String page = MAIN_page;
     page.replace("%SETPOINT%", String(SetPoint, 1));
     page.replace("%CURRENTTEMP%", String(CurrentTemp, 1));
     page.replace("%LASTCMD%", LastRecvd);
     page.replace("%AP_SSID%", String(ap_ssid));
     page.replace("%AP_PASSWORD%", String(ap_password));
     server.send(200, "text/html; charset=utf-8", page);
 }
 
 /**
  * @brief Обработчик установки температуры.
  * 
  * @details Читает переданное значение температуры, обновляет переменную SetPoint,
  * отправляет команду устройству и сохраняет изменения в EEPROM.
  */
 void handleSetTemp() {
     if (server.hasArg("temp")) {
         SetPoint = server.arg("temp").toFloat();
         LastRecvd = "Установить " + String(SetPoint) + " °C";
         sendTemperatureToDevice(SetPoint);
         saveLastRecvd();  ///< Сохранение последней команды в EEPROM.
         saveSettingsTemp();  ///< Сохранение SetPoint в EEPROM.
     }
     server.sendHeader("Location", "/");
     server.send(303);
 }
 
 /**
  * @brief Обработчик получения текущей температуры.
  * 
  * @details Возвращает JSON с текущей температурой и статусом `targetReached` (достигнута ли цель).
  */
 void handleGetTemp() {
     extern bool targetReached;
     String json = "{\"temp\": " + String(CurrentTemp, 1) + 
                   ", \"reached\": " + String(targetReached ? "true" : "false") + "}";
     server.send(200, "application/json; charset=utf-8", json);
 }
 
 /**
  * @brief Обработчик данных датчиков.
  * 
  * @details Возвращает JSON с показаниями пяти температурных датчиков.
  */
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
 
 /**
  * @brief Обработчик данных PID-регуляторов.
  * 
  * @details Возвращает JSON с текущими управляющими значениями (PID).
  */
 void handlePids() {
     String json = "{";
     json += "\"pid1\": " + String(D36.CV[0]) + ",";
     json += "\"pid2\": " + String(D36.CV[1]) + ",";
     json += "\"pid3\": " + String(D36.CV[2]) + ",";
     json += "\"pid4\": " + String(D36.CV[3]);
     json += "}";
     server.send(200, "application/json; charset=utf-8", json);
 }
 
 /**
  * @brief Обработчик генерации SVG-графика.
  */
 void handleGraphSVG() {
     drawGraph();
 }
 
 /**
  * @brief Обработчик страницы с графиками.
  */
 void handleGraphHTML() {
     server.send_P(200, "text/html; charset=utf-8", GRAPH_PAGE);
 }
 
 /**
  * @brief Обработчик настройки Wi-Fi в режиме точки доступа.
  * 
  * @details Принимает новые параметры SSID и пароля, сохраняет их и перезапускает устройство.
  */
 void handleSetAP() {
     String newSSID = server.arg("ssid");
     String newPassword = server.arg("password");
 
     // Проверки на корректность данных
     if (newSSID.length() == 0 || newSSID.length() > 31) {
         server.send(400, "text/plain", "Invalid SSID");
         return;
     }
     if (newPassword.length() > 0 && newPassword.length() < 8) {
         server.send(400, "text/plain", "Invalid password");
         return;
     }
 
     // Сохранение новых значений
     memset(ap_ssid, 0, sizeof(ap_ssid));
     memset(ap_password, 0, sizeof(ap_password));
     strncpy(ap_ssid, newSSID.c_str(), sizeof(ap_ssid) - 1);
     strncpy(ap_password, newPassword.c_str(), sizeof(ap_password) - 1);
 
     saveAPSettings();
     
     server.send(200, "text/plain", "OK");
 
     // Отложенная перезагрузка устройства
     delay(100);
     ESP.restart();
 }
 
 /**
  * @brief Инициализация маршрутов веб-сервера.
  * 
  * @param server Экземпляр `ESP8266WebServer`.
  */
 void initWebHandlers(ESP8266WebServer &server) {
     server.on("/", handleRoot);
     server.on("/set_temp", handleSetTemp);
     server.on("/get_temp", handleGetTemp);
     server.on("/sensors", handleSensors);
     server.on("/pids", handlePids);
     server.on("/graph_svg", handleGraphSVG);
     server.on("/graph", handleGraphHTML);
     server.on("/set_ap", HTTP_POST, handleSetAP);
 }
 