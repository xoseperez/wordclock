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
#include "catalan.h"

void loadLanguageCatalan(byte hour, byte minute, unsigned long * matrix) {

  /*

  00 => en punt
  01 => tocada/es
  02 => tocada/es
  03 => ben tocada/es
  04 => ben tocada/es
  
  05 => vora mig quart
  
  06 => vora mig quart
  07 => mig quart
  08 => mig quart tocat
  09 => vora un quart menys 5
  10 => un quart menys 5
  11 => un quart menys 5 tocat
  12 => un quart menys 5 tocat
  13 => un quart menys 5 ben tocat
  14 => vora un quart
  15 => un quart
  16 => un quart tocat
  17 => un quart tocat
  18 => un quart ben tocat
  19 => vora un quart i 5
  20 => un quart i 5
  
  21 => vora un quart i mig
  22 => un quart i mig
  23 => un quart i mig tocat
  24 => vora dos quarts menys 5
  25 => dos quarts menys 5
  26 => dos quarts menys 5 tocats
  27 => dos quarts menys 5 tocats
  28 => dos quarts menys 5 ben tocats
  29 => vora dos quarts
  30 => dos quarts
  31 => dos quarts tocats
  32 => dos quarts tocats
  33 => dos quarts ben tocats
  34 => vora dos quarts i 5
  35 => dos quarts i 5

  36 => vora dos quarts i mig
  37 => dos quarts i mig
  38 => dos quarts i mig tocats
  39 => vora tres quarts menys 5
  40 => tres quarts menys 5
  41 => tres quarts menys 5 tocats
  42 => tres quarts menys 5 tocats
  43 => tres quarts menys 5 ben tocats
  44 => vora tres quarts
  45 => tres quarts
  46 => tres quarts tocats
  47 => tres quarts tocats
  48 => tres quarts ben tocats
  49 => vora tres quarts i 5
  50 => tres quarts i 5

  51 => vora tres quart i mig
  52 => tres quarts i mig
  53 => tres quarts i mig tocats
  54 => vora menys 5
  55 => menys 5
  56 => menys 5 tocats
  57 => menys 5 tocats
  58 => menys 5 ben tocats
  59 => vora

  */

  // indica si l'hora es referencia a l'actual o la posterior
  bool hour_is_current = (minute < 5);

  // hora en format 12 i referida
  byte hour_12 = (hour > 12) ? hour - 12 : hour;
  if (!hour_is_current) hour_12++;
  if (hour_12 == 13) hour_12 = 1;

  // indica si l'hora va precedida de l'article (la, les) o del determinant (de, d')
  bool hour_with_article = (minute < 5) || (minute > 58);

  // indica si l'hora és plural o singular
  bool hour_is_singular = ((5 <= minute) && (minute <= 23)) || ((minute < 5) && (hour_12 == 1));
 
  // VERB
  if (hour_is_singular) {
    loadCode((clockword) CAT_ES, matrix);
  } else {
    loadCode((clockword) CAT_SON, matrix);
  }

  // EN PUNT
  if (minute == 0) {
    loadCode((clockword) CAT_EN_PUNT, matrix);
  }

  // PRIMERS MINUTS
  if ((1 <= minute) && (minute <= 4)) {
    if (hour_is_singular) {
      loadCode((clockword) CAT_TOCADA, matrix); 
    } else {
      loadCode((clockword) CAT_TOCADES, matrix); 
    }
    if (minute > 2) {
      loadCode((clockword) CAT_BEN, matrix);  
    }
  }
  if (minute == 5) {
    loadCode((clockword) CAT_VORA, matrix);  
    loadCode((clockword) CAT_MIG, matrix);  
    loadCode((clockword) CAT_QUART, matrix);  
  }

  // FRANJA DEL MIG
  //if ((6 <= minute) && (minute <= 53)) {
  if (6 <= minute) {
    
    // convenient words
    clockword tocat = (hour_is_singular) ? (clockword) CAT_TOCAT : (clockword) CAT_TOCATS;

    byte quarts = (minute - 6) / 15;
    byte index = (minute - 6) % 15;
    if (index >= 3) quarts++;

    if (quarts == 1) loadCode((clockword) CAT_UN_Q, matrix);  
    if (quarts == 2) loadCode((clockword) CAT_DOS_Q, matrix);  
    if (quarts == 3) loadCode((clockword) CAT_TRES_Q, matrix);  
    if (index < 3) {
      if (quarts == 0) {
        loadCode((clockword) CAT_MIG, matrix);  
      } else {
        loadCode((clockword) CAT_I_MIG, matrix);  
      }
    }
    if (quarts < 1) {
        loadCode((clockword) CAT_QUART, matrix);  
    } else if (quarts < 4) {
        loadCode((clockword) CAT_QUARTS, matrix);  
    }

    switch (index) {

      case 0:
        loadCode((clockword) CAT_VORA, matrix);  
        break;

      case 2:
        loadCode(tocat, matrix);  
        break;

      case 3:
        loadCode((clockword) CAT_VORA, matrix);  
        loadCode((clockword) CAT_MENYS, matrix);  
        loadCode((clockword) CAT_CINC_Q, matrix);  
        break;

      case 4:
        loadCode((clockword) CAT_MENYS, matrix);  
        loadCode((clockword) CAT_CINC_Q, matrix);  
        break;

      case 5:
      case 6:
        loadCode((clockword) CAT_MENYS, matrix);  
        loadCode((clockword) CAT_CINC_Q, matrix);  
        loadCode(tocat, matrix);  
        break;

      case 7:
        loadCode((clockword) CAT_MENYS, matrix);  
        loadCode((clockword) CAT_CINC_Q, matrix);  
        loadCode((clockword) CAT_BEN_Q, matrix);  
        loadCode(tocat, matrix);  
        break;

      case 8:
        loadCode((clockword) CAT_VORA, matrix);  
        break;

      case 10:
      case 11:
        loadCode(tocat, matrix);  
        break;

      case 12:
        loadCode((clockword) CAT_BEN_Q, matrix);  
        loadCode(tocat, matrix);  
        break;

      case 13:
        loadCode((clockword) CAT_VORA, matrix);  
        loadCode((clockword) CAT_I, matrix);  
        loadCode((clockword) CAT_CINC_Q, matrix);  
        break;

      case 14:
        loadCode((clockword) CAT_I, matrix);  
        loadCode((clockword) CAT_CINC_Q, matrix);  
        break;

    }

  }

  // HORES
  switch (hour_12) {
    case  1: loadCode((clockword) CAT_UNA, matrix); break;
    case  2: loadCode((clockword) CAT_DUES, matrix); break;
    case  3: loadCode((clockword) CAT_TRES, matrix); break;
    case  4: loadCode((clockword) CAT_QUATRE, matrix); break;
    case  5: loadCode((clockword) CAT_CINC, matrix); break;
    case  6: loadCode((clockword) CAT_SIS, matrix); break;
    case  7: loadCode((clockword) CAT_SET, matrix); break;
    case  8: loadCode((clockword) CAT_VUIT, matrix); break;
    case  9: loadCode((clockword) CAT_NOU, matrix); break;
    case 10: loadCode((clockword) CAT_DEU, matrix); break;
    case 11: loadCode((clockword) CAT_ONZE, matrix); break;
    case 12: loadCode((clockword) CAT_DOTZE, matrix); break;
  }


  // ULTIMS MINUTS
  /*
  if (minute == 54) {
    loadCode((clockword) CAT_TRES_Q, matrix);  
    loadCode((clockword) CAT_QUARTS, matrix);  
    loadCode((clockword) CAT_I_MIG, matrix);  
    loadCode((clockword) CAT_TOCATS, matrix);  
  }
  if (minute == 55) {
    loadCode((clockword) CAT_TRES_Q, matrix);  
    loadCode((clockword) CAT_QUARTS, matrix);  
    loadCode((clockword) CAT_I_MIG, matrix);  
    loadCode((clockword) CAT_BEN_Q, matrix);  
    loadCode((clockword) CAT_TOCATS, matrix);  
  }
  if (minute >= 56) {
    loadCode((clockword) CAT_VORA, matrix);  
  }
  */

  // PARTICULES DE LES HORES
  if (hour_with_article) {
    if (hour_is_singular) {
      loadCode((clockword) CAT_LA, matrix);
    } else {
      loadCode((clockword) CAT_LES, matrix);
    }
  } else {
    if (hour_12 == 1) {
      loadCode((clockword) CAT_D_UNA, matrix);
    } else if (hour_12 == 11) {
      loadCode((clockword) CAT_D_ONZE, matrix);
    } else {
      loadCode((clockword) CAT_DE, matrix);
    }
  }

  // FRANJA HORARIA
  if (hour < 6) {
    loadCode((clockword) CAT_DE_F, matrix);
    loadCode((clockword) CAT_LA_F, matrix);
    loadCode((clockword) CAT_NIT, matrix);
  } else if (hour < 13) {
    loadCode((clockword) CAT_DEL, matrix);
    loadCode((clockword) CAT_MATI, matrix);
  } else if (hour < 21) {
    loadCode((clockword) CAT_DE_F, matrix);
    loadCode((clockword) CAT_LA_F, matrix);
    loadCode((clockword) CAT_TARDA, matrix);
  } else {
    loadCode((clockword) CAT_DE_F, matrix);
    loadCode((clockword) CAT_LA_F, matrix);
    loadCode((clockword) CAT_NIT, matrix);
  }

}