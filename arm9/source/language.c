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
#include "language.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ini.h>
#include "fatwrapper.h"
#include "globals.h"
#include "general.h"

char l_days[14][30];
char l_months[24][30];
char l_ampm[2][30];

char l_home[60];
char l_go[60];
char l_welcome[60];
char l_prev[60];
char l_next[60];
char l_reminder[60];
char l_editreminder[60];
char l_viewday[60];
char l_dayview[60];
char l_save[60];
char l_back[60];
char l_editselected[60];
char l_calendar[60];
char l_create[60];
char l_prevfield[60];
char l_nextfield[60];
char l_browser[60];
char l_open[60];
char l_launch[60];
char l_prevday[60];
char l_nextday[60];
char l_up[60];
char l_new[60];
char l_delete[60];
char l_rename[60];
char l_copy[60];
char l_paste[60];
char l_more[60];
char l_confirm[60];
char l_filename[60];
char l_calculator[60];
char l_yes[60];
char l_no[60];
char l_showhidden[60];
char l_memory[60];
char l_edit[60];
char l_pageup[60];
char l_pagedown[60];
char l_working[60];
char l_reset[60];
char l_choosepal[60];
char l_todo[60];
char l_title[60];
char l_scribble[60];
char l_saveformat[60];
char l_pause[60];
char l_play[60];
char l_soundmode[60];
char l_oneshot[60];
char l_sequential[60];
char l_nextfile[60];
char l_prevfile[60];
char l_randfile[60];
char l_hold[60];
char l_database[60];
char l_view[60];
char l_hide[60];
char l_done[60];
char l_cancel[60];
char l_date[60];
char l_version[60];
char l_size[60];
char l_namesort[60];
char l_datesort[60];
char l_swap[60];
char l_record[60];
char l_recordingmode[60];
char l_playback[60];
char l_length[60];
char l_stop[60];
char l_irc[60];
char l_send[60];
char l_disconnect[60];
char l_reconnect[60];
char l_launchalt[60];
char l_fatinit[60];
char l_mkdir[60];
char l_cut[60];
char l_defaulthomebrew[60];
char l_proxy[60];
char l_drag[256];
char l_autoconnect[60];
char l_ircnickname[60];
char l_altnickname[60];
char l_server[60];
char l_none[60];
char l_swapab[60];
char l_normalboot[60];
char l_altboot[60];
char l_autobullet[60];
char l_volume[60];
char l_newdir[60];
char l_iconset[60];
char l_settingdesc[60];
char l_loadpic[60];
char l_continue[60];
char l_rendering[60];
char l_tochange[60];
char l_game[60];
char l_webbrowser[60];
char l_forward[60];
char l_homepage[60];
char l_query[60];
char l_append[60];
char l_author[60];
char l_composer[60];
char l_converter[60];
char l_ripper[60];
char l_saving[60];
char l_fixed[60];
char l_variable[60];
char l_editorfont[60];
char l_ircfont[60];
char l_lefthand[60];
char l_righthand[60];
char l_handposition[60];
char l_daily[60];
char l_weekly[60];
char l_monthly[60];
char l_annually[60];
char l_wifimode[60];
char l_firmware[60];
char l_fixedip[60];
char l_autodns[60];
char l_manualdns[60];
char l_scan[60];
char l_wepkey[60];
char l_ip[60];
char l_subnet[60];
char l_gateway[60];
char l_primarydns[60];
char l_secondarydns[60];
char l_autohide[60];
char l_downloaddir[60];
char l_imageorientation[60];
char l_landscape[60];
char l_letter[60];
char l_keyclick[60];
char l_click[60];
char l_silent[60];
char l_exit[60];

char l_title[60];
char l_artist[60];
char l_album[60];
char l_genre[60];
char l_comment[60];
char l_year[60];
char l_track[60];

char l_buffer[60];
char l_status[60];
char l_notconnected[60];
char l_associating[60];
char l_connecting[60];
char l_errorconnecting[60];
char l_streaming[60];
char l_buffering[60];
char l_curSong[60];

char l_changedto[60];
char l_alreadyexists[60];
char l_createddirectory[60];
char l_starteddownload[60];
char l_connectingtohost[60];
char l_deleted[60];
char l_motd[60];
char l_description[60];
char l_retrieving[60];
char l_errorretrieving[60];
char l_gettingpackage[60];
char l_failedpackage[60];
char l_connecting[60];
char l_getmotd[60];
char l_getlist[60];
char l_download[60];
char l_nodbentries[60];
char l_refresh[60];
char l_waitforinput[60];

char l_configuration[60];
char l_addressbook[60];
char l_language[60];
char l_startscreen[60];
char l_timeformat[60];
char l_nameformat[60];
char l_first[60];
char l_last[60];
char l_unknown[60];
char l_htmlmode[60];
char l_secondclick[60];
char l_dateformat[60];
char l_textonly[60];
char l_extended[60];
char l_extendedimages[60];

char l_firstname[60];
char l_lastname[60];
char l_nickname[60];
char l_homephone[60];
char l_workphone[60];
char l_cellphone[60];
char l_address[60];
char l_city[60];
char l_state[60];
char l_zip[60];
char l_email[60];
char l_comments[60];

char l_notodo[60];
char l_novcard[60];
char l_nowifi[60];
char l_nofat[256];
char l_createdir[256];
char l_noentries[256];
char l_rendererror[256];
char l_preview[256];
char l_noscribble[60];
//char l_noid3[60];
char l_nodefaultboot[256];
char l_launchingchishm[256];
char l_chishmnodldi[256];
char l_autopatch[256];
char l_fatalerror[256];
char l_fatalfree[256];
char l_fatalirc[256];
char l_outoffiles[256];

char l_numbers[13];
char l_symbols[13];
char l_lowercase[35];
char l_uppercase[35];
char l_special0[35];
char l_special1[35];

char l_filesize[60];
char l_filetype[60];
char l_ndsfile[60];
char l_txtfile[60];
char l_vcffile[60];
char l_dvpfile[60];
char l_remfile[60];
char l_picfile[60];
char l_unknownfile[60];
char l_directory[60];
char l_binfile[60];
char l_todofile[60];
char l_soundfile[60];
char l_playlistfile[60];
char l_hbfile[60];
char l_pkgfile[60];
char l_shortcutfile[60];

char l_globalsettings[60];
char l_homescreensettings[60];
char l_browsersettings[60];
char l_scribblesettings[60];
char l_todosettings[60];
char l_websettings[60];
char l_ircsettings[60];
char l_editorsettings[60];
char l_wifisettings[60];
char l_imagesettings[60];

