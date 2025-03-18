#ifndef GRAPH_PAGE_H
#define GRAPH_PAGE_H

const char GRAPH_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>График температур</title>
    <style>
      body {
        background-color: #cccccc;
        font-family: Arial, Helvetica, sans-serif;
        color: #000088;
        margin: 0;
        padding: 20px;
        text-align: center;
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
      h1 {
        font-size: 20px;
        margin-bottom: 10px;
        text-align: center;
      }
      h3 {
        font-size: 16px;
        margin: 10px 0 5px;
      }
      p {
        font-size: 14px;
        margin: 5px 0;
      }
      a {
        color: #007BFF;
        text-decoration: none;
        font-size: 14px;
      }
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
        Promise.all([
          fetch('/sensors'),
          fetch('/pids')
        ])
        .then(responses => Promise.all(responses.map(r => r.json())))
        .then(([sensors, pids]) => {
          let html = "<h3>Датчики температуры: </h3>";
          html += "<p>Sensor 1: " + sensors.sensor1 + " °C</p>";
          html += "<p>Sensor 2: " + sensors.sensor2 + " °C</p>";
          html += "<p>Sensor 3: " + sensors.sensor3 + " °C</p>";
          html += "<p>Sensor 4: " + sensors.sensor4 + " °C</p>";
          html += "<p>Central: " + sensors.central + " °C</p>";
          html += "<h3>ПИД регуляторы: </h3>";
          html += "<p>PID 1: " + pids.pid1 + "</p>";
          html += "<p>PID 2: " + pids.pid2 + "</p>";
          html += "<p>PID 3: " + pids.pid3 + "</p>";
          html += "<p>PID 4: " + pids.pid4 + "</p>";
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
      <h1>Зависимость температуры центрального датчика от времени</h1>
      <div id="svgContainer"></div>
      <div id="sensorValues" style="margin-top:20px;"></div>
      <div class="centerAlign">
        <a href="/" class="buttonLink">Вернуться на главную</a>
      </div>
    </div>
  </body>
</html>
)=====";

#endif
