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
#include "resources.h"

// hbdb
char *res_MOTD = { "http://www.dragonminded.com/dsorganize/?req=MOTD&ver=3" };
char *res_CARD = { "http://www.dragonminded.com/dsorganize/?req=HBLIST&ver=3&card=" };
char *res_PKG = { "http://www.dragonminded.com/dsorganize/?req=PACKAGE&ver=3&pkg=%s" };

// irc
char *res_aboutIRC = { ":%s NOTICE %s :%cVERSION DSOrganize IRC 3.2 by DragonMinded%c%c%c" };

// homebrew database agent
char *res_uAgent = { "DSOrganize Homebrew Database" };

// sound streaming agent
char *res_sAgent = { "DSOrganize Streaming Client" };

// web browser agent
char *res_bAgent = { "DSOrganize Web Browser" };
