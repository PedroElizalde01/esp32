#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "nombre de tu wifi";     // CHANGE
const char* password = "contraseña"; // CHANGE
const char* botToken = "telegram token";   // CHANGE

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

const unsigned long checkInterval = 1000;
unsigned long lastCheckTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();
  
  Serial.println("Bot de Telegram iniciado.");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastCheckTime > checkInterval) {
    lastCheckTime = currentTime;

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    for (int i = 0; i < numNewMessages; i++) {
      String chat_id = bot.messages[i].chat_id;
      String text = bot.messages[i].text;
      String from_name = bot.messages[i].from_name;

      Serial.println("Nuevo mensaje:");
      Serial.println("De: " + from_name);
      Serial.println("Mensaje: " + text);

      enviarUbicacion(chat_id);
    }
  }
}

void enviarUbicacion(String chat_id) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient https;
    String url = "https://api.telegram.org/bot" + String(botToken) + "/sendLocation";

    https.begin(url);
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // fake location
    float fake_latitude = -34.6037;
    float fake_longitude = -58.3816;

    String payload = "chat_id=" + chat_id +
                     "&latitude=" + String(fake_latitude, 6) +
                     "&longitude=" + String(fake_longitude, 6);

    int httpResponseCode = https.POST(payload);

    if (httpResponseCode > 0) {
      String response = https.getString();
      Serial.println("Respuesta Telegram:");
      Serial.println(response);
    } else {
      Serial.print("Error en POST: ");
      Serial.println(httpResponseCode);
    }
    https.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}
