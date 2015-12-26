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

// game of life configuration
#define UPDATE_GAMEOFLIFE 200
#define GAMEOFLIFE_SEEDS 128
#define GAMEOFLIFE_AUTORESET 50
#define GAMEOFLIFE_BRIGHTNESS 16

// matrix configuration
#define UPDATE_MATRIX 25
#define MATRIX_BIRTH_RATIO 75
#define MATRIX_SPEED_MIN 6
#define MATRIX_SPEED_MAX 1
#define MATRIX_LENGTH_MIN 5
#define MATRIX_LENGTH_MAX 15
#define MATRIX_LIFE_MIN 10
#define MATRIX_LIFE_MAX 30
#define MATRIX_MAX_RAYS 40

// modes
#define TOTAL_MODES 3
#define MODE_CLOCK 0
#define MODE_GAMEOFLIFE 1
#define MODE_MATRIX 2
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
   unsigned int pixel = TOTAL_PIXELS - ( MATRIX_WIDTH * y + ((y % 2 == 1) ? x : MATRIX_HEIGHT - x - 1)) - 1;
   return pixel;
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
 * Load current time into LED matrix
 */
void updateClock() {

   matrix.clear();
   matrix.setBrightness(brightness);
   for (byte row=0; row < 16; row++) {
      unsigned int value = 1;
      for (byte col=0; col < 16; col++) {
         if ((time_pattern[row] & value) > 0) {
            unsigned int pixel = 16 * row + ((row % 2 == 0) ? col : 15 - col);
            matrix.setPixelColor(pixel, colors[color]);
         }
         value <<= 1;
      }
   }
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

   byte i = 0;

   if (!force && (next_update > millis())) return;

   if (current_num_rays < MATRIX_MAX_RAYS) {
      bool create = random(0, 100) < MATRIX_BIRTH_RATIO;
      if (create) {
         i=0;
         while (ray[i].life > 0) i++;
         ray[i].x = random(0, MATRIX_WIDTH);
         ray[i].y = random(0, MATRIX_HEIGHT);
         ray[i].speed = random(MATRIX_SPEED_MAX, MATRIX_SPEED_MIN);
         ray[i].length = random(MATRIX_LENGTH_MIN, MATRIX_LENGTH_MAX);
         ray[i].life = random(MATRIX_LIFE_MIN, MATRIX_LIFE_MAX);
         current_num_rays++;
      }
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
            int col = ray[i].y - p;
            if (0 <= col && col < MATRIX_HEIGHT) {
               matrix.setPixelColor(pixelIndex(ray[i].x, MATRIX_HEIGHT - col - 1), getMatrixColor(p, ray[i].length));
               active = true;
            }
         }

         // still active?
         if (!active) ray[i].life = 0;
         if (ray[i].life == 0) current_num_rays--;

      }
   }


   matrix.setBrightness(DEFAULT_BRIGHTNESS);
   matrix.show();

   count++;
   next_update += UPDATE_MATRIX;

}

// === GAME OF LIFE ============================================================

void initGameOfLife() {

   byte x,y;
   byte index;
   byte numCells = 0;

   matrix.clear();
   while (numCells < GAMEOFLIFE_SEEDS) {
      x = random(0, MATRIX_WIDTH);
      y = random(0, MATRIX_HEIGHT);
      index = pixelIndex(x, y);
      if (matrix.getPixelColor(index) == 0) {
         matrix.setPixelColor(index, COLOR_GREEN);
         numCells++;
      }
   }
   matrix.setBrightness(GAMEOFLIFE_BRIGHTNESS);
   matrix.show();

}

bool isAlive(unsigned int * matrix, int x, int y) {

   if (x < 0) x += MATRIX_WIDTH;
   if (x >= MATRIX_WIDTH) x -= MATRIX_WIDTH;
   if (y < 0) y += MATRIX_HEIGHT;
   if (y >= MATRIX_HEIGHT) y -= MATRIX_HEIGHT;

   unsigned int row = 1 << (MATRIX_WIDTH-x-1);
   bool alive = ((matrix[y] & row) != 0);

   return alive;

}

byte countNeighbours(unsigned int * matrix, int x, int y) {
      byte neighbours = 0;
      if (isAlive(matrix, x-1, y-1)) neighbours++;
      if (isAlive(matrix, x-1, y)) neighbours++;
      if (isAlive(matrix, x-1, y+1)) neighbours++;
      if (isAlive(matrix, x, y-1)) neighbours++;
      if (isAlive(matrix, x, y+1)) neighbours++;
      if (isAlive(matrix, x+1, y-1)) neighbours++;
      if (isAlive(matrix, x+1, y)) neighbours++;
      if (isAlive(matrix, x+1, y+1)) neighbours++;
      return neighbours;
}

void updateGameOfLife(bool force = false) {

   static byte numCells = 0;
   static byte prevCells = 0;
   static byte autoResetCount = 0;
   static unsigned long next_update = millis();

   byte x, y;
   unsigned int index;
   unsigned int current[MATRIX_HEIGHT];

   if (!force && (next_update > millis())) return;

   if (numCells == 0) {

      initGameOfLife();
      numCells = GAMEOFLIFE_SEEDS;

   } else {

      // Create an image of current situation
      for (y=0; y<MATRIX_HEIGHT; y++) {
         unsigned int row = 0;
         for (x=0; x<MATRIX_WIDTH; x++) {

            index = pixelIndex(x, y);
            row <<= 1;
            if (matrix.getPixelColor(index) != 0) row += 1;

         }
         current[y] = row;

      }

      matrix.clear();
      numCells = 0;
      for (x=0; x<MATRIX_WIDTH; x++) {
         for (y=0; y<MATRIX_HEIGHT; y++) {

            index = pixelIndex(x, y);
            byte neighbours = countNeighbours(current, x, y);
            bool live = isAlive(current, x, y);

            // a living cell
            if (live) {

               // keeps on living if 2-3 neighbours
               if (neighbours == 2 || neighbours == 3) {
                  matrix.setPixelColor(index, COLOR_RED);
                  numCells++;

               // else dies due to under/over-population
               } else {
                  // NOP
               }

            // a dead cell
            } else {

               // comes to life if 3 living neighbours
               if (neighbours == 3) {
                  matrix.setPixelColor(index, COLOR_GREEN);
                  numCells++;

               // else nothing
               } else {
                  // NOP
               }

            }
         }
      }

      matrix.setBrightness(GAMEOFLIFE_BRIGHTNESS);
      matrix.show();

   }

   if (numCells == prevCells) autoResetCount++;
   if (autoResetCount == GAMEOFLIFE_AUTORESET) {
      numCells = 0;
      autoResetCount = 0;
   }
   prevCells = numCells;

   next_update += UPDATE_GAMEOFLIFE;

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

      case MODE_GAMEOFLIFE:
         updateGameOfLife(force);
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
