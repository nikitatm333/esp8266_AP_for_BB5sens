# Изменения

- Обновленный веб интерфейс;
- Иерархия проекта;
- Выбор режима работы (STA или AP). Выделяется время на подключение к существующей сети (режим STA), по истечению времени ESP начинает работу как AP;
- Настройка AP в веб интерфейсе. Возможность изменения имени сети (ssid) и пароля (password);
- Алгоритмы чтения UART и вывод графика без изменений;
- Команда RESET - сброс энергонезависимой памяти (зажать 2 кнопки).

# Функционал

- Управление температурой АЧТ;
- Индикация достижения уставной температуры. Зеленый - достигли, оранжевый - нет;
- Сохранение последней команды в энергонезависимую память;
- Настройка AP;
- Вывод подробной информации с АЧТ:
  - показания 5 датчиков,
  - показания PID-регуляторов,
  - график зависимости температуры от времени.

# UART Protocol

**Формат строки, принимаемой по UART:**

- **Общее описание:**  
  Прошивка ожидает строку, состоящую из 14 элементов, разделённых пробелами. Если строка не соответствует формату (например, из-за обрыва провода или наличия "мусора"), она игнорируется.

- **Структура строки:**  
  - **Первые 6 элементов:** Значения температуры с датчиков (тип `float`).  
    Пример: `36.6 36.5 36.7 36.8 36.6 36.7`
  - **Следующие 4 элемента:** Значения для PID-регуляторов (тип `short`).  
    Пример: `100 100 100 100`
  - **Последние 4 элемента:** Символы статуса (тип `char`).  
    Пример: `A B C D`

- **Пример строки:**  
  `36.6 36.5 36.7 36.8 36.6 36.7 100 100 100 100 A B C D`

- **Обработка ошибок:**  
  При обработке строки функция вызывает `sscanf` и проверяет, что распознано ровно 14 элементов. Если количество успешно прочитанных элементов меньше 14, строка считается некорректной и игнорируется.

# Web API

**Доступные эндпоинты веб-сервера:**

- **GET /**  
  Отправляет HTML-страницу главного интерфейса с подстановкой данных:
  - Уставка температуры (`%SETPOINT%`);
  - Текущая температура (`%CURRENTTEMP%`);
  - Последняя команда (`%LASTCMD%`);
  - Параметры точки доступа (`%AP_SSID%` и `%AP_PASSWORD%`).

- **GET /get_temp**  
  Возвращает JSON с текущей температурой и статусом достижения целевой температуры.  
  Пример:  
  ```json
  { "temp": 36.6, "reached": true }

- **GET /sensors**
  Возвращает JSON с показаниями пяти температурных датчиков.
  Пример:
  ```json
  { "sensor1": 36.6, "sensor2": 36.5, "sensor3": 36.7, "sensor4": 36.8, "central": 36.6 }

- **GET /pids**
  Возвращает JSON с текущими значениями PID-регуляторов.
  Пример:
  ```json
  { "pid1": 100, "pid2": 100, "pid3": 100, "pid4": 100 }

- **GET /graph_svg**
  Генерирует и отправляет SVG-график зависимости температуры от времени.

- **GET /graph**
  Отправляет HTML-страницу с графическим отображением данных (страница графика).

- **POST /set_ap**
  Настраивает параметры точки доступа (AP).
  Параметры:
	- ssid – новое имя сети (1–31 символ);
	- password – новый пароль (если указан, не менее 8 символов).
	Ответ:
	- При корректных данных возвращает "OK" (HTTP 200);
	- При ошибке (например, некорректный ssid или password) возвращает HTTP 400.

- **GET/POST /set_temp**
  Устанавливает новую температуру.
  Параметры:
	- temp – значение желаемой температуры.
	Действия:
	- Обновляет переменную SetPoint;
	- Формирует строку команды и отправляет её через UART;
	- Сохраняет последнюю команду и новое значение температуры в EEPROM.
	Ответ:
	- При успешном выполнении происходит редирект на главную страницу (HTTP 303);
	- Если установка (например, сохранение в EEPROM) не удалась, возвращает HTTP 500 с сообщением об ошибке.


- **UART Command "RESET"**
  При получении строки "RESET" по UART происходит:
	- Очистка EEPROM;
	- Установка параметров точки доступа в режим восстановления (SSID: ESP_RESET, пустой пароль);
	- Вывод сообщения о смене режима в последовательный порт.

