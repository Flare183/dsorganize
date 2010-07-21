/*	MikMod sound library
	(c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is free software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.
 
	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================

  $Id: mmio.c,v 1.2 2004/02/06 19:29:05 raph Exp $

  Portable file I/O routines

==============================================================================*/

/*

	The way this module works:

	- _mm_fopen will call the errorhandler [see mmerror.c] in addition to
	  setting _mm_errno on exit.
	- _mm_iobase is for internal use.  It is used by Player_LoadFP to
	  ensure that it works properly with wad files.
	- _mm_read_I_* and _mm_read_M_* differ : the first is for reading data
	  written by a little endian (intel) machine, and the second is for reading
	  big endian (Mac, RISC, Alpha) machine data.
	- _mm_write functions work the same as the _mm_read functions.
	- _mm_read_string is for reading binary strings.  It is basically the same
	  as an fread of bytes.

*/

/* FIXME
	the _mm_iobase variable ought to be MREADER-specific. It will eventually
	become a private field of the MREADER structure, but this will require a
	soname version bump.

	In the meantime, the drawback is that if you use the xxx_LoadFP functions,
	you can't have several MREADER objects with different iobase values.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include "..\..\fatwrapper.h"
#include "..\..\general.h"

#include "..\include\mikmod_internals.h"
#include "..\virtualfile.h"

#define COPY_BUFSIZE  1024

static long _mm_iobase=0,temp_iobase=0;

VIRTUAL_FILE* _mm_fopen(CHAR* fname,CHAR* attrib)
{
	VIRTUAL_FILE *vfp = safeMalloc(sizeof(VIRTUAL_FILE));
	
	if(fname[0] == '?' && fname[1] == 'p')
	{
		const u8 **modLoc = ((const u8 **)fname) + 8;	
		u32	*modSize = ((u32 *)fname) + 3;
		
		vfp->pos = 0;
		vfp->size = *modSize;
		vfp->buffer = safeMalloc(vfp->size);
		
		memcpy(vfp->buffer, modLoc[0], vfp->size);
	}
	else
	{
		DRAGON_FILE *fp;
	
		if(!(fp=DRAGON_fopen(fname,attrib))) {
			_mm_errno = MMERR_OPENING_FILE;
			if(_mm_errorhandler) _mm_errorhandler();
			
			free(vfp);
			vfp = NULL;
			return vfp;
		}
		
		vfp->pos = 0;
		vfp->size = DRAGON_flength(fp);
		vfp->buffer = safeMalloc(vfp->size);
		
		DRAGON_fread(vfp->buffer,vfp->size,1,fp);
		DRAGON_fclose(fp);
	}
	
	return vfp;
}

BOOL _mm_FileExists(CHAR* fname)
{
	if(DRAGON_FileExists(fname)==FE_FILE)
		return 1;
	
	return 0;	
}

int _mm_fclose(VIRTUAL_FILE *vfp)
{
	if(vfp == NULL)
		return 0;
	
	if(vfp->buffer != NULL)
		free(vfp->buffer);
	vfp->buffer = NULL;
	
	if(vfp != NULL)
		free(vfp);
	vfp = NULL;
	
	return 1;
}

/* Sets the current file-position as the new _mm_iobase */
void _mm_iobase_setcur(MREADER* reader)
{
	temp_iobase=_mm_iobase;  /* store old value in case of revert */
	_mm_iobase=reader->Tell(reader);
}

/* Reverts to the last known _mm_iobase value. */
void _mm_iobase_revert(void)
{
	_mm_iobase=temp_iobase;
}

/*========== File Reader */

typedef struct MFILEREADER {
	MREADER core;
	VIRTUAL_FILE*   file;
} MFILEREADER;

static BOOL _mm_FileReader_Eof(MREADER* reader)
{	
	u32 fpos = ((MFILEREADER*)reader)->file->pos;
	u32 fsize = ((MFILEREADER*)reader)->file->size;
	
	if(fpos == fsize)
		return true;
	
	return false;
	
	//return DRAGON_feof(((MFILEREADER*)reader)->file);
}

static BOOL _mm_FileReader_Read(MREADER* reader,void* ptr,size_t size)
{
	u32 fpos = ((MFILEREADER*)reader)->file->pos;
	u32 fsize = ((MFILEREADER*)reader)->file->size;
	
	if(fpos + size > fsize)
		size = fsize - fpos;
	
	if(size == 0)
		return 0;
	
	memcpy(ptr, ((MFILEREADER*)reader)->file->buffer + fpos, size);	
	((MFILEREADER*)reader)->file->pos += size;
	
	return size;
	
	//return DRAGON_fread(ptr,size,1,((MFILEREADER*)reader)->file);
}

