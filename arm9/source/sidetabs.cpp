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
 
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <libfb/libcommon.h>
#include "sidetabs.h"
#include "general.h"
#include "colors.h"

// shared with tabs
SIDETAB_TYPE *sideTabs = NULL;
int numTabs = 0;
int activeTab = 0;
int defaultTab = 0;

void initSideTabs()
{
	destroySideTabs();
	
	numTabs = 0;
	activeTab = 0;
	defaultTab = 0;
}

void destroySideTabs()
{
	if(sideTabs)
	{
		for(int i=0;i<numTabs;i++)
		{
			if(sideTabs[i].icon)
				trackFree(sideTabs[i].icon);
			
			sideTabs[i].icon = NULL;
		}
		
		trackFree(sideTabs);
	}
	
	sideTabs = NULL;
	numTabs = 0;
}

bool isSideTabsActive()
{
	if(!sideTabs)
		return false;
	if(!numTabs)
		return false;
	
	return true;
}

void resetSideTabs()
{
	activeTab = defaultTab;
}

void displaySideTabs()
{
	if(!isSideTabsActive())
	{
		return;
	}
	
	for(int i=0;i<numTabs;i++)
	{
		if(sideTabs[i].enabled == SIDEBAR_DISABLED)
		{
			bg_drawRect(0, i*SIDETAB_HEIGHT + SIDETAB_START_Y, SIDETAB_DISABLEDWIDTH-1, i*SIDETAB_HEIGHT + SIDETAB_START_Y, widgetBorderColor);
			bg_drawRect(SIDETAB_DISABLEDWIDTH, i*SIDETAB_HEIGHT+1 + SIDETAB_START_Y, SIDETAB_DISABLEDWIDTH, i*SIDETAB_HEIGHT + SIDETAB_HEIGHT-1 + SIDETAB_START_Y, widgetBorderColor);
			bg_drawRect(0, (i+1)*SIDETAB_HEIGHT + SIDETAB_START_Y, SIDETAB_DISABLEDWIDTH-1, (i+1)*SIDETAB_HEIGHT + SIDETAB_START_Y, widgetBorderColor);
			
			bg_drawRect(0, i*SIDETAB_HEIGHT+1 + SIDETAB_START_Y, SIDETAB_DISABLEDWIDTH-1, i*SIDETAB_HEIGHT + SIDETAB_HEIGHT-2 + SIDETAB_START_Y, widgetFillColor);
		}
		else
		{
			bg_drawRect(0, i*SIDETAB_HEIGHT + SIDETAB_START_Y, SIDETAB_WIDTH + (i == activeTab ? SIDETAB_DISABLEDWIDTH : 0) - 1, i*SIDETAB_HEIGHT + SIDETAB_START_Y, widgetBorderColor);
			bg_drawRect(SIDETAB_WIDTH + (i == activeTab ? SIDETAB_DISABLEDWIDTH : 0), i*SIDETAB_HEIGHT+1 + SIDETAB_START_Y, SIDETAB_WIDTH + (i == activeTab ? SIDETAB_DISABLEDWIDTH : 0), i*SIDETAB_HEIGHT + SIDETAB_HEIGHT-1 + SIDETAB_START_Y, widgetBorderColor);
			bg_drawRect(0, (i+1)*SIDETAB_HEIGHT + SIDETAB_START_Y, SIDETAB_WIDTH + (i == activeTab ? SIDETAB_DISABLEDWIDTH : 0) - 1, (i+1)*SIDETAB_HEIGHT + SIDETAB_START_Y, widgetBorderColor);
			
			bg_drawRect(0, i*SIDETAB_HEIGHT+1 + SIDETAB_START_Y, SIDETAB_WIDTH + (i == activeTab ? SIDETAB_DISABLEDWIDTH : 0) - 1, i*SIDETAB_HEIGHT + SIDETAB_HEIGHT-2 + SIDETAB_START_Y, widgetFillColor);
			
			if(sideTabs[i].icon != NULL)
			{
				int x = (SIDETAB_WIDTH + (i == activeTab ? SIDETAB_DISABLEDWIDTH : 0)) - (SIDETAB_ICON_WIDTH);
				
				bg_dispSprite(x, i*SIDETAB_HEIGHT+1 + SIDETAB_START_Y, sideTabs[i].icon, 31775);
			}
		}
	}
}

void addSideTab(u32 value, uint16 *iconData)
{
	if(!numTabs)
	{	
		numTabs = 1;
		sideTabs = (SIDETAB_TYPE *)trackMalloc(sizeof(SIDETAB_TYPE), "side tabs");
	}
	else
	{
		numTabs++;
		sideTabs = (SIDETAB_TYPE *)trackRealloc(sideTabs, sizeof(SIDETAB_TYPE) * numTabs);
	}
	
	memset(&sideTabs[numTabs - 1], 0, sizeof(SIDETAB_TYPE));
	
	sideTabs[numTabs - 1].value = value;
	sideTabs[numTabs - 1].enabled = SIDEBAR_ENABLED;
	
	if(!iconData)
		sideTabs[numTabs - 1].icon = NULL;
	else
	{
		sideTabs[numTabs - 1].icon = (uint16 *)trackMalloc(SIDETAB_ICON_SIZE, "side tabs icon");
		memcpy(sideTabs[numTabs - 1].icon, iconData, SIDETAB_ICON_SIZE);
	}
}

void setSideTabEnabled(u32 value, bool enabled)
{
	for(int i=0;i<numTabs;i++)
	{
		if(sideTabs[i].value == value)
		{
			if(enabled)
				sideTabs[i].enabled = SIDEBAR_ENABLED;
			else
				sideTabs[i].enabled = SIDEBAR_DISABLED;
			
			return;
		}
	}
}

void setDefaultSideTab(u32 value)
{
	if(!isSideTabsActive())
	{
		return;
	}
	
	for(int i=0;i<numTabs;i++)
	{
		if(sideTabs[i].value == value)
		{
			defaultTab = i;
			return;
		}
	}
}

bool isSideTab(int px, int py)
{
	if(!isSideTabsActive())
	{
		return false;
	}
	
	if(px > SIDETAB_WIDTH + SIDETAB_OUT)
		return false;
	
	py -= SIDETAB_START_Y;
	
	int i = (py / SIDETAB_HEIGHT);
	
	if(i < 0)
		return false;
	if(i >= numTabs)
		return false;
	
	return true;
}

bool clickSideTab(int px, int py)
{
	if(!isSideTabsActive())
	{
		return false;
	}
	
	if(px > SIDETAB_WIDTH + SIDETAB_OUT)
		return false;
	
	py -= SIDETAB_START_Y;
	
	int i = (py / SIDETAB_HEIGHT);
	
	if(i < 0)
		return false;
	if(i >= numTabs)
		return false;
	
	int oldTab = activeTab;
	
	if(sideTabs[i].enabled == SIDEBAR_ENABLED)
		activeTab = i;
	
	if(oldTab != activeTab)
		return true;
	
	return false;
}

u32 getActiveSideTab()
{
	if(!isSideTabsActive())
	{
		return 0;
	}
	
	return sideTabs[activeTab].value;
}
