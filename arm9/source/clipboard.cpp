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
#include "clipboard.h"
#include "general.h"

static char *emptyBoard = "";
static char *clipData = NULL;

void initClipboard()
{
	clipData = NULL;
}

void clearClipboard()
{
	if(clipData != NULL)
		trackFree(clipData);
	
	clipData = NULL;
}

void setClipboard(char *cData)
{
	int tLen = strlen(cData);
	
	clearClipboard();
	
	clipData = (char *)trackMalloc(tLen + 1, "clipboard data");
	memset(clipData, 0, tLen + 1);
	strcpy(clipData, cData);
}

void setClipboardRange(char *cData, int start, int end)
{
	int tLen = end - start;
	
	clearClipboard();
	
	clipData = (char *)trackMalloc(tLen + 1, "clipboard data");
	memset(clipData, 0, tLen + 1);
	memcpy(clipData, cData + start, tLen);
}

char *getClipboard()
{
	if(clipData == NULL)
	{
		return emptyBoard;
	}
	else
	{
		return clipData;
	}
}
