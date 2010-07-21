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
#include <math.h>
#include <stdlib.h>
#include <libfb/libcommon.h>
#include "general.h"
#include "drawtools.h"

static int *stack = NULL;
static int stackPointer = 0;
static int penWidth;
static bool drawCheckered = false;
static uint16 *drawBuffer;

void setPenWidth(int x)
{
	penWidth = x;
}

void setDrawCheckered(bool x)
{
	drawCheckered = x;
}

void setDrawBuffer(uint16 *x)
{
	drawBuffer = x;
}

uint16 *getDrawBuffer()
{
	return drawBuffer;
}

bool pop(int &x, int &y) 
{ 
    if(stackPointer > 0) 
    { 
        int p = stack[stackPointer]; 
        x = p / 192; 
        y = p % 192; 
        stackPointer--; 
        return 1; 
    }     
    else 
    { 
        return 0; 
    }    
}    

bool push(int x, int y) 
{ 
    if(stackPointer < STACK_SIZE - 1) 
    { 
        stackPointer++; 
        stack[stackPointer] = 192 * x + y; 
        return 1; 
    }     
    else 
    { 
        return 0; 
    }    
}     

void emptyStack() 
{ 
    int x, y; 
    while(pop(x, y)); 
}

// drawing routines

void s_setPixel(int cx, int cy, uint16 color)
{
	int s = 0;
	
	if(penWidth > 1)
		s = penWidth / 2;
	else
	{
		if(cx < 0 || cx > 255 || cy < 0 || cy > 191)
			return;
		if(drawCheckered)
		{
			if(((cx+cy) & 1) == 1)
				drawBuffer[cx + cy*256] = color | BIT(15);
			else
				drawBuffer[cx + cy*256] = (0xFFFF - color) | BIT(15);
		}
		else
			drawBuffer[cx + cy*256] = color | BIT(15);
		return;
	}

	for(int y=(cy-s);y<(cy+s);y++)
	{
		if(y >= 0 && y <= 191)	
		{
			for(int x=(cx-s);x<(cx+s);x++)
			{
				if(x >= 0 && x <= 255)
				{
					if(drawCheckered)
					{
						if(((x+y) & 1) == 1)
							drawBuffer[x + y*256] = color | BIT(15);
						else
							drawBuffer[x + y*256] = (0xFFFF - color) | BIT(15);
					}
					else
						drawBuffer[x + y*256] = color | BIT(15);
				}
			}
		}
	}
}

void s_drawLine(int x0, int y0, int x1, int y1, uint16 color)
{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;

	if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;                                                  // dy is now 2*dy
	dx <<= 1;                                                  // dx is now 2*dx

	s_setPixel(x0, y0, color);
	
	if (dx > dy) 
	{
		int fraction = dy - (dx >> 1);                         // same as 2*dy - dx
		while (x0 != x1) 
		{
			if (fraction >= 0) 
			{
				y0 += stepy;
				fraction -= dx;                                // same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;                                    // same as fraction -= 2*dy
			s_setPixel(x0, y0, color);
		}
	} 
	else 
	{
		int fraction = dx - (dy >> 1);
		while (y0 != y1) 
		{
			if (fraction >= 0) 
			{
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			s_setPixel(x0, y0, color);
		}
	}
}

// these two routines will look ugly, I converted them from pascal

void plot4EllipsePoints(int centerX, int centerY, int x, int y, uint16 color)
{
	s_setPixel(centerX + x, centerY + y, color);
	s_setPixel(centerX - x, centerY + y, color);
	s_setPixel(centerX - x, centerY - y, color);
	s_setPixel(centerX + x, centerY - y, color);
}

void s_drawEllipse(int centerX, int centerY, int XRadius, int YRadius, uint16 color) 
{
	int X = XRadius;
	int Y = 0;
	int XChange = YRadius * YRadius * (1 - (2 * XRadius));
	int YChange = XRadius * XRadius;
	int TwoASquare = (XRadius * XRadius) << 1;
	int TwoBSquare = (YRadius * YRadius) << 1;
	int StoppingX = TwoBSquare * XRadius;
	int StoppingY = 0;
	int EllipseError = 0;

	while(StoppingX >= StoppingY)
	{
        plot4EllipsePoints(centerX, centerY, X, Y, color);
		
		Y++;
		StoppingY+=TwoASquare;
		EllipseError+=YChange;
		YChange+=TwoASquare;
		
		if(((2 * EllipseError) + XChange) > 0)
		{
			X--;
			StoppingX-=TwoBSquare;
			EllipseError+=XChange;
			XChange+=TwoBSquare;
        }
	}
  
	X = 0;
	Y = YRadius;
	XChange = YRadius * YRadius;
	YChange = XRadius * XRadius * (1 - (2 * YRadius));
	EllipseError = 0;
	StoppingX = 0;
	StoppingY = TwoASquare * YRadius;
	
	while(StoppingX <= StoppingY)
    {
        plot4EllipsePoints(centerX, centerY, X, Y, color);
		
		X++;
		StoppingX+=TwoBSquare;
		EllipseError+=XChange;
		XChange+=TwoBSquare;
		
		if(((2 * EllipseError) + YChange) > 0)
        {
			Y--;
			StoppingY-=TwoASquare;
			EllipseError+=YChange;
			YChange+=TwoASquare;
        }
    }
}

void s_floodFill(int x, int y, uint16 newColor, uint16 oldColor) 
{ 
	newColor |= BIT(15);
	oldColor |= BIT(15);
	
    if(newColor == oldColor) 
		return; //avoid infinite loop
	
	stackPointer = 0;
	stack = (int *)safeMalloc(STACK_SIZE);
	
    emptyStack();
    
    if(!push(x, y)) 
	{
		free(stack);
		return;
	}
	
    while(pop(x, y))
    {
        drawBuffer[x + (y * 256)] = newColor;
        if(x + 1 < 256 && drawBuffer[x + 1 + (y * 256)] == oldColor)
        {          
            if(!push(x + 1, y))
			{
				free(stack);
				return;           
			}
        }    
        if(x - 1 >= 0 && drawBuffer[x - 1 + (y * 256)] == oldColor)
        {
            if(!push(x - 1, y))
			{
				free(stack);
				return;           
			}
        }    
        if(y + 1 < 192 && drawBuffer[x + ((y + 1) * 256)] == oldColor)
        {
            if(!push(x, y + 1)) 
			{
				free(stack);
				return;           
			}
        }    
        if(y - 1 >= 0 && drawBuffer[x + ((y - 1) * 256)] == oldColor)
        {
            if(!push(x, y - 1)) 
			{
				free(stack);
				return;
			}
        }    
    }   

	free(stack); 
}

void s_spray(int x, int y, int radius, uint16 color)
{
	for(int b=y-radius;b<y+radius+1;b++)
	{
		for(int a=x-radius;a<x+radius+1;a++)
		{
			int d = (rand() >> 8) & 1;
			
			int u = abs(x - a);
			int v = abs(y - b);
			
			int c = (int)sqrt((u*u) + (v*v));
			
			penWidth = 1;
			if(c <= radius && d == 1)
				s_setPixel(a, b, color);
		}
	}
}

// color picker

void separateRGB(uint16 color, int &r, int &g, int &b)
{
	b = (color >> 10) & 31;
	g = (color >> 5) & 31;
	r = color & 31;
}

int setRGB(int a)
{
	if(a < 0)
		return 0;
	if(a > 31)
		return 31;
	
	return a;
}

uint16 drawSlider(int x, int y, uint16 baseColor, int slidePosition, uint16 *buffer)
{	
	int mr;
	int mg;
	int mb;
	
	uint16 finalColor = baseColor;
	
	separateRGB(baseColor, mr, mg, mb);
	
	double rf = (double)31;
	double gf = (double)31;
	double bf = (double)31;
		
	double rr = (double)(31 - mr) / (double)(31*3);
	double gr = (double)(31 - mg) / (double)(31*3);
	double br = (double)(31 - mb) / (double)(31*3);
	
	for(int j=0;j<31*3;j++)
	{
		for(int i=0;i<10;i++)
			buffer[i + x + ((y + j) * 256)] = RGB15((int)rf,(int)gf,(int)bf) | BIT(15);
		
		if(j == slidePosition)
			finalColor = RGB15((int)rf,(int)gf,(int)bf);
		
		rf -= rr;
		gf -= gr;
		bf -= br;
	}
	
	rr = (double)(mr) / (double)(31*3);
	gr = (double)(mg) / (double)(31*3);
	br = (double)(mb) / (double)(31*3);
	
	for(int j=0;j<31*3;j++)
	{
		for(int i=0;i<10;i++)
			buffer[i + x + ((y + j + (31*3)) * 256)] = RGB15((int)rf,(int)gf,(int)bf) | BIT(15);
			
		if((j + (31*3)) == slidePosition)
			finalColor = RGB15((int)rf,(int)gf,(int)bf);
		
		rf -= rr;
		gf -= gr;
		bf -= br;
	}
	
	return finalColor;
}

void drawChooser(int x, int y, uint16 *buffer)
{
	int red = 31*2;
	int green = 0;
	int blue = -31 * 2;
	
	int redDir = -1;
	int greenDir = 1;
	int blueDir = 1;
	
	int r;
	int g;
	int b;
	
	for(int i=0;i<31*6;i++)
	{
		r = setRGB(red);
		g = setRGB(green);
		b = setRGB(blue);
		
		double rf = (double)r;
		double gf = (double)g;
		double bf = (double)b;
		
		double rr = (double)((31/2) - r) / (double)(31*6);
		double gr = (double)((31/2) - g) / (double)(31*6);
		double br = (double)((31/2) - b) / (double)(31*6);
		
		for(int j=0;j<31*6;j++)
		{			
			buffer[i + x + ((y + j) * 256)] = RGB15((int)rf,(int)gf,(int)bf) | BIT(15);
			
			rf += rr;
			gf += gr;
			bf += br;
		}
		
		red += redDir;
		green += greenDir;
		blue += blueDir;
		
		// limits
		if(redDir == 0)
			redDir = 1;
		if(greenDir == 0)
			greenDir = -1;
		if(blueDir == 0)
			blueDir = -1;
		
		if(red == -31 && redDir == -1)
			redDir = 0;
		if(green == 31*2 && greenDir == 1)
			greenDir = 0;
		if(blue == 31*2 && blueDir == 1)
			blueDir = 0;
	}
}

