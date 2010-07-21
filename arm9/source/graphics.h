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
 
#ifndef _GRAPHICS_INCLUDED
#define _GRAPHICS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <nds.h>
#include <stdio.h>

#define ICON_SIZE 256

typedef struct
{
	char iconURL[ICON_SIZE];
} ICON_LIST;

extern uint16 k_caps[];
extern uint16 k_shift[];
extern uint16 k_del[];
extern uint16 k_space[];
extern uint16 k_bsp[];
extern uint16 k_ret[];
extern uint16 k_spl[];
extern uint16 delete_g[];
extern uint16 errmsg[];
extern uint16 up_arrow[];
extern uint16 down_arrow[];
extern uint16 wb_stop[];
extern uint16 wb_radio[];
extern uint16 wb_ret[];

//-----------------
//home screen stuff
//-----------------

extern uint16 *spr_unavailable;
extern uint16 *spr_Scribble;
extern uint16 *spr_Cal;
extern uint16 *spr_Day;
extern uint16 *spr_Add;
extern uint16 spr_left[];
extern uint16 spr_right[];
extern uint16 *spr_browser;
extern uint16 *spr_next;
extern uint16 *spr_back;
extern uint16 *spr_calc;
extern uint16 *spr_Todo;
extern uint16 *spr_irc;
extern uint16 *spr_webbrowser;
extern uint16 *spr_hbdb;

extern uint16 *spr_webbrowser;
extern uint16 *tab_keyboard;
extern uint16 *tab_special;
extern uint16 *tab_favorites;
extern uint16 *tab_irccapture;
extern uint16 *tab_colors;
extern uint16 *tab_nicks;

//------
// icons
//------

extern uint16 *folder;
extern uint16 *vcfile;
extern uint16 *dpfile;
extern uint16 *txtfile;
extern uint16 *reminderfile;
extern uint16 *unknown;
extern uint16 *ndsfile;
extern uint16 *picfile;
extern uint16 *todofile;
extern uint16 *soundfile;
extern uint16 *playlistfile;
extern uint16 *pluginfile;
extern uint16 *hbfile;
extern uint16 *exefile;
extern uint16 *shortcutfile;
extern uint16 *ndsgenericlarge;
extern uint16 *unknownlarge;
extern uint16 *diretorylarge;
extern uint16 *vcfilelarge;
extern uint16 *dpfilelarge;
extern uint16 *reminderfilelarge;
extern uint16 *txtfilelarge;
extern uint16 *picfilelarge;
extern uint16 *todofilelarge;
extern uint16 *soundfilelarge;
extern uint16 *playlistfilelarge;
extern uint16 *pluginfilelarge;
extern uint16 *hbfilelarge;
extern uint16 *exefilelarge;
extern uint16 *cantlaunchlarge;

//---------------------------------
// remaning stuff for config screen
//---------------------------------
extern uint16 *scribbledrag;
extern uint16 *browserdrag;
extern uint16 *calcdrag;
extern uint16 *ircdrag;
extern uint16 *webbrowserdrag;
extern uint16 *hbdbdrag;

//----------------
// todo list stuff
//----------------

extern uint16 onhold[];
extern uint16 normal[];
extern uint16 normalsemi[];
extern uint16 urgent[];
extern uint16 urgentsemi[];
extern uint16 finished[];

//---------------
// scribble tools
//---------------

extern uint16 tool_pencil[];
extern uint16 tool_pen[];
extern uint16 tool_colorgrab[];
extern uint16 tool_erasor[];
extern uint16 tool_floodfill[];
extern uint16 tool_line[];
extern uint16 tool_thickline[];
extern uint16 tool_rect[];
extern uint16 tool_thickrect[];
extern uint16 tool_circle[];
extern uint16 tool_thickcircle[];
extern uint16 tool_spray[];
extern uint16 tool_replace[];
extern uint16 tool_text[];

// for cute little cow animation
extern uint16 s_cow1[];
extern uint16 s_cow2[];
extern uint16 s_cow3[];
extern uint16 s_cow4[];

// for bold stuff on irc
extern uint16 irc_bold[];
extern uint16 irc_underline[];
extern uint16 irc_color[];

// for day view
extern uint16 dv_daily[];
extern uint16 dv_weekly[];
extern uint16 dv_monthly[];
extern uint16 dv_annually[];

// for configuration
extern uint16 *c_global;
extern uint16 *c_home;
extern uint16 *c_scribble;
extern uint16 *c_todo;
extern uint16 *c_editor;
extern uint16 *c_browser;
extern uint16 *c_irc;
extern uint16 *c_web;
extern uint16 *c_wifi;
extern uint16 *c_image;

extern uint16 *c_unlocked;
extern uint16 *c_locked;

// for keyboards
extern uint16 normalKeyboard[];
extern uint16 bold[];
extern uint16 underline[];
extern uint16 color[];
extern uint16 backspace[];

int listIconSets(ICON_LIST buffer[]);
void loadIconSet();
void initIconSet();
void loadIconGraphics();

#ifdef __cplusplus
}
#endif

#endif
