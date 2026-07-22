#include <SPI.h>
#include <Adafruit_PN532.h>
#include <HID-Project.h>

#define PN532_SS 9
#define BUTTON 2
#define LED 10

Adafruit_PN532 nfc(PN532_SS);

String currentCommand = "";

void blink(int repeat) {
  for (int i = 0; i < repeat; i++) {
    analogWrite(LED, 8);
    delay(120);
    analogWrite(LED, 0);
    delay(120);
  }
}

void slowPrint(const char *str, unsigned long delayMs) {
  while (*str) {
    BootKeyboard.write(*str++);
    delay(delayMs);
  }
}

bool readTag(String &outText) {
  uint8_t uid[7];
  uint8_t uidLength;

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength))
    return false;

  uint8_t data[128];
  int index = 0;

  for (uint8_t page = 4; page < 40; page++) {
    uint8_t buf[4];

    if (!nfc.mifareultralight_ReadPage(page, buf))
      break;

    for (int i = 0; i < 4; i++) {
      data[index++] = buf[i];
    }

    if (page % 4 == 0)
      delay(1);
  }

  int i = 0;
  while (i < index && data[i] != 0x03) i++;
  if (i >= index) return true;

  i++;
  i++;
  i++;
  uint8_t typeLen = data[i++];
  uint8_t payloadLen = data[i++];

  i += typeLen;

  uint8_t langLen = data[i++];
  i += langLen;

  int textLen = payloadLen - 1 - langLen;
  if (textLen <= 0) return true;

  String result = "";

  for (int j = 0; j < textLen && i < index; j++) {
    result += (char)data[i++];
  }

  outText = result;
  return true;
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  BootKeyboard.begin();
  delay(2000);

  blink(3);

  nfc.begin();

  if (!nfc.getFirmwareVersion()) {
    while (1)
      ;
  }

  nfc.SAMConfig();
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {

    String payload;
    if (readTag(payload))
      currentCommand = payload;
    else
      currentCommand = "";

    if (currentCommand.length() > 0) {
      blink(2);
      BootKeyboard.press(KEY_LEFT_GUI);
      BootKeyboard.press(KEY_LEFT_SHIFT);
      BootKeyboard.press('D');
      delay(100);

      BootKeyboard.releaseAll();

      delay(300);

      slowPrint(currentCommand.c_str(), 35);
      delay(50);
      BootKeyboard.write(KEY_RETURN);
    }

    while (digitalRead(BUTTON) == LOW)
      delay(20);
  }
}
