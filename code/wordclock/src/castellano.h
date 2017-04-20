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

#define ESP_ES          {0, 0x0C00}
#define ESP_SON         {0, 0x0700}
#define ESP_CASI        {0, 0x000F}

#define ESP_LA          {4, 0x0600}
#define ESP_LAS         {4, 0x0700}

#define ESP_CINCO       {5, 0xF800}
#define ESP_OCHO        {5, 0x0F00}
#define ESP_UNA         {5, 0x0007}

#define ESP_DOS         {6, 0xE000}
#define ESP_NUEVE       {6, 0x1F00}
#define ESP_DIEZ        {6, 0x00F0}
#define ESP_ONCE        {6, 0x000F}

#define ESP_TRES        {7, 0x1E00}
#define ESP_SEIS        {7, 0x03C0}
#define ESP_CUATRO      {7, 0x003F}

#define ESP_SIETE       {8, 0xF800}
#define ESP_DOCE        {8, 0x0780}
#define ESP_Y           {8, 0x0020}
#define ESP_MENOS       {8, 0x001F}

#define ESP_VEINTICINCO {10, 0xFFE0}
#define ESP_CINCO_B     {10, 0x03E0}
#define ESP_MEDIA       {10, 0x001F}

#define ESP_CUARTO      {11, 0xFC00}
#define ESP_DIEZ_B      {11, 0x03C0}
#define ESP_VEINTE      {11, 0x003F}

#define ESP_EN_PUNTO    {12, 0x601F}

#define ESP_PASADA      {13, 0x03F0}
#define ESP_PASADAS     {13, 0x03F8}

#define ESP_DE_F        {14, 0xC000}
#define ESP_LA_F        {14, 0x1800}

#define ESP_MANANA      {15, 0xFC00}
#define ESP_TARDE       {15, 0x03E0}
#define ESP_NOCHE       {15, 0x001F}

void loadCode(clockword code, unsigned int * matrix);
void loadLanguageCastellano(byte hour, byte minute, unsigned int * matrix);
