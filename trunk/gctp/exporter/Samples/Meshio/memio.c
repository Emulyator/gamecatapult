/*********************************************************************/
/*                                                                   */
/* FILE :        memio.c                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Sep 15, 2000    last update                         */
/*                                                                   */
/* Copyright (C) 2000, D-STORM, Inc.                                 */
/*                                                                   */
/*********************************************************************/

#include <lwserver.h>
#include <lwio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memio.h"

#define MAX_STACK_DEPTH    128

#define _SWAP(a,b)	{ a^=b; b^=a; a^=b; }
#define	_BYTE	    unsigned char
#define	_WORD	    unsigned short

#ifdef _WIN32
#define	BSWAP_W(w)	_SWAP( (((_BYTE *)&w)[0]), (((_BYTE *)&w)[1]) )
#define	BSWAP_L(l)	{ BSWAP_W( (((_WORD *)&l)[0])); \
					  BSWAP_W( (((_WORD *)&l)[1])); \
					_SWAP( (((_WORD *)&l)[0]), (((_WORD *)&l)[1]) ) }
#else
#define	BSWAP_W(w)		
#define	BSWAP_L(l)	
#endif

typedef struct st_MEMWriteData {
	char    *base;
	char	*buff;
	int      byteswrite;
	int      buff_len;
	int      depth;
	int      leaf;
	char    *size_stack[MAX_STACK_DEPTH];
	char    *size_pos;
	FILE    *debug;
} MEMWriteData;

typedef struct st_MEMReadData {
	char    *base;
	char	*buff;
	int      bytesread;
	int      buff_len;
	int      depth;
	int      leaf;
	char    *size_stack[MAX_STACK_DEPTH];
	char    *size_pos;
	FILE    *debug;
} MEMReadData;



static void MEM_write        ( void *data, const char  *vals, int len );
static void MEM_writeU1      ( void *data, const unsigned char  *vals, int n );
static void MEM_writeU2      ( void *data, const unsigned short *vals, int n );
static void MEM_writeU4      ( void *data, const unsigned long  *vals, int n );
static void MEM_writeI1      ( void *data, const char  *vals, int n );
static void MEM_writeI2      ( void *data, const short *vals, int n );
static void MEM_writeI4      ( void *data, const long  *vals, int n );
static void MEM_writeFP      ( void *data, const float *vals, int n );
static void MEM_writeStr     ( void *data, const char *str );
static void MEM_writeID      ( void *data, const LWBlockIdent *block );
static void MEM_writeBeginBlk( void *data, const LWBlockIdent *block, int leaf );
static void MEM_writeEndBlk  ( void *data );
static int  MEM_writeDepth   ( void *data );

static int  MEM_read         ( void *data, char  *vals, int len );
static int  MEM_readU1       ( void *data, unsigned char  *vals, int n );
static int  MEM_readU2       ( void *data, unsigned short *vals, int n );
static int  MEM_readU4       ( void *data, unsigned long  *vals, int n );
static int  MEM_readI1       ( void *data, char  *vals, int n );
static int  MEM_readI2       ( void *data, short *vals, int n );
static int  MEM_readI4       ( void *data, long  *vals, int n );
static int  MEM_readFP       ( void *data, float *vals, int n );
static int  MEM_readStr      ( void *data, char  *str, int max );
static LWID MEM_readID       ( void *data, const LWBlockIdent *block );
static LWID MEM_readFindBlk  ( void *data, const LWBlockIdent *block );
static void MEM_readEndBlk   ( void *data );
static int  MEM_readDepth    ( void *data );

static char          *ID_Str ( LWID id );
static unsigned char *ID_Find( void *data, LWID id );



/***************************  SAVE STATE  ********************************/

MEMSaveState *
MEM_openSave( void *buff, int buff_len, int ioMode )
{
	MEMSaveState  *save;
	MEMWriteData  *writeData;

	save = (MEMSaveState *) calloc( 1, sizeof(MEMSaveState) );
	if (save == NULL) return NULL;

	save->ioMode   = ioMode;
	save->write    = MEM_write;
	save->writeI1  = MEM_writeI1;
	save->writeI2  = MEM_writeI2;
	save->writeI4  = MEM_writeI4;
	save->writeU1  = MEM_writeU1;
	save->writeU2  = MEM_writeU2;
	save->writeU4  = MEM_writeU4;
	save->writeFP  = MEM_writeFP;
	save->writeStr = MEM_writeStr;
	save->writeID  = MEM_writeID;
	save->beginBlk = MEM_writeBeginBlk;
	save->endBlk   = MEM_writeEndBlk;
	save->depth    = MEM_writeDepth;

	writeData = (MEMWriteData *) calloc( 1, sizeof(MEMWriteData) );
	if (writeData == NULL) return NULL;

	writeData->base       = buff;
	writeData->buff       = buff;
	writeData->buff_len   = buff_len;
	writeData->byteswrite = 0;
	writeData->depth      = 0;
	writeData->debug      = NULL;  /* fopen( "MEMIO.TXT", "wt" );  */

	save->writeData = writeData;

	return save;
}


