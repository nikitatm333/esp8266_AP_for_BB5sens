#ifndef GRAPH_PAGE_H
#define GRAPH_PAGE_H

const char GRAPH_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>График</title>
    <style>
      body { 
        background-color: #cccccc; 
        font-family: Arial, Helvetica, sans-serif; 
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
        box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        text-align: left;
      }
      a { 
        color: #007BFF; 
        text-decoration: none; 
      }
      .centerAlign {
        text-align: center;
      }
    </style>
    <script>
      function updateGraph() {
        fetch('/graph_svg')
          .then(response => response.text())
          .then(data => {
            document.getElementById('svgContainer').innerHTML = data;
          });
      }
      function updateSensors() {
        // Делаем 2 отдельных запроса
        Promise.all([
          fetch('/sensors'),
          fetch('/pids')
        ])
        .then(responses => Promise.all(responses.map(r => r.json())))
        .then(([sensors, pids]) => {
          let html = "<h3>Датчики температуры</h3>";
          html += "sensor_1: " + sensors.sensor1 + " °C<br>";
          html += "sensor_2: " + sensors.sensor2 + " °C<br>";
          html += "sensor_3: " + sensors.sensor3 + " °C<br>";
          html += "sensor_4: " + sensors.sensor4 + " °C<br>";
          html += "center_sensor: " + sensors.central + " °C<br>";
          html += "<h3>ПИД регуляторы</h3>";
          html += "pid_1: " + pids.pid1 + "<br>";
          html += "pid_2: " + pids.pid2 + "<br>";
          html += "pid_3: " + pids.pid3 + "<br>";
          html += "pid_4: " + pids.pid4 + "<br>";
          document.getElementById("sensorValues").innerHTML = html;
        });
      }
      setInterval(function(){ updateGraph(); updateSensors(); }, 2000);
      window.onload = function() {
        updateGraph();
        updateSensors();
      };
    </script>
  </head>
  <body>
    <div class="container">
      <div id="svgContainer"></div>
      <div id="sensorValues" style="margin-top:20px; font-size:1.2em;"></div>
      <p class="centerAlign"><a href="/">Вернуться на главную</a></p>
    </div>
  </body>
</html>
)=====";

#endif
