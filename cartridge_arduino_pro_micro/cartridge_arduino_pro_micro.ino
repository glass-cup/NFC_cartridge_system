#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Keyboard.h>

#define PN532_SS 9
#define BUTTON 2
#define LED 10

Adafruit_PN532 nfc(PN532_SS);

// ---- sizing ----
// Read buffer: big enough for a full tag dump.
// Command buffer: adjust this to your real maximum typed command length.
constexpr size_t TAG_BUFFER_SIZE = 512;
constexpr size_t COMMAND_BUFFER_SIZE = 256;   // 255 chars + null terminator

char currentCommand[COMMAND_BUFFER_SIZE + 1] = {0};
uint8_t tagData[TAG_BUFFER_SIZE];

// ---- UI ----
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
    Keyboard.write(*str++);
    delay(delayMs);
  }
}

// ---- read tag payload ----
// Returns true if a tag was seen. If a valid text payload was extracted,
// it is copied into outText.
bool readTag(char *outText, size_t outSize) {
  outText[0] = '\0';

  uint8_t uid[7];
  uint8_t uidLength = 0;

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    return false;
  }

  size_t index = 0;

  // Read enough pages to cover the tag buffer.
  // For NTAG-type tags, pages are 4 bytes each.
  for (uint8_t page = 4; page < 40; page++) {
    uint8_t buf[4];

    if (!nfc.mifareultralight_ReadPage(page, buf)) {
      break;
    }

    if (index + 4 > TAG_BUFFER_SIZE) {
      break;
    }

    for (int i = 0; i < 4; i++) {
      tagData[index++] = buf[i];
    }
  }

  // Find NDEF TLV (0x03)
  size_t i = 0;
  while (i < index && tagData[i] != 0x03) {
    i++;
  }

  if (i >= index) {
    return true; // tag seen, but no NDEF TLV found
  }

  i++; // skip TLV tag

  if (i >= index) {
    return true;
  }

  // Standard TLV length byte
  uint8_t tlvLen = tagData[i++];

  if (i + tlvLen > index) {
    return true;
  }

  // NDEF record header
  if (i + 3 > index) {
    return true;
  }

  uint8_t recordHeader = tagData[i++];
  uint8_t typeLen = tagData[i++];
  uint8_t payloadLen = tagData[i++];

  (void)recordHeader; // not used in this simple parser

  if (i + typeLen > index) {
    return true;
  }

  // Most text records use type "T" (1 byte). Skip the type field.
  i += typeLen;

  if (i >= index) {
    return true;
  }

  // Text record payload:
  // byte 0 = status, lower 6 bits = language code length
  uint8_t status = tagData[i++];
  uint8_t langLen = status & 0x3F;

  if (langLen > payloadLen) {
    return true;
  }

  if (i + langLen > index) {
    return true;
  }

  i += langLen;

  int textLen = payloadLen - 1 - langLen;
  if (textLen <= 0) {
    return true;
  }

  size_t copyLen = (size_t)textLen;
  if (copyLen >= outSize) {
    copyLen = outSize - 1; // truncate safely
  }

  if (i + copyLen > index) {
    copyLen = index - i;
  }

  memcpy(outText, &tagData[i], copyLen);
  outText[copyLen] = '\0';

  return true;
}

// ---- setup ----
void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  Keyboard.begin();
  delay(2000);

  blink(3);

  nfc.begin();

  if (!nfc.getFirmwareVersion()) {
    // Do not hard-freeze forever; keep the board alive and visible.
    while (1) {
      blink(1);
      delay(1000);
    }
  }

  nfc.SAMConfig();
}

// ---- loop ----
void loop() {
  char payload[COMMAND_BUFFER_SIZE + 1];

  if (readTag(payload, sizeof(payload))) {
    if (payload[0] != '\0') {
      strncpy(currentCommand, payload, sizeof(currentCommand) - 1);
      currentCommand[sizeof(currentCommand) - 1] = '\0';
    } else {
      currentCommand[0] = '\0';
    }
  }

  if (digitalRead(BUTTON) == LOW) {
    blink(2);

    if (currentCommand[0] != '\0') {
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('D');
      delay(100);
      Keyboard.releaseAll();
      delay(300);

      slowPrint(currentCommand, 35);

      delay(500);          // pause before Enter
      Keyboard.write(KEY_RETURN);
    }

    while (digitalRead(BUTTON) == LOW) {
      delay(20);
    }
  }

  delay(20); // reduces busy polling and helps stability
}