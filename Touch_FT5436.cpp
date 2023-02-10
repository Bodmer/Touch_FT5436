
#include "Touch_FT5436.h"

/***************************************************************************************
** Function name:           serviceInterrupt
** Description:             Touch detect interrupt service routine
***************************************************************************************/
void Touch_FT5436::serviceInterrupt(void) {
    detected = true;
    detachInterrupt(digitalPinToInterrupt(interruptPin));
}

/***************************************************************************************
** Function name:           Touch_FT5436
** Description:             Constructor
***************************************************************************************/
Touch_FT5436::Touch_FT5436(void) {
}

/***************************************************************************************
** Function name:           begin
** Description:             Initialise with pins, int and rst pins optional
***************************************************************************************/
void Touch_FT5436::begin(int8_t sda, int8_t scl, int8_t intPin, int8_t rstPin) {
    detected = false;
    margin = 5;
    pointCount = 0;
    lastPointCount = 0;
    released = 0;
    readToggle = true;

    if (rstPin >= 0)
    {
      pinMode(rstPin, OUTPUT);
      delay(10);
      digitalWrite(rstPin, HIGH);
      delay(100);
    }

    for (uint8_t i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
        pointX[i] = -1;
        pointY[i] = -1;
        lastPointX[i] = -1;
        lastPointY[i] = -1;
    }
    #if defined(ARDUINO_ARCH_RP2040)
      Wire.setSDA(sda);
      Wire.setSCL(scl);
    #else
      Wire.setPins(sda, scl);
    #endif
    Wire.begin();
    Wire.setClock(FT_I2C_FREQ);

    writeReg(FT_REG_DEV_MODE, 0x00);
    writeReg(FT_REG_POINT_RATE, 14);

    interruptPin = intPin;
    if (interruptPin >= 0) {
      attachInterrupt(digitalPinToInterrupt(interruptPin), serviceInterrupt, FALLING);
    }
}

/***************************************************************************************
** Function name:           jitterMargin
** Description:             Jitter allowance for a change in touch position
***************************************************************************************/
int8_t Touch_FT5436::jitterMargin(int8_t margin) {
    if (margin >= 0) this->margin = margin;
    return this->margin;
}

/***************************************************************************************
** Function name:           maxPointCount
** Description:             Set/get maximum points (returns last value if count = 0)
***************************************************************************************/
uint8_t Touch_FT5436::maxPointCount(int8_t count) {
  if (count >= 0 && count <= 5) maxPoints = count;
  return maxPoints;
}

/***************************************************************************************
** Function name:           pointDetected
** Description:             If touch detected, read points and return true, else false
***************************************************************************************/
bool Touch_FT5436::pointDetected(void) {
  bool changed = false;
  static bool  first = true;

  if (interruptPin < 0 || detected) {
    readPoints();
    detected = false;
    released = 0;
    if (pointCount < lastPointCount) released = lastPointCount - pointCount;

    if (interruptPin >= 0) {
      attachInterrupt(digitalPinToInterrupt(interruptPin), serviceInterrupt, FALLING);
    }

    // If the positions are different then changed is true
    for (uint8_t i = 0; i < pointCount; i++) {
      if (abs(lastPointX[i] - pointX[i]) > margin
          || abs(lastPointY[i] - pointY[i]) > margin)
        changed = true;
        // Copy points
        lastPointX[i] = pointX[i];
        lastPointY[i] = pointY[i];
        
    }

    // or, if the point count is different then changed is true
    if (lastPointCount != pointCount) changed = true;
    // Copy point count
    lastPointCount = pointCount;
  }

  // Make sure lastPoints are updated before returning values
  if (first) { first = false; changed = false; }

  return changed;
}

/***************************************************************************************
** Function name:           releaseCount
** Description:             Return count if touch points released
***************************************************************************************/
uint8_t Touch_FT5436::releaseCount(void) {
  if (released) {
    uint8_t r = released;
    released = 0;
    return r;
  }
  return 0;
}
/***************************************************************************************
** Function name:           getPointCount
** Description:             Get multi-touch point count (0 = none yet or 1-5 touches)
***************************************************************************************/
uint8_t  Touch_FT5436::getPointCount(void) {
    return pointCount;
}

