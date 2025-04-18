/**
 * @mainpage ESP8266 Web Server Manual
 * @brief Руководство пользователя по системе управления температурой АЧТ на базе ESP8266.
 *
 * @section overview Обзор системы
 * Данная прошивка предназначена для управления температурой Абсолютно Черного Тела (АЧТ) и включает реализацию веб-сервера на базе ESP8266.
 * При включении питания устройство пытается подключиться к заданной Wi-Fi сети (режим STA). Если в течение 10 секунд подключение не происходит,
 * ESP8266 автоматически переходит в режим точки доступа (AP), что позволяет пользователю подключиться к устройству для дальнейшей настройки.
 *
 * @section features Функциональные возможности веб-сервера
 * - **Управление температурой АЧТ:** Пользователь может устанавливать требуемую температуру через веб-интерфейс.
 * - **Индикация достижения уставной температуры:** На веб-странице отображается состояние:
 *   - Зеленый индикатор – уставная температура достигнута.
 *   - Оранжевый индикатор – температура еще не достигнута.
 * - **Сохранение параметров в энергонезависимой памяти:** Последняя команда и настройки точки доступа (AP) сохраняются в EEPROM.
 * - **Сброс настроек:** Одновременное удержание двух кнопок на устройстве в течение 10 секунд инициирует сброс EEPROM.
 *   После отправки команды RESET на ESP8266 для применения изменений требуется отключить и вновь включить устройство.
 * - **Настройка точки доступа (AP):** Через веб-интерфейс можно изменить параметры точки доступа (SSID и пароль).
 * - **Вывод подробной информации:** На веб-странице доступны следующие данные:
 *   - Показания 5 температурных датчиков.
 *   - Значения, полученные от PID-регуляторов.
 *   - График зависимости температуры от времени.
 *
 * @section startup Порядок работы при включении питания
 * 1. **Инициализация:** При включении питания ESP8266 запускается в режиме станции (STA).
 * 2. **Попытка подключения к Wi-Fi:** Устройство пытается подключиться к заданной сети в течение 10 секунд.
 * 3. **Переход в режим AP:** Если подключение не удалось, ESP8266 автоматически запускает режим точки доступа (AP).
 * 4. **Настройка параметров:** Для изменения параметров точки доступа откройте главную страницу веб-сервера и перейдите в раздел настроек.
 *    Введите новые значения для SSID и пароля. После внесения изменений обязательно выполните перезагрузку устройства (отключите и включите питание).
 *
 * @section recovery Режим восстановления настроек
 * Если у пользователя отсутствует возможность подключения к текущей сети и изменения параметров через веб-интерфейс, предусмотрен механизм сброса EEPROM:
 * - Одновременное удержание двух кнопок на устройстве в течение 10 секунд инициирует сброс энергонезависимой памяти.
 * - После сброса ESP8266 получает команду RESET, и для корректного применения настроек устройство необходимо перезагрузить.
 * - В результате сброса запускается точка доступа с SSID "ESP_RESET" без пароля.
 *
 * @section usage Инструкция по эксплуатации
 * - **Подключение:** Включите устройство. Если оно успешно подключается к заданной сети, его IP-адрес отобразится в последовательном порту.
 *   Если подключение не происходит, устройство работает в режиме точки доступа, к которому можно подключиться напрямую.
 * - **Управление температурой:** На главной странице веб-сервера установите требуемую температуру. Система отобразит состояние достижения уставной температуры.
 * - **Настройка точки доступа:** Перейдите в раздел настроек веб-сервера для изменения параметров AP. После внесения изменений обязательно выполните перезагрузку устройства.
 * - **Сброс настроек:** При необходимости сброса удерживайте одновременно две кнопки на устройстве в течение 10 секунд.
 *
 * @note Для корректной работы всех функций, особенно после изменения настроек, рекомендуется выполнять перезагрузку устройства.
 * 
 * @file main.cpp
 *   ## Полезные ссылки ~W
 * - Основной файл проекта: @ref main.cpp
 * - Обработка uart: @ref serial_processing.cpp
 * - Веб обработчики: @ref web_handlers.cpp
 * - Настройка энергонезависимой памяти: @ref settings.cpp
 * - Алгоритмы построения зависимости температуры от времени: @ref graph.cpp
 * - Обработка достижения установочной температуры: @ref temp_control.cpp

 * @author tnv
 * @date 13.03.2025
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
 #include "index.h" 
 #include "graph_page.h"
 #include "web_handlers.h"
 #include "settings.h"
 #include "serial_processing.h"
 #include "temp_control.h"
 #include "globals.h"
 #include <EEPROM.h>
 #include <math.h>
 
 // Настройки подключения к Wi-Fi
 const char* ssid = "INFRATEST";          ///< SSID сети Wi-Fi.
 const char* password = "^I={test}.1206";  ///< Пароль от Wi-Fi.
 
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
     
     Serial.println("Access point started");
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
     Serial.print("Connecting to Wi-Fi");
 
     unsigned long startAttemptTime = millis(); ///< Время начала попытки подключения.
     const unsigned long wifiTimeout = 10000;   ///< Тайм-аут подключения в миллисекундах (10 секунд).
 
     while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
         delay(500);
         Serial.print(".");
     }
 
     if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to Wi-Fi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to Wi-Fi. Starting AP mode...");
        startAccessPoint();
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
     Serial.println("Web server started");
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
 