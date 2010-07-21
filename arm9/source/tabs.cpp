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
#include "tabs.h"
#include "general.h"
#include "colors.h"

/* The tabs implementation here is largely derrived from the
   Sidetabs implementation I wrote earlier, simply because that
   came first.  Only overridden is the drawing function, really. */
   
extern SIDETAB_TYPE *sideTabs;
extern int numTabs;
extern int activeTab;
extern int defaultTab;

void initTabs()
{
	initSideTabs();
}

void destroyTabs()
{
	destroySideTabs();
}

void resetTabs()
{
	resetSideTabs();
}

void addTab(u32 value, uint16 *iconData)
{
	addSideTab(value, iconData);
}

void setTabEnabled(u32 value, bool enabled)
{
	setSideTabEnabled(value, enabled);
}

void setDefaultTab(u32 value)
{
	setDefaultSideTab(value);
}

u32 getActiveTab()
{
	return getActiveSideTab();
}

bool clickTab(int px, int py)
{
	if(!isSideTabsActive())
	{
		return false;
	}
	
	if(py > TAB_BOTTOM || py < (TAB_BOTTOM - TAB_HEIGHT))
		return false;
	
	py -= (TAB_BOTTOM - TAB_HEIGHT);
	
	int i = ((px - TAB_X_OFFSET) / TAB_WIDTH);
	
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

void displayTabs()
{
	if(!isSideTabsActive())
	{
		return;
	}
	
	for(int i=0;i<numTabs;i++)
	{
		if(sideTabs[i].enabled == SIDEBAR_DISABLED)
		{
			bg_drawRect(TAB_X_OFFSET + (i * TAB_WIDTH), (TAB_BOTTOM - TAB_DISABLED_HEIGHT) + 1, TAB_X_OFFSET + (i * TAB_WIDTH), TAB_BOTTOM, widgetBorderColor);
			bg_drawRect(TAB_X_OFFSET + (i * TAB_WIDTH) + 1, (TAB_BOTTOM - TAB_DISABLED_HEIGHT), TAB_X_OFFSET + ((i + 1) * TAB_WIDTH) - 1, (TAB_BOTTOM - TAB_DISABLED_HEIGHT), widgetBorderColor);
			bg_drawRect(TAB_X_OFFSET + ((i + 1) * TAB_WIDTH), (TAB_BOTTOM - TAB_DISABLED_HEIGHT) + 1, TAB_X_OFFSET + ((i + 1) * TAB_WIDTH), TAB_BOTTOM, widgetBorderColor);
		}
		else
		{
			bg_drawRect(TAB_X_OFFSET + (i * TAB_WIDTH), (TAB_BOTTOM - TAB_HEIGHT) + 1, TAB_X_OFFSET + (i * TAB_WIDTH), TAB_BOTTOM, widgetBorderColor);
			bg_drawRect(TAB_X_OFFSET + (i * TAB_WIDTH) + 1, (TAB_BOTTOM - TAB_HEIGHT), TAB_X_OFFSET + ((i + 1) * TAB_WIDTH) - 1, (TAB_BOTTOM - TAB_HEIGHT), widgetBorderColor);
			bg_drawRect(TAB_X_OFFSET + ((i + 1) * TAB_WIDTH), (TAB_BOTTOM - TAB_HEIGHT) + 1, TAB_X_OFFSET + ((i + 1) * TAB_WIDTH), TAB_BOTTOM, widgetBorderColor);
			
			if(sideTabs[i].icon)
			{
				bg_dispSprite((i * TAB_WIDTH) + 2 + TAB_X_OFFSET, (TAB_BOTTOM - TAB_HEIGHT) + 1, sideTabs[i].icon, 31775);
			}
		}
	}
	
	bg_drawRect(0, TAB_BOTTOM, 255, TAB_BOTTOM, widgetBorderColor);
	bg_drawRect(TAB_X_OFFSET + (activeTab * TAB_WIDTH), TAB_BOTTOM, TAB_X_OFFSET + ((activeTab + 1) * TAB_WIDTH), TAB_BOTTOM, genericFillColor);
}

void setTab(u32 value)
{
	if(!isSideTabsActive())
	{
		return;
	}
	
	if(value >= sideTabs[numTabs - 1].value)
	{
		activeTab = numTabs - 1;
		return;
	}
	
	for(int i=0;i<numTabs;i++)
	{
		if(sideTabs[i].value > value)
		{
			activeTab = i - 1;
			return;
		}
	}
	
	return;
}
