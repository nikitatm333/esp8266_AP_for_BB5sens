/**
 * @mainpage
 * @brief Главная
 * @file main.cpp
 * @brief Основной файл прошивки для ESP8266, включающий подключение к Wi-Fi, запуск веб-сервера и управление устройством.
 * 
 * @details В данном файле реализованы функции подключения к Wi-Fi, переключения в режим точки доступа (AP) 
 * при неудачном подключении, запуск веб-сервера и обработка входящих данных через последовательный порт.
 * 
 * ## Полезные ссылки🔗
 * - Основной файл проекта: @ref main.cpp
 * - Обработка uart: @ref serial_processing.cpp
 * - Веб обработчики: @ref web_handlers.cpp
 * - Настройка энергонезависимой памяти: @ref settings.cpp
 * - Алгоритмы построения зависимости температуры от времени: @ref graph.cpp
 * - Обработка достижения установочной температуры: @ref temp_control.cpp
 */

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
 const char* ssid = "INFRATEST";          ///< SSID сети Wi-Fi.
 const char* password = "^I={test}.1206    8";  ///< Пароль от Wi-Fi.
 
 // Стандартный 80-ый порт веб-сервера
 ESP8266WebServer server(80);
 
 /**
  * @brief Инициализация точки доступа (AP).
  * 
  * @details Загружает настройки точки доступа и запускает её, если подключение к Wi-Fi не удалось.
  * В случае отсутствия данных SSID или пароля задаются значения по умолчанию.
  */
 void startAccessPoint() {
     loadAPSettings();
     
     if (ap_ssid[0] == 0xFF || ap_ssid[0] == '\0') {
         strncpy(ap_ssid, "ESP_AP", sizeof(ap_ssid));
         ap_ssid[sizeof(ap_ssid)-1] = '\0';
         ap_password[0] = '\0';
     }
 
     if (ap_password[0] == '\0' || ap_password[0] == 0xFF) {
         WiFi.softAP(ap_ssid);
     } else {
         WiFi.softAP(ap_ssid, ap_password);
     }
     
     Serial.println("Точка доступа запущена");
 }
 
 /**
  * @brief Обработчик подключения к сети Wi-Fi.
  * 
  * @details Устанавливает режим станции (STA) и пытается подключиться к сети. 
  * В случае неудачи переключается в режим точки доступа (AP).
  */
 void connectToWiFi() {
     WiFi.mode(WIFI_STA);  // Устанавливаем режим станции
     WiFi.begin(ssid, password);
     Serial.print("Подключение к Wi-Fi");
 
     unsigned long startAttemptTime = millis(); ///< Время начала попытки подключения.
     const unsigned long wifiTimeout = 10000;   ///< Тайм-аут подключения в миллисекундах (10 секунд).
 
     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
         delay(500);
         Serial.print(".");
     }
 
     if (WiFi.status() == WL_CONNECTED) {
         Serial.println("\nПодключено к Wi-Fi");
         Serial.print("IP-адрес: ");
         Serial.println(WiFi.localIP());
     } else {
         Serial.println("\nНе удалось подключиться к Wi-Fi. Запуск режима AP...");
         startAccessPoint(); // Переход в режим AP
     }
 }
 
 /**
  * @brief Функция инициализации устройства.
  * 
  * @details Настраивает последовательный порт, пытается подключиться к Wi-Fi, загружает настройки, 
  * инициализирует буфер графика и регистрирует веб-обработчики.
  */
 void setup() {
     Serial.begin(115200);
     // clearEEPROM(); // Очистка энергонезависимой памяти устройства
     connectToWiFi(); // Попытка подключения к Wi-Fi
     
     loadSettingsTemp();  ///< Загружаем сохранённую установочную температуру.
     loadLastRecvd();     ///< Загрузка последней команды перед отключением питания.
     
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
 
 /** 
  * @brief Главный цикл программы.
  * 
  * @details Обрабатывает HTTP-запросы, поступающие на веб-сервер, считывает данные с последовательного порта
  * и обновляет статус достижения температуры.
  */
 void loop() {
     server.handleClient();
     processSerialData();
     updateTargetStatus();  ///< Обновляем статус достижения температуры.
 }
 