/***************************************************************************************
** Function name:           getPointX
** Description:             Get X coordinate of touch point 1-5
***************************************************************************************/
int16_t Touch_FT5436::getPointX(uint8_t point) {
    if (point > pointCount || point == 0) return -1;
    return pointX[point - 1];
}

/***************************************************************************************
** Function name:           getPointY
** Description:             Get Y coordinate of touch point 1-5
***************************************************************************************/
int16_t Touch_FT5436::getPointY(uint8_t point) {
    if (point > pointCount || point == 0) return -1;
    return pointY[point - 1];
}

/***************************************************************************************
** Function name:           getGesture
** Description:             Get gesture (not supported by all controllers)
***************************************************************************************/
uint8_t Touch_FT5436::getGesture(void) {
  /*
    uint8_t gestureID = reg[FT_GEST_ID];
    uint8_t gesture = 0;         //  No gesture default

    switch (gestureID) {
      case FT5436_GEST_ID_ZOOM_IN:
        gesture = ZOOM_IN;       //  1
        break;
      case FT5436_GEST_ID_ZOOM_OUT:
        gesture = ZOOM_OUT;      //  2
        break;
      // These are not always supported
      case FT5436_GEST_ID_MOVE_RIGHT:
        gesture = SWIPE_RIGHT;   //  3 = to  3 o'clock
        break;
      case FT5436_GEST_ID_MOVE_DOWN:
        gesture = SWIPE_DOWN;    //  6 = to  6 o'clock
        break;
      case FT5436_GEST_ID_MOVE_LEFT:
        gesture = SWIPE_LEFT;    //  9 = to  9 o'clock
        break;
      case FT5436_GEST_ID_MOVE_UP:
        gesture = SWIPE_UP;      // 12 = to  12 o'clock
        break;

    }

    return gesture;
*/
  return 0;
}

/***************************************************************************************
** Function name:           readPoints
** Description:             Read the points from the sensor and save them
***************************************************************************************/
void Touch_FT5436::readPoints(void) {
    uint8_t n = 0;

    Wire.beginTransmission(FT_I2C_ADDR);
    Wire.write(FT_REG_DEV_MODE);
    Wire.endTransmission();
    Wire.requestFrom(FT_I2C_ADDR, POINT_READ_BUF);
    while (Wire.available() && n < POINT_READ_BUF) reg[n++] = Wire.read();
    Wire.endTransmission(FT_I2C_ADDR);

    pointCount = reg[FT_TD_STATUS] & FT_MAX_ID;

    if (pointCount > 5) pointCount = 5;

    for (uint8_t i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
        uint8_t r = FT_ONE_TCH_LEN * i;
        pointX[i] = ((reg[FT_TOUCH_X_H_POS + r] & 0x0F) << 8) | reg[FT_TOUCH_X_L_POS + r];
        pointY[i] = ((reg[FT_TOUCH_Y_H_POS + r] & 0x0F) << 8) | reg[FT_TOUCH_Y_L_POS + r];

        // Check for bad coordinates when interrupt not used
        //if ((reg[FT_TOUCH_X_H_POS + r] & 0xC0) != 0x80 || (reg[FT_TOUCH_Y_H_POS + r] & 0xF0) == 0xF0) {
        //  pointCount = i;
        //  break;
        //}
    }

/*
    Wire.beginTransmission(FT_I2C_ADDR);
    Wire.write(FT_REG_DEV_MODE);
    Wire.endTransmission();
    Wire.requestFrom(FT_I2C_ADDR, POINT_READ_BUF);
    while (Wire.available() && n < POINT_READ_BUF) reg[n++] = Wire.read();
    Wire.endTransmission(FT_I2C_ADDR);
*/
    // Check to see if point count has reduced
    //if (pointCount > (reg[FT_TD_STATUS] & FT_MAX_ID)) pointCount = reg[FT_TD_STATUS] & FT_MAX_ID;

    if (pointCount > maxPoints) pointCount = maxPoints;
    
}

/***************************************************************************************
** Function name:           writeReg
** Description:             Write data or command to sensor
***************************************************************************************/
void Touch_FT5436::writeReg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(FT_I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission(FT_I2C_ADDR);
}