// irc
char irc_nicksinchannel[60] = { "* Nicks in channel:" };
char irc_nicklist[60] = { "%c1%s2" };
char irc_cannotpart[60] = { "*** Cannot part non-channel!" };
char irc_noserver[60] = { "*** No server specified!" };
char irc_nochannel[60] = { "*** No channel specified!" };
char irc_invalidchannel[60] = {"*** Not a valid channel!" };
char irc_noaction[60] = { "*** No action specified!" };
char irc_notinchannel[60] = { "*** You are not on a channel!" };
char irc_actionsent[60] = { "* %s1 %s2" };
char irc_invalidnick[60] = { "*** Not a valid nickname!" };
char irc_nomessage[60] = { "*** No message specified!" };
char irc_noticesent[60] = { "-> -%s1- %s2" };
char irc_messagesent[60] = { "-> *%s1* %s2" };
char irc_cannotclose[60] = { "*** Cannot close non-pm window!" };
char irc_noraw[60] = { "*** No raw command specified!" };
char irc_unknown[60] = { "*** Unknown command!" };
char irc_channelsent[60] = { "<%s1> %s2" };
char irc_receivenotice[60] = { "-%s1- %s2" };
char irc_receiveauth[60] = { "%s1" };
char irc_version[60] = { "Version request from %s1" };
char irc_time[60] = { "Time request from %s1" };
char irc_ping[60] = { "Ping request from %s1" };
char irc_actionreceived[60] = { "* %s1 %s2" };
char irc_unknownctcp[60] = { "Unknown CTCP from %s1: %s2" };
char irc_ctcpsent[60] = { "-> [%s1] %s2" };
char irc_ctcpreply[60] = { "[%s1 %s2 reply] %s3" };
char irc_channelreceived[60] = { "<%s1> %s2" };
char irc_join[60] = { "* Joins: %s1" };
char irc_partnomessage[60] = { "* Parts: %s1" };
char irc_partmessage[60] = { "* Parts: %s1 (%s2%c0)" };
char irc_youkicked[60] = { "* You have been kicked from %s1 by %s2 (%s3%c0)" };
char irc_kicked[60] = { "* %s1 was kicked by %s2 (%s3%c0)" };
char irc_quitnomessage[60] = { "* Quits: %s1" };
char irc_quitmessage[60] = { "* Quits: %s1 (%s2%c0)" };
char irc_settopic[60] = { "* %s1 changes topic to: '%s2%c0'" };
char irc_younick[60] = { "* You are now known as %s1" };
char irc_nick[60] = { "* %s1 is now known as %s2" };
char irc_invite[60] = { "%s1 has invited you to %s2" };
char irc_error[60] = { "ERROR: %s1" };
char irc_mode[60] = { "%s1 sets mode: %s2" };
char irc_network[60] = { "You are on the %s1 network." };
char irc_nicklength[60] = { "Max nick length is %d1." };
char irc_supportedmodes[60] = { "Server supports modes %s1 (%s2)." };
char irc_motdstart[60] = { "Message of the Day:" };
char irc_motdend[60] = { "End of MOTD." };
char irc_nomotd[60] = { "No MOTD present." };
char irc_altnickinuse[60] = { "Nick and alternate nick both in use!" };
char irc_nickinuse[60] = { "Nick already in use!" };
char irc_topic[60] = { "* Topic is '%s1%c0'" };
char irc_notopic[60] = { "* No topic set!" };
char irc_nonickgiven[60] = { "No nick given!" };
char irc_erroneousnick[60] = { "Erroneous nick!" };
char irc_serverbanned[60] = { "Cannot connect to server, you are banned!" };
char irc_cantsend[60] = { "Cannot send to channel!" };
char irc_endwhois[60] = { "* End of WHOIS." };
char irc_endwhowas[60] = { "* End of WHOWAS." };
char irc_back[60] = { "You are no longer marked as away." };
char irc_away[60] = { "You are now marked as away." };
char irc_nouser[60] = { "No such user or channel '%s1'" };
char irc_301[60] = { "* %s1 is away: %s2" };
char irc_307[60] = { "* %s1 has identified for this nick" };
char irc_310[60] = { "* %s1 is available for help" };
char irc_311[60] = { "* %s1 is %s2@%s3 * %s4" };
char irc_312[60] = { "* %s1 is on %s2 (%s3)" };
char irc_313[60] = { "* %s1 is an operator" };
char irc_314[60] = { "* %s1 was %s2@%s3 * %s4" };
char irc_319[60] = { "* %s1 is in: %s2" };
char irc_341[60] = { "You have invited %s1 to %s2" };
char irc_403[60] = { "No such channel '%s1'" };
char irc_406[60] = { "There was no such user '%s1'" };
char irc_421[60] = { "Unknown command '%s1'" };
char irc_442[60] = { "You are not on %s1!" };
char irc_443[60] = { "%s1 is already on %s2!" };
char irc_461[60] = { "Not enough parameters for '%s1'" };
char irc_471[60] = { "Cannot join %s1, channel is full!" };
char irc_473[60] = { "Cannot join %s1, invite only!" };
char irc_474[60] = { "Cannot join %s1, banned!" };
char irc_475[60] = { "Cannot join %s1, need correct key!" };
char irc_477[60] = { "Cannot join %s1, requires registered nick!" };
char irc_482[60] = { "You are not a channel operator!" };
char irc_671[60] = { "* %s is using a secure connection" };

// normal irc
char l_ircdisconnect[60];
char l_ircconnectingto[60];
char l_helpavailable[60];
char l_hotspot[60];
char l_ircfailed[60];
char l_irccancelled[60];
char l_irccannotconnect[60];
char l_ircserverclosed[60];
char l_ircon[60];
char l_limit[60];
char l_key[60];
char l_conversation[60];

char curLang[256];

void copyIfMatch(char *curVal, char *matchVal, char *inData, char *outVar, int size)
{
	if(strcmp(matchVal, curVal) == 0)
	{
		strncpy(outVar, inData, size);
	}
}

