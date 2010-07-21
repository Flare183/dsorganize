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
 
#ifndef _LANG_INCLUDED
#define _LANG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define LANG_SIZE 256
#define LANGUAGE_READ_SIZE 1024

typedef struct
{
	char langURL[LANG_SIZE];
} LANG_LIST;

void initLanguage();
void loadLanguage(char *filename);
void loadIRCConfig();
int listLanguages(LANG_LIST buffer[]);

extern char l_days[14][30];
extern char l_months[24][30];
extern char l_ampm[2][30];

extern char l_home[60];
extern char l_go[60];
extern char l_welcome[60];
extern char l_prev[60];
extern char l_next[60];
extern char l_reminder[60];
extern char l_editreminder[60];
extern char l_viewday[60];
extern char l_dayview[60];
extern char l_save[60];
extern char l_back[60];
extern char l_editselected[60];
extern char l_calendar[60];
extern char l_create[60];
extern char l_prevfield[60];
extern char l_nextfield[60];
extern char l_browser[60];
extern char l_open[60];
extern char l_launch[60];
extern char l_prevday[60];
extern char l_nextday[60];
extern char l_up[60];
extern char l_new[60];
extern char l_delete[60];
extern char l_rename[60];
extern char l_copy[60];
extern char l_paste[60];
extern char l_more[60];
extern char l_confirm[60];
extern char l_filename[60];
extern char l_calculator[60];
extern char l_yes[60];
extern char l_no[60];
extern char l_showhidden[60];
extern char l_memory[60];
extern char l_edit[60];
extern char l_pageup[60];
extern char l_pagedown[60];
extern char l_working[60];
extern char l_reset[60];
extern char l_choosepal[60];
extern char l_todo[60];
extern char l_title[60];
extern char l_scribble[60];
extern char l_saveformat[60];
extern char l_pause[60];
extern char l_play[60];
extern char l_soundmode[60];
extern char l_oneshot[60];
extern char l_sequential[60];
extern char l_nextfile[60];
extern char l_prevfile[60];
extern char l_randfile[60];
extern char l_hold[60];
extern char l_database[60];
extern char l_view[60];
extern char l_hide[60];
extern char l_done[60];
extern char l_cancel[60];
extern char l_date[60];
extern char l_version[60];
extern char l_size[60];
extern char l_namesort[60];
extern char l_datesort[60];
extern char l_swap[60];
extern char l_record[60];
extern char l_recordingmode[60];
extern char l_playback[60];
extern char l_length[60];
extern char l_stop[60];
extern char l_irc[60];
extern char l_send[60];
extern char l_disconnect[60];
extern char l_reconnect[60];
extern char l_launchalt[60];
extern char l_fatinit[60];
extern char l_mkdir[60];
extern char l_cut[60];
extern char l_defaulthomebrew[60];
extern char l_proxy[60];
extern char l_drag[256];
extern char l_autoconnect[60];
extern char l_ircnickname[60];
extern char l_altnickname[60];
extern char l_server[60];
extern char l_none[60];
extern char l_swapab[60];
extern char l_normalboot[60];
extern char l_altboot[60];
extern char l_autobullet[60];
extern char l_volume[60];
extern char l_newdir[60];
extern char l_iconset[60];
extern char l_settingdesc[60];
extern char l_loadpic[60];
extern char l_saving[60];
extern char l_continue[60];
extern char l_rendering[60];
extern char l_tochange[60];
extern char l_game[60];
extern char l_webbrowser[60];
extern char l_forward[60];
extern char l_homepage[60];
extern char l_query[60];
extern char l_append[60];
extern char l_author[60];
extern char l_composer[60];
extern char l_converter[60];
extern char l_ripper[60];
extern char l_fixed[60];
extern char l_variable[60];
extern char l_editorfont[60];
extern char l_ircfont[60];
extern char l_lefthand[60];
extern char l_righthand[60];
extern char l_handposition[60];	
extern char l_daily[60];
extern char l_weekly[60];
extern char l_monthly[60];
extern char l_annually[60];
extern char l_wifimode[60];
extern char l_firmware[60];
extern char l_fixedip[60];
extern char l_autodns[60];
extern char l_manualdns[60];
extern char l_scan[60];
extern char l_wepkey[60];
extern char l_ip[60];
extern char l_subnet[60];
extern char l_gateway[60];
extern char l_primarydns[60];
extern char l_secondarydns[60];
extern char l_autohide[60];
extern char l_downloaddir[60];
extern char l_imagesettings[60];
extern char l_imageorientation[60];
extern char l_landscape[60];
extern char l_letter[60];
extern char l_keyclick[60];
extern char l_click[60];
extern char l_silent[60];
extern char l_exit[60];

extern char l_title[60];
extern char l_artist[60];
extern char l_album[60];
extern char l_genre[60];
extern char l_comment[60];
extern char l_year[60];
extern char l_track[60];

extern char l_buffer[60];
extern char l_status[60];
extern char l_notconnected[60];
extern char l_associating[60];
extern char l_connecting[60];
extern char l_errorconnecting[60];
extern char l_streaming[60];
extern char l_buffering[60];
extern char l_curSong[60];

extern char l_changedto[60];
extern char l_alreadyexists[60];
extern char l_createddirectory[60];
extern char l_starteddownload[60];
extern char l_connectingtohost[60];
extern char l_deleted[60];
extern char l_motd[60];
extern char l_description[60];
extern char l_retrieving[60];
extern char l_errorretrieving[60];
extern char l_gettingpackage[60];
extern char l_failedpackage[60];
extern char l_connecting[60];
extern char l_getmotd[60];
extern char l_getlist[60];
extern char l_download[60];
extern char l_nodbentries[60];
extern char l_refresh[60];
extern char l_waitforinput[60];