void
MEM_closeSave( MEMSaveState *save )
{
	MEMWriteData   *writeData;
	if (save) {
		writeData = (MEMWriteData *) save->writeData;
		if (0) {
			FILE  *fp = fopen( "BIN.BIN", "wb" );
			fwrite( writeData->base, 1, writeData->byteswrite, fp );
			fclose( fp );
		}
		if (writeData->debug) fclose( writeData->debug );
		free( save->writeData );
		free( save );
	}
}


int
MEM_sizeSave( MEMSaveState *save )
{
	MEMWriteData   *writeData = (MEMWriteData *) save->writeData;
	if (writeData) {
		return writeData->byteswrite;
	} else {
		return 0;
	}
}



static void
MEM_write( void *data, const char *vals, int len )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_write] %d bytes\n", len );
	}

	if (writeData->buff_len - writeData->byteswrite >= len)
	{
		memcpy( writeData->buff, vals, len );
		writeData->buff += len;
		writeData->byteswrite += len;
	}
}



static void
MEM_writeU1( void *data, const unsigned char *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	int             size = sizeof(unsigned char) * n;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeU1] %d, %d bytes\n", n, size );
	}

	if (writeData->buff_len - writeData->byteswrite >= size)
	{
		memcpy( writeData->buff, vals, size );
		writeData->buff += size;
		writeData->byteswrite += size;
	}
}



static void
MEM_writeU2( void *data, const unsigned short *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	int             size = sizeof(unsigned short) * n;
	int             i;
	unsigned short *buff;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeU2] %d, %d bytes\n", n, size );
	}

	if (writeData->buff_len - writeData->byteswrite >= size)
	{
		buff = (unsigned short *) writeData->buff;

		for (i = 0; i < n; i++) {
			buff[i] = vals[i];
			BSWAP_W(buff[i]);
		}
		writeData->buff += size;
		writeData->byteswrite += size;
	}
}



static void
MEM_writeU4( void *data, const unsigned long *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	int             size = sizeof(unsigned long) * n;
	int             i;
	unsigned long  *buff;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeU4] %d, %d bytes\n", n, size );
	}

	if (writeData->buff_len - writeData->byteswrite >= size)
	{
		buff = (unsigned long *) writeData->buff;

		for (i = 0; i < n; i++) {
			buff[i] = vals[i];
			BSWAP_L(buff[i]);
		}
		writeData->buff += size;
		writeData->byteswrite += size;
	}
}


static void
MEM_writeI1( void *data, const char *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeI1]\n" );
	}
	MEM_writeU1( data, (const unsigned char *) vals, n );
}


static void
MEM_writeI2( void *data, const short *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeI2]\n" );
	}
	MEM_writeU2( data, (const unsigned short *) vals, n );
}


static void
MEM_writeI4( void *data, const long *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeI4]\n" );
	}
	MEM_writeU4( data, (const unsigned long *) vals, n );
}


static void
MEM_writeFP( void *data, const float *vals, int n )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeFP]\n" );
	}
	MEM_writeU4( data, (const unsigned long *) vals, n );
}


static void
MEM_writeStr( void *data, const char *str )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	int   c, bytesread = 0;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeStr] %s\n", str );
	}

    do  {
        c = str[bytesread++];
        if (writeData->buff_len - writeData->byteswrite > 0) {
        	*writeData->buff++ = (unsigned char) c;
        	writeData->byteswrite++;
        }
    } while (c);

	if (bytesread & 1) {
        if (writeData->buff_len - writeData->byteswrite > 0) {
        	*writeData->buff++ = 0x00;
        	writeData->byteswrite++;
        }
	}
}


static void
MEM_writeID( void *data, const LWBlockIdent *block )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_writeID] %s\n", ID_Str(block->id) );
	}
	MEM_writeU4( data, (const unsigned long *) &block->id, 1 );
}