void setLangString(char *curHeader, char *str, char *data)
{
	// errors
	if(strcmp(curHeader,"Errors") == 0)
	{
		copyIfMatch(str, "NoVCard", data, l_novcard, sizeof(l_novcard));
		copyIfMatch(str, "NoFAT", data, l_nofat, sizeof(l_nofat));
		copyIfMatch(str, "CreateDir", data, l_createdir, sizeof(l_createdir));
		copyIfMatch(str, "NoFiles", data, l_noentries, sizeof(l_noentries));
		copyIfMatch(str, "RenderError", data, l_rendererror, sizeof(l_rendererror));
		copyIfMatch(str, "Preview", data, l_preview, sizeof(l_preview));
		copyIfMatch(str, "NoToDo", data, l_notodo, sizeof(l_notodo));
		copyIfMatch(str, "NoScribble", data, l_noscribble, sizeof(l_noscribble));
		copyIfMatch(str, "NoDefaultBoot", data, l_nodefaultboot, sizeof(l_nodefaultboot));
		copyIfMatch(str, "LaunchChishm", data, l_launchingchishm, sizeof(l_launchingchishm));
		copyIfMatch(str, "NoDLDI", data, l_chishmnodldi, sizeof(l_chishmnodldi));
		copyIfMatch(str, "AutoPatch", data, l_autopatch, sizeof(l_autopatch));
		copyIfMatch(str, "OutOfMemory", data, l_fatalerror, sizeof(l_fatalerror));
		copyIfMatch(str, "DoubleFree", data, l_fatalfree, sizeof(l_fatalfree));
		copyIfMatch(str, "FatalIRC", data, l_fatalirc, sizeof(l_fatalirc));
		copyIfMatch(str, "NoWifi", data, l_nowifi, sizeof(l_nowifi));
		copyIfMatch(str, "OutOfFiles", data, l_outoffiles, sizeof(l_outoffiles));
	}
	
	// date headers
	if(strcmp(curHeader,"Dates") == 0)
	{
		if(strcmp(str,"Sunday") == 0)
		{
			strncpy(l_days[0], data, 29);
			strncpy(l_days[7], data, 3);
			l_days[7][3] = 0;					
		}
		if(strcmp(str,"Monday") == 0)
		{
			strncpy(l_days[1], data, 29);
			strncpy(l_days[8], data, 3);
			l_days[8][3] = 0;					
		}
		if(strcmp(str,"Tuesday") == 0)
		{
			strncpy(l_days[2], data, 29);
			strncpy(l_days[9], data, 3);
			l_days[9][3] = 0;					
		}
		if(strcmp(str,"Wednesday") == 0)
		{
			strncpy(l_days[3], data, 29);
			strncpy(l_days[10], data, 3);
			l_days[10][3] = 0;					
		}
		if(strcmp(str,"Thursday") == 0)
		{
			strncpy(l_days[4], data, 29);
			strncpy(l_days[11], data, 3);
			l_days[11][3] = 0;					
		}
		if(strcmp(str,"Friday") == 0)
		{
			strncpy(l_days[5], data, 29);
			strncpy(l_days[12], data, 3);
			l_days[12][3] = 0;					
		}
		if(strcmp(str,"Saturday") == 0)
		{
			strncpy(l_days[6], data, 29);
			strncpy(l_days[13], data, 3);
			l_days[13][3] = 0;					
		}
		
		if(strcmp(str,"January") == 0)
		{
			strncpy(l_months[0], data, 29);
			strncpy(l_months[12], data, 3);
			l_months[12][3] = 0;					
		}
		if(strcmp(str,"February") == 0)
		{
			strncpy(l_months[1], data, 29);
			strncpy(l_months[13], data, 3);
			l_months[13][3] = 0;					
		}
		if(strcmp(str,"March") == 0)
		{
			strncpy(l_months[2], data, 29);
			strncpy(l_months[14], data, 3);
			l_months[14][3] = 0;					
		}
		if(strcmp(str,"April") == 0)
		{
			strncpy(l_months[3], data, 29);
			strncpy(l_months[15], data, 3);
			l_months[15][3] = 0;					
		}
		if(strcmp(str,"May") == 0)
		{
			strncpy(l_months[4], data, 29);
			strncpy(l_months[16], data, 3);
			l_months[16][3] = 0;					
		}
		if(strcmp(str,"June") == 0)
		{
			strncpy(l_months[5], data, 29);
			strncpy(l_months[17], data, 3);
			l_months[17][3] = 0;					
		}
		if(strcmp(str,"July") == 0)
		{
			strncpy(l_months[6], data, 29);
			strncpy(l_months[18], data, 3);
			l_months[18][3] = 0;					
		}
		if(strcmp(str,"August") == 0)
		{
			strncpy(l_months[7], data, 29);
			strncpy(l_months[19], data, 3);
			l_months[19][3] = 0;					
		}
		if(strcmp(str,"September") == 0)
		{
			strncpy(l_months[8], data, 29);
			strncpy(l_months[20], data, 3);
			l_months[20][3] = 0;					
		}
		if(strcmp(str,"October") == 0)
		{
			strncpy(l_months[9], data, 29);
			strncpy(l_months[21], data, 3);
			l_months[21][3] = 0;					
		}
		if(strcmp(str,"November") == 0)
		{
			strncpy(l_months[10], data, 29);
			strncpy(l_months[22], data, 3);
			l_months[22][3] = 0;					
		}
		if(strcmp(str,"December") == 0)
		{
			strncpy(l_months[11], data, 29);
			strncpy(l_months[23], data, 3);
			l_months[23][3] = 0;					
		}
		
		if(strcmp(str,"AM") == 0)
			strncpy(l_ampm[0], data, 29);
			
		if(strcmp(str,"PM") == 0)
			strncpy(l_ampm[1], data, 29);
	}

	// address language stuff
	if(strcmp(curHeader,"Address") == 0)
	{
		if(strcmp(str,"FirstName") == 0)
			strncpy(l_firstname, data, 59);
		if(strcmp(str,"LastName") == 0)
			strncpy(l_lastname, data, 59);
		if(strcmp(str,"Nickname") == 0)
			strncpy(l_nickname, data, 59);
		if(strcmp(str,"HomePhone") == 0)
			strncpy(l_homephone, data, 59);
		if(strcmp(str,"WorkPhone") == 0)
			strncpy(l_workphone, data, 59);
		if(strcmp(str,"CellPhone") == 0)
			strncpy(l_cellphone, data, 59);
		if(strcmp(str,"Address") == 0)
			strncpy(l_address, data, 59);
		if(strcmp(str,"City") == 0)
			strncpy(l_city, data, 59);
		if(strcmp(str,"State") == 0)
			strncpy(l_state, data, 59);
		if(strcmp(str,"Zip") == 0)
			strncpy(l_zip, data, 59);
		if(strcmp(str,"EMail") == 0)
			strncpy(l_email, data, 59);
		if(strcmp(str,"Comment") == 0)
			strncpy(l_comments, data, 59);
	}

	// general navigation
	if(strcmp(curHeader,"Navigation") == 0)
	{
		if(strcmp(str,"Home") == 0)
			strncpy(l_home, data, 59);
		if(strcmp(str,"Welcome") == 0)
			strncpy(l_welcome, data, 59);
		if(strcmp(str,"Previous") == 0)
			strncpy(l_prev, data, 59);
		if(strcmp(str,"Next") == 0)
			strncpy(l_next, data, 59);
		if(strcmp(str,"EditReminder") == 0)
			strncpy(l_editreminder, data, 59);
		if(strcmp(str,"Reminder") == 0)
			strncpy(l_reminder, data, 59);
		if(strcmp(str,"ViewDay") == 0)
			strncpy(l_viewday, data, 59);
		if(strcmp(str,"DayView") == 0)
			strncpy(l_dayview, data, 59);
		if(strcmp(str,"AddressBook") == 0)
			strncpy(l_addressbook, data, 59);
		if(strcmp(str,"Save") == 0)
			strncpy(l_save, data, 59);
		if(strcmp(str,"Back") == 0)
			strncpy(l_back, data, 59);
		if(strcmp(str,"EditSelected") == 0)
			strncpy(l_editselected, data, 59);
		if(strcmp(str,"Calendar") == 0)
			strncpy(l_calendar, data, 59);
		if(strcmp(str,"Create") == 0)
			strncpy(l_create, data, 59);
		if(strcmp(str,"PreviousField") == 0)
			strncpy(l_prevfield, data, 59);
		if(strcmp(str,"NextField") == 0)
			strncpy(l_nextfield, data, 59);
		if(strcmp(str,"Configuration") == 0)
			strncpy(l_configuration, data, 59);
		if(strcmp(str,"Language") == 0)
			strncpy(l_language, data, 59);
		if(strcmp(str,"StartScreen") == 0)
			strncpy(l_startscreen, data, 59);
		if(strcmp(str,"TimeFormat") == 0)
			strncpy(l_timeformat, data, 59);
		if(strcmp(str,"NameFormat") == 0)
			strncpy(l_nameformat, data, 59);
		if(strcmp(str,"First") == 0)
			strncpy(l_first, data, 59);
		if(strcmp(str,"Last") == 0)
			strncpy(l_last, data, 59);
		if(strcmp(str,"HTMLMode") == 0)
			strncpy(l_htmlmode, data, 59);
		if(strcmp(str,"SecondClick") == 0)
			strncpy(l_secondclick, data, 59);
		if(strcmp(str,"DateFormat") == 0)
			strncpy(l_dateformat, data, 59);						
		if(strcmp(str,"Unknown") == 0)
			strncpy(l_unknown, data, 59);
		if(strcmp(str,"Browser") == 0)
			strncpy(l_browser, data, 59);
		if(strcmp(str,"Open") == 0)
			strncpy(l_open, data, 59);
		if(strcmp(str,"Launch") == 0)
			strncpy(l_launch, data, 59);
		if(strcmp(str,"PrevDay") == 0)
			strncpy(l_prevday, data, 59);
		if(strcmp(str,"NextDay") == 0)
			strncpy(l_nextday, data, 59);
		if(strcmp(str,"Up") == 0)
			strncpy(l_up, data, 59);
		if(strcmp(str,"NewFile") == 0)
			strncpy(l_new, data, 59);
		if(strcmp(str,"Delete") == 0)
			strncpy(l_delete, data, 59);
		if(strcmp(str,"Rename") == 0)
			strncpy(l_rename, data, 59);
		if(strcmp(str,"Copy") == 0)
			strncpy(l_copy, data, 59);
		if(strcmp(str,"Paste") == 0)
			strncpy(l_paste, data, 59);
		if(strcmp(str,"More") == 0)
			strncpy(l_more, data, 59);
		if(strcmp(str,"Confirm") == 0)
			strncpy(l_confirm, data, 59);
		if(strcmp(str,"Filename") == 0)
			strncpy(l_filename, data, 59);						
		if(strcmp(str,"Calculator") == 0)
			strncpy(l_calculator, data, 59);
		if(strcmp(str,"Yes") == 0)
			strncpy(l_yes, data, 59);
		if(strcmp(str,"No") == 0)
			strncpy(l_no, data, 59);
		if(strcmp(str,"ShowHidden") == 0)
			strncpy(l_showhidden, data, 59);
		if(strcmp(str,"Memory") == 0)
			strncpy(l_memory, data, 59);
		if(strcmp(str,"Edit") == 0)
			strncpy(l_edit, data, 59);
		if(strcmp(str,"PageUp") == 0)
			strncpy(l_pageup, data, 59);
		if(strcmp(str,"PageDown") == 0)
			strncpy(l_pagedown, data, 59);
		if(strcmp(str,"Working") == 0)
			strncpy(l_working, data, 59);
		if(strcmp(str,"Reset") == 0)
			strncpy(l_reset, data, 59);
		if(strcmp(str,"ChoosePal") == 0)
			strncpy(l_choosepal, data, 59);
		if(strcmp(str,"ToDo") == 0)
			strncpy(l_todo, data, 59);
		if(strcmp(str,"Title") == 0)
			strncpy(l_title, data, 59);
		if(strcmp(str,"ScribblePad") == 0)
			strncpy(l_scribble, data, 59);
		if(strcmp(str,"SaveFormat") == 0)
			strncpy(l_saveformat, data, 59);
		if(strcmp(str,"Pause") == 0)
			strncpy(l_pause, data, 59);
		if(strcmp(str,"Play") == 0)
			strncpy(l_play, data, 59);						
		if(strcmp(str,"SoundMode") == 0)
			strncpy(l_soundmode, data, 59);
		if(strcmp(str,"OneShot") == 0)
			strncpy(l_oneshot, data, 59);
		if(strcmp(str,"Sequential") == 0)
			strncpy(l_sequential, data, 59);
		if(strcmp(str,"NextFile") == 0)
			strncpy(l_nextfile, data, 59);
		if(strcmp(str,"PrevFile") == 0)
			strncpy(l_prevfile, data, 59);
		if(strcmp(str,"RandFile") == 0)
			strncpy(l_randfile, data, 59);
		if(strcmp(str,"Hold") == 0)
			strncpy(l_hold, data, 59);
		if(strcmp(str,"DataBase") == 0)
			strncpy(l_database, data, 59);
		if(strcmp(str,"View") == 0)
			strncpy(l_view, data, 59);
		if(strcmp(str,"Hide") == 0)
			strncpy(l_hide, data, 59);
		if(strcmp(str,"Done") == 0)
			strncpy(l_done, data, 59);
		if(strcmp(str,"Cancel") == 0)
			strncpy(l_cancel, data, 59);
		if(strcmp(str,"Date") == 0)
			strncpy(l_date, data, 59);
		if(strcmp(str,"Version") == 0)
			strncpy(l_version, data, 59);
		if(strcmp(str,"Size") == 0)
			strncpy(l_size, data, 59);
		if(strcmp(str,"TextOnly") == 0)
			strncpy(l_textonly, data, 59);
		if(strcmp(str,"Extended") == 0)
			strncpy(l_extended, data, 59);
		if(strcmp(str,"ExtendedImages") == 0)
			strncpy(l_extendedimages, data, 59);
		if(strcmp(str,"DateSort") == 0)
			strncpy(l_datesort, data, 59);
		if(strcmp(str,"NameSort") == 0)
			strncpy(l_namesort, data, 59);
		if(strcmp(str,"Swap") == 0)
			strncpy(l_swap, data, 59);
		if(strcmp(str,"Record") == 0)
			strncpy(l_record, data, 59);
		if(strcmp(str,"RecordMode") == 0)
			strncpy(l_recordingmode, data, 59);
		if(strcmp(str,"Playback") == 0)
			strncpy(l_playback, data, 59);
		if(strcmp(str,"Length") == 0)
			strncpy(l_length, data, 59);
		if(strcmp(str,"Stop") == 0)
			strncpy(l_stop, data, 59);
		if(strcmp(str,"IRC") == 0)
			strncpy(l_irc, data, 59);
		if(strcmp(str,"Send") == 0)
			strncpy(l_send, data, 59);
		if(strcmp(str,"Disconnect") == 0)
			strncpy(l_disconnect, data, 59);
		if(strcmp(str,"Reconnect") == 0)
			strncpy(l_reconnect, data, 59);
		if(strcmp(str,"LaunchAlt") == 0)
			strncpy(l_launchalt, data, 59);
		if(strcmp(str,"MKDir") == 0)
			strncpy(l_mkdir, data, 59);
		if(strcmp(str,"Cut") == 0)
			strncpy(l_cut, data, 59);
		if(strcmp(str,"DefaultHomebrew") == 0)
			strncpy(l_defaulthomebrew, data, 59);
		if(strcmp(str,"Proxy") == 0)
			strncpy(l_proxy, data, 59);
		if(strcmp(str,"Drag") == 0)
			strncpy(l_drag, data, 255);
		if(strcmp(str,"AutoConnect") == 0)
			strncpy(l_autoconnect, data, 59);
		if(strcmp(str,"IRCNickname") == 0)
			strncpy(l_ircnickname, data, 59);
		if(strcmp(str,"IRCServer") == 0)
			strncpy(l_server, data, 59);
		if(strcmp(str,"IRCAltNick") == 0)
			strncpy(l_altnickname, data, 59);
		if(strcmp(str,"None") == 0)
			strncpy(l_none, data, 59);
		if(strcmp(str,"SwapAB") == 0)
			strncpy(l_swapab, data, 59);
		if(strcmp(str,"NormalBoot") == 0)
			strncpy(l_normalboot, data, 59);
		if(strcmp(str,"AltBoot") == 0)
			strncpy(l_altboot, data, 59);
		if(strcmp(str,"AutoBullet") == 0)
			strncpy(l_autobullet, data, 59);
		if(strcmp(str,"Volume") == 0)
			strncpy(l_volume, data, 59);
		if(strcmp(str,"NewDir") == 0)
			strncpy(l_newdir, data, 59);
		if(strcmp(str,"IconSet") == 0)
			strncpy(l_iconset, data, 59);
		if(strcmp(str,"SettingDescription") == 0)
			strncpy(l_settingdesc, data, 59);
		if(strcmp(str, "LoadPicture") == 0)
			strncpy(l_loadpic, data, 59);
		if(strcmp(str, "Continue") == 0)
			strncpy(l_continue, data, 59);
		if(strcmp(str, "Rendering") == 0)
			strncpy(l_rendering, data, 59);
		if(strcmp(str, "ToChange") == 0)
			strncpy(l_tochange, data, 59);
		if(strcmp(str, "Game") == 0)
			strncpy(l_game, data, 59);
		if(strcmp(str, "Go") == 0)
			strncpy(l_go, data, 59);
		if(strcmp(str, "WebBrowser") == 0)
			strncpy(l_webbrowser, data, 59);
		if(strcmp(str, "Forward") == 0)
			strncpy(l_forward, data, 59);
		if(strcmp(str, "Homepage") == 0)
			strncpy(l_homepage, data, 59);
		if(strcmp(str, "Query") == 0)
			strncpy(l_query, data, 59);
		if(strcmp(str, "Append") == 0)
			strncpy(l_append, data, 59);
		if(strcmp(str, "Author") == 0)
			strncpy(l_author, data, 59);
		if(strcmp(str, "Composer") == 0)
			strncpy(l_composer, data, 59);
		if(strcmp(str, "Converter") == 0)
			strncpy(l_converter, data, 59);
		if(strcmp(str, "Ripper") == 0)
			strncpy(l_ripper, data, 59);
		if(strcmp(str, "Fixed") == 0)
			strncpy(l_fixed, data, 59);
		if(strcmp(str, "Variable") == 0)
			strncpy(l_variable, data, 59);
		if(strcmp(str, "EditorFont") == 0)
			strncpy(l_editorfont, data, 59);
		if(strcmp(str, "IRCFont") == 0)
			strncpy(l_ircfont, data, 59);
		if(strcmp(str, "HandPosition") == 0)
			strncpy(l_handposition, data, 59);
		if(strcmp(str, "LeftHand") == 0)
			strncpy(l_lefthand, data, 59);
		if(strcmp(str, "RightHand") == 0)
			strncpy(l_righthand, data, 59);
		if(strcmp(str, "Daily") == 0)
			strncpy(l_daily, data, 59);
		if(strcmp(str, "Weekly") == 0)
			strncpy(l_weekly, data, 59);
		if(strcmp(str, "Monthly") == 0)
			strncpy(l_monthly, data, 59);
		if(strcmp(str, "Annually") == 0)
			strncpy(l_annually, data, 59);
		if(strcmp(str, "WifiMode") == 0)
			strncpy(l_wifimode, data, 59);
		if(strcmp(str, "Firmware") == 0)
			strncpy(l_firmware, data, 59);
		if(strcmp(str, "FixedIP") == 0)
			strncpy(l_fixedip, data, 59);
		if(strcmp(str, "AutoDNS") == 0)
			strncpy(l_autodns, data, 59);
		if(strcmp(str, "ManualDNS") == 0)
			strncpy(l_manualdns, data, 59);
		if(strcmp(str, "Scan") == 0)
			strncpy(l_scan, data, 59);
		if(strcmp(str, "WEPKey") == 0)
			strncpy(l_wepkey, data, 59);
		if(strcmp(str, "IP") == 0)
			strncpy(l_ip, data, 59);
		if(strcmp(str, "Subnet") == 0)
			strncpy(l_subnet, data, 59);
		if(strcmp(str, "Gateway") == 0)
			strncpy(l_gateway, data, 59);
		if(strcmp(str, "PrimaryDNS") == 0)
			strncpy(l_primarydns, data, 59);
		if(strcmp(str, "SecondaryDNS") == 0)
			strncpy(l_secondarydns, data, 59);
		if(strcmp(str, "AutoHide") == 0)
			strncpy(l_autohide, data, 59);
		if(strcmp(str, "DownloadDir") == 0)
			strncpy(l_downloaddir, data, 59);
		if(strcmp(str, "DisplayOrientation") == 0)
			strncpy(l_imageorientation, data, 59);
		if(strcmp(str, "Landscape") == 0)
			strncpy(l_landscape, data, 59);
		if(strcmp(str, "Letter") == 0)
			strncpy(l_letter, data, 59);
		if(strcmp(str, "KeySound") == 0)
			strncpy(l_keyclick, data, 59);
		if(strcmp(str, "ClickNoise") == 0)
			strncpy(l_click, data, 59);
		if(strcmp(str, "Silent") == 0)
			strncpy(l_silent, data, 59);
		if(strcmp(str, "SavePicture") == 0)
			strncpy(l_saving, data, 59);
	}

	// regional keyboard stuff
	if(strcmp(curHeader,"Regional") == 0)
	{
		if(strcmp(str,"Numbers") == 0)
			strncpy(l_numbers, data, 12);
		if(strcmp(str,"Symbols") == 0)
			strncpy(l_symbols, data, 12);
		if(strcmp(str,"Lowercase") == 0)
			strncpy(l_lowercase, data, 34);
		if(strcmp(str,"Uppercase") == 0)
			strncpy(l_uppercase, data, 34);
		if(strcmp(str,"Special1") == 0)
			strncpy(l_special0, data, 34);
		if(strcmp(str,"Special2") == 0)
			strncpy(l_special1, data, 34);
	}

	// browser stuff
	if(strcmp(curHeader,"Browser") == 0)
	{
		if(strcmp(str,"FileSize") == 0)
			strncpy(l_filesize, data, 59);
		if(strcmp(str,"FileType") == 0)
			strncpy(l_filetype, data, 59);
		if(strcmp(str,"NDSFile") == 0)
			strncpy(l_ndsfile, data, 59);
		if(strcmp(str,"TXTFile") == 0)
			strncpy(l_txtfile, data, 59);
		if(strcmp(str,"VCFFile") == 0)
			strncpy(l_vcffile, data, 59);
		if(strcmp(str,"DPLFile") == 0)
			strncpy(l_dvpfile, data, 59);
		if(strcmp(str,"REMFile") == 0)
			strncpy(l_remfile, data, 59);
		if(strcmp(str,"PicFile") == 0)
			strncpy(l_picfile, data, 59);
		if(strcmp(str,"Directory") == 0)
			strncpy(l_directory, data, 59);
		if(strcmp(str,"Unknown") == 0)
			strncpy(l_unknownfile, data, 59);
		if(strcmp(str,"BINFile") == 0)
			strncpy(l_binfile, data, 59);
		if(strcmp(str,"TodoFile") == 0)
			strncpy(l_todofile, data, 59);
		if(strcmp(str,"SoundFile") == 0)
			strncpy(l_soundfile, data, 59);
		if(strcmp(str,"PlayListFile") == 0)
			strncpy(l_playlistfile, data, 59);
		if(strcmp(str,"HBFile") == 0)
			strncpy(l_hbfile, data, 59);
		if(strcmp(str,"PKGFile") == 0)
			strncpy(l_pkgfile, data, 59);
		if(strcmp(str,"ShortcutFile") == 0)
			strncpy(l_shortcutfile, data, 59);
	}

	// id3 stuff
	if(strcmp(curHeader,"Id3") == 0)
	{
		if(strcmp(str,"Title") == 0)
			strncpy(l_title, data, 59);
		if(strcmp(str,"Artist") == 0)
			strncpy(l_artist, data, 59);
		if(strcmp(str,"Album") == 0)
			strncpy(l_album, data, 59);
		if(strcmp(str,"Genre") == 0)
			strncpy(l_genre, data, 59);
		if(strcmp(str,"Comment") == 0)
			strncpy(l_comment, data, 59);
		if(strcmp(str,"Year") == 0)
			strncpy(l_year, data, 59);
		if(strcmp(str,"Track") == 0)
			strncpy(l_track, data, 59);
	}

	// streaming stuff
	if(strcmp(curHeader,"Streaming") == 0)
	{
		if(strcmp(str,"Buffer") == 0)
			strncpy(l_buffer, data, 59);
		if(strcmp(str,"Status") == 0)
			strncpy(l_status, data, 59);
		if(strcmp(str,"NotConnected") == 0)
			strncpy(l_notconnected, data, 59);
		if(strcmp(str,"Associating") == 0)
			strncpy(l_associating, data, 59);
		if(strcmp(str,"Connecting") == 0)
			strncpy(l_connecting, data, 59);
		if(strcmp(str,"ErrorConnecting") == 0)
			strncpy(l_errorconnecting, data, 59);
		if(strcmp(str,"Streaming") == 0)
			strncpy(l_streaming, data, 59);
		if(strcmp(str,"Buffering") == 0)
			strncpy(l_buffering, data, 59);
		if(strcmp(str,"Playing") == 0)
			strncpy(l_curSong, data, 59);
	}

	// database stuff
	if(strcmp(curHeader,"Database") == 0)
	{
		if(strcmp(str,"ChangedDirectory") == 0)
			strncpy(l_changedto, data, 59);
		if(strcmp(str,"AlreadyExists") == 0)
			strncpy(l_alreadyexists, data, 59);
		if(strcmp(str,"CreatedDirectory") == 0)
			strncpy(l_createddirectory, data, 59);
		if(strcmp(str,"StartedDownload") == 0)
			strncpy(l_starteddownload, data, 59);
		if(strcmp(str,"ConnectingHost") == 0)
			strncpy(l_connectingtohost, data, 59);
		if(strcmp(str,"Deleted") == 0)
			strncpy(l_deleted, data, 59);
		if(strcmp(str,"MOTD") == 0)
			strncpy(l_motd, data, 59);
		if(strcmp(str,"Description") == 0)
			strncpy(l_description, data, 59);
		if(strcmp(str,"Retrieving") == 0)
			strncpy(l_retrieving, data, 59);
		if(strcmp(str,"ErrorRetrieving") == 0)
			strncpy(l_errorretrieving, data, 59);
		if(strcmp(str,"GettingPackage") == 0)
			strncpy(l_gettingpackage, data, 59);
		if(strcmp(str,"FailedPackage") == 0)
			strncpy(l_failedpackage, data, 59);
		if(strcmp(str,"Connecting") == 0)
			strncpy(l_connecting, data, 59);
		if(strcmp(str,"GetMOTD") == 0)
			strncpy(l_getmotd, data, 59);
		if(strcmp(str,"GetList") == 0)
			strncpy(l_getlist, data, 59);
		if(strcmp(str,"Download") == 0)
			strncpy(l_download, data, 59);
		if(strcmp(str,"NoDBEntries") == 0)
			strncpy(l_nodbentries, data, 59);
		if(strcmp(str,"Refresh") == 0)
			strncpy(l_refresh, data, 59);
		if(strcmp(str,"Input") == 0)
			strncpy(l_waitforinput, data, 59);
		if(strcmp(str,"Exit") == 0)
			strncpy(l_exit, data, 59);
	}

	// irc stuff
	if(strcmp(curHeader,"IRC") == 0)
	{
		if(strcmp(str,"Disconnected") == 0)
			strncpy(l_ircdisconnect, data, 59);					
		if(strcmp(str,"Connecting") == 0)
			strncpy(l_ircconnectingto, data, 59);
		if(strcmp(str,"HelpAvailable") == 0)
			strncpy(l_helpavailable, data, 59);
		if(strcmp(str,"Connected") == 0)
			strncpy(l_hotspot, data, 59);
		if(strcmp(str,"FailedConnect") == 0)
			strncpy(l_ircfailed, data, 59);
		if(strcmp(str,"CancelledConnect") == 0)
			strncpy(l_irccancelled, data, 59);
		if(strcmp(str,"CannotConnect") == 0)
			strncpy(l_irccannotconnect, data, 59);
		if(strcmp(str,"ServerClosed") == 0)
			strncpy(l_ircserverclosed, data, 59);
		if(strcmp(str,"On") == 0)
			strncpy(l_ircon, data, 59);
		if(strcmp(str,"Limit") == 0)
			strncpy(l_limit, data, 59);
		if(strcmp(str,"Key") == 0)
			strncpy(l_key, data, 59);
		if(strcmp(str,"ConversationWith") == 0)
			strncpy(l_conversation, data, 59);
	}
	
	// settings stuff
	if(strcmp(curHeader,"Settings") == 0)
	{
		if(strcmp(str,"Global") == 0)
			strncpy(l_globalsettings, data, 59);
		if(strcmp(str,"Home") == 0)
			strncpy(l_homescreensettings, data, 59);
		if(strcmp(str,"Browser") == 0)
			strncpy(l_browsersettings, data, 59);
		if(strcmp(str,"Scribble") == 0)
			strncpy(l_scribblesettings, data, 59);
		if(strcmp(str,"Todo") == 0)
			strncpy(l_todosettings, data, 59);
		if(strcmp(str,"Web") == 0)
			strncpy(l_websettings, data, 59);
		if(strcmp(str,"IRC") == 0)
			strncpy(l_ircsettings, data, 59);
		if(strcmp(str,"Editor") == 0)
			strncpy(l_editorsettings, data, 59);
		if(strcmp(str,"Wifi") == 0)
			strncpy(l_wifisettings, data, 59);
		if(strcmp(str,"Image") == 0)
			strncpy(l_imagesettings, data, 59);
	}
}