static int _mm_FileReader_Get(MREADER* reader)
{
	char c;
	
	u32 fpos = ((MFILEREADER*)reader)->file->pos;
	u32 fsize = ((MFILEREADER*)reader)->file->size;
	
	if(fpos == fsize)
		return 0;
	
	c = ((MFILEREADER*)reader)->file->buffer[fpos];
	((MFILEREADER*)reader)->file->pos++;	
	
	//DRAGON_fread(&c,1,1,((MFILEREADER*)reader)->file);
	
	return c;
}

static BOOL _mm_FileReader_Seek(MREADER* reader,long offset,int whence)
{
	u32 fpos = ((MFILEREADER*)reader)->file->pos;
	u32 fsize = ((MFILEREADER*)reader)->file->size;
	
	switch(whence)
	{
		case 0: // beginning of file
			if(offset < 0)
				offset = 0;
			if(offset > fsize)
				offset = fsize;
			
			((MFILEREADER*)reader)->file->pos = offset;
			break;
		case 1: // current location
			offset = offset + fpos;
			if(offset < 0)
				offset = 0;
			if(offset > fsize)
				offset = fsize;
			
			((MFILEREADER*)reader)->file->pos = offset;
			break;
		case 2:
			offset = offset + fsize;
			if(offset < 0)
				offset = 0;
			if(offset > fsize)
				offset = fsize;
				
			((MFILEREADER*)reader)->file->pos = offset;
			break;
	}
	
	return true;
	
	//return DRAGON_fseek(((MFILEREADER*)reader)->file, (whence==SEEK_SET)?offset+_mm_iobase:offset,whence);
}

static long _mm_FileReader_Tell(MREADER* reader)
{
	return ((MFILEREADER*)reader)->file->pos;
	//return DRAGON_ftell(((MFILEREADER*)reader)->file)-_mm_iobase;
}

MREADER *_mm_new_file_reader(VIRTUAL_FILE* fp)
{
	MFILEREADER* reader=(MFILEREADER*)_mm_malloc(sizeof(MFILEREADER));
	if (reader) {
		reader->core.Eof =&_mm_FileReader_Eof;
		reader->core.Read=&_mm_FileReader_Read;
		reader->core.Get =&_mm_FileReader_Get;
		reader->core.Seek=&_mm_FileReader_Seek;
		reader->core.Tell=&_mm_FileReader_Tell;
		reader->file=fp;
	}
	return (MREADER*)reader;
}

void _mm_delete_file_reader (MREADER* reader)
{
	if(reader) free(reader);
}

/*========== File Writer */

typedef struct MFILEWRITER {
	MWRITER core;
	VIRTUAL_FILE*   file;
} MFILEWRITER;

static BOOL _mm_FileWriter_Seek(MWRITER* writer,long offset,int whence)
{
	return false;
	//return DRAGON_fseek(((MFILEWRITER*)writer)->file,offset,whence);
}

static long _mm_FileWriter_Tell(MWRITER* writer)
{	
	return 0;
	//return DRAGON_ftell(((MFILEWRITER*)writer)->file);
}

static BOOL _mm_FileWriter_Write(MWRITER* writer,void* ptr,size_t size)
{
	return false;
	//return (DRAGON_fwrite(ptr,size,1,((MFILEWRITER*)writer)->file)==size);
}

static BOOL _mm_FileWriter_Put(MWRITER* writer,int value)
{
	return false;
	//return DRAGON_fputc(value,((MFILEWRITER*)writer)->file);
}

MWRITER *_mm_new_file_writer(VIRTUAL_FILE* fp)
{
	MFILEWRITER* writer=(MFILEWRITER*)_mm_malloc(sizeof(MFILEWRITER));
	if (writer) {
		writer->core.Seek =&_mm_FileWriter_Seek;
		writer->core.Tell =&_mm_FileWriter_Tell;
		writer->core.Write=&_mm_FileWriter_Write;
		writer->core.Put  =&_mm_FileWriter_Put;
		writer->file=fp;
	}
	return (MWRITER*) writer;
}

void _mm_delete_file_writer (MWRITER* writer)
{
	if(writer) free (writer);
}

/*========== Write functions */

void _mm_write_string(CHAR* data,MWRITER* writer)
{
	if(data)
		_mm_write_UBYTES(data,strlen(data),writer);
}

void _mm_write_M_UWORD(UWORD data,MWRITER* writer)
{
	_mm_write_UBYTE(data>>8,writer);
	_mm_write_UBYTE(data&0xff,writer);
}

void _mm_write_I_UWORD(UWORD data,MWRITER* writer)
{
	_mm_write_UBYTE(data&0xff,writer);
	_mm_write_UBYTE(data>>8,writer);
}

void _mm_write_M_ULONG(ULONG data,MWRITER* writer)
{
	_mm_write_M_UWORD(data>>16,writer);
	_mm_write_M_UWORD(data&0xffff,writer);
}

void _mm_write_I_ULONG(ULONG data,MWRITER* writer)
{
	_mm_write_I_UWORD(data&0xffff,writer);
	_mm_write_I_UWORD(data>>16,writer);
}

