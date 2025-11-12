#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BleKeyboard.h>
#include <Encoder.h>  // Rotary encoder library

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 17
#define OLED_SCL 16
#define OLED_RESET -1

#define BUTTON1 18
#define BUTTON2 19
#define BUTTON3 21

#define ENCODER_CLK 26
#define ENCODER_DT 25
#define ENCODER_SW 27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BleKeyboard bleKeyboard("MacropadPrototype2.01.1", "NexQ", 100);
Encoder encoder(ENCODER_DT, ENCODER_CLK);

long oldPosition  = 0;
unsigned long lastDebounce = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed!");
    for (;;);
  }

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  pinMode(ENCODER_SW, INPUT_PULLUP);

  bleKeyboard.begin();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Waiting for BT...");
  display.display();
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    display.clearDisplay();
    display.setCursor(5, 25);
    display.println("Bluetooth not connected");
    display.display();
    delay(500);
    return;
  }

  // Handle normal buttons
  if (digitalRead(BUTTON1) == LOW) {
  bleKeyboard.press(KEY_MEDIA_VOLUME_DOWN); 
  bleKeyboard.release(KEY_MEDIA_VOLUME_DOWN);
  showOLED("Volume Down --------||||||||||||||||||");
  delay(300);

  }
  else if (digitalRead(BUTTON2) == LOW){
   bleKeyboard.press(KEY_MEDIA_VOLUME_UP); 
  bleKeyboard.release(KEY_MEDIA_VOLUME_UP);
  showOLED("Volume uppp ||||||||||||||||||||-----");
  delay(300);
  }
  else if (digitalRead(BUTTON3) == LOW) sendKey('w', "W Pressed");

 if (!bleKeyboard.isConnected()) return;

  // --- Rotary encoder rotation ---
long oldPosition = 0;
long currentPosition = encoder.read() / 2; // divide by 2 for one detent per step
long positionChange = currentPosition - oldPosition;

if(positionChange > 0) {
    for(long i = 0; i < positionChange; i++) {
        bleKeyboard.press(KEY_MEDIA_VOLUME_UP);
        bleKeyboard.release(KEY_MEDIA_VOLUME_UP);
    }
} else if(positionChange < 0) {
    for(long i = 0; i < -positionChange; i++) {
        bleKeyboard.press(KEY_MEDIA_VOLUME_DOWN);
        bleKeyboard.release(KEY_MEDIA_VOLUME_DOWN);
    }
}
oldPosition = currentPosition;
Serial.print("Pos: "); Serial.print(currentPosition);
Serial.print(" Change: "); Serial.println(positionChange);


// Encoder button (Play/Pause)
if (digitalRead(ENCODER_SW) == LOW && millis() - lastDebounce > 500) {
  bleKeyboard.press(KEY_MEDIA_PLAY_PAUSE);
  bleKeyboard.release(KEY_MEDIA_PLAY_PAUSE);
  showOLED("Play/Pause");
  lastDebounce = millis();
}

  // Handle encoder button (Play/Pause)
  if (digitalRead(ENCODER_SW) == LOW && millis() - lastDebounce > 500) {
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    showOLED("Play/Pause");
    Serial.println("Play/Pause toggled");
    lastDebounce = millis();
  }
}

void sendKey(char key, const char* msg) {
  bleKeyboard.write(key);
  showOLED(msg);
  Serial.print("Sent key: "); Serial.println(key);
  delay(300);
}

void showOLED(const char* msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println(msg);
  display.display();
}