void setIRCString(char *curHeader, char *str, char *data)
{
	if(strcmp(curHeader,"ErrorStrings") == 0)
	{
		if(strcmp(str,"InvalidNick") == 0)
			strcpy(irc_invalidnick, data);
		if(strcmp(str,"InvalidChannel") == 0)
			strcpy(irc_invalidchannel, data);
		if(strcmp(str,"CannotPart") == 0)
			strcpy(irc_cannotpart, data);
		if(strcmp(str,"CannotClose") == 0)
			strcpy(irc_cannotclose, data);
		if(strcmp(str,"NoServer") == 0)
			strcpy(irc_noserver, data);						
		if(strcmp(str,"NoChannel") == 0)
			strcpy(irc_nochannel, data);						
		if(strcmp(str,"NoAction") == 0)
			strcpy(irc_noaction, data);						
		if(strcmp(str,"NoMessage") == 0)
			strcpy(irc_nomessage, data);						
		if(strcmp(str,"NoRaw") == 0)
			strcpy(irc_noraw, data);						
		if(strcmp(str,"NotInChannel") == 0)
			strcpy(irc_notinchannel, data);
		if(strcmp(str,"UnknownCommand") == 0)
			strcpy(irc_unknown, data);
	}
	
	if(strcmp(curHeader,"LangStrings") == 0)
	{					
		if(strcmp(str,"MOTDStart") == 0)
			strcpy(irc_motdstart, data);						
		if(strcmp(str,"MOTDEnd") == 0)
			strcpy(irc_motdend, data);						
		if(strcmp(str,"NoMotd") == 0)
			strcpy(irc_nomotd, data);					
		if(strcmp(str,"NoNickGiven") == 0)
			strcpy(irc_nonickgiven, data);						
		if(strcmp(str,"ErroneousNick") == 0)
			strcpy(irc_erroneousnick, data);						
		if(strcmp(str,"NickInUse") == 0)
			strcpy(irc_nickinuse, data);						
		if(strcmp(str,"AltNickInUse") == 0)
			strcpy(irc_altnickinuse, data);						
		if(strcmp(str,"BannedFromServer") == 0)
			strcpy(irc_serverbanned, data);						
		if(strcmp(str,"CannotSend") == 0)
			strcpy(irc_cantsend, data);					
		if(strcmp(str,"Away") == 0)
			strcpy(irc_away, data);						
		if(strcmp(str,"Back") == 0)
			strcpy(irc_back, data);				
	}
	
	if(strcmp(curHeader,"Numeric") == 0)
	{
		if(strcmp(str,"301") == 0)
			strcpy(irc_301, data);	
		if(strcmp(str,"307") == 0)
			strcpy(irc_307, data);	
		if(strcmp(str,"310") == 0)
			strcpy(irc_310, data);	
		if(strcmp(str,"311") == 0)
			strcpy(irc_311, data);	
		if(strcmp(str,"312") == 0)
			strcpy(irc_312, data);	
		if(strcmp(str,"313") == 0)
			strcpy(irc_313, data);	
		if(strcmp(str,"314") == 0)
			strcpy(irc_314, data);	
		if(strcmp(str,"319") == 0)
			strcpy(irc_319, data);	
		if(strcmp(str,"341") == 0)
			strcpy(irc_341, data);
		if(strcmp(str,"401") == 0)
			strcpy(irc_nouser, data);
		if(strcmp(str,"403") == 0)
			strcpy(irc_403, data);
		if(strcmp(str,"406") == 0)
			strcpy(irc_406, data);
		if(strcmp(str,"421") == 0)
			strcpy(irc_421, data);	
		if(strcmp(str,"442") == 0)
			strcpy(irc_442, data);	
		if(strcmp(str,"443") == 0)
			strcpy(irc_443, data);	
		if(strcmp(str,"461") == 0)
			strcpy(irc_461, data);
		if(strcmp(str,"471") == 0)
			strcpy(irc_471, data);	
		if(strcmp(str,"473") == 0)
			strcpy(irc_473, data);	
		if(strcmp(str,"474") == 0)
			strcpy(irc_474, data);	
		if(strcmp(str,"475") == 0)
			strcpy(irc_475, data);	
		if(strcmp(str,"477") == 0)
			strcpy(irc_477, data);	
		if(strcmp(str,"482") == 0)
			strcpy(irc_482, data);
		if(strcmp(str,"671") == 0)
			strcpy(irc_671, data);	
	}
	
	if(strcmp(curHeader,"Customizable") == 0)
	{
		if(strcmp(str,"VersionReceived") == 0)
			strcpy(irc_version, data);	
		if(strcmp(str,"TimeReceived") == 0)
			strcpy(irc_time, data);	
		if(strcmp(str,"PingReceived") == 0)
			strcpy(irc_ping, data);	
		if(strcmp(str,"UnknownCTCP") == 0)
			strcpy(irc_unknownctcp, data);
		if(strcmp(str,"CTCPSent") == 0)
			strcpy(irc_ctcpsent, data);
		if(strcmp(str,"CTCPReply") == 0)
			strcpy(irc_ctcpreply, data);
		if(strcmp(str,"Network") == 0)
			strcpy(irc_network, data);	
		if(strcmp(str,"NickLength") == 0)
			strcpy(irc_nicklength, data);	
		if(strcmp(str,"SupportedModes") == 0)
			strcpy(irc_supportedmodes, data);	
		if(strcmp(str,"NickList") == 0)
			strcpy(irc_nicklist, data);	
		if(strcmp(str,"Join") == 0)
			strcpy(irc_join, data);	
		if(strcmp(str,"Part") == 0)
			strcpy(irc_partnomessage, data);	
		if(strcmp(str,"PartMessage") == 0)
			strcpy(irc_partmessage, data);	
		if(strcmp(str,"Quit") == 0)
			strcpy(irc_quitnomessage, data);	
		if(strcmp(str,"QuitMessage") == 0)
			strcpy(irc_quitmessage, data);	
		if(strcmp(str,"ActionSent") == 0)
			strcpy(irc_actionsent, data);	
		if(strcmp(str,"ActionReceived") == 0)
			strcpy(irc_actionreceived, data);	
		if(strcmp(str,"NoticeSent") == 0)
			strcpy(irc_noticesent, data);	
		if(strcmp(str,"NoticeReceived") == 0)
			strcpy(irc_receivenotice, data);	
		if(strcmp(str,"AuthReceived") == 0)
			strcpy(irc_receiveauth, data);	
		if(strcmp(str,"MessageSent") == 0)
			strcpy(irc_messagesent, data);	
		if(strcmp(str,"ChannelSent") == 0)
			strcpy(irc_channelsent, data);	
		if(strcmp(str,"ChannelReceived") == 0)
			strcpy(irc_channelreceived, data);	
		if(strcmp(str,"KickSelf") == 0)
			strcpy(irc_youkicked, data);	
		if(strcmp(str,"Kick") == 0)
			strcpy(irc_kicked, data);	
		if(strcmp(str,"NickSelf") == 0)
			strcpy(irc_younick, data);	
		if(strcmp(str,"Nick") == 0)
			strcpy(irc_nick, data);	
		if(strcmp(str,"NicksInChannel") == 0)
			strcpy(irc_nicksinchannel, data);	
		if(strcmp(str,"NoTopic") == 0)
			strcpy(irc_notopic, data);	
		if(strcmp(str,"Topic") == 0)
			strcpy(irc_topic, data);	
		if(strcmp(str,"SetTopic") == 0)
			strcpy(irc_settopic, data);	
		if(strcmp(str,"SetMode") == 0)
			strcpy(irc_mode, data);	
		if(strcmp(str,"InviteReceived") == 0)
			strcpy(irc_invite, data);	
		if(strcmp(str,"Error") == 0)
			strcpy(irc_error, data);	
		if(strcmp(str,"WhoisEnd") == 0)
			strcpy(irc_endwhois, data);	
		if(strcmp(str,"WhowasEnd") == 0)
			strcpy(irc_endwhowas, data);	
	}
}

