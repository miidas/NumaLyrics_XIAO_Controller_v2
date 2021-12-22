// AppData\Local\Arduino15\packages\Seeeduino\hardware\samd\1.8.2\variants\XIAO_m0\variant.h
//#define PIN_LED_RXL          (12u)
//#define PIN_LED_TXL          (11u)

#define LOOP_WAIT 200
#define TIMEOUT 200

#define BTN_CNT 4
#define BTN_WAIT 100

#define BTN_ACT_CLICK 0
#define BTN_ACT_DOUBLE_CLICK 1
#define BTN_ACT_NONE 0xFF

// SBA
// 0x00 - Click only
// 0x01 - Click & Double click
// 0x02 - Click, Double click & Long click
#define DEVICE_SBA 0x01
#define DEVICE_UUID "5b22945b-4fd3-401a-9b13-1b2fee2520c9"

#define FW_VERSION "v1.0.0"
#define FW_NAME "Xiao Controller for NumaLyrics"

void setup()
{
  for (auto i = 0; i < BTN_CNT; i++) {
    pinMode(5 + i, INPUT_PULLUP);
  }
}

void loop()
{
  static char buf[32];
  static uint8_t ret;
  while (1)
  {
    for (auto i = 0; i < BTN_CNT; i++) {
      ret = readBtnAction(5 + i);
      if (ret != BTN_ACT_NONE) {
        sprintf(buf, "BTN+%d,%d", i, ret);
        Serial.println(buf);
        delay(LOOP_WAIT);
      }
    }
    serialHandler();
  }
}

void serialHandler()
{
  if (Serial.available() > 0)
  {
    String cmd = Serial.readStringUntil('\n');

    Serial.print(cmd);
    Serial.print('+');

    if (cmd.startsWith("IDN")) // Identification
    {
      Serial.print(FW_NAME);
      Serial.print(',');
      Serial.print(FW_VERSION);
      Serial.print(',');
      Serial.println(DEVICE_UUID);
    }
    else if (cmd.startsWith("NOB")) // Number of buttons
    {
      Serial.println(BTN_CNT);
    }
    else if (cmd.startsWith("SBA")) // Supported button action
    {
      Serial.println(DEVICE_SBA);
    }
    else
    {
      Serial.println();
    }
  }
}

uint8_t readBtnAction(uint8_t port) {
  static unsigned long ts, te, td;
  ts = millis();
  if (digitalRead(port) == LOW) {
    while (digitalRead(port) != HIGH);
    te = millis();
    td = te - ts;
    if (td < TIMEOUT) {
      delay(BTN_WAIT);
      while (millis() - te < TIMEOUT) {
        if (digitalRead(port) == LOW) {
          while (digitalRead(port) != HIGH);
          return BTN_ACT_DOUBLE_CLICK;
        }
      }
    }
    return BTN_ACT_CLICK;
  }
  return BTN_ACT_NONE;
}
