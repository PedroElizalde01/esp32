#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "nombre de wifi";     
const char* password = "contraseña wifi"; 

const char* botToken = "token bot";

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
  
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  
  Serial.println("Bot de Telegram iniciado. Envía mensajes al bot para verlos en el monitor serial.");
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
      
      Serial.println("-----");
      Serial.print("De: ");
      Serial.println(from_name);
      Serial.print("ID del chat: ");
      Serial.println(chat_id);
      Serial.print("Mensaje: ");
      Serial.println(text);
      Serial.println("-----");
      
      // answer
      // bot.sendMessage(chat_id, "Mensaje recibido: " + text, "");
    }
  }
}
