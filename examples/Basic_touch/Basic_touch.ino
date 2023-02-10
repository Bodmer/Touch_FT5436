// Use of the capacitive touch panel
#define SDA_FT5436  21
#define SCL_FT5436  22
#define INT_FT5436  18
 
#include <Touch_FT5436.h>

Touch_FT5436 touch = Touch_FT5436();

void setup() {
  Serial.begin(115200);
  touch.begin(SDA_FT5436, SCL_FT5436, INT_FT5436);

  touch.maxPointCount(5);
  Serial.print("Maximum multi-point count = ");
  Serial.println(touch.maxPointCount());

  // Position change must exceed this margin before a single continuous touch is reported again.
  touch.jitterMargin(5);
  Serial.print("Touch jitter margin = ");
  Serial.println(touch.maxPointCount());
}

void loop() {
  if(touch.pointDetected()) {
    uint8_t touches = touch.getPointCount();
    for (uint8_t i = 1; i <= touches; i++) {
      Serial.print("Point "); Serial.print(i);
      Serial.print(": x = "); Serial.print(touch.getPointX(i));
      Serial.print(", y = "); Serial.println(touch.getPointY(i));
    }
  }
}