void loadConfig(char *fName, void (*dataProcess)(char *, char *, char *))
{
	char *str = (char *)safeMalloc(LANGUAGE_READ_SIZE + 1);
	char data[256];
	char curHeader[64];
	int x = 0;
	
	DRAGON_chdir("/");
	DRAGON_FILE *fFile = DRAGON_fopen(fName, "r");
	
	memset(curHeader, 0, 64);
	
	while(!DRAGON_feof(fFile))
	{
		memset(str, 0, LANGUAGE_READ_SIZE + 1);		
		DRAGON_fgets(str, LANGUAGE_READ_SIZE, fFile);
		
		switch(str[0])
		{
			case '[':
				//section header
				x = 0;
				while(str[x + 1] != ']')
				{
					if(x < 63)
					{
						curHeader[x] = str[x + 1];					
						x++;
					}
				}
				
				curHeader[x] = 0;
				
				break;
			case ';':
				//comment
				
				//no need to handle anything here
				
				break;
			case 13: // skip nothingness
			case 10:
			case 0:
				break;
			default:
				if(strlen(str) == 0)
					break;
				
				//setting name
				memset(data, 0, 256);
				
				x = 0;
				while(str[x] != '=' && str[x] != 0)
					x++;
				
				if(str[x] == 0)
					break;
				
				str[x] = 0;
				
				x++;
				uint16 y = 0;
				while(str[x] != 0)
				{
					if(y < 255)
					{
						data[y] = str[x];
						y++;
					}
					
					x++;
				}
				
				data[y] = 0;
				
				dataProcess(curHeader, str, data);
				break;
		}				
	}
	
	DRAGON_fclose(fFile);
	free(str);
}

