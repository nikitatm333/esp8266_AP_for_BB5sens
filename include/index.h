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
        font-family: Arial, Helvetica, sans-serif;
        color: #000088;
        margin: 0;
        padding: 20px;
      }
      .container {
        position: relative;
        max-width: 400px;
        margin: auto;
        padding: 20px;
        background: white;
        border-radius: 10px;
        box-shadow: 0px 4px 6px rgba(0,0,0,0.1);
      }
      h1 {
        margin: 0;
        font-size: 22px;
        text-align: center;
        margin-bottom: 20px;
      }
      label {
        display: block;
        margin-bottom: 5px;
        font-weight: bold;
      }
      input[type='number'], input[type='text'], input[type='password'] {
        width: 100%;
        padding: 10px;
        font-size: 16px;
        border: 1px solid #ccc;
        border-radius: 5px;
        box-sizing: border-box;
        margin-bottom: 15px;
      }
      input[type='submit'] {
        width: 100%;
        padding: 12px;
        font-size: 16px;
        color: white;
        background-color: #007BFF;
        border: none;
        border-radius: 5px;
        cursor: pointer;
      }
      input[type='submit']:hover {
        background-color: #0056b3;
      }
      p {
        font-size: 1.1em;
        margin: 10px 0;
      }
      /* Для переноса последней команды */
      .lastCmd {
        display: block;
      }
      /* Кнопка для ссылки "Подробно" */
      .buttonLink {
        display: inline-block;
        padding: 8px 16px;
        border: none;
        border-radius: 5px;
        background-color: #007BFF;
        color: white;
        text-decoration: none;
        font-size: 14px;
        cursor: pointer;
      }
      .buttonLink:hover {
        background-color: #0056b3;
      }
      /* Контейнер для кнопки "Подробно" и иконки настройки */
      .buttonContainer {
        position: relative;
        text-align: center;
        margin: 15px 0;
      }
      /* Иконка настройки в виде кнопки */
      .buttonIcon {
        display: inline-block;
        width: 40px;
        height: 40px;
        border-radius: 50%;
        background-color: #007BFF;
        color: white;
        text-align: center;
        line-height: 40px;
        font-size: 20px;
        cursor: pointer;
        transition: background-color 0.3s;
      }
      .buttonIcon:hover {
        background-color: #0056b3;
      }
      /* Позиционирование иконки (гаечного ключа) */
      .buttonContainer #settingsIcon {
        position: absolute;
        right: 10px;
        top: 50%;
        transform: translateY(-50%);
      }
      @media (max-width: 480px) {
        .container {
          width: 90%;
        }
      }
      /* Стили модального окна */
      .modal {
        display: none;
        position: fixed;
        z-index: 100;
        left: 0;
        top: 0;
        width: 100%;
        height: 100%;
        overflow: auto;
        background-color: rgba(0,0,0,0.4);
      }
      .modal-content {
        background-color: white;
        margin: 15% auto;
        padding: 20px;
        border-radius: 10px;
        width: 90%;
        max-width: 400px;
        box-shadow: 0px 4px 6px rgba(0,0,0,0.1);
      }
      .close {
        color: #aaa;
        float: right;
        font-size: 28px;
        font-weight: bold;
        cursor: pointer;
      }
      .close:hover,
      .close:focus {
        color: black;
        text-decoration: none;
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
            document.getElementById("currentTemp").innerText = data.temp;
            if(data.reached) {
              document.getElementById("tempInput").style.backgroundColor = "rgba(0,255,0,0.5)";
            } else {
              document.getElementById("tempInput").style.backgroundColor = "rgba(255, 166, 0, 0.38)";
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
      document.addEventListener('DOMContentLoaded', function() {
        var modal = document.getElementById("settingsModal");
        var settingsIcon = document.getElementById("settingsIcon");
        var closeBtn = document.getElementsByClassName("close")[0];
        settingsIcon.onclick = function() {
          modal.style.display = "block";
        }
        closeBtn.onclick = function() {
          modal.style.display = "none";
        }
        window.onclick = function(event) {
          if (event.target == modal) {
            modal.style.display = "none";
          }
        }
        document.getElementById("apSettingsForm").addEventListener("submit", function(e) {
          e.preventDefault();
          if(!confirm("Изменение настроек влечет отключение от текущей AP. Продолжить?")) return;
          let formData = new FormData(this);
          fetch('/set_ap', {
            method: 'POST',
            body: new URLSearchParams(formData)
          })
          .then(response => {
            if(response.ok) {
              alert('Настройки сохранены. Отключите устройство от питания и включите повторно...');
              setTimeout(() => location.reload(), 3000);
            }
          });
        });
      });
      setInterval(updateTemperature, 1000);
    </script>
  </head>
  <body>
    <div class="container">
      <h1>Управление температурой АЧТ</h1>
      <form onsubmit="setTemperature(event)">
        <label for="tempInput">Установить температуру:</label>
        <input type="number" id="tempInput" name="temp" value="%SETPOINT%" step="0.1" min="-20" max="100" required>
        <input type="submit" value="Установить">
      </form>
      <p><b>Текущая температура:</b><br><span id="currentTemp">%CURRENTTEMP%</span> °C</p>
      <p><b>Последняя команда:</b><br><span id="lastCmd">%LASTCMD%</span></p>
      <div class="buttonContainer">
        <a href="/graph" class="buttonLink">Подробно</a>
        <div id="settingsIcon" class="buttonIcon">🔧</div>
      </div>
    </div>
    <!-- Модальное окно настроек -->
    <div id="settingsModal" class="modal">
      <div class="modal-content">
        <span class="close">&times;</span>
        <h2>Настройки точки доступа</h2>
        <form id="apSettingsForm">
          <label for="ap_ssid_input">SSID:</label>
          <input type="text" id="ap_ssid_input" name="ssid" value="%AP_SSID%" required>
          <label for="ap_password_input">PASSWORD:</label>
          <input type="password" id="ap_password_input" name="password" value="%AP_PASSWORD%">
          <input type="submit" value="Сохранить">
        </form>
      </div>
    </div>
  </body>
</html>
)=====";

#endif