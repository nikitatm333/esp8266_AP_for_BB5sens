# Сборка файла будет происходить с помощью PlatformIO
# Проше всего установить PlatformIO с помощью python
# Используем компактный базовый образ Python
FROM python:3.9-slim

# Обновляем пакеты и устанавливаем необходимые зависимости
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем PlatformIO через pip
RUN pip install --no-cache-dir platformio

# Задаем рабочую директорию
WORKDIR /project

# Копируем весь проект в контейнер
COPY . /project

# Собираем проект с помощью PlatformIO
RUN pio run

# По умолчанию запускаем bash
CMD ["bash"]
