#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); // U8X8_PIN_NONE means that there is no reset pin in the display 

void setup(void) {
  Wire.begin(21, 22); // SDA, SCL
  u8g2.begin();
  
  u8g2.clearBuffer();          
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  u8g2.drawStr(10, 30, "Hello world!"); 
  u8g2.sendBuffer();          
}

void loop(void) {
}
