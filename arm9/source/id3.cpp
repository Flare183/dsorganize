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
#include "fatwrapper.h"
#include "id3.h"
#include "globals.h"

// for ogg vorbis stuff
#include <libsound/ivorbiscodec.h>
#include <libsound/ivorbisfile.h>

void stripCRLF(char *str)
{
	int x = 0;
	int y = 0;
	
	while(str[y] != 0)
	{
		if(str[y] == 13 || str[y] == 10)
			y++;
		else
		{
			str[x] = str[y];
			x++;
			y++;
		}
	}
	
	str[x] = 0;
}

void getID3V1(DRAGON_FILE *fp, ID3V1_TYPE *id3)
{
	char id3Data[128];
	int fLoc = DRAGON_ftell(fp);
	
	DRAGON_fseek(fp, -128, SEEK_END);
	DRAGON_fread(id3Data, 128, 1, fp);
	DRAGON_fseek(fp, fLoc, 0);
	
	char ID[4];
	strncpy(ID, id3Data, 3);
	ID[3] = 0;
	
	if(strcmp(ID, "TAG") != 0)
	{
		id3->present = false;
		return;
	}
	
	id3->present = true;
	char *tmpData = id3Data + 3;
	
	strncpy(id3->title, tmpData, 30);
	id3->title[30] = 0;
	stripCRLF(id3->title);
	tmpData += 30;
	
	strncpy(id3->artist, tmpData, 30);
	id3->artist[30] = 0;
	stripCRLF(id3->artist);
	tmpData += 30;
	
	strncpy(id3->album, tmpData, 30);
	id3->album[30] = 0;
	stripCRLF(id3->album);
	tmpData += 30;
    
	strncpy(id3->year, tmpData, 4);
	id3->year[4] = 0;
	stripCRLF(id3->year);
	tmpData += 4;
	
	strncpy(id3->comment, tmpData, 28);
	id3->comment[28] = 0;
	stripCRLF(id3->comment);
	tmpData += 29;
	
	id3->track = (int)(*tmpData);
	tmpData ++;
    
	id3->genre = (int)(*tmpData);
	getGenre(id3->genre, id3->genreStr);
}

void getOggInfo(OggVorbis_File *vf, ID3V1_TYPE *id3)
{
	id3->present = false; // set to false, then if any tag is found, set to true
	
	vorbis_comment *vc = ov_comment(vf,-1);
	
	char *comment = NULL;
	
	// title
	comment = vorbis_comment_query(vc, "TITLE", 0);
	if(comment != NULL)
	{
		strncpy(id3->title, comment, 30);
		id3->title[30] = 0;
		stripCRLF(id3->title);
		id3->present = true;
	}	
	
	// artist
	comment = vorbis_comment_query(vc, "ARTIST", 0);
	if(comment != NULL)
	{
		strncpy(id3->artist, comment, 30);
		id3->artist[30] = 0;
		stripCRLF(id3->artist);
		id3->present = true;
	}
	
	// album
	comment = vorbis_comment_query(vc, "ALBUM", 0);
	if(comment != NULL)
	{
		strncpy(id3->album, comment, 30);
		id3->album[30] = 0;
		stripCRLF(id3->album);
		id3->present = true;
	}
	
	// year
	comment = vorbis_comment_query(vc, "YEAR", 0);
	if(comment != NULL)
	{
		strncpy(id3->year, comment, 4);
		id3->year[4] = 0;
		stripCRLF(id3->year);
		id3->present = true;
	}
	
	// comment
	comment = vorbis_comment_query(vc, "COMMENT", 0);
	if(comment != NULL)
	{
		strncpy(id3->comment, comment, 28);
		id3->comment[28] = 0;
		stripCRLF(id3->comment);
		id3->present = true;
	}
		
	// track
	comment = vorbis_comment_query(vc, "TRACK", 0);
	if(comment != NULL)
	{
		id3->track = 0;
		
		while(comment[0] != 0)
		{
			if(comment[0] >= '0' && comment[0] <= '9')
			{
				id3->track *= 10;
				id3->track += comment[0] - '0';
			}
			
			comment++;
		}
		// str to int
		id3->present = true;
	}
	
	// genre
	comment = vorbis_comment_query(vc, "GENRE", 0);
	if(comment != NULL)
	{
		strncpy(id3->genreStr, comment, 60);
		id3->genreStr[60] = 0;
		stripCRLF(id3->genreStr);
		id3->present = true;
	}
}

