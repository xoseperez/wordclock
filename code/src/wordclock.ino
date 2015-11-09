/*

  Word Clock
  Copyright (C) 2015 by Xose Pérez <xose dot perez at gmail dot com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Time.h>
#include <DS1307RTC.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "debounceEvent.h"
#include "wordclock.h"
#include "catalan.h"
#include "castellano.h"

// ===========================================
// Configuration
// ===========================================

#define DEBUG
#define SERIAL_BAUD 9600
#define DEBOUNCE_DELAY 100
#define DEFAULT_DATETIME 1434121829
#define UPDATE_CLOCK_INTERVAL 60000
#define TOTAL_PIXELS 256
#define TOTAL_LANGUAGES 2
#define TOTAL_COLORS 5

#define DEFAULT_BRIGHTNESS 32
#define DEFAULT_COLOR 3

// mode
#define MODE_NORMAL 0
#define MODE_CHANGE 1

// languages
#define LANGUAGE_CATALAN 0
#define LANGUAGE_SPANISH 1

// pin definitions
#define PIN_BUTTON_MODE 5
#define PIN_BUTTON_BRIGHTNESS 6
#define PIN_BUTTON_COLOR 7
#define PIN_BUTTON_LANGUAGE 8
#define PIN_LEDSTRIP 4

// ===========================================
// Globals
// ===========================================

byte mode = MODE_NORMAL;
byte language = LANGUAGE_CATALAN;
byte brightness = DEFAULT_BRIGHTNESS;
byte color = DEFAULT_COLOR;

// Pixel strip
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(TOTAL_PIXELS, PIN_LEDSTRIP, NEO_RGB + NEO_KHZ800);

// Buttons
void buttonCallback(uint8_t pin, uint8_t event);
DebounceEvent buttonHour = DebounceEvent(PIN_BUTTON_BRIGHTNESS, buttonCallback);
DebounceEvent buttonMinute = DebounceEvent(PIN_BUTTON_COLOR, buttonCallback);
DebounceEvent buttonMode = DebounceEvent(PIN_BUTTON_MODE, buttonCallback);
DebounceEvent buttonFunction = DebounceEvent(PIN_BUTTON_LANGUAGE, buttonCallback);

// Colors
unsigned long colors[TOTAL_COLORS] = {
  pixels.Color(255,255,255),
  pixels.Color(255,10,10),
  pixels.Color(10,255,10),
  pixels.Color(10,10,255),
  pixels.Color(248,222,0)
};

// ===========================================
// Interrupt routines
// ===========================================

// ===========================================
// Methods
// ===========================================

void reset() {
  setTime(DEFAULT_DATETIME);
  RTC.set(DEFAULT_DATETIME);
}

void printDigits(int digits, bool semicolon = false){
  if (semicolon) Serial.print(":");
  if(digits < 10) Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  Serial.print("Time: ");
  printDigits(hour(), false);
  printDigits(minute(), true);
  printDigits(second(), true);
  Serial.println();
}

void loadCode(clockword code, unsigned long * matrix) {
  matrix[code.row] = matrix[code.row] | code.positions;
}

void update(bool force = false) {

  // RTC sync not working
  if (timeStatus() != timeSet) {
    #ifdef DEBUG
      Serial.println("Wrong timeStatus, configuring DS1307");
    #endif
    //reset();
    delay(1000);
    return;
  }

  // Check previous values for hour and minute and
  // update only if they have changed
  static int previous_hour = -1;
  static int previous_minute = -1;
  int current_hour = hour();
  int current_minute = minute();
  if ((!force) && (current_hour == previous_hour) && (current_minute == previous_minute)) return;
  previous_hour = current_hour;
  previous_minute = current_minute;

  digitalClockDisplay();

  // The matrix array holds the pixels values
  unsigned long matrix[16] = {0};

  // Load strings
  if (language == LANGUAGE_CATALAN) {
    loadLanguageCatalan(current_hour, current_minute, matrix);
  } else {
    loadLanguageCastellano(current_hour, current_minute, matrix);
  }

  pixels.clear();
  pixels.setBrightness(brightness);
  for (unsigned int row=0; row < 16; row++) {
    unsigned long value = 1;
    for (unsigned int col=0; col < 16; col++) {
      unsigned int pixel = 16 * row + ((row % 2 == 0) ? col : 15 - col);
      pixels.setPixelColor(pixel, (matrix[row] & value) == 0 ? 0 : colors[color]);
      value <<= 1;
    }
  }
  pixels.show();

}

// Shifts time forward
// the specified number of hours and minutes
void shiftTime(int hours, int minutes) {
  long shift = (hours * 60 + minutes) * 60;
  RTC.set(RTC.get() + shift);
  adjustTime(shift);
}

// Sets seconds to 0 for current hour:minute
void resetSeconds() {
  long shift = -second();
  RTC.set(RTC.get() + shift);
  adjustTime(shift);
}

// There are 4 buttons
// MODE button: sets the clock in CHANGE MODE while pressed
// BRIGHTNESS button: increases brightness (sums 1 to hour when in CHANGE MODE)
// COLOR button: changes color (sums 1 to minute when in CHANGE MODE)
// LANGUAGE button: changes LANGUAGE

void buttonCallback(uint8_t pin, uint8_t event) {

  if (event == EVENT_PRESSED) {

    switch (pin) {

      case PIN_BUTTON_MODE:
        mode = MODE_CHANGE;
        Serial.println("CHANGE MODE");
        break;

      case PIN_BUTTON_BRIGHTNESS:
        if (mode == MODE_CHANGE) {
          shiftTime(1, 0);
        } else {
          if (brightness == 0) {
            brightness = 16;
          } else {
            brightness *= 2;
          }
          if (brightness == 256) brightness = 0;
        }
        break;

      case PIN_BUTTON_COLOR:
        if (mode == MODE_CHANGE) {
          shiftTime(0, minute() == 59 ? -59 : 1);
        } else {
          color = (color + 1) % TOTAL_COLORS;
        }
        break;

      case PIN_BUTTON_LANGUAGE:
        if (mode == MODE_NORMAL) {
          language = (language + 1) % TOTAL_LANGUAGES;
        }
        break;

    }

    update(true);

  }

  if (event == EVENT_RELEASED) {
    if (pin == PIN_BUTTON_MODE) {
      mode = MODE_NORMAL;
      resetSeconds();
      update(true);
    }
  }


}

void setup() {

  Serial.begin(SERIAL_BAUD);

  // Initialise random number generation
  randomSeed(analogRead(0));

  // Config RTC provider
  setSyncProvider(RTC.get);
  if (timeStatus() != timeSet) reset();

  // Start display and initialize all to OFF
  pixels.begin();
  pixels.show();

}

void loop() {

  // Debounce buttons
  buttonHour.loop();
  buttonMinute.loop();
  buttonMode.loop();
  buttonFunction.loop();

  // Update display
  update();

}
