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

#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>
#include "debounceEvent.h"
#include "wordclock.h"
#include "catalan.h"
#include "castellano.h"

// =============================================================================
// Configuration
// =============================================================================

#define DEBUG
#define SERIAL_BAUD 9600
#define DEBOUNCE_DELAY 100

// matrix configuration
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define TOTAL_PIXELS MATRIX_WIDTH * MATRIX_HEIGHT
#define DEFAULT_BRIGHTNESS 32

// clock configuration
#define TOTAL_COLORS 5
#define DEFAULT_COLOR 3
#define TOTAL_LANGUAGES 2
#define LANGUAGE_CATALAN 0
#define LANGUAGE_SPANISH 1

// matrix configuration
#define UPDATE_MATRIX 25
#define MATRIX_BIRTH_RATIO 75
#define MATRIX_SPEED_MIN 4
#define MATRIX_SPEED_MAX 1
#define MATRIX_LENGTH_MIN 5
#define MATRIX_LENGTH_MAX 20
#define MATRIX_LIFE_MIN 10
#define MATRIX_LIFE_MAX 40
#define MATRIX_MAX_RAYS 80
#define STICKY_COUNT 1000
#define STICKY_PAUSE 5000

// modes
#define TOTAL_MODES 2
#define MODE_CLOCK 0
#define MODE_MATRIX 1
#define MODE_CHANGE 8
#define MODE_CHANGED 9

// colors
#define COLOR_WHITE 16777215
#define COLOR_RED 16714250
#define COLOR_GREEN 720650
#define COLOR_BLUE 658175
#define COLOR_YELLOW 16309760

// pin definitions
#define PIN_BUTTON_MODE 5
#define PIN_BUTTON_BRIGHTNESS 6
#define PIN_BUTTON_COLOR 7
#define PIN_BUTTON_LANGUAGE 8
#define PIN_LEDSTRIP 4

// =============================================================================
// Globals
// =============================================================================

byte mode = MODE_CLOCK;
byte language = LANGUAGE_CATALAN;
byte color = DEFAULT_COLOR;
byte brightness = DEFAULT_BRIGHTNESS;

// Pixel strip
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(TOTAL_PIXELS, PIN_LEDSTRIP, NEO_GRB + NEO_KHZ800);

// Buttons
void buttonCallback(uint8_t pin, uint8_t event);
DebounceEvent buttonHour = DebounceEvent(PIN_BUTTON_BRIGHTNESS, buttonCallback);
DebounceEvent buttonMinute = DebounceEvent(PIN_BUTTON_COLOR, buttonCallback);
DebounceEvent buttonMode = DebounceEvent(PIN_BUTTON_MODE, buttonCallback);
DebounceEvent buttonFunction = DebounceEvent(PIN_BUTTON_LANGUAGE, buttonCallback);

// RTC
RTC_DS1307 rtc;

// Clock colors
unsigned long colors[TOTAL_COLORS] = { COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW };

// The matrix array holds the time pattern matrix values
unsigned int time_pattern[16] = {0};


// =============================================================================
// Interrupt routines
// =============================================================================

// =============================================================================
// Methods
// =============================================================================

// Get pixel index in matrix from X,Y coords
unsigned int pixelIndex(int x, int y) {
   return MATRIX_WIDTH * y + ((y % 2 == 0) ? x : MATRIX_HEIGHT - x - 1);
}

// === TIME ====================================================================

/**
 * Resets DS1307 time to compile time
 */