extern char l_configuration[60];
extern char l_addressbook[60];
extern char l_language[60];
extern char l_startscreen[60];
extern char l_timeformat[60];
extern char l_nameformat[60];
extern char l_first[60];
extern char l_last[60];
extern char l_unknown[60];
extern char l_htmlmode[60];
extern char l_secondclick[60];
extern char l_dateformat[60];
extern char l_textonly[60];
extern char l_extended[60];
extern char l_extendedimages[60];

extern char l_firstname[60];
extern char l_lastname[60];
extern char l_nickname[60];
extern char l_homephone[60];
extern char l_workphone[60];
extern char l_cellphone[60];
extern char l_address[60];
extern char l_city[60];
extern char l_state[60];
extern char l_zip[60];
extern char l_email[60];
extern char l_comments[60];

extern char l_notodo[60];
extern char l_novcard[60];
extern char l_nowifi[60];
extern char l_nofat[256];
extern char l_createdir[256];
extern char l_noentries[256];
extern char l_rendererror[256];
extern char l_preview[256];
extern char l_noscribble[60];
//extern char l_noid3[60];
extern char l_nodefaultboot[256];
extern char l_launchingchishm[256];
extern char l_chishmnodldi[256];
extern char l_autopatch[256];
extern char l_fatalerror[256];
extern char l_fatalfree[256];
extern char l_fatalirc[256];
extern char l_outoffiles[256];

extern char l_numbers[13];
extern char l_symbols[13];
extern char l_lowercase[35];
extern char l_uppercase[35];
extern char l_special0[35];
extern char l_special1[35];

extern char l_filesize[60];
extern char l_filetype[60];
extern char l_ndsfile[60];
extern char l_txtfile[60];
extern char l_vcffile[60];
extern char l_dvpfile[60];
extern char l_remfile[60];
extern char l_picfile[60];
extern char l_unknownfile[60];
extern char l_directory[60];
extern char l_binfile[60];
extern char l_todofile[60];
extern char l_soundfile[60];
extern char l_playlistfile[60];
extern char l_pluginfile[60];
extern char l_exefile[60];
extern char l_hbfile[60];
extern char l_pkgfile[60];
extern char l_shortcutfile[60];	

extern char l_globalsettings[60];
extern char l_homescreensettings[60];
extern char l_browsersettings[60];
extern char l_scribblesettings[60];
extern char l_todosettings[60];
extern char l_websettings[60];
extern char l_ircsettings[60];
extern char l_editorsettings[60];
extern char l_wifisettings[60];

// irc
extern char irc_nicksinchannel[60];
extern char irc_nicklist[60];
extern char irc_cannotpart[60];
extern char irc_noserver[60];
extern char irc_nochannel[60];
extern char irc_invalidchannel[60];
extern char irc_noaction[60];
extern char irc_notinchannel[60];
extern char irc_actionsent[60];
extern char irc_invalidnick[60];
extern char irc_nomessage[60];
extern char irc_noticesent[60];
extern char irc_messagesent[60];
extern char irc_cannotclose[60];
extern char irc_noraw[60];
extern char irc_unknown[60];
extern char irc_channelsent[60];
extern char irc_receivenotice[60];
extern char irc_receiveauth[60];
extern char irc_version[60];
extern char irc_time[60];
extern char irc_ping[60];
extern char irc_actionreceived[60];
extern char irc_unknownctcp[60];
extern char irc_ctcpsent[60];
extern char irc_ctcpreply[60];
extern char irc_channelreceived[60];
extern char irc_join[60];
extern char irc_partnomessage[60];
extern char irc_partmessage[60];
extern char irc_youkicked[60];
extern char irc_kicked[60];
extern char irc_quitnomessage[60];
extern char irc_quitmessage[60];
extern char irc_settopic[60];
extern char irc_younick[60];
extern char irc_nick[60];
extern char irc_invite[60];
extern char irc_error[60];
extern char irc_mode[60];
extern char irc_network[60];
extern char irc_nicklength[60];
extern char irc_supportedmodes[60];
extern char irc_motdstart[60];
extern char irc_motdend[60];
extern char irc_nomotd[60];
extern char irc_altnickinuse[60];
extern char irc_nickinuse[60];
extern char irc_topic[60];
extern char irc_notopic[60];
extern char irc_nonickgiven[60];
extern char irc_erroneousnick[60];
extern char irc_serverbanned[60];
extern char irc_cantsend[60];
extern char irc_endwhois[60];
extern char irc_endwhowas[60];
extern char irc_back[60];
extern char irc_away[60];
extern char irc_nouser[60];
extern char irc_301[60];
extern char irc_307[60];
extern char irc_310[60];
extern char irc_311[60];
extern char irc_312[60];
extern char irc_313[60];
extern char irc_314[60];
extern char irc_319[60];
extern char irc_341[60];
extern char irc_403[60];
extern char irc_406[60];
extern char irc_421[60];
extern char irc_442[60];
extern char irc_443[60];
extern char irc_461[60];
extern char irc_471[60];
extern char irc_473[60];
extern char irc_474[60];
extern char irc_475[60];
extern char irc_477[60];
extern char irc_482[60];
extern char irc_671[60];

// normal irc
extern char l_ircdisconnect[60];
extern char l_ircconnectingto[60];
extern char l_helpavailable[60];
extern char l_hotspot[60];
extern char l_ircfailed[60];
extern char l_irccancelled[60];
extern char l_irccannotconnect[60];
extern char l_ircserverclosed[60];
extern char l_ircon[60];
extern char l_limit[60];
extern char l_key[60];
extern char l_conversation[60];

#ifdef __cplusplus
}
#endif

#endif
