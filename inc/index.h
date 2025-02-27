#ifndef INDEX_HTML
#define INDEX_HTML

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
        Температура: <input type="number" name="temp" value="%SETPOINT%" step="0.1" min="-20" max="100"><br><br>
        <input type="submit" value="Установить">
      </form>
      <p>Текущая температура: %CURRENTTEMP%</p>
      <p>Последняя команда: %LASTCMD%</p>
     </body>
    </html>
    )=====";
    

#endif