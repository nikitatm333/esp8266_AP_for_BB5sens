#ifndef INDEX_HTML
#define INDEX_HTML

const char MAIN_page[] PROGMEM = R"=====( 
    <!DOCTYPE HTML>
    <html>
     <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Управление АЧТ</title>
      <style>
        body { 
          background-color: #cccccc; 
          font-family: Arial, Helvetica, Sans-Serif; 
          color: #000088; 
          text-align: center;
          margin: 0;
          padding: 20px;
        }
        .container {
          max-width: 400px;
          margin: auto;
          padding: 20px;
          background: white;
          border-radius: 10px;
          box-shadow: 0px 4px 6px rgba(0, 0, 0, 0.1);
        }
        .leftAlign {
          text-align: left;
        }
        input[type='number'] {
          width: 80%;
          max-width: 200px;
          font-size: 18px;
          margin: 10px 0;
          padding: 10px;
          border: 1px solid #ccc;
          border-radius: 5px;
        }
        input[type='submit'] {
          width: 100%;
          padding: 10px;
          font-size: 18px;
          color: white;
          background-color: #007BFF;
          border: none;
          border-radius: 5px;
          cursor: pointer;
        }
        input[type='submit']:hover {
          background-color: #0056b3;
        }
        p { font-size: 1.2em; }
        @media (max-width: 480px) {
          .container {
            width: 90%;
          }
        }
      </style>
      <script>
        let isFetching = false;

        function updateTemperature() {
            if (isFetching) return;
            isFetching = true;
            fetch('/get_temp')
            .then(response => response.json())
            .then(data => {
                // Обновляем значение текущей температуры (для отображения)
                document.getElementById("currentTemp").innerText = data.temp;
                // Меняем цвет фона поля ввода установленной температуры:
                // Если достигнута целевая температура – зеленый, иначе – оранжевый
                if(data.reached) {
                  document.getElementById("tempInput").style.backgroundColor = "rgba(0,255,0,0.5)";
                } else {
                  document.getElementById("tempInput").style.backgroundColor = "rgba(255,165,0,0.5)";
                }
            })
            .finally(() => { isFetching = false; });
        }

        function setTemperature(event) {
            event.preventDefault();
            let tempValue = document.getElementById("tempInput").value;
            fetch('/set_temp?temp=' + tempValue, { method: 'POST' })
            .then(response => response.text())
            .then(() => {
                document.getElementById("lastCmd").innerText = "Установить " + tempValue + " °C";
            });
        }

        setInterval(updateTemperature, 1000);
      </script>
     </head>
     <body>
      <div class="container">
        <h1>Управление температурой АЧТ</h1>
        <form onsubmit="setTemperature(event)">
          <label for="tempInput">Установить температуру:</label><br>
          <input type="number" id="tempInput" name="temp" value="%SETPOINT%" step="0.1" min="-20" max="100"><br>
          <input type="submit" value="Установить">
        </form>
        <p class="leftAlign"><b>Текущая температура центрального датчика:</b> <strong><span id="currentTemp">%CURRENTTEMP%</span> °C</strong></p>
        <p class="leftAlign"><b>Последняя команда:</b> <br> <strong><span id="lastCmd">%LASTCMD%</span></strong></p>
        <p><a href="/graph">Подробно</a></p>
      </div>
     </body>
    </html>
)=====";
    
#endif