static void
MEM_writeBeginBlk( void *data, const LWBlockIdent *block, int leaf )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	unsigned short  zero = 0;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_beginBlk] leaf<%d>\n", leaf );
	}

	MEM_writeID( data, block );
	writeData->leaf = leaf;
	if (leaf) {
		writeData->size_pos = writeData->buff;
	} else if (writeData->depth < MAX_STACK_DEPTH) {
		writeData->size_stack[ writeData->depth ] = writeData->buff;
		writeData->depth++;
	}
	MEM_writeU2( data, &zero, 1 );
}


static void
MEM_writeEndBlk( void *data )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;
	unsigned short *size;


	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_endBlk]\n" );
	}
	if (writeData->leaf) {
		size    = (unsigned short *) writeData->size_pos;
		size[0] = (unsigned short) (writeData->buff - writeData->size_pos) - sizeof(short);
	} else if (writeData->depth > 0) {
		writeData->depth--;
		size    = (unsigned short *) writeData->size_stack[ writeData->depth ];
		size[0] = (unsigned short) (writeData->buff - writeData->size_stack[ writeData->depth ]) - sizeof(short);
	}
	BSWAP_W(size[0]);
	writeData->leaf = 0;
}


static int
MEM_writeDepth( void *data )
{
	MEMWriteData   *writeData = (MEMWriteData *) data;

	if (writeData->debug) {
		fprintf( writeData->debug, "[MEM_depth] depth %d\n", writeData->depth );
	}
	return writeData->depth;
}



/***************************  LOAD STATE  ********************************/

MEMLoadState *
MEM_openLoad( void *buff, int buff_len, int ioMode )
{
	MEMLoadState  *load;
	MEMReadData   *readData;

	load = (MEMLoadState *) calloc( 1, sizeof(MEMLoadState) );
	if (load == NULL) return NULL;

	load->ioMode  = ioMode;
	load->read    = MEM_read;
	load->readI1  = MEM_readI1;
	load->readI2  = MEM_readI2;
	load->readI4  = MEM_readI4;
	load->readU1  = MEM_readU1;
	load->readU2  = MEM_readU2;
	load->readU4  = MEM_readU4;
	load->readFP  = MEM_readFP;
	load->readStr = MEM_readStr;
	load->readID  = MEM_readID;
	load->findBlk = MEM_readFindBlk;
	load->endBlk  = MEM_readEndBlk;
	load->depth   = MEM_readDepth;

	readData = (MEMReadData *) calloc( 1, sizeof(MEMReadData) );
	if (readData == NULL) return NULL;

	readData->base       = buff;
	readData->buff       = buff;
	readData->buff_len   = buff_len;
	readData->bytesread  = 0;
	readData->depth      = 0;
	readData->debug      = NULL; /* fopen( "MEMIO.TXT", "wt" ); */

	load->readData = readData;

	return load;
}


void
MEM_closeLoad( MEMLoadState *load )
{
	MEMReadData   *readData;
	if (load) {
		readData = (MEMReadData *) load->readData;
		if (readData->debug) fclose( readData->debug );
		free( load->readData );
		free( load );
	}
}


static int
MEM_read( void *data, char *vals, int len )
{
	MEMReadData   *readData = (MEMReadData *) data;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_read]\n" );
	}
	if (len > (readData->buff_len - readData->bytesread)) {
		len = readData->buff_len - readData->bytesread;
	}
	if (len > 0) {
		memcpy( vals, readData->buff, len );
		readData->buff      += len;
		readData->bytesread += len;
	}
	return len;
}


static int
MEM_readU1( void *data, unsigned char *vals, int n )
{
	MEMReadData   *readData = (MEMReadData *) data;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readU1]\n" );
	}
	if (n > (readData->buff_len - readData->bytesread)) {
		n = readData->buff_len - readData->bytesread;
	}
	if (n > 0) {
		memcpy( vals, readData->buff, n );
		readData->buff      += n;
		readData->bytesread += n;
	}
	return n;
}


static int
MEM_readU2( void *data, unsigned short *vals, int n )
{
	MEMReadData     *readData = (MEMReadData *) data;
	unsigned short  *buff;
	int              i, left;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readU2]\n" );
	}
	left = (int) ((readData->buff_len - readData->bytesread) / sizeof(short));
	if (n > left) {
		n = left;
	}

	buff = (unsigned short *) readData->buff;

	for (i = 0; i < n; i++) {
		vals[i] = buff[i];
		BSWAP_W(vals[i]);
	}
	readData->buff      += n * sizeof(short);
	readData->bytesread += n * sizeof(short);

	return n;
}


