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
 
#ifndef _CONTROLS_INCLUDED
#define _CONTROLS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// scrollbar metrics
#define DEFAULT_SCROLL_LEFT_L 5
#define DEFAULT_SCROLL_LEFT_R 240
#define DEFAULT_SCROLL_WIDTH 10
#define SCROLL_ARROW_HEIGHT 12
#define SCROLL_HANDLE_HEIGHT 12

// list location
#define LIST_LEFT 5
#define LIST_RIGHT 250
#define LIST_TOP 14
#define LIST_BOTTOM 158

#define LIST_WIDTH ((LIST_RIGHT - LIST_LEFT) - (DEFAULT_SCROLL_WIDTH + 2))
#define LIST_STEP 16

// for handedness
#define HAND_RIGHT 0
#define HAND_LEFT 1

// for keys on keyboard
#define DEFAULT_TOP 138
#define DEFAULT_BOTTOM 155

#define INS_LEFT 13
#define INS_TOP DEFAULT_TOP
#define INS_RIGHT 44
#define INS_BOTTOM DEFAULT_BOTTOM
#define INS_CENTER ((INS_LEFT + INS_RIGHT) / 2)

#define CLEAR_LEFT 200
#define CLEAR_TOP DEFAULT_TOP
#define CLEAR_RIGHT 242
#define CLEAR_BOTTOM DEFAULT_BOTTOM
#define CLEAR_CENTER ((CLEAR_LEFT + CLEAR_RIGHT) / 2)

#define DELETE_LEFT 156
#define DELETE_TOP DEFAULT_TOP
#define DELETE_RIGHT 198
#define DELETE_BOTTOM DEFAULT_BOTTOM
#define DELETE_CENTER ((DELETE_LEFT + DELETE_RIGHT) / 2)

#define CTRL_LEFT 46
#define CTRL_TOP DEFAULT_TOP
#define CTRL_RIGHT 88
#define CTRL_BOTTOM DEFAULT_BOTTOM
#define CTRL_CENTER ((CTRL_LEFT + CTRL_RIGHT) / 2)

#define SELECT_LEFT 90
#define SELECT_TOP DEFAULT_TOP
#define SELECT_RIGHT 132
#define SELECT_BOTTOM DEFAULT_BOTTOM
#define SELECT_CENTER ((SELECT_LEFT + SELECT_RIGHT) / 2)

// for button locations
#define O0_TOP 162
#define O0_BOTTOM 182
#define O0_FORWARDLEFT 10
#define O0_FORWARDRIGHT 126
#define O0_BACKLEFT 130
#define O0_BACKRIGHT 244
#define O0_RBUTTON 253
#define O0_CENTER 128
#define O0_HEIGHT 192

#define O90_TOP 226
#define O90_BOTTOM 246
#define O90_FORWARDLEFT 10
#define O90_FORWARDRIGHT 94
#define O90_BACKLEFT 98
#define O90_BACKRIGHT 180
#define O90_RBUTTON 189
#define O90_CENTER 96
#define O90_HEIGHT 256

#define TOPAREA 15

#define KEYBOARD_LEFT 13
#define KEYBOARD_TOP 37

#define MAX_ORIENTATIONS 10

enum {
	CT_BUTTON,
	CT_SCROLL,
	CT_LIST,
	CT_HOME,
	CT_LBUTTON,
	CT_RBUTTON,
	CT_KEYBOARD,
	CT_HITBOX,
	CT_HOTKEY,
	CT_SCREEN,
	CT_SIDEBAR,
	CT_CUSTOM,
	CT_DELETED = 0xFF
};

#define CE_DOWN 0
#define CE_MOVE 1
#define CE_UP 2
#define CE_DELETE 3

#define ATTR_VISIBLE BIT(0)
#define ATTR_ENABLED BIT(1)
#define ATTR_ABSCOORD BIT(2)
#define ATTR_REPEAT BIT(3)
#define ATTR_GRAYED BIT(4)
#define ATTR_ALWAYSDRAW BIT(5)

#define CONTROL_HOME 0xFFF0
#define CONTROL_FORWARD 0xFFF1
#define CONTROL_BACKWARD 0xFFF2
#define CONTROL_L 0xFFF3
#define CONTROL_R 0xFFF4
#define CONTROL_SCREEN 0xFFF5
#define CONTROL_LIST 0xFFF6
#define CONTROL_KEYBOARD 0xFFF7
#define CONTROL_SIDEBAR 0xFFF8
#define CONTROL_SCROLL 0xFFF8

#define SCREEN_FB 0
#define SCREEN_BG 1

// data types
typedef struct
{
	u32 controlID;
	
	int x;
	int y;
	int bx;
	int by;
	u32 state;
	u8 attributes;
	u8 repeatstart;
	u8 repeatcontinue;
	s8 repeatcount;
	u32 type;
	u32 hotkey1;
	u32 hotkey2;
	
	void (*touchDown)(int x, int y);
	void (*touchMove)(int x, int y);
	void (*touchUp)();
	
	void *priv;
	void *priv2;
	void *priv3;
} CONTROL_STRUCT;

// global stuff
void addHome(char *title, void (*tD)(int, int));
void addDefaultButtons(char *ltitle, void (*tL)(int, int), char *rtitle, void (*tR)(int, int));
void addDefaultLR(char *ltitle, void (*tL)(int, int), char *rtitle, void (*tR)(int, int));
void addListBox(int x, int y, int bx, int by, u32 maxItems, char *emptyText, void (*cb1)(int, int, int), void (*cb2)(int,int));
void addKeyboard(int x, int y, bool displayExtra, bool displayDelete, int whichScreen, void (*actionKB)(char), void (*del)());
void addHotKey(u32 controlID, u32 hotkeyDown, void (*a1)(int,int), u32 hotkeyMove, void (*a2)(int,int));
void addSideBar(void (*cb1)(int, int));
void addScrollBar(int x, int y, int by, int maxValue, int *curValue, void (*cb)(int,int));
void addCustom(u32 controlID, int x, int y, int bx, int by, void (*df)());

void drawTimeDate();
int getScreenWidth();
int getScreenHeight();
int getScreenCenter();

// keyboard stuff
int getLLocation();
int getRLocation();

// rounded buttons
void drawRoundedButton(int x, int y, int bx, int by, char *text, bool select, bool highlight);

// textboxes
void drawTextBox(int x, int y, int bx, int by, const char *text, bool centered, bool selected, bool italics);

// left handed settings
void setHand(int m);
int getHand();
void swapHands();

// for orientation
void setNewOrientation(int orientation);
void setOldOrientation();
int getOrientation();

// for global controls stuff
void clearControls();
void updateControlRepeats();
void drawControls(bool specialOnly);
void queueControls(int x, int y, int type);
void queueHotKeys(u32 hkdown, u32 hkheld);
void executeQueuedControls();
u32 getControlID();
void registerScreenUpdate(void (*su)(), void (*fr)());
void newControl(u32 controlID, int x, int y, int bx, int by, u32 type, const void *priv);
bool doesControlExist(u32 controlID);
void deleteControl(u32 controlID);
void setControlCallbacks(u32 controlID, void (*touchDown)(int x, int y), void (*touchMove)(int x, int y), void (*touchUp)());
void setControlVisible(u32 controlID, bool visible);
void setControlEnabled(u32 controlID, bool enabled);
void setControlGrayed(u32 controlID, bool grayed);
void setControlAbsoluteCoords(u32 controlID, bool abscoord);
void setControlState(u32 controlID, u32 state);
void setControlHotkey(u32 controlID, u32 hotkeydown, u32 hotkeyheld);
void setControlRepeat(u32 controlID, u8 repeatStart, u8 repeatContinue);
void setControlAlwaysDraw(u32 controlID, bool alwaysdraw);
void setControlAttribute(u32 controlID, u32 attrib, bool state);
void updateControl(u32 controlID, const void *priv);

#ifdef __cplusplus
}
#endif

#endif