void resetTime() {
   #ifdef DEBUG
      Serial.println(F("Reseting DS1307"));
   #endif
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

/**
 * Shifts time the specified number of hours, minutes and seconds
 * @param  int hours         Number of hours forward positive
 * @param  int minutes       Number of minutes forward positive
 * @param  int seconds       Number of seconds forward positive
 */
void shiftTime(int hours, int minutes, int seconds) {
   DateTime newTime = DateTime(rtc.now().unixtime() + seconds+60*(minutes+60*hours));
   rtc.adjust(newTime);
   mode = MODE_CHANGED;
}

/**
 * Sets seconds to 0 for current hour:minute
 */
void resetSeconds() {
   shiftTime(0, 0, -rtc.now().second());
}

/**
 * Prints current time part throu serial
 * @param  int  digits        Number to print
 * @param  bool semicolon     Whether to prepend a semicolon or not, defaults to false
 */
void printDigits(int digits, bool semicolon = false){
   if (semicolon) Serial.print(F(":"));
   if(digits < 10) Serial.print(F("0"));
   Serial.print(digits);
}

/**
 * Displays time throu serial
 * @param  DateTime now           DateTime object
 */
void digitalClockDisplay(DateTime now){
   Serial.print(F("Time: "));
   printDigits(now.hour(), false);
   printDigits(now.minute(), true);
   printDigits(now.second(), true);
   Serial.println();
}

// === CLOCK ===================================================================

/**
 * Loads a word code into the time matrix
 * @param  clockword code          a tupla defining the leds to be lit to form a word
 * @param  unsigned  int *         LED matrix array representation
 */
void loadCode(clockword code, unsigned int * matrix) {
   matrix[code.row] = matrix[code.row] | code.positions;
}

/**
 * Loads current time pattern in time_pattern matrix
 * @param  bool force         Update regardless the time since last update
 */
bool loadTimePattern(bool force = false) {

   static int previous_hour = -1;
   static int previous_minute = -1;

   // Check previous values for hour and minute and
   // update only if they have changed
   DateTime now = rtc.now();
   int current_hour = now.hour();
   int current_minute = now.minute();
   if ((!force) && (current_hour == previous_hour) && (current_minute == previous_minute)) return false;
   previous_hour = current_hour;
   previous_minute = current_minute;

   digitalClockDisplay(now);

   // Reset time pattern
   for (byte i=0; i<MATRIX_HEIGHT; i++) time_pattern[i] = 0;

   // Load strings
   if (language == LANGUAGE_CATALAN) {
      loadLanguageCatalan(current_hour, current_minute, time_pattern);
   } else {
      loadLanguageCastellano(current_hour, current_minute, time_pattern);
   }

   return true;

}

/**
 * Count the number of lit LEDs for current time pattern
 * @return byte Number of LEDs
 */
byte countLEDs() {

   byte char_total = 0;

   for (byte y=0; y<MATRIX_HEIGHT; y++) {
      unsigned int row = time_pattern[y];
      while (row>0) {
         if (row & 1) char_total++;
         row >>= 1;
      }
   }

   return char_total;

}

void loadTimeInMatrix(unsigned int * pattern, unsigned long color) {

   for (byte y=0; y < 16; y++) {
      unsigned int value = 1;
      for (byte x=0; x < 16; x++) {
         if ((pattern[y] & value) > 0) {
            matrix.setPixelColor(pixelIndex(x, y), color);
         }
         value <<= 1;
      }
   }

}

/**
 * Load current time into LED matrix
 */
void updateClock() {
   matrix.clear();
   matrix.setBrightness(brightness);
   loadTimeInMatrix(time_pattern, colors[color]);
   matrix.show();
}

// === MATRIX ==================================================================

/**
 * Calculates color of the falling ray depending on distance to the tip
 * @param  byte current       Position of current led in ray
 * @param  byte total         Ray length
 */
unsigned long getMatrixColor(byte current, byte total) {
   byte green = map(current, 0, total, 255, 0);
   byte red = current == 0 ? 255 : 0;
   return matrix.Color(red, green, 0);
}

/**
 * Updates matrix effect
 * @param  bool force         Update regardless the time since last update
 */
void updateMatrix(bool force = false) {

   static unsigned long count = 0;
   static unsigned long next_update = millis();
   static byte current_num_rays = 0;
   static ray_struct ray[MATRIX_MAX_RAYS];

   static bool sticky = false;
   static bool create = true;
   static byte char_total = 0;
   static byte char_so_far = 0;
   static unsigned int local_pattern[MATRIX_HEIGHT];

   byte i = 0;

   if (!force && (next_update > millis())) return;

   if (create && (current_num_rays < MATRIX_MAX_RAYS)) {
      bool do_create = random(0, 100) < MATRIX_BIRTH_RATIO;
      if (do_create) {
         i=0;
         while (ray[i].life > 0) i++;
         ray[i].x = random(0, MATRIX_WIDTH);
         ray[i].y = random(-5, 5);
         ray[i].speed = random(MATRIX_SPEED_MAX, MATRIX_SPEED_MIN);
         ray[i].length = random(MATRIX_LENGTH_MIN, MATRIX_LENGTH_MAX);
         ray[i].life = random(MATRIX_LIFE_MIN, MATRIX_LIFE_MAX);
         current_num_rays++;
      }
   }

   if ((!sticky) && (count > STICKY_COUNT)) {
      sticky = true;
      loadTimePattern();
      char_total = countLEDs();
      for (i=0; i<MATRIX_HEIGHT; i++) {
         local_pattern[i] = 0;
      }
      char_so_far = 0;
   }

   matrix.clear();

   for (i=0; i<MATRIX_MAX_RAYS; i++) {
      if (ray[i].life > 0) {

         // update ray position depending on speed
         if (count % ray[i].speed == 0) {
            ray[i].y = ray[i].y + 1;
            ray[i].life = ray[i].life - 1;
         }

         // get colors for each pixel
         byte start = 0;
         if (ray[i].life <= ray[i].length) {
            start = ray[i].length - ray[i].life ;
         }

         bool active = false;
         for (byte p=start; p<ray[i].length; p++) {
            int y = ray[i].y - p;
            if (0 <= y && y < MATRIX_HEIGHT) {
               matrix.setPixelColor(pixelIndex(ray[i].x, y), getMatrixColor(p, ray[i].length));
            }
            active |= (y < MATRIX_HEIGHT);
         }
         if (!active) ray[i].life = 0;

         // we are in sticky mode
         if (sticky) {

            byte y = ray[i].y;

            if (0 <= y && y < MATRIX_HEIGHT) {

               // check if we have hit a led in the time_pattern matrix
               unsigned int value = 1 << ray[i].x;
               if ((time_pattern[y] & value) == value) {

                     // check if we have already hit this led before
                     if ((local_pattern[y] & value) != value) {

                        // kill the ray
                        ray[i].life = 0;
                        char_so_far++;

                        // save it into local pattern
                        local_pattern[y] = local_pattern[y] + value;

                        // are we done?
                        if (char_so_far == char_total) {
                           create = false;
                        }

                     }

               }

            }

         }

         if (sticky or !create) {
            // draw hit leds
            loadTimeInMatrix(local_pattern, COLOR_YELLOW);
         }

         // free ray if dead
         if (ray[i].life == 0) current_num_rays--;

      }
   }


   matrix.setBrightness(DEFAULT_BRIGHTNESS);
   matrix.show();

   if ((current_num_rays == 0) and !create) {
      sticky = false;
      create = true;
      count = 0;
      delay(STICKY_PAUSE);
   } else {
      count++;
   }

   next_update += UPDATE_MATRIX;

}

// === GENERAL =================================================================

void eeprom_save() {
   EEPROM.update(0, mode);
   EEPROM.update(1, language);
   EEPROM.update(2, color);
   EEPROM.update(3, brightness);
}

void eeprom_retrieve() {
   mode = EEPROM.read(0);
   language = EEPROM.read(1);
   color = EEPROM.read(2);
   brightness = EEPROM.read(3);
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Update display depending on current mode
void update(bool force = false) {

   switch (mode) {

      case MODE_CLOCK:
      case MODE_CHANGE:
      case MODE_CHANGED:
         if (loadTimePattern(force)) {
            updateClock();
         }
         break;

      case MODE_MATRIX:
         updateMatrix(force);
         break;

   }

}

// There are 4 buttons
// MODE button: changes mode, when hold in MODE_CLOCK enters MODE_CHANGE
// BRIGHTNESS button: increases brightness (sums 1 to hour when in MODE_CHANGE)
// COLOR button: changes color (sums 1 to minute when in MODE_CHANGE)
// LANGUAGE button: changes LANGUAGE

void buttonCallback(uint8_t pin, uint8_t event) {

   if (event == EVENT_PRESSED) {

      switch (pin) {

         case PIN_BUTTON_MODE:
            if (mode == MODE_CLOCK) {
               mode = MODE_CHANGE;
               Serial.print(F("MODE: "));
               Serial.println(mode);
            }
            break;

         case PIN_BUTTON_BRIGHTNESS:
            if (mode == MODE_CHANGE || mode == MODE_CHANGED) {
               shiftTime(1, 0, 0);
            } else if (mode == MODE_CLOCK) {
               if (brightness == 0) {
                  brightness = 16;
               } else {
                  brightness *= 2;
               }
               if (brightness == 256) brightness = 0;
               eeprom_save();
            }
            break;

         case PIN_BUTTON_COLOR:
            if (mode == MODE_CHANGE || mode == MODE_CHANGED) {
               shiftTime(0, rtc.now().minute() == 59 ? -59 : 1, 0);
            } else if (mode == MODE_CLOCK) {
               color = (color + 1) % TOTAL_COLORS;
               eeprom_save();
            }
            break;

         case PIN_BUTTON_LANGUAGE:
            if (mode == MODE_CLOCK) {
               language = (language + 1) % TOTAL_LANGUAGES;
               eeprom_save();
            }
            break;

      }

      update(true);

   }

   if (event == EVENT_RELEASED) {
      if (pin == PIN_BUTTON_MODE) {
         if (mode == MODE_CHANGE) {
            mode = MODE_CLOCK;
         }
         if (mode == MODE_CHANGED) {
            resetSeconds();
            mode = MODE_CLOCK;
         } else {
            mode = (mode + 1) % TOTAL_MODES;
            eeprom_save();
         }
         Serial.print(F("MODE: "));
         Serial.println(mode);
         update(true);
      }
   }


}

void setup() {

   Serial.begin(SERIAL_BAUD);

   // Config RTC
   if (!rtc.begin()) {
      Serial.println(F("Couldn't find RTC"));
      while(1);
   }
   if (!rtc.isrunning()) {
      resetTime();
   }

   // Initialise random number generation
   randomSeed(rtc.now().unixtime());

   // Start display and initialize all to OFF
   matrix.begin();
   matrix.show();

   // get stored values from EEPROM
   eeprom_retrieve();

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
