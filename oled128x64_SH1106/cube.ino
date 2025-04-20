#include <U8g2lib.h>
#include <Wire.h>
#include <math.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const float CUBE_SIZE = 30.0;
const int CENTER_X = 64;
const int CENTER_Y = 32;
const float ROTATION_SPEED = 0.03;  // Slow = smooth
const float VIEW_DISTANCE = 3.0;
const uint8_t FRAME_DELAY = 60;

const float VERTICES[8][3] PROGMEM = {
  {-1, -1, -1}, {1, -1, -1},
  {1, 1, -1}, {-1, 1, -1},
  {-1, -1, 1}, {1, -1, 1},
  {1, 1, 1}, {-1, 1, 1}
};

const uint8_t EDGES[12][2] PROGMEM = {
  {0, 1}, {1, 2}, {2, 3}, {3, 0},
  {4, 5}, {5, 6}, {6, 7}, {7, 4},
  {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

float angleX = 0;
float angleY = 0;
unsigned long lastFrameTime = 0;

void setup() {
  Wire.begin(21, 22);
  u8g2.begin();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastFrameTime < FRAME_DELAY) {
    return;
  }
  lastFrameTime = currentTime;

  u8g2.clearBuffer();
  
  int16_t projected[8][2];
  
  for (uint8_t i = 0; i < 8; i++) {
    float x = pgm_read_float(&VERTICES[i][0]);
    float y = pgm_read_float(&VERTICES[i][1]);
    float z = pgm_read_float(&VERTICES[i][2]);
    
    float tempY = cos(angleX) * y - sin(angleX) * z;
    float tempZ = sin(angleX) * y + cos(angleX) * z;
    y = tempY;
    z = tempZ;
    
    float tempX = cos(angleY) * x - sin(angleY) * z;
    tempZ = sin(angleY) * x + cos(angleY) * z;
    x = tempX;
    z = tempZ;
    
    float scale = 1.0 / (z + VIEW_DISTANCE);
    projected[i][0] = CENTER_X + x * CUBE_SIZE * scale;
    projected[i][1] = CENTER_Y + y * CUBE_SIZE * scale;
  }
  
  for (uint8_t i = 0; i < 12; i++) {
    uint8_t a = pgm_read_byte(&EDGES[i][0]);
    uint8_t b = pgm_read_byte(&EDGES[i][1]);
    
    u8g2.drawLine(
      projected[a][0], projected[a][1],
      projected[b][0], projected[b][1]
    );
  }
  
  u8g2.sendBuffer();
  
  angleX += ROTATION_SPEED * 0.5;  // slower rotation on X axis
  angleY += ROTATION_SPEED;
}

