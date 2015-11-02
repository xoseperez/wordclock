/*

  Debounce buttons and trigger events
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

#ifndef _DEBOUNCE_EVENT_h
#define _DEBOUNCE_EVENT_h

#define DEBOUNCE_DELAY 100
#define EVENT_CHANGED 0
#define EVENT_PRESSED 1
#define EVENT_RELEASED 2

typedef void(*callback_t)(uint8_t pin, uint8_t event);

class DebounceEvent {

    private:

        uint8_t _pin;
        uint8_t _status;
        uint8_t _defaultStatus;
        unsigned long _delay; 
        callback_t _callback;
    
    public:

        DebounceEvent(uint8_t pin, callback_t callback = false, uint8_t defaultStatus = HIGH, unsigned long delay = DEBOUNCE_DELAY);
        bool loop();

};

#endif
