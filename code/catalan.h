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

#define CAT_ES          {0, 0xC000}
#define CAT_SON         {0, 0x7000}
#define CAT_VORA        {0, 0x00F0}

#define CAT_UN_Q        {1, 0xC000}
#define CAT_DOS_Q       {1, 0x3800}
#define CAT_TRES_Q      {1, 0x0780}
#define CAT_MIG         {1, 0x0007}

#define CAT_QUART       {2, 0xF800}
#define CAT_QUARTS      {2, 0xFC00}
#define CAT_I           {2, 0x0100}
#define CAT_I_MIG       {2, 0x0107}
#define CAT_MENYS       {2, 0x00F8}

#define CAT_CINC_Q      {3, 0xF000}
#define CAT_BEN_Q       {3, 0x0700}
#define CAT_TOCAT       {3, 0x003E}
#define CAT_TOCATS      {3, 0x003F}

#define CAT_LES         {4, 0xE000}
#define CAT_DE          {4, 0x1800}
#define CAT_LA          {4, 0x0600}
#define CAT_DUES        {4, 0x00F0}
#define CAT_SIS         {4, 0x001C}
#define CAT_SET         {4, 0x0007}

#define CAT_CINC        {5, 0xF000}
#define CAT_VUIT        {5, 0x00F0}
#define CAT_UNA         {5, 0x0007}
#define CAT_D_UNA       {5, 0x000F}

#define CAT_ONZE        {6, 0x7800}
#define CAT_D_ONZE      {6, 0xF800}
#define CAT_DOTZE       {6, 0x07C0}
#define CAT_NOU         {6, 0x0038}
#define CAT_DEU         {6, 0x0007}

#define CAT_DOS         {7, 0xE000}

#define CAT_QUATRE      {8, 0xFC00}
#define CAT_TRES        {8, 0x1E00}

#define CAT_BEN         {12, 0xE000}
#define CAT_TOCADES     {12, 0x07F0}
#define CAT_EN_PUNT     {12, 0x600F}

#define CAT_TOCADA      {13, 0xFC00}
#define CAT_DE_F        {13, 0x0006}
#define CAT_DEL         {13, 0x0007}

#define CAT_LA_F        {14, 0xC000}
#define CAT_MATI        {14, 0x3C00}
#define CAT_NIT         {14, 0x0380}
#define CAT_TARDA       {14, 0x00F8}

void loadCode(clockword code, unsigned long * matrix);
void loadLanguageCatalan(byte hour, byte minute, unsigned long * matrix);