void getGenre(int g, char *str)
{
	DRAGON_chdir(d_base);
	DRAGON_FILE *fp = DRAGON_fopen("genre.dat", "r");
	DRAGON_fseek(fp, (g * 60), SEEK_SET);
	DRAGON_fread(str, 1, 60, fp);
	DRAGON_fclose(fp);
}
/*
void outputGenresToFile()
{
	char tmpStr[60];
	
	DRAGON_FILE *fp = DRAGON_fopen("/genre.dat", "w");
	
	for(int i=0;i<=125;i++)
	{
		memset(tmpStr, 0, 60);
		getGenre(i, tmpStr);
		DRAGON_fwrite(tmpStr, 1, 60, fp);
	}
	
	DRAGON_fclose(fp);
}

void getGenre(int g, char *str)
{
	switch(g)
	{
		case 0:
			strcpy(str, "Blues");
			break;
		case 1:
			strcpy(str, "Classic Rock");
			break;
		case 2:
			strcpy(str, "Country");
			break;
		case 3:
			strcpy(str, "Dance");
			break;
		case 4:
			strcpy(str, "Disco");
			break;
		case 5:
			strcpy(str, "Funk");
			break;
		case 6:
			strcpy(str, "Grunge");
			break;
		case 7:
			strcpy(str, "Hip-Hop");
			break;
		case 8:
			strcpy(str, "Jazz");
			break;
		case 9:
			strcpy(str, "Metal");
			break;
		case 10:
			strcpy(str, "New Age");
			break;
		case 11:
			strcpy(str, "Oldies");
			break;
		case 12:
			strcpy(str, "Other");
			break;
		case 13:
			strcpy(str, "Pop");
			break;
		case 14:
			strcpy(str, "R&B");
			break;
		case 15:
			strcpy(str, "Rap");
			break;
		case 16:
			strcpy(str, "Reggae");
			break;
		case 17:
			strcpy(str, "Rock");
			break;
		case 18:
			strcpy(str, "Techno");
			break;
		case 19:
			strcpy(str, "Industrial");
			break;
		case 20:
			strcpy(str, "Alternative");
			break;
		case 21:
			strcpy(str, "Ska");
			break;
		case 22:
			strcpy(str, "Death Metal");
			break;
		case 23:
			strcpy(str, "Pranks");
			break;
		case 24:
			strcpy(str, "Soundtrack");
			break;
		case 25:
			strcpy(str, "Euro-Techno");
			break;
		case 26:
			strcpy(str, "Ambient");
			break;
		case 27:
			strcpy(str, "Trip-Hop");
			break;
		case 28:
			strcpy(str, "Vocal");
			break;
		case 29:
			strcpy(str, "Jazz+Funk");
			break;
		case 30:
			strcpy(str, "Fusion");
			break;
		case 31:
			strcpy(str, "Trance");
			break;
		case 32:
			strcpy(str, "Classical");
			break;
		case 33:
			strcpy(str, "Instrumental");
			break;
		case 34:
			strcpy(str, "Acid");
			break;
		case 35:
			strcpy(str, "House");
			break;
		case 36:
			strcpy(str, "Game");
			break;
		case 37:
			strcpy(str, "Sound Clip");
			break;
		case 38:
			strcpy(str, "Gospel");
			break;
		case 39:
			strcpy(str, "Noise");
			break;
		case 40:
			strcpy(str, "Alternative Rock");
			break;
		case 41:
			strcpy(str, "Bass");
			break;
		case 42:
			strcpy(str, "Soul");
			break;
		case 43:
			strcpy(str, "Punk");
			break;
		case 44:
			strcpy(str, "Space");
			break;
		case 45:
			strcpy(str, "Meditative");
			break;
		case 46:
			strcpy(str, "Instrumental Pop");
			break;
		case 47:
			strcpy(str, "Instrumental Rock");
			break;
		case 48:
			strcpy(str, "Ethnic");
			break;
		case 49:
			strcpy(str, "Gothic");
			break;
		case 50:
			strcpy(str, "Darkwave");
			break;
		case 51:
			strcpy(str, "Techno-Industrial");
			break;
		case 52:
			strcpy(str, "Electronic");
			break;
		case 53:
			strcpy(str, "Pop-Folk");
			break;
		case 54:
			strcpy(str, "Eurodance");
			break;
		case 55:
			strcpy(str, "Dream");
			break;
		case 56:
			strcpy(str, "Southern Rock");
			break;
		case 57:
			strcpy(str, "Comedy");
			break;
		case 58:
			strcpy(str, "Cult");
			break;
		case 59:
			strcpy(str, "Gangsta");
			break;
		case 60:
			strcpy(str, "Top 40");
			break;
		case 61:
			strcpy(str, "Christian Rap");
			break;
		case 62:
			strcpy(str, "Pop/Funk");
			break;
		case 63:
			strcpy(str, "Jungle");
			break;
		case 64:
			strcpy(str, "Native American");
			break;
		case 65:
			strcpy(str, "Cabaret");
			break;
		case 66:
			strcpy(str, "New Wave");
			break;
		case 67:
			strcpy(str, "Psychadelic");
			break;
		case 68:
			strcpy(str, "Rave");
			break;
		case 69:
			strcpy(str, "Showtunes");
			break;
		case 70:
			strcpy(str, "Trailer");
			break;
		case 71:
			strcpy(str, "Lo-Fi");
			break;
		case 72:
			strcpy(str, "Tribal");
			break;
		case 73:
			strcpy(str, "Acid Punk");
			break;
		case 74:
			strcpy(str, "Acid Jazz");
			break;
		case 75:
			strcpy(str, "Polka");
			break;
		case 76:
			strcpy(str, "Retro");
			break;
		case 77:
			strcpy(str, "Musical");
			break;
		case 78:
			strcpy(str, "Rock & Roll");
			break;
		case 79:
			strcpy(str, "Hard Rock");
			break;
		case 80:
			strcpy(str, "Folk");
			break;
		case 81:
			strcpy(str, "Folk-Rock");
			break;
		case 82:
			strcpy(str, "National Folk");
			break;
		case 83:
			strcpy(str, "Swing");
			break;
		case 84:
			strcpy(str, "Fast Fusion");
			break;
		case 85:
			strcpy(str, "Bebob");
			break;
		case 86:
			strcpy(str, "Latin");
			break;
		case 87:
			strcpy(str, "Revival");
			break;
		case 88:
			strcpy(str, "Celtic");
			break;
		case 89:
			strcpy(str, "Bluegrass");
			break;
		case 90:
			strcpy(str, "Avantgarde");
			break;
		case 91:
			strcpy(str, "Gothic Rock");
			break;
		case 92:
			strcpy(str, "Progressive Rock");
			break;
		case 93:
			strcpy(str, "Psychedelic Rock");
			break;
		case 94:
			strcpy(str, "Symphonic Rock");
			break;
		case 95:
			strcpy(str, "Slow Rock");
			break;
		case 96:
			strcpy(str, "Big Band");
			break;
		case 97:
			strcpy(str, "Chorus");
			break;
		case 98:
			strcpy(str, "Easy Listening");
			break;
		case 99:
			strcpy(str, "Acoustic");
			break;
		case 100:
			strcpy(str, "Humour");
			break;
		case 101:
			strcpy(str, "Speech");
			break;
		case 102:
			strcpy(str, "Chanson");
			break;
		case 103:
			strcpy(str, "Opera");
			break;
		case 104:
			strcpy(str, "Chamber Music");
			break;
		case 105:
			strcpy(str, "Sonata");
			break;
		case 106:
			strcpy(str, "Symphony");
			break;
		case 107:
			strcpy(str, "Booty Bass");
			break;
		case 108:
			strcpy(str, "Primus");
			break;
		case 109:
			strcpy(str, "Porn Groove");
			break;
		case 110:
			strcpy(str, "Satire");
			break;
		case 111:
			strcpy(str, "Slow Jam");
			break;
		case 112:
			strcpy(str, "Club");
			break;
		case 113:
			strcpy(str, "Tango");
			break;
		case 114:
			strcpy(str, "Samba");
			break;
		case 115:
			strcpy(str, "Folklore");
			break;
		case 116:
			strcpy(str, "Ballad");
			break;
		case 117:
			strcpy(str, "Power Ballad");
			break;
		case 118:
			strcpy(str, "Rhythmic Soul");
			break;
		case 119:
			strcpy(str, "Freestyle");
			break;
		case 120:
			strcpy(str, "Duet");
			break;
		case 121:
			strcpy(str, "Punk Rock");
			break;
		case 122:
			strcpy(str, "Drum Solo");
			break;
		case 123:
			strcpy(str, "Acapella");
			break;
		case 124:
			strcpy(str, "Euro-House");
			break;
		case 125:
			strcpy(str, "Dance Hall");
			break;
	}
}
*/

