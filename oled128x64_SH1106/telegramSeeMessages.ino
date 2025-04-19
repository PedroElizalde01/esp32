#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <vector>

// Pantalla OLED SH1106 por I2C (SDA=21, SCL=22)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// WiFi
const char* ssid = "wifi";
const char* password = "password";

// Telegram
const char* botToken = "toeken";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

const unsigned long checkInterval = 1000;
unsigned long lastCheckTime = 0;


void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(21, 22); // SDA, SCL
  u8g2.begin();

  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(0, 12, "Conectando a:");
  u8g2.drawStr(0, 24, ssid);
  u8g2.sendBuffer();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int dotX = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    u8g2.drawStr(dotX, 40, ".");
    dotX += 6;
    if (dotX > 120) dotX = 0;
    u8g2.sendBuffer();
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "WiFi conectado");
  u8g2.drawStr(0, 24, "IP:");
  u8g2.setCursor(0, 36);
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();

  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

void displayWrappedText(String text, uint8_t xStart = 8, uint8_t yStart = 28, uint8_t lineHeight = 14) {
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  int maxWidth = 120 - xStart - 4;
  std::vector<String> lines;

  String currentLine = "";
  String word = "";
  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c == ' ' || c == '\n') {
      String testLine = currentLine + word + " ";
      if (u8g2.getStrWidth(testLine.c_str()) > maxWidth) {
        lines.push_back(currentLine);
        currentLine = word + " ";
      } else {
        currentLine = testLine;
      }
      word = "";
      if (c == '\n') {
        lines.push_back(currentLine);
        currentLine = "";
      }
    } else {
      word += c;
    }
  }
  currentLine += word;
  if (currentLine.length() > 0) lines.push_back(currentLine);

  if (lines.size() * lineHeight <= (64 - yStart)) {
    for (size_t i = 0; i < lines.size(); i++) {
      u8g2.setCursor(xStart, yStart + i * lineHeight);
      u8g2.print(lines[i]);
    }
    u8g2.sendBuffer();
  } else {
    for (int offset = 0; offset < lines.size() * lineHeight - (64 - yStart) + lineHeight; offset++) {
      u8g2.clearBuffer();
      u8g2.drawRFrame(0, 0, 128, 64, 4);
      u8g2.setCursor(8, 12);
      u8g2.setFont(u8g2_font_unifont_t_symbols);
      u8g2.print("Mensaje largo");

      u8g2.setFont(u8g2_font_unifont_t_symbols);
      for (size_t i = 0; i < lines.size(); i++) {
        int y = yStart + i * lineHeight - offset;
        if (y > 12 && y < 64) {
          u8g2.setCursor(xStart, y);
          u8g2.print(lines[i]);
        }
      }
      u8g2.sendBuffer();
      delay(80);
    }
  }
}

void loop() {
  unsigned long currentTime = millis();
  u8g2.clearBuffer();

  if (currentTime - lastCheckTime > checkInterval) {
    lastCheckTime = currentTime;

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    for (int i = 0; i < numNewMessages; i++) {
      String chat_id = bot.messages[i].chat_id;
      String text = bot.messages[i].text;
      String from_name = bot.messages[i].from_name;

      u8g2.clearBuffer();
      u8g2.drawRFrame(0, 0, 128, 64, 4);
      u8g2.setFont(u8g2_font_6x12_tr);
      u8g2.setCursor(8, 12);
      u8g2.print("De: ");
      u8g2.print(from_name);
      displayWrappedText(text);
      delay(500);
    }
  }
}
