/***************************************************************************
 *                                                                         *
 *  This file is part of DSOrganize.                                       *
 *                                                                         *
 *  DSOrganize is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  DSOrganize is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with DSOrganize.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/
 
#ifndef _TABS_INCLUDED
#define _TABS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define TAB_BOTTOM 32
#define TAB_WIDTH 20
#define TAB_HEIGHT 17
#define TAB_DISABLED_HEIGHT 3
#define TAB_X_OFFSET 5

void initTabs();
void destroyTabs();
void resetTabs();
void displayTabs();
void addTab(u32 value, uint16 *iconData);
void setTabEnabled(u32 value, bool enabled);
void setDefaultTab(u32 value);
bool clickTab(int px, int py);
void setTab(u32 value);
u32 getActiveTab();

#ifdef __cplusplus
}
#endif

#endif
