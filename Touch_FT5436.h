/* This is a library to handling capacitive multitouch sensors using the FocalTech FT5436
   True Multi-Touch Capacitive Touch Panel Controller
   
   Created by Bodmer 17-04-2021
*/
 
#ifndef Touch_FT5436_H
#define Touch_FT5436_H

#include "Arduino.h"
#include "Wire.h"
 
// Controller does not report these gestures...
// Gestures
//#define FT_NO_GESTURE   0
//#define FT_ZOOM_IN      1
//#define FT_ZOOM_OUT     2
//#define FT_SWIPE_RIGHT  3 // To  3 o'clock
//#define FT_SWIPE_DOWN   6 // To  6 o'clock
//#define FT_SWIPE_LEFT   9 // To  9 o'clock
//#define FT_SWIPE_UP    12 // To 12 o'clock

/* FT5436 definitions */
#define FT_I2C_ADDR 0x38
#define FT_I2C_FREQ 400000

#define FT_META_REGS    3
#define FT_ONE_TCH_LEN  6
#define FT_TCH_LEN(x)   (FT_META_REGS + FT_ONE_TCH_LEN * x)

#define CFG_MAX_TOUCH_POINTS 5

#define FT_TD_STATUS       2
#define FT_TOUCH_EVENT_POS 3

#define FT_TOUCH_CONTACT   2
#define FT_TOUCH_X_H_POS   3
#define FT_TOUCH_X_L_POS   4
#define FT_TOUCH_Y_H_POS   5
#define FT_TOUCH_Y_L_POS   6

#define FT_TOUCH_ID_POS    5
#define FT_TOUCH_DOWN      0

#define POINT_READ_BUF  (3 + FT_ONE_TCH_LEN * CFG_MAX_TOUCH_POINTS)

#define FT_PRESS              0x7F
#define FT_MAX_ID             0x0F

/*register address*/

#define FT_REG_DEV_MODE       0x00
#define FT_DEV_MODE_REG_CAL   0x02
#define FT_REG_ID             0xA3
#define FT_REG_PMODE          0xA5
#define FT_REG_FW_VER         0xA6
#define FT_REG_FW_VENDOR_ID   0xA8
#define FT_REG_POINT_RATE     0x88
#define FT_REG_THGROUP        0x80
#define FT_REG_ECC            0xCC
#define FT_REG_RESET_FW       0x07

#define FT_REG_FW_MIN_VER     0xB2
#define FT_REG_FW_SUB_MIN_VER 0xB3

/* power register bits*/
#define FT_PMODE_ACTIVE       0x00
#define FT_PMODE_MONITOR      0x01
#define FT_PMODE_STANDBY      0x02
#define FT_PMODE_HIBERNATE    0x03
#define FT_FACTORYMODE_VALUE  0x40
#define FT_WORKMODE_VALUE     0x00
#define FT_RST_CMD_REG1       0xFC
#define FT_RST_CMD_REG2       0xBC
#define FT_READ_ID_REG        0x90
#define FT_ERASE_APP_REG      0x61
#define FT_ERASE_PANEL_REG    0x63
#define FT_FW_START_REG       0xBF

#define FT_STATUS_NUM_TP_MASK 0x0F

#define FT_VTG_MIN_UV      2600000
#define FT_VTG_MAX_UV      3300000
#define FT_I2C_VTG_MIN_UV  1800000
#define FT_I2C_VTG_MAX_UV  1800000

#define FT_COORDS_ARR_SIZE 4
#define MAX_BUTTONS        4
#define FT_FW_NAME_MAX_LEN 50

#define FT_8BIT_SHIFT      8
#define FT_4BIT_SHIFT      4


#define FT5X16_ID   0x0A
#define FT5X06_ID   0x55
#define FT6X06_ID   0x06
#define FT6X36_ID   0x36

#define FT_UPGRADE_AA   0xAA
#define FT_UPGRADE_55   0x55

#define FT_FW_MIN_SIZE  8
#define FT_FW_MAX_SIZE  (54 * 1024)

static volatile bool   detected;
static volatile int8_t interruptPin;

class Touch_FT5436 {
public:
  Touch_FT5436(void);
 
  void     begin(int8_t sda, int8_t scl, int8_t intPin = -1, int8_t rstPin = -1);

  int8_t   jitterMargin(int8_t margin = -1);
  uint8_t  maxPointCount(int8_t count = -1);

  bool     pointDetected(void);
  uint8_t  releaseCount(void);

  uint8_t  getPointCount(void);

  int16_t  getPointX(uint8_t point);
  int16_t  getPointY(uint8_t point);
  uint8_t  getGesture(void);

           // Copy of register values
  uint8_t  reg[POINT_READ_BUF];
private:
  static void serviceInterrupt(void);          // Touch detect interrupt
  void     writeReg(uint8_t reg, uint8_t val);
  void     readPoints(void);

  uint8_t  maxPoints;  // How many multi-point touches will be reported
  uint8_t  margin;

  uint8_t  pointCount; // How many touch points have been detected
  int16_t  pointX[CFG_MAX_TOUCH_POINTS];  // x coordinates of touches
  int16_t  pointY[CFG_MAX_TOUCH_POINTS];  // y coordinates of touches

           // These are used to detect changes in touch position or count
  uint8_t  lastPointCount;
  int16_t  lastPointX[CFG_MAX_TOUCH_POINTS];
  int16_t  lastPointY[CFG_MAX_TOUCH_POINTS];

  uint8_t  released;
  bool     readToggle;

  //         // Copy of register values
  //uint8_t  reg[FT5436_REG_COUNT];
};
 
#endif
 