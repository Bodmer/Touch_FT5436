// Use of the capacitive touch panel
#define SDA_FT5436  4
#define SCL_FT5436  5
#define INT_FT5436  17
 
#include <Touch_FT5436.h>

Touch_FT5436 touch = Touch_FT5436();

#include <TFT_eSPI.h>

// Library instance
TFT_eSPI    tft = TFT_eSPI();         // Declare object "tft"

uint16_t col[5] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_MAGENTA, TFT_CYAN };

void setup() {
  Serial.begin(115200);
  //while(!Serial);
  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  touch.begin(SDA_FT5436, SCL_FT5436, INT_FT5436, 2);

  touch.maxPointCount(5);
  Serial.print("Maximum multi-point count = ");
  Serial.println(touch.maxPointCount());

  // Position change must exceed this margin before a single continuous touch is reported again.
  touch.jitterMargin(0);
  Serial.print("Touch jitter margin = ");
  Serial.println(touch.maxPointCount());

  tft.init();
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  if(touch.pointDetected()) {
    uint8_t touches = touch.getPointCount();
    for (uint8_t i = 1; i <= touches; i++) {
      //Serial.print("Point "); Serial.print(i);
      //Serial.print(": x = "); Serial.print(touch.getPointX(i));
      //Serial.print(", y = "); Serial.println(touch.getPointY(i));
      tft.fillCircle(touch.getPointX(i), touch.getPointY(i), 2, col[i-1]);
    }
  }
}