void loadIRCConfig()
{
	char fName[256];
	
	sprintf(fName, "%sIRC.ini", d_base);
	
	DRAGON_chdir("/");
	if(DRAGON_FileExists(fName) != FE_FILE)
		return;
	
	loadConfig(fName, setIRCString);
}

void initLanguage()
{
	// set days up
	strcpy(l_days[0], "Sunday");
	strcpy(l_days[1], "Monday");
	strcpy(l_days[2], "Tuesday");
	strcpy(l_days[3], "Wednesday");
	strcpy(l_days[4], "Thursday");
	strcpy(l_days[5], "Friday");
	strcpy(l_days[6], "Saturday");
	strcpy(l_days[7], "Sun");
	strcpy(l_days[8], "Mon");
	strcpy(l_days[9], "Tue");
	strcpy(l_days[10], "Wed");
	strcpy(l_days[11], "Thu");
	strcpy(l_days[12], "Fri");
	strcpy(l_days[13], "Sat");
	
	// set months up
	strcpy(l_months[0], "January");
	strcpy(l_months[1], "February");
	strcpy(l_months[2], "March");
	strcpy(l_months[3], "April");
	strcpy(l_months[4], "May");
	strcpy(l_months[5], "June");
	strcpy(l_months[6], "July");
	strcpy(l_months[7], "August");
	strcpy(l_months[8], "September");
	strcpy(l_months[9], "October");
	strcpy(l_months[10], "November");
	strcpy(l_months[11], "December");
	strcpy(l_months[12], "Jan");
	strcpy(l_months[13], "Feb");
	strcpy(l_months[14], "Mar");
	strcpy(l_months[15], "Apr");
	strcpy(l_months[16], "May");
	strcpy(l_months[17], "Jun");
	strcpy(l_months[18], "Jul");
	strcpy(l_months[19], "Aug");
	strcpy(l_months[20], "Sep");
	strcpy(l_months[21], "Oct");
	strcpy(l_months[22], "Nov");
	strcpy(l_months[23], "Dec");
	
	// set am/pm up
	strcpy(l_ampm[0], "AM");
	strcpy(l_ampm[1], "PM");
	
	// set up navigation stuff
	strcpy(l_home, "Home");
	strcpy(l_welcome, "Welcome");
	strcpy(l_prev, "Prev");
	strcpy(l_next, "Next");
	strcpy(l_reminder, "Reminder");
	strcpy(l_editreminder, "Edit Reminder");
	strcpy(l_viewday, "View Day");
	strcpy(l_dayview, "Day Planner");
	strcpy(l_save, "Save");
	strcpy(l_back, "Back");
	strcpy(l_editselected, "Edit Selected");
	strcpy(l_calendar, "Calendar");
	strcpy(l_create, "Create");
	strcpy(l_prevfield, "Prev Field");
	strcpy(l_nextfield, "Next Field");
	strcpy(l_configuration, "Configuration");
	strcpy(l_addressbook, "Address Book");
	strcpy(l_unknown, "Unknown");
	strcpy(l_browser, "Browser");
	strcpy(l_open, "Open");
	strcpy(l_launch, "Launch");
	strcpy(l_prevday, "Prev Day");
	strcpy(l_nextday, "Next Day");
	strcpy(l_up, "Up");
	strcpy(l_new, "New File");
	strcpy(l_delete, "Delete");
	strcpy(l_rename, "Rename");
	strcpy(l_copy, "Copy");
	strcpy(l_paste, "Paste");
	strcpy(l_more, "More");
	strcpy(l_confirm, "Confirm");
	strcpy(l_filename, "Filename");
	strcpy(l_calculator, "Calculator");
	strcpy(l_yes, "Yes");
	strcpy(l_no, "No");
	strcpy(l_showhidden, "Show Hidden Files");
	strcpy(l_memory, "Memory");
	strcpy(l_edit, "Edit");
	strcpy(l_pageup, "Page Up");
	strcpy(l_pagedown, "Page Down");
	strcpy(l_working, "Working");
	strcpy(l_reset, "Reset");
	strcpy(l_choosepal, "Choose Pallate File");
	strcpy(l_todo, "Todo List");
	strcpy(l_title, "Title");
	strcpy(l_scribble, "Scribble Pad");	
	strcpy(l_saveformat, "Scribble Save Format");	
	strcpy(l_pause, "Pause");
	strcpy(l_play, "Play");
	strcpy(l_soundmode, "Playback Mode");
	strcpy(l_oneshot, "Single File");
	strcpy(l_sequential, "Sequential");
	strcpy(l_nextfile, "Next File");
	strcpy(l_prevfile, "Prev File");
	strcpy(l_randfile, "Random");
	strcpy(l_hold, "Hold");
	strcpy(l_database, "Homebrew Database");
	strcpy(l_view, "View");
	strcpy(l_hide, "Toggle Hidden");
	strcpy(l_done, "Done");
	strcpy(l_cancel, "Cancel");
	strcpy(l_date, "Date");
	strcpy(l_version, "Version");
	strcpy(l_size, "Size");
	strcpy(l_datesort, "Sort by Date");
	strcpy(l_namesort, "Sort by Name");
	strcpy(l_swap, "Swap");
	strcpy(l_record, "Record");
	strcpy(l_recordingmode, "Recording Mode");
	strcpy(l_playback, "Playback");
	strcpy(l_length, "Length");
	strcpy(l_stop, "Stop");
	strcpy(l_irc, "IRC");
	strcpy(l_send, "Send");
	strcpy(l_disconnect, "D/C");
	strcpy(l_reconnect, "Reconnect");
	strcpy(l_launchalt, "Launch Alternate");
	strcpy(l_mkdir, "New Dir");
	strcpy(l_cut, "Cut");
	strcpy(l_defaulthomebrew, "Default Homebrew Directory");
	strcpy(l_proxy, "HTTP Proxy");
	strcpy(l_drag, "Drag the icons with your stylus to arrange the home screen as desired.");
	strcpy(l_autoconnect, "Autoconnect to IRC Server");
	strcpy(l_ircnickname, "IRC Nickname");
	strcpy(l_altnickname, "IRC Alternate Nickname");
	strcpy(l_server, "IRC Server");
	strcpy(l_none, "None");	
	strcpy(l_swapab, "Swap A/B Buttons");
	strcpy(l_normalboot, "Default Boot Method");
	strcpy(l_altboot, "Alternate Boot Method");
	strcpy(l_autobullet, "Auto Bullets in Todo");
	strcpy(l_volume, "Volume");
	strcpy(l_newdir, "New Directory");
	strcpy(l_iconset, "Icon Set");
	strcpy(l_settingdesc, "Setting Description");
	strcpy(l_continue, "Continue");
	strcpy(l_loadpic, "Loading picture...");
	strcpy(l_rendering, "Rendering...");
	strcpy(l_tochange, "to change");
	strcpy(l_game, "Game");
	strcpy(l_go, "Go");
	strcpy(l_webbrowser, "Web Browser");
	strcpy(l_forward, "Forward");
	strcpy(l_homepage, "Home Page");
	strcpy(l_query, "Query");
	strcpy(l_append, "Append");
	strcpy(l_author, "Author");
	strcpy(l_composer, "Composer");
	strcpy(l_converter, "Converter");
	strcpy(l_ripper, "Ripper");
	strcpy(l_fixed, "Fixed Width");
	strcpy(l_variable, "Variable Width");
	strcpy(l_editorfont, "Editor Font");
	strcpy(l_ircfont, "IRC Font");
	strcpy(l_lefthand, "Left Handed");
	strcpy(l_righthand, "Right Handed");
	strcpy(l_handposition, "Hand Position");
	strcpy(l_daily, "Once");
	strcpy(l_weekly, "Weekly");
	strcpy(l_monthly, "Monthly");
	strcpy(l_annually, "Annually");
	strcpy(l_wifimode, "Wifi Mode");
	strcpy(l_firmware, "Firmware");
	strcpy(l_fixedip, "Fixed IP");
	strcpy(l_autodns, "Auto DNS");
	strcpy(l_manualdns, "Manual DNS");
	strcpy(l_scan, "Scan");
	strcpy(l_wepkey, "WEP Key");
	strcpy(l_ip, "IP Address");
	strcpy(l_subnet, "Subnet");
	strcpy(l_gateway, "Gateway");
	strcpy(l_primarydns, "Primary DNS");
	strcpy(l_secondarydns, "Secondary DNS");
	strcpy(l_autohide, "Auto Hide On Navigate");
	strcpy(l_downloaddir, "Download Directory");
	strcpy(l_imageorientation, "Display Orientation");
	strcpy(l_landscape, "Landscape");
	strcpy(l_letter, "Letter");
	strcpy(l_keyclick, "Keyboard Sound");
	strcpy(l_click, "Click Noise");
	strcpy(l_silent, "Silent");
	strcpy(l_saving, "Saving picture...");
	
	// set up id3 stuff
	strcpy(l_title, "Title");
	strcpy(l_artist, "Artist");
	strcpy(l_album, "Album");
	strcpy(l_genre, "Genre");
	strcpy(l_comment, "Comment");
	strcpy(l_year, "Year");
	strcpy(l_track, "Track No");
	
	// set up streaming stuff
	strcpy(l_buffer, "Buffer");
	strcpy(l_status, "Status");
	strcpy(l_notconnected, "Not connected.");
	strcpy(l_associating, "Associating...");
	strcpy(l_connecting, "Connecting to stream...");
	strcpy(l_errorconnecting, "There was an error connecting!");
	strcpy(l_streaming, "Streaming!");
	strcpy(l_buffering, "Buffering stream...");
	strcpy(l_curSong, "Playing");
	strcpy(l_exit, "Exit");

	// set up database stuff
	strcpy(l_changedto, "Changed to directory");
	strcpy(l_alreadyexists, "already exists");
	strcpy(l_createddirectory, "Created directory");
	strcpy(l_starteddownload, "Started download from");
	strcpy(l_connectingtohost, "Connecting to host...");
	strcpy(l_deleted, "Deleted");
	strcpy(l_motd, "MOTD");
	strcpy(l_description, "Description");
	strcpy(l_retrieving, "Retrieving...");
	strcpy(l_errorretrieving, "Unable to retrieve.");
	strcpy(l_gettingpackage, "Getting package information.");
	strcpy(l_failedpackage, "Failed downloading package contents.");
	strcpy(l_connecting, "Connecting...");
	strcpy(l_getmotd, "Getting MOTD...");
	strcpy(l_getlist, "Getting list...");
	strcpy(l_download,"Download");
	strcpy(l_nodbentries, "No entries in database!");
	strcpy(l_refresh, "Refresh List");
	strcpy(l_waitforinput, "Waiting for input");
	
	// set up addressbook stuff
	strcpy(l_firstname, "First Name");
	strcpy(l_lastname, "Last Name");
	strcpy(l_nickname, "Nickname");
	strcpy(l_homephone, "Home Phone");
	strcpy(l_workphone, "Work Phone");
	strcpy(l_cellphone, "Cell Phone");
	strcpy(l_address, "Address");
	strcpy(l_city, "City");
	strcpy(l_state, "State");
	strcpy(l_zip, "Zip Code");
	strcpy(l_email, "E-Mail");
	strcpy(l_comments, "Comments");
	
	// set up configuration stuff	
	strcpy(l_language, "Language");
	strcpy(l_startscreen, "Startup Screen");
	strcpy(l_timeformat, "Time Format");
	strcpy(l_nameformat, "Name Format");
	strcpy(l_first, "First");
	strcpy(l_last, "Last");
	strcpy(l_htmlmode, "HTML Mode");
	strcpy(l_secondclick, "Action on Second Click");
	strcpy(l_dateformat, "Date Format");
	strcpy(l_textonly, "Text Only");
	strcpy(l_extended, "Extended");
	strcpy(l_extendedimages, "Extended with Images");
	
	// set up errors	
	strcpy(l_fatinit, "Initializing DSOrganize");
	strcpy(l_novcard, "No vCard files found...");
	strcpy(l_nofat, "You are not using a card that is compatible with Chishm's FAT library.  Please make sure you have properly DLDI patched DSOrganize!");
	strcpy(l_createdir, "The 'DSOrganize' directory was not found!  It has been created for you, along with default settings.");
	strcpy(l_noentries, "No files in this directory...");
	strcpy(l_rendererror, "An error occured while rendering the file.");
	strcpy(l_preview, "Preview mode not available because the image is too small.");
	strcpy(l_notodo, "No todo entries found...");
	strcpy(l_nowifi, "No access points found...");
	strcpy(l_noscribble, "No scribble creations found...");
	//strcpy(l_noid3, "ID3 v1.1 data not present!");
	strcpy(l_nodefaultboot, "There is no built in bootloader for your card.  Please configure the bootloaders in the configuration screen.");
	strcpy(l_launchingchishm, "Attempting to launch your file with the Chishm loader.");
	strcpy(l_chishmnodldi, "You haven't patched DSOrganize with any DLDI file.  The Chishm loader requires you to have a DLDI patched DSOrganize or a dldi file in your resources directory.");
	strcpy(l_autopatch, "Attempting to patch with DLDI...");
	strcpy(l_fatalerror, "Fatal Error!  DSOrganize has run out of memory.");
	strcpy(l_fatalfree, "Fatal Error!  DSOrganize has failed to free a block of memory.");
	strcpy(l_fatalirc, "Fatal Error!  There is no room in the command buffer for new commands.");
	strcpy(l_outoffiles, "Fatal Error!  DSOrganize has run out of free file handles.");
	
	// set up regional keyboard
	strcpy(l_numbers, "1234567890-=");
	strcpy(l_symbols, "!@#$%^&*()_+");
	strcpy(l_lowercase, "qwertyuiopasdfghjklzxcvbnm,./[];'`");
	strcpy(l_uppercase, "QWERTYUIOPASDFGHJKLZXCVBNM<>?{}:\"~");
	strcpy(l_special0,  "‡·‚„‰ÂËÈÍÎÏÌÓÔÒÚÛÙıˆ˘˙˚¸˝Ä°ø˛£ÊÁ•");
	strcpy(l_special1,  "¿¡¬√ƒ≈»… ÀÃÕŒœ—“”‘’÷Ÿ⁄€‹›Ä°ø–ﬁ£∆«•");
	
	// set up browser strings
	strcpy(l_filesize, "File Size");
	strcpy(l_filetype, "File Type");
	strcpy(l_ndsfile, "Nintendo DS File");
	strcpy(l_txtfile, "Text File");
	strcpy(l_vcffile, "vCard File");
	strcpy(l_dvpfile, "Day Planner File");
	strcpy(l_remfile, "Reminder File");
	strcpy(l_picfile, "Picture");
	strcpy(l_directory, "Directory");
	strcpy(l_unknownfile, "Unknown Format");
	strcpy(l_binfile, "Raw Mode 5 Data");
	strcpy(l_todofile, "Todo File");
	strcpy(l_soundfile, "Audio File");
	strcpy(l_playlistfile, "Playlist");
	strcpy(l_hbfile, "Custom Database File");
	strcpy(l_pkgfile, "Custom Package File");
	strcpy(l_shortcutfile, "DSOrganize Shortcut");
	
	// set up irc strings
	strcpy(l_ircdisconnect, "Disconnected from server.");
	strcpy(l_ircconnectingto, "Connecting to");
	strcpy(l_helpavailable, "Type /help for available commands!");
	strcpy(l_hotspot, "Connected to hotspot!");
	strcpy(l_ircfailed, "Failed to connect.");
	strcpy(l_irccancelled, "Cancelled connect.");
	strcpy(l_irccannotconnect, "Could not connect to server!");
	strcpy(l_ircserverclosed, "Server closed connection.");
	strcpy(l_ircon, "on");
	strcpy(l_limit, "Limit");
	strcpy(l_key, "Key");
	strcpy(l_conversation, "Conversation with");
	
	// set up config strings
	strcpy(l_globalsettings, "Global Settings");
	strcpy(l_homescreensettings, "Home Screen Settings");
	strcpy(l_browsersettings, "Browser Settings");
	strcpy(l_scribblesettings, "Scribble Settings");
	strcpy(l_todosettings, "Todo Settings");
	strcpy(l_websettings, "Web Browser Settings");
	strcpy(l_ircsettings, "IRC Settings");
	strcpy(l_editorsettings, "Text Editor Settings");
	strcpy(l_wifisettings, "Wifi Settings");
	strcpy(l_imagesettings, "Image Viewer Settings");
}

void loadLanguage(char *filename)
{	
	char fName[256];
	char tName[256];
	
	strcpy(curLang,filename);
	
	strcpy(tName,filename);
	strlwr(tName);
	if(strcmp(tName,"english") != 0)	
		sprintf(fName, "%s%s.lng", d_lang, tName);
	else
	{
		initLanguage();
		return;
	}
	
	loadConfig(fName, setLangString);
	
	// check lengths for keyboard stuff
	
	while(strlen(l_numbers) < 12)
		strcat(l_numbers, " ");
	
	while(strlen(l_symbols) < 12)
		strcat(l_symbols, " ");
	
	while(strlen(l_lowercase) < 34)
		strcat(l_lowercase, " ");
	
	while(strlen(l_uppercase) < 34)
		strcat(l_uppercase, " ");
	
	while(strlen(l_special0) < 34)
		strcat(l_special0, " ");
	
	while(strlen(l_special1) < 34)
		strcat(l_special1, " ");
}

bool isLangauge(char *file)
{
	uint16 x = strlen(file);
	
	if(x < 3)
		return false;
		
	x--;
	
	if((file[x-3] == '.') && (file[x-2] == 'l' || file[x-2] == 'L') && (file[x-1] == 'n' || file[x-1] == 'N') && (file[x] == 'g' || file[x] == 'G'))
		return true;
	
	return false;	
}

int compareLanguage(const void * a, const void * b)
{
	char *v1 = (char *)a;
	char *v2 = (char *)b;
	
	char str1[64];
	char str2[64];
	
	strncpy(str1, v1, 63);
	strncpy(str2, v2, 63);
	
	strlwr(str1);
	strlwr(str2);
	
	return strcmp(str1, str2);	
}

int listLanguages(LANG_LIST buffer[])
{
	char tmpFile[256];
	int fType;
	
	strcpy(buffer[0].langURL,"English");
	uint16 pos = 1;

	DRAGON_chdir(d_lang);
	fType = DRAGON_FindFirstFile(tmpFile);
	
	while(fType != FE_NONE)
	{
		if(fType == FE_FILE)
		{
			if(isLangauge(tmpFile))
			{
				tmpFile[strlen(tmpFile)-4] = 0;
				strcpy(buffer[pos].langURL,tmpFile);
				pos++;
			}
		}
		
		fType = DRAGON_FindNextFile(tmpFile);
	}
	
	DRAGON_closeFind();
	DRAGON_chdir("/");
	
	if(pos > 1) // lets sort this list too
		qsort(buffer, pos, LANG_SIZE, compareLanguage);
	
	return pos;
}