void _mm_write_M_SWORD(SWORD data,MWRITER* writer)
{
	_mm_write_M_UWORD((UWORD)data,writer);
}

void _mm_write_I_SWORD(SWORD data,MWRITER* writer)
{
	_mm_write_I_UWORD((UWORD)data,writer);
}

void _mm_write_M_SLONG(SLONG data,MWRITER* writer)
{
	_mm_write_M_ULONG((ULONG)data,writer);
}

void _mm_write_I_SLONG(SLONG data,MWRITER* writer)
{
	_mm_write_I_ULONG((ULONG)data,writer);
}

#if defined __STDC__ || defined _MSC_VER || defined MPW_C
#define DEFINE_MULTIPLE_WRITE_FUNCTION(type_name,type)						\
void _mm_write_##type_name##S (type *buffer,int number,MWRITER* writer)		\
{																			\
	while(number-->0)														\
		_mm_write_##type_name(*(buffer++),writer);							\
}
#else
#define DEFINE_MULTIPLE_WRITE_FUNCTION(type_name,type)						\
void _mm_write_/**/type_name/**/S (type *buffer,int number,MWRITER* writer)	\
{																			\
	while(number-->0)														\
		_mm_write_/**/type_name(*(buffer++),writer);						\
}
#endif

DEFINE_MULTIPLE_WRITE_FUNCTION(M_SWORD,SWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(M_UWORD,UWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_SWORD,SWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_UWORD,UWORD)

DEFINE_MULTIPLE_WRITE_FUNCTION(M_SLONG,SLONG)
DEFINE_MULTIPLE_WRITE_FUNCTION(M_ULONG,ULONG)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_SLONG,SLONG)
DEFINE_MULTIPLE_WRITE_FUNCTION(I_ULONG,ULONG)

/*========== Read functions */

int _mm_read_string(CHAR* buffer,int number,MREADER* reader)
{
	return reader->Read(reader,buffer,number);
}

UWORD _mm_read_M_UWORD(MREADER* reader)
{
	UWORD result=((UWORD)_mm_read_UBYTE(reader))<<8;
	result|=_mm_read_UBYTE(reader);
	return result;
}

UWORD _mm_read_I_UWORD(MREADER* reader)
{
	UWORD result=_mm_read_UBYTE(reader);
	result|=((UWORD)_mm_read_UBYTE(reader))<<8;
	return result;
}

ULONG _mm_read_M_ULONG(MREADER* reader)
{
	ULONG result=((ULONG)_mm_read_M_UWORD(reader))<<16;
	result|=_mm_read_M_UWORD(reader);
	return result;
}

ULONG _mm_read_I_ULONG(MREADER* reader)
{
	ULONG result=_mm_read_I_UWORD(reader);
	result|=((ULONG)_mm_read_I_UWORD(reader))<<16;
	return result;
}

SWORD _mm_read_M_SWORD(MREADER* reader)
{
	return((SWORD)_mm_read_M_UWORD(reader));
}

SWORD _mm_read_I_SWORD(MREADER* reader)
{
	return((SWORD)_mm_read_I_UWORD(reader));
}

SLONG _mm_read_M_SLONG(MREADER* reader)
{
	return((SLONG)_mm_read_M_ULONG(reader));
}

SLONG _mm_read_I_SLONG(MREADER* reader)
{
	return((SLONG)_mm_read_I_ULONG(reader));
}

#if defined __STDC__ || defined _MSC_VER || defined MPW_C
#define DEFINE_MULTIPLE_READ_FUNCTION(type_name,type)						\
int _mm_read_##type_name##S (type *buffer,int number,MREADER* reader)		\
{																			\
	while(number-->0)														\
		*(buffer++)=_mm_read_##type_name(reader);							\
	return !reader->Eof(reader);											\
}
#else
#define DEFINE_MULTIPLE_READ_FUNCTION(type_name,type)						\
int _mm_read_/**/type_name/**/S (type *buffer,int number,MREADER* reader)	\
{																			\
	while(number-->0)														\
		*(buffer++)=_mm_read_/**/type_name(reader);							\
	return !reader->Eof(reader);											\
}
#endif

DEFINE_MULTIPLE_READ_FUNCTION(M_SWORD,SWORD)
DEFINE_MULTIPLE_READ_FUNCTION(M_UWORD,UWORD)
DEFINE_MULTIPLE_READ_FUNCTION(I_SWORD,SWORD)
DEFINE_MULTIPLE_READ_FUNCTION(I_UWORD,UWORD)

DEFINE_MULTIPLE_READ_FUNCTION(M_SLONG,SLONG)
DEFINE_MULTIPLE_READ_FUNCTION(M_ULONG,ULONG)
DEFINE_MULTIPLE_READ_FUNCTION(I_SLONG,SLONG)
DEFINE_MULTIPLE_READ_FUNCTION(I_ULONG,ULONG)

/* ex:set ts=4: */
