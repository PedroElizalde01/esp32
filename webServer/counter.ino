#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "wifi";
const char* password = "pasdword";

WebServer server(80);

int contador = 0;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Conectando a WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Conectado a la red WiFi: ");
  Serial.println(ssid);
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/contador", handleContador);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
  
  static unsigned long ultimaActualizacion = 0;
  unsigned long tiempoActual = millis();
  
  if (tiempoActual - ultimaActualizacion >= 1000) {
    contador++;
    ultimaActualizacion = tiempoActual;
    Serial.print("Contador: ");
    Serial.println(contador);
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html>\
<html>\
<head>\
  <title>Contador ESP32</title>\
  <meta name='viewport' content='width=device-width, initial-scale=1'>\
  <style>\
    body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 30px; }\
    .contador { font-size: 80px; margin: 20px; color: #0066cc; }\
  </style>\
  <script>\
    setInterval(function() {\
      fetch('/contador')\
        .then(response => response.text())\
        .then(data => {\
          document.getElementById('valorContador').innerHTML = data;\
        });\
    }, 1000);\
  </script>\
</head>\
<body>\
  <h1>Contador ESP32</h1>\
  <div class='contador'><span id='valorContador'>" + String(contador) + "</span></div>\
  <p>Este contador se actualiza automáticamente cada segundo</p>\
</body>\
</html>";
  
  server.send(200, "text/html", html);
}

void handleContador() {
  server.send(200, "text/plain", String(contador));
}