static int
MEM_readU4( void *data, unsigned long *vals, int n )
{
	MEMReadData     *readData = (MEMReadData *) data;
	unsigned long   *buff;
	int              i, left;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readU4]\n" );
	}
	left = (int) ((readData->buff_len - readData->bytesread) / sizeof(long));
	if (n > left) {
		n = left;
	}

	buff = (unsigned long *) readData->buff;

	for (i = 0; i < n; i++) {
		vals[i] = buff[i];
		BSWAP_L(vals[i]);
	}
	readData->buff      += n * sizeof(long);
	readData->bytesread += n * sizeof(long);

	return n;
}


static int
MEM_readI1( void *data, char *vals, int n )
{
	return MEM_readU1( data, (unsigned char *) vals, n );
}


static int
MEM_readI2( void *data, short *vals, int n )
{
	return MEM_readU2( data, (unsigned short *) vals, n );
}


static int
MEM_readI4( void *data, long *vals, int n )
{
	return MEM_readU4( data, (unsigned long *) vals, n );
}


static int
MEM_readFP( void *data, float *vals, int n )
{
	return MEM_readU4( data, (unsigned long *) vals, n );
}


static int
MEM_readStr( void *data, char *vals, int max )
{
	MEMReadData     *readData = (MEMReadData *) data;
	char            *buff;
	int              n = 0;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readStr]\n" );
	}
	buff = readData->buff;

	while (*buff) {
		if (n < max) return 0;
		*vals++ = *buff++; n++;
	}
	if (n & 1) {
		if (n < max) return 0;
		*vals++ = *buff++; n++;
	}
	readData->buff      += n;
	readData->bytesread += n;

	return n;
}


static LWID
MEM_readID( void *data, const LWBlockIdent *block )
{
	MEMReadData     *readData = (MEMReadData *) data;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readID]\n" );
	}
	MEM_readU4( data, (unsigned long *)&block->id, 1 );
	return block->id;
}


static LWID
MEM_readFindBlk( void *data, const LWBlockIdent *block )
{
	MEMReadData     *readData = (MEMReadData *) data;
	unsigned char   *buff;
	unsigned short   size;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readFindBlk] depth = %d\n", readData->depth );
	}

	while (block->id)
	{
		if ((buff = ID_Find( data, block->id )) != NULL)
		{
			if (readData->depth < MAX_STACK_DEPTH) {
				readData->leaf      = 0;
				readData->buff      = buff;
				readData->bytesread = (int)(buff - readData->base);
				readData->size_stack[ readData->depth++ ] = readData->buff;
				if (readData->debug) {
					fprintf( readData->debug, "\t[FOUND] <%s>\n", ID_Str(block->id) );
				}
				MEM_readU2( data, &size, 1 );
				return block->id;
			} else {
				readData->leaf = 1;
				return 0L;
			}
		}
		block++;
	}

	readData->leaf = 1;
	return 0L;
}


static void
MEM_readEndBlk( void *data )
{
	MEMReadData     *readData = (MEMReadData *) data;
	unsigned short   size, *ss;

	if (readData->leaf) {
		readData->leaf = 0;
		if (readData->debug) {
			fprintf( readData->debug, "[MEM_readEndBlk] depth = %d\n", readData->depth );
		}
		return;
	}
	if (readData->depth > 0)
	{
		readData->depth--;
		ss   = (unsigned short *) readData->size_stack[ readData->depth ];
		size = *ss; BSWAP_W(size);
		readData->buff      = readData->size_stack[ readData->depth ] + size + sizeof(short);
		readData->bytesread = (int)(readData->buff - readData->base);
	}
	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readEndBlk] depth = %d\n", readData->depth );
	}
}


static int
MEM_readDepth( void *data )
{
	MEMReadData   *readData = (MEMReadData *) data;

	if (readData->debug) {
		fprintf( readData->debug, "[MEM_readDepth] depth %d\n", readData->depth );
	}
	return readData->depth;
}


static char *
ID_Str( LWID id )
{
	static char str[5];
	str[4] = 0x00;
	str[3] = (char) (id & 0x000000ff);
	str[2] = (char)((id & 0x0000ff00) >> 8);
	str[1] = (char)((id & 0x00ff0000) >> 16);
	str[0] = (char)((id & 0xff000000) >> 24);
	return str;
}


static unsigned char *
ID_Find( void *data, LWID id )
{
	MEMReadData     *readData = (MEMReadData *) data;
	LWID            *ss, temp_id;
	unsigned char   *buff;
	int              left;

	buff = readData->buff;
	left = readData->buff_len - readData->bytesread;

	while (left > 3) {
		ss = (LWID *) buff;
		temp_id = *ss;
		BSWAP_L(temp_id);
		if (temp_id == id) {
			return buff + sizeof(LWID);
		}
		buff++;
		left--;
	}

	return NULL;
}



	


