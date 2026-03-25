#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// --- Setări Wi-Fi ---
const char *ssid = "iPhone - valibosu"; // Pune numele rețelei tale (între ghilimele)
const char *password = "valibosu";      // Pune parola rețelei tale

// Setarile senzorului
const unsigned DHTPIN = 5;
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

void trimieTemperatura()
{
  float t = dht.readTemperature();
  String textTemperatura = String(t);
  server.send(200, "text/plain", textTemperatura);
}
void trimieUmiditate()
{
  float u = dht.readHumidity();
  String textUmiditate = String(u);
  server.send(200, "text/plain", textUmiditate);
}
void trimitePaginaWeb()
{
  String paginaHTML = R"===(<html>

<head>
    <title>Monitor IoT</title>
    <style>
        body {
            background-color: #222;
            color: white;
            font-family: sans-serif;
            text-align: center;
        }

        .card {
            background-color: #333;
            padding: 20px;
            border-radius: 10px;
            margin: 15px;
            width: 250px;
            display: inline-block;
        }

        span {
            font-size: 30x;
            font-weight: bold;
        }
    </style>
</head>

<body>
    <h1> Statie Meteo Smart</h1>
    <div class="card">
        <p> Temperatura: <span id="valoare_temp">%TEMP%</span> °C</p>
    </div>
    <div class="card">
        <p> Umiditate: <span id="valoare_umiditate">--</span> %</p>
    </div>
    <div class="card">
        <p> Nivelul luminii: <span id="valoare_nivel_lumina">--</span></p>
    </div>
    <script>
        setInterval(function () {
            fetch('/temperatura')
                .then(Response => Response.text())
                .then(data => {
                    document.getElementById("valoare_temp").innerHTML = data;
                });
            fetch('/umiditate')
                .then(response => response.text())
                .then(data => {
                    document.getElementById("valoare_umiditate").innerHTML = data;
                });
        }, 5000);
    </script>
</body>
</html>)===";
  server.send(200, "text/html", paginaHTML);
}
void setup()
{
  // Pornim comunicarea serială pentru a vedea mesaje pe laptop
  Serial.begin(115200);
  delay(10);

  Serial.println("Pornire sistem...");
  dht.begin();

  Serial.println('\n');
  Serial.print("Ma conectez la: ");
  Serial.println(ssid);
  // Începem conexiunea la Wi-Fi
  WiFi.begin(ssid, password);

  // Așteptăm până se conectează (afișăm punctulețe)
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Dacă a ieșit din "while", înseamnă că s-a conectat!
  Serial.println('\n');
  Serial.println("Conexiune WiFi stabilita cu succes! 🎉");
  Serial.print("Adresa IP a ESP8266-ului este: ");
  Serial.println(WiFi.localIP());

  server.on("/temperatura", trimieTemperatura);
  server.on("/umiditate", trimieUmiditate);
  server.on("/", trimitePaginaWeb);

  server.begin();
}

void loop()
{
  // Deocamdată nu facem nimic aici, doar am testat netul.
  server.handleClient();
}
