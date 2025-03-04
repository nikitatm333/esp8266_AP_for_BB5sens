const char REG_PAGE[] PROGMEM = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <title>Настройка точки доступа</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            form { max-width: 300px; }
            input { margin-bottom: 10px; width: 100%; padding: 8px; }
            button { padding: 10px 20px; }
        </style>
    </head>
    <body>
        <h2>Настройка точки доступа</h2>
        <form action="/set_ap" method="post">
            <input type="text" name="ssid" placeholder="SSID" required maxlength="31">
            <input type="password" name="password" placeholder="Пароль (минимум 8 символов)" maxlength="31">
            <button type="submit">Сохранить</button>
        </form>
    </body>
    </html>
    )=====";