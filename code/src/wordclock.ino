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
#include <RTClib.h>
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
#define UPDATE_GAMEOFLIFE 100
#define GAMEOFLIFE_SEEDS 128
#define GAMEOFLIFE_AUTORESET 50
#define GAMEOFLIFE_BRIGHTNESS 16

// modes
#define TOTAL_MODES 2
#define MODE_CLOCK 0
#define MODE_GAMEOFLIFE 1
#define MODE_CHANGE 8
#define MODE_CHANGED 9

// pin definitions
#define PIN_BUTTON_MODE 5
#define PIN_BUTTON_BRIGHTNESS 6
#define PIN_BUTTON_COLOR 7
#define PIN_BUTTON_LANGUAGE 8
#define PIN_LEDSTRIP 4

// ===========================================
// Globals
// ===========================================

byte mode = MODE_CLOCK;
byte language = LANGUAGE_CATALAN;
byte brightness = DEFAULT_BRIGHTNESS;
byte color = DEFAULT_COLOR;

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
unsigned long colors[TOTAL_COLORS] = {
   matrix.Color(255,255,255),
   matrix.Color(255,10,10),
   matrix.Color(10,255,10),
   matrix.Color(10,10,255),
   matrix.Color(248,222,0)
};

// Game Of Life
uint32_t newCellColor = matrix.Color(10,255,10);
uint32_t oldCellColor = matrix.Color(255,10,10);
byte numCells = 0;
byte prevCells = 0;
byte autoResetCount = 0;
unsigned long nextupdate = 0;

// =============================================================================
// Interrupt routines
// =============================================================================

// =============================================================================
// Methods
// =============================================================================


// === TIME ====================================================================

void resetTime() {
   #ifdef DEBUG
   Serial.println(F("Reseting DS1307"));
   #endif
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

// Shifts time the specified number of hours, minutes and seconds
void shiftTime(int hours, int minutes, int seconds) {
   DateTime newTime = DateTime(rtc.now().unixtime() + seconds+60*(minutes+60*hours));
   rtc.adjust(newTime);
   mode = MODE_CHANGED;
}

// Sets seconds to 0 for current hour:minute
void resetSeconds() {
   shiftTime(0, 0, -rtc.now().second());
}

void printDigits(int digits, bool semicolon = false){
   if (semicolon) Serial.print(F(":"));
   if(digits < 10) Serial.print(F("0"));
   Serial.print(digits);
}

void digitalClockDisplay(DateTime now){
   Serial.print(F("Time: "));
   printDigits(now.hour(), false);
   printDigits(now.minute(), true);
   printDigits(now.second(), true);
   Serial.println();
}

// === GAME OF LIFE ============================================================

// Get pixel index in matrix from X,Y coords
unsigned int pixelIndex(int x, int y) {
   unsigned int pixel = TOTAL_PIXELS - ( MATRIX_WIDTH * y + ((y % 2 == 1) ? x : MATRIX_HEIGHT - x - 1)) - 1;
   return pixel;
}

void initGameOfLife() {

   byte x,y;
   byte index;

   matrix.clear();
   numCells = 0;
   while (numCells < GAMEOFLIFE_SEEDS) {
      x = random(0, MATRIX_WIDTH);
      y = random(0, MATRIX_HEIGHT);
      index = pixelIndex(x, y);
      if (matrix.getPixelColor(index) == 0) {
         matrix.setPixelColor(index, newCellColor);
         numCells++;
      }
   }
   matrix.setBrightness(GAMEOFLIFE_BRIGHTNESS);
   matrix.show();

   nextupdate = millis() + UPDATE_GAMEOFLIFE;

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

void updateGameOfLife() {

   byte x, y;
   unsigned int index;
   unsigned int current[MATRIX_HEIGHT];

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
               matrix.setPixelColor(index, oldCellColor);
               numCells++;

            // else dies due to under/over-population
            } else {
               // NOP
            }

         // a dead cell
         } else {

            // comes to life if 3 living neighbours
            if (neighbours == 3) {
               matrix.setPixelColor(index, newCellColor);
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

   if (numCells == prevCells) autoResetCount++;
   if (autoResetCount == GAMEOFLIFE_AUTORESET) {
      numCells = 0;
      autoResetCount = 0;
   }
   prevCells = numCells;

   nextupdate += UPDATE_GAMEOFLIFE;

}


// === CLOCK ===================================================================

void loadCode(clockword code, unsigned long * matrix) {
   matrix[code.row] = matrix[code.row] | code.positions;
}

void updateClock(bool force = false) {

   // Check previous values for hour and minute and
   // update only if they have changed
   DateTime now = rtc.now();
   static int previous_hour = -1;
   static int previous_minute = -1;
   int current_hour = now.hour();
   int current_minute = now.minute();
   if ((!force) && (current_hour == previous_hour) && (current_minute == previous_minute)) return;
   previous_hour = current_hour;
   previous_minute = current_minute;

   digitalClockDisplay(now);

   // The matrix array holds the matrix values
   unsigned long pattern[16] = {0};

   // Load strings
   if (language == LANGUAGE_CATALAN) {
      loadLanguageCatalan(current_hour, current_minute, pattern);
   } else {
      loadLanguageCastellano(current_hour, current_minute, pattern);
   }

   matrix.clear();
   matrix.setBrightness(brightness);
   for (unsigned int row=0; row < 16; row++) {
      unsigned long value = 1;
      for (unsigned int col=0; col < 16; col++) {
         unsigned int pixel = 16 * row + ((row % 2 == 0) ? col : 15 - col);
         matrix.setPixelColor(pixel, (pattern[row] & value) == 0 ? 0 : colors[color]);
         value <<= 1;
      }
   }
   matrix.show();

}

// === GENERAL =================================================================

// Update display depending on current mode
void update(bool force = false) {

   switch (mode) {

      case MODE_CLOCK:
      case MODE_CHANGE:
         updateClock(force);
         break;

      case MODE_GAMEOFLIFE:
         if (numCells == 0) {
            initGameOfLife();
         } else {
            if (millis() > nextupdate) updateGameOfLife();
         }
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
            if (mode == MODE_CHANGE) {
               shiftTime(1, 0, 0);
            } else if (mode == MODE_CLOCK) {
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
               shiftTime(0, rtc.now().minute() == 59 ? -59 : 1, 0);
            } else if (mode == MODE_CLOCK) {
               color = (color + 1) % TOTAL_COLORS;
            }
            break;

         case PIN_BUTTON_LANGUAGE:
            if (mode == MODE_CLOCK) {
               language = (language + 1) % TOTAL_LANGUAGES;
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
         }
         nextupdate = millis();
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
