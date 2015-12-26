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

#include <Arduino.h>
#include "wordclock.h"
#include "castellano.h"

void loadLanguageCastellano(byte hour, byte minute, unsigned int * matrix) {

  /*

  00 => en punto
  01 => pasada/s
  02 => pasada/s

  03 => casi y cinco
  04 => casi y cinco
  05 => y cinco
  06 => y cinco pasadas
  07 => y cinco pasadas

  08 => casi y diez
  09 => casi y diez
  10 => y diez
  11 => y diez pasadas
  12 => y diez pasadas

  13 => casi y cuarto
  14 => casi y cuarto
  15 => y cuarto
  16 => y cuarto pasadas
  17 => y cuarto pasadas

  18 => casi y veinte
  19 => casi y veinte
  20 => y veinte
  21 => y veinte pasadas
  22 => y veinte pasadas

  23 => casi y veinticinco
  24 => casi y veinticinco
  25 => y veinticinco
  26 => y veinticinco pasadas
  27 => y veinticinco pasadas

  28 => casi y media
  29 => casi y media
  30 => y media
  31 => y media pasadas
  32 => y media pasadas

  33 => casi menos veinticinco
  34 => casi menos veinticinco
  35 => menos veinticinco
  36 => menos veinticinco pasadas
  37 => menos veinticinco pasadas

  38 => casi menos veinte
  39 => casi menos veinte
  40 => menos veinte
  41 => menos veinte pasadas
  42 => menos veinte pasadas

  43 => casi menos cuarto
  44 => casi menos cuarto
  45 => menos cuarto
  46 => menos cuarto pasadas
  47 => menos cuarto pasadas

  48 => casi menos diez
  49 => casi menos diez
  50 => menos diez
  51 => menos diez pasadas
  52 => menos diez pasadas

  53 => casi menos cinco
  54 => casi menos cinco
  55 => menos cinco
  56 => menos cinco pasadas
  57 => menos cinco pasadas

  58 => casi
  59 => casi

  */

  // indica si la hora se referencia a la actual o a la posterior
  bool hour_is_current = (minute < 33);

  // hora en formato 12 y referida
  byte hour_12 = (hour > 12) ? hour - 12 : hour;
  if (!hour_is_current) hour_12++;
  if (hour_12 == 13) hour_12 = 1;

  // indica si la hora es plural o singular
  bool hour_is_singular = (hour_12 == 1);

  // ARTICLE
  if (hour_is_singular) {
    loadCode((clockword) ESP_ES, matrix);
    loadCode((clockword) ESP_LA, matrix);
  } else {
    loadCode((clockword) ESP_SON, matrix);
    loadCode((clockword) ESP_LAS, matrix);
  }

  // BLOCKS
  byte reference = ((minute + 2) / 5) % 12;
  byte index = (minute + 2) % 5;
  if (reference == 0) {
    // NOP
  } else if (reference < 7) {
    loadCode((clockword) ESP_Y, matrix);
  } else {
    loadCode((clockword) ESP_MENOS, matrix);
  }
  if (reference ==  1) loadCode((clockword) ESP_CINCO_B, matrix);
  if (reference ==  2) loadCode((clockword) ESP_DIEZ_B, matrix);
  if (reference ==  3) loadCode((clockword) ESP_CUARTO, matrix);
  if (reference ==  4) loadCode((clockword) ESP_VEINTE, matrix);
  if (reference ==  5) loadCode((clockword) ESP_VEINTICINCO, matrix);
  if (reference ==  6) loadCode((clockword) ESP_MEDIA, matrix);
  if (reference ==  7) loadCode((clockword) ESP_VEINTICINCO, matrix);
  if (reference ==  8) loadCode((clockword) ESP_VEINTE, matrix);
  if (reference ==  9) loadCode((clockword) ESP_CUARTO, matrix);
  if (reference == 10) loadCode((clockword) ESP_DIEZ_B, matrix);
  if (reference == 11) loadCode((clockword) ESP_CINCO_B, matrix);

  // MODIFIERS
  if (index < 2) loadCode((clockword) ESP_CASI, matrix);
  if (index > 2) {
    if (hour_is_singular) {
      loadCode((clockword) ESP_PASADA, matrix);
    } else {
      loadCode((clockword) ESP_PASADAS, matrix);
    }
  }
  if (minute == 0) loadCode((clockword) ESP_EN_PUNTO, matrix);

  // HORAS
  switch (hour_12) {
    case  1: loadCode((clockword) ESP_UNA, matrix); break;
    case  2: loadCode((clockword) ESP_DOS, matrix); break;
    case  3: loadCode((clockword) ESP_TRES, matrix); break;
    case  4: loadCode((clockword) ESP_CUATRO, matrix); break;
    case  5: loadCode((clockword) ESP_CINCO, matrix); break;
    case  6: loadCode((clockword) ESP_SEIS, matrix); break;
    case  7: loadCode((clockword) ESP_SIETE, matrix); break;
    case  8: loadCode((clockword) ESP_OCHO, matrix); break;
    case  9: loadCode((clockword) ESP_NUEVE, matrix); break;
    case 10: loadCode((clockword) ESP_DIEZ, matrix); break;
    case 11: loadCode((clockword) ESP_ONCE, matrix); break;
    default: loadCode((clockword) ESP_DOCE, matrix); break;
  }

  // FRANJA HORARIA
  loadCode((clockword) ESP_DE_F, matrix);
  loadCode((clockword) ESP_LA_F, matrix);
  if (hour < 6) {
    loadCode((clockword) ESP_NOCHE, matrix);
  } else if (hour < 13) {
    loadCode((clockword) ESP_MANANA, matrix);
  } else if (hour < 21) {
    loadCode((clockword) ESP_TARDE, matrix);
  } else {
    loadCode((clockword) ESP_NOCHE, matrix);
  }

}
