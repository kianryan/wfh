
/* WFH Keyboard
   Displays keys in a number of modes.
   Slack/Teams/Zoom/Spotify
   Can be adapted for other uses.

   Displays panel of icons alongside buttons.
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// #define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN 10

int buttonState = HIGH;

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  // Setup the first pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pin_ISR, CHANGE);

  Keyboard.begin();
}

void loop()
{
  testdrawchar();
  display.display();
  delay(25);
}

void testdrawchar(void)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font

  // Draw header data.
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("MODE:"));
  display.setCursor(CHAR_WIDTH * 6, 0);
  display.setTextSize(2);
  display.print(F("SPOTIFY"));

  if (buttonState == LOW)
  {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  }
  else
  {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw 'normal' text
  }

  display.setCursor(34, CHAR_HEIGHT * 2);
  display.print(F("A A A"));
  display.setCursor(34, CHAR_HEIGHT * 4);
  display.print(F("A A A"));
  display.setCursor(34, CHAR_HEIGHT * 6);
  display.print(F("A A A"));
}

class KeyboardMap
{
public:
  char keys[4];
  char key;
  bool alt;
  bool win;
  bool ctrl;

  KeyboardMap(char k, bool a, bool w, bool c) {
    key = k;
    alt = a;
    win = w;
    ctrl = c;
  }
};

// https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/

KeyboardMap m[1] {{ 'A', false, false, false }};

/* Let's expand on this.
   We only want to dispatch the keyboard event on the interrupt /if/ the interrupt is low. */

void pin_ISR()
{
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW)
  {
    /* a response is likely to be a modifer */
    KeyboardMap k = m[0];

    if (k.alt) Keyboard.press(KEY_LEFT_ALT);
    if (k.ctrl) Keyboard.press(KEY_LEFT_CTRL);
    if (k.win) Keyboard.press(KEY_LEFT_GUI);

    Keyboard.press(k.key);

    Keyboard.releaseAll();
  }
}