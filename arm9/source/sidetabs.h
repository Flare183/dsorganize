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
 
#ifndef _SIDETABS_INCLUDED
#define _SIDETABS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	u32 value;
	u32 enabled;
	uint16 *icon;
} SIDETAB_TYPE;

#define SIDETAB_START_Y 37
#define SIDETAB_HEIGHT 19
#define SIDETAB_WIDTH 16
#define SIDETAB_OUT 6
#define SIDETAB_DISABLEDWIDTH 3
#define SIDETAB_ICON_SIZE 2048  // 4kb to play with
#define SIDETAB_ICON_WIDTH 19

#define SIDEBAR_ENABLED 0
#define SIDEBAR_DISABLED 1

void initSideTabs();
void destroySideTabs();
void resetSideTabs();
void displaySideTabs();
void addSideTab(u32 value, uint16 *iconData);
void setSideTabEnabled(u32 value, bool enabled);
void setDefaultSideTab(u32 value);
bool isSideTab(int px, int py);
bool clickSideTab(int px, int py);
u32 getActiveSideTab();
bool isSideTabsActive();

#ifdef __cplusplus
}
#endif

#endif
