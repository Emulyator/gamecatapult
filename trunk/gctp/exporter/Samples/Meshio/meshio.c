/*********************************************************************/
/*                                                                   */
/* FILE :        meshio.c                                            */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Dec 14, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#include <lwserver.h>
#include <lwcmdseq.h>
#include <lwmeshedt.h>
#include <lwimage.h>
#include <lwsurf.h>
#include <lwtxtr.h>
#include <lwenvel.h>
#include <lwhost.h>

#include "obj2.h"
#include "lwo2.h"
#include "memio.h"
#include "misc.h"
#include "chunk.h"
#include "meshio.h"

/******  TYPE DECLARATIONS  *****/


typedef struct st_HashPoint {
	LWPntID		id;
	int			no;
	struct st_HashPoint	*sibling;
} HashPoint;

typedef struct st_HashTable {
	int			size;
	HashPoint 	*points;
	HashPoint	*(*addPoint)( struct st_HashTable *hash, LWPntID id, int no );
	HashPoint	*(*searchPoint)( struct st_HashTable *hash, LWPntID id );
	HashPoint	*(*getEntry)( struct st_HashTable *hash, LWPntID id );
} HashTable;


typedef struct st_StoreInfo 
{
	GlobalFunc             *global;
	LWMeshInfo             *mesh;
	int                     obj_no;

	PntNO                   pnt;
	PolNO                   pol;
	LWID                    lwid;
	int                     ndim;
	const char             *name;

	int                     num_pnts;
	int                     base_pnt;
	HashTable              *hash;
	PntNO                  *poly_pnts;

	LWStateQueryFuncs      *query;
	LWSurfaceFuncs         *surff;
	LWTextureFuncs	       *txtrf;
	LWEnvelopeFuncs        *envlf;
	LWChannelInfo          *chaninfo;
	LWImageList            *imglist;
	LWObjectFuncs          *objfunc;
	LWFileIOFuncs          *fileio;

	OBJ2                   *obj;
	int                     flags;

} StoreInfo;


typedef struct st_ClearInfo 
{
	MeshEditOp             *op;
	PTagLWID               *ptid;
	const char             *name;
	LWID                    lwid;
	int                     ndim;
	int                     flags;

} ClearInfo;


typedef struct st_SaveInfo 
{
	MeshEditOp             *op;
	VMapData               *vmap;
	VMadData               *vmad;
	PTagLWID               *ptid;
	LWID                   type;
	OBJ2_ID                obj;
	int                    pnt_idx;
	int                    pol_idx[OBJ_PSET_NUM];
	int                     flags;

} SaveInfo;

#define PSET_TYPE_FACE		0
#define PSET_TYPE_CURV		1
#define PSET_TYPE_PTCH		2
#define PSET_TYPE_MBAL		3
#define PSET_TYPE_BONE		4



#define ENVL_COLR_RED     "Color.R"
#define ENVL_COLR_GREEN   "Color.G"
#define ENVL_COLR_BLUE    "Color.B"
#define ENVL_LUMI         "Luminosity"
#define ENVL_DIFF         "Diffuse"
#define ENVL_SPEC         "Specular"
#define ENVL_REFL         "Reflective"
#define ENVL_TRAN         "Transparency"
#define ENVL_TRNL         "Translucency"
#define ENVL_RIND         "Refractive Index"
#define ENVL_BUMP         "Bump"
#define ENVL_GLOS         "Glossiness"
#define ENVL_SHRP         "DiffuseSharpness"
#define ENVL_RSAN         "ReflectionSeamAngle"
#define ENVL_TSAN         "RefractionSeamAngle"
#define ENVL_CLRF         "ColorFilter"
#define ENVL_CLRH         "ColorHighlight"
#define ENVL_ADTR         "AdditiveTransparency"
#define ENVL_GVAL         "GlowIntensity"
#define ENVL_LCOL_RED     "PolygonOutlineColor.Red"
#define ENVL_LCOL_GREEN   "PolygonOutlineColor.Green"
#define ENVL_LCOL_BLUE    "PolygonOutlineColor.Blue"
#define ENVL_LSIZ         "PolygonOutlineSize"

/******  DECLARATIONS FOR STATIC FUNCTIONS  *****/

static void    FreeInfo      ( StoreInfo *info );
static void    StoreImages   ( StoreInfo *info );
static void    StoreSurfaces ( StoreInfo *info );
static int     FetchSurface  ( StoreInfo *info, LWSurfaceID surfID, U1 *data );
static void    StoreMeshes   ( StoreInfo *info );
static void    StoreVMaps    ( StoreInfo *info );
static void    StorePoints   ( StoreInfo *info );
static int     ScanPoints    ( StoreInfo *info, LWPntID pntID );
static int     ScanVMaps     ( StoreInfo *info, LWPntID pntID );
static void    StorePolygons ( StoreInfo *info );
static int     ScanPolygons  ( StoreInfo *info, LWPolID polID );
static void    StoreDisconMap( StoreInfo *info );
static int     ScanDisconMap ( StoreInfo *info, LWPolID polID );
static PntNO   FetchPoint    ( StoreInfo *info, LWPntID pntID );
static void    FetchTexture  ( StoreInfo *info, LWO2_surf *lwo2, LWTextureID texID, LWID type );
static short   FetchEnvelope ( StoreInfo *info, LWEnvelopeID envlID );
static short   FetchChannel  ( StoreInfo *info, LWChanGroupID grpID, char *name );
static LWID    FetchColorType( StoreInfo *info, const char *colorVMap );

static EDError VMapClearFunc ( ClearInfo *info, const EDPointInfo   *point );
static EDError VMadClearFunc ( ClearInfo *info, const EDPolygonInfo *poly );
static EDError PTagClearFunc ( ClearInfo *info, const EDPolygonInfo *poly );
static EDError SetVertexMaps ( SaveInfo  *info, const EDPointInfo   *point );
static EDError SetDisconMaps ( SaveInfo  *info, const EDPolygonInfo *poly );
static EDError SetPolygonTags( SaveInfo  *info, const EDPolygonInfo *poly );

static HashTable *HashMakeTable( int num_pnts );
static void       HashFreeTable( HashTable *hash );


/******  STORE GEOMETORIES MODLER TO OBJ2  ******/


int Mesh_Object( GlobalFunc *global )
{
	LWStateQueryFuncs      *query;
	LWObjectFuncs          *objfunc;
	const char             *objname, *name;
	int                    n, num_objs;
	unsigned long          prodinfo;
	unsigned long          systemid;

	query    = (LWStateQueryFuncs *)(*global)( LWSTATEQUERYFUNCS_GLOBAL, GFUSE_TRANSIENT );
	objfunc  = (LWObjectFuncs *)    (*global)( LWOBJECTFUNCS_GLOBAL    , GFUSE_TRANSIENT );
	prodinfo = (unsigned long)      (*global)( LWPRODUCTINFO_GLOBAL    , GFUSE_TRANSIENT );
	systemid = (unsigned long)      (*global)( LWSYSTEMID_GLOBAL       , GFUSE_TRANSIENT );

	num_objs = (*objfunc->numObjects)();

	objname = (*query->object)();
	if (objname == NULL) return (-1);

	for (n = 0; n < num_objs; n++)
	{
		name = (*objfunc->filename)( n );
		if (objname != NULL && name != NULL) {
			if (strcmp(name, objname) == 0) return n;
		}
	}

//
//  BUG FOR LW6.5X
//
	if ((systemid & LWSYS_MODELER) && LWINF_GETBUILD(prodinfo) < 471) return (-1);

	for (n = 0; n < num_objs; n++)
	{
		name = (*objfunc->refName)( n );
		if (objname != NULL && name != NULL) {
			if (strcmp(name, objname) == 0) return n;
		}
	}

	return (-1);
}


int Mesh_Find( GlobalFunc *global, const char *objname )
{
	LWObjectFuncs          *objfunc;
	const char             *name;
	int                    n, num_objs;

	objfunc  = (*global)( LWOBJECTFUNCS_GLOBAL, GFUSE_TRANSIENT );
	num_objs = (*objfunc->numObjects)();

	for (n = 0; n < num_objs; n++)
	{
		name = (*objfunc->filename)( n );
		if (objname != NULL && name != NULL) {
			if (strcmp(name, objname) == 0) return n;
		}
	}

	for (n = 0; n < num_objs; n++)
	{
		name = (*objfunc->refName)( n );
		if (objname != NULL && name != NULL) {
			if (strcmp(name, objname) == 0) return n;
		}
	}

	return (-1);
}


OBJError Mesh_Load( OBJ2 *obj, void *global, int obj_no, int flags )
{
	StoreInfo       *info;
	LWObjectFuncs   *objfunc;
	const char      *filename;

	if (global == NULL) return OBJ_PARM_ERROR;

	info = (StoreInfo *) calloc( 1, sizeof(StoreInfo) );
	if (info == NULL) return OBJ_MEM_ERROR;

	info->global   = (GlobalFunc *) global;
	info->obj      = obj;
	info->obj_no   = obj_no;
	info->flags    = flags;

	info->query    = (*info->global)( LWSTATEQUERYFUNCS_GLOBAL, GFUSE_TRANSIENT );
	info->surff    = (*info->global)( LWSURFACEFUNCS_GLOBAL   , GFUSE_TRANSIENT );
	info->txtrf    = (*info->global)( LWTEXTUREFUNCS_GLOBAL   , GFUSE_TRANSIENT );
	info->envlf    = (*info->global)( LWENVELOPEFUNCS_GLOBAL  , GFUSE_TRANSIENT );
	info->chaninfo = (*info->global)( LWCHANNELINFO_GLOBAL    , GFUSE_TRANSIENT );
	info->imglist  = (*info->global)( LWIMAGELIST_GLOBAL      , GFUSE_TRANSIENT );
	info->objfunc  = (*info->global)( LWOBJECTFUNCS_GLOBAL    , GFUSE_TRANSIENT );
	info->fileio   = (*info->global)( LWFILEIOFUNCS_GLOBAL    , GFUSE_TRANSIENT );

	if (info->surff   == NULL || 
        info->imglist == NULL || info->objfunc == NULL) return OBJ_PARM_ERROR;

	objfunc   = info->objfunc;
	obj->type = LWO2_FILE;
	filename  = (*objfunc->filename)( obj_no );
	if (filename) {
		strcpy( obj->name, filename );
	} else {
		strcpy( obj->name, (*objfunc->userName)( obj_no ) );
	}
									/*  Store Surface Info  */
	StoreSurfaces( info );
									/*  Store Image Info  */
	StoreImages( info );

	if (flags & MESH_FLAG_MERGE) {
		StorePoints  ( info );
		StorePolygons( info );
	} else {
		StoreMeshes  ( info );
	}

	FreeInfo( info );

	return OBJ_NO_ERROR;
}


static void FreeInfo ( StoreInfo *info )
{
	if (info)
	{
		if (info->poly_pnts) {
			free( info->poly_pnts );
		}
		HashFreeTable( info->hash );
		free( info );
	}
}



/*  Store All Image Filenames  */

static void StoreImages( StoreInfo *info )
{
	OBJ2           *obj     = info->obj;
	LWImageList    *imglist = info->imglist;
	LWImageID       image;
	char           *filename;

	image = (*imglist->first)();

	while (image)
	{
		filename = (char *) (*imglist->filename)( image, 0 );
		LWO2_addClipStil( obj, filename );
		image = (*imglist->next)( image );
	}
}



/*  Find Image and return the Index  */

static U4 FindImage( StoreInfo *info, LWImageID imageID )
{
	LWImageList    *imglist = info->imglist;
	LWImageID       image;
	U4              index = 1;

	image = (*imglist->first)();

	while (image) 
	{
		if (image == imageID) {
			return index;
		}
		index++;
		image = (*imglist->next)( image );
	}

	return 0;
}



/*  Store All Surface Chunks  */

static void StoreSurfaces( StoreInfo *info )
{
	OBJ2             *obj      = info->obj;
	LWSurfaceFuncs   *surff    = info->surff;
	LWObjectFuncs    *objfunc  = info->objfunc;
	const char       *name     = NULL;
	const char       *filename, *object;
	LWSurfaceID		  surfID;
	int               size;
	U1               *buff;

	filename = (*objfunc->filename)( info->obj_no );

	if (filename == NULL) {
		filename = (*objfunc->refName)( info->obj_no );
	}

	surfID   = (*surff->first)();
	buff     = (U1 *) malloc( 32767 );

	while (surfID)
	{
		name   = (*surff->name)( surfID );
		object = (*surff->sceneObject)( surfID );

		if ((filename == NULL && object == NULL) ||
			(filename && object && strcmp( object, filename ) == 0))
		{
			(*obj->newSurf)( obj, (char *) name );
			size = FetchSurface( info, surfID, buff );
			(*obj->setSurf)( obj, (char *) name, size, buff );
		}
		surfID = (*surff->next)( surfID );
	}

	if (buff) free( buff );
}


static void FetchTexture( StoreInfo *info, LWO2_surf *lwo2, LWTextureID texID, LWID chan )
{
	OBJ2            *obj   = info->obj;
	LWSurfaceFuncs  *surff = info->surff;
	LWTextureFuncs	*txtrf = info->txtrf;
	LWTLayerID		 layr;
	LWImageID		 imgID;
	MEMSaveState    *save;
	LWO2_surf_blok  *blok;
	LWO2_blok_head  *head;
	char             buff[4096];
	int              nbytes;
    ID4              id;
    U4               type;
	U2               size;
	ChunkBuff        ch;

	save   = MEM_openSave( buff, sizeof(buff), LWIO_BINARY );
	(*txtrf->save)( texID, (LWSaveState *) save );
	nbytes = MEM_sizeSave( save );
	MEM_closeSave( save );

	chunk_buff_init( &ch, buff, nbytes );
	pull_id4( id, &ch );
	pull_u2 ( &size, 1, &ch );

	layr = (*txtrf->firstLayer)( texID );

    while ((int) ch.bytesread < nbytes) 
    {
		pull_id4( id, &ch );
		pull_u2 ( &size, 1, &ch );
		type = MAKE_ID(id[0],id[1],id[2],id[3]);

		if (type == ID_BLOK) 
		{
			type = (*txtrf->layerType)( layr );
			blok = LWO2_unpackBlok( obj, size, ch.buff, lwo2 );
			head = blok->head;
			if (head && head->chan) {
				head->chan->texture_type = chan;
			}
			if (blok->imag) {
				(*txtrf->getParam)( layr, TXTAG_IMAGE, &imgID );
				blok->imag->texture_image = FindImage( info, imgID );
			}
			layr = (*txtrf->nextLayer)( texID, layr );
		}

		seek_buff( size, &ch );
	}

	if (0) {
		FILE  *fp = fopen("C:\\txtr.txt", "wb");
//		fwrite(buff, size, 1, fp);
		LWO2_dumpSurf( obj, lwo2, fp );
		fclose(fp);
	}
}


static short FetchEnvelope( StoreInfo *info, LWEnvelopeID envlID )
{
	OBJ2            *obj   = info->obj;
	LWEnvelopeFuncs *envlf = info->envlf;
	LWFileIOFuncs   *fileio = info->fileio;
	MEMSaveState    *save;
	LWO2_envl        envl;
	EnvNO            no;
	char             buff[4096];
	int				 size;
	short            idx, *p;

	save = MEM_openSave( buff, sizeof(buff), LWIO_BINARY );
	(*envlf->save)( envlID, (LWSaveState *) save );
	size = MEM_sizeSave( save );
	MEM_closeSave( save );

	p    = (short *) (buff + 6);
//	idx  = MSB2(*p);
	idx  = (*obj->envlCount)( obj ) + 1;
	no   = (*obj->newEnvelope)( obj, idx );
	LWO2_unpackEnvelope( obj, size-8, buff+8, &envl );
	size = LWO2_packEnvelope( obj, buff, &envl );
	LWO2_freeEnvelope( obj, &envl );
	(*obj->setEnvelope)( obj, no, size, buff );

	return idx;
}


static short FetchChannel( StoreInfo *info, LWChanGroupID grpID, char *name )
{
	LWChannelInfo  *chaninfo = info->chaninfo;
	LWChannelID     chanID   = NULL;
	LWEnvelopeID    envlID   = NULL;
	const char     *channelName;

	while ((chanID = (*chaninfo->nextChannel)( grpID, chanID )) != NULL)
	{
		channelName = (*chaninfo->channelName)( chanID );
		if (strcmp( channelName, name ) == 0)
		{
			envlID = (*chaninfo->channelEnvelope)( chanID );
			return FetchEnvelope( info, envlID );
		}
	}
	return 0;
}



static int FetchSurface( StoreInfo *info, LWSurfaceID surfID, U1 *buff )
{
	OBJ2             *obj      = info->obj;
	LWSurfaceFuncs   *surff    = info->surff;
	LWTextureID       texID;
	LWEnvelopeID      envlID;
	LWChanGroupID     grpID;
	LWImageID		  imageID;
	LWO2_surf         lwo2;
	double           *fval;
	int               ival, size;
	const char       *colorVMap;

	memset( &lwo2, 0x00, sizeof(LWO2_surf) );
											/*  COLR  */
	if ((fval = (*surff->getFlt)( surfID, SURF_COLR )) != NULL) {
		lwo2.colr = (LWO2_surf_colr *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_colr) );
		lwo2.colr->color[0] = (F4) fval[0];
		lwo2.colr->color[1] = (F4) fval[1];
		lwo2.colr->color[2] = (F4) fval[2];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_COLR )) != NULL) {
		if (lwo2.colr == NULL) {
			lwo2.colr = (LWO2_surf_colr *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_colr) );
		}
		lwo2.colr->envelope = FetchEnvelope( info, envlID );
		grpID  = (*surff->chanGrp)( surfID );
		FetchChannel( info, grpID, ENVL_COLR_GREEN );
		FetchChannel( info, grpID, ENVL_COLR_BLUE );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_COLR )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_COLR );
	}

											/*  LUMI  */
	if ((fval = (*surff->getFlt)( surfID, SURF_LUMI )) != NULL) {
		lwo2.lumi = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		lwo2.lumi->intensity = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_LUMI )) != NULL) {
		if (lwo2.lumi == NULL) {
			lwo2.lumi = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		}
		lwo2.lumi->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_LUMI )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_LUMI );
	}

											/*  DIFF  */
	if ((fval = (*surff->getFlt)( surfID, SURF_DIFF )) != NULL) {
		lwo2.diff = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		lwo2.diff->intensity = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_DIFF )) != NULL) {
		if (lwo2.diff == NULL) {
			lwo2.diff = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		}
		lwo2.diff->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_DIFF )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_DIFF );
	}

											/*  SPEC  */
	if ((fval = (*surff->getFlt)( surfID, SURF_SPEC )) != NULL) {
		lwo2.spec = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		lwo2.spec->intensity = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_SPEC )) != NULL) {
		if (lwo2.spec == NULL) {
			lwo2.spec = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		}
		lwo2.spec->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_SPEC )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_SPEC );
	}

											/*  GLOS  */
	if ((fval = (*surff->getFlt)( surfID, SURF_GLOS )) != NULL) {
		lwo2.glos = (LWO2_surf_glos *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_glos) );
		lwo2.glos->glossiness = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_GLOS )) != NULL) {
		if (lwo2.glos == NULL) {
			lwo2.glos = (LWO2_surf_glos *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_glos) );
		}
		lwo2.glos->envelope = FetchEnvelope( info, envlID );
	}

											/*  REFL  */
	if ((fval = (*surff->getFlt)( surfID, SURF_REFL )) != NULL) {
		lwo2.refl = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		lwo2.refl->intensity = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_REFL )) != NULL) {
		if (lwo2.refl == NULL) {
			lwo2.refl = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		}
		lwo2.refl->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_REFL )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_REFL );
	}

											/*  TRAN  */
	if ((fval = (*surff->getFlt)( surfID, SURF_TRAN )) != NULL) {
		lwo2.tran = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		lwo2.tran->intensity = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_TRAN )) != NULL) {
		if (lwo2.tran == NULL) {
			lwo2.tran = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		}
		lwo2.tran->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_TRAN )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_TRAN );
	}

											/*  RIND  */
	if ((fval = (*surff->getFlt)( surfID, SURF_RIND )) != NULL) {
		lwo2.rind = (LWO2_surf_rind *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_rind) );
		lwo2.rind->refractive_index = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_RIND )) != NULL) {
		if (lwo2.rind == NULL) {
			lwo2.rind = (LWO2_surf_rind *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_rind) );
		}
		lwo2.rind->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_RIND )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_RIND );
	}

											/*  TRNL  */
	if ((fval = (*surff->getFlt)( surfID, SURF_TRNL )) != NULL) {
		lwo2.trnl = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		lwo2.trnl->intensity = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_TRNL )) != NULL) {
		if (lwo2.trnl == NULL) {
			lwo2.trnl = (LWO2_surf_base *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_base) );
		}
		lwo2.trnl->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_TRNL )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_TRNL );
	}

											/*  BUMP  */
	if ((fval = (*surff->getFlt)( surfID, SURF_BUMP )) != NULL) {
		lwo2.bump = (LWO2_surf_bump *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_bump) );
		lwo2.bump->strenght = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_BUMP )) != NULL) {
		if (lwo2.bump == NULL) {
			lwo2.bump = (LWO2_surf_bump *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_bump) );
		}
		lwo2.bump->envelope = FetchEnvelope( info, envlID );
	}
	if ((texID = (*surff->getTex)( surfID, SURF_BUMP )) != NULL) {
		FetchTexture( info, &lwo2, texID, ID_BUMP );
	}

											/*  GVAL  */
	if ((fval = (*surff->getFlt)( surfID, SURF_GVAL )) != NULL) {
		lwo2.gval = (LWO2_surf_gval *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_gval) );
		lwo2.gval->value = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_GVAL )) != NULL) {
		if (lwo2.gval == NULL) {
			lwo2.gval = (LWO2_surf_gval *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_gval) );
		}
		lwo2.gval->envelope = FetchEnvelope( info, envlID );
	}

											/*  LSIZ  */
	if ((fval = (*surff->getFlt)( surfID, SURF_LSIZ )) != NULL) {
		lwo2.lsiz = (LWO2_surf_lsiz *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_lsiz) );
		lwo2.lsiz->size = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_LSIZ )) != NULL) {
		if (lwo2.lsiz == NULL) {
			lwo2.lsiz = (LWO2_surf_lsiz *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_lsiz) );
		}
		lwo2.lsiz->envelope = FetchEnvelope( info, envlID );
	}

											/*  CLRH  */
	if ((fval = (*surff->getFlt)( surfID, SURF_CLRH )) != NULL) {
		lwo2.clrh = (LWO2_surf_clrh *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_clrh) );
		lwo2.clrh->color_highlights = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_CLRH )) != NULL) {
		if (lwo2.clrh == NULL) {
			lwo2.clrh = (LWO2_surf_clrh *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_clrh) );
		}
		lwo2.clrh->envelope = FetchEnvelope( info, envlID );
	}

											/*  CLRF  */
	if ((fval = (*surff->getFlt)( surfID, SURF_CLRF )) != NULL) {
		lwo2.clrf = (LWO2_surf_clrf *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_clrf) );
		lwo2.clrf->color_filter = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_CLRF )) != NULL) {
		if (lwo2.clrf == NULL) {
			lwo2.clrf = (LWO2_surf_clrf *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_clrf) );
		}
		lwo2.clrf->envelope = FetchEnvelope( info, envlID );
	}

											/*  ADTR  */
	if ((fval = (*surff->getFlt)( surfID, SURF_ADTR )) != NULL) {
		lwo2.adtr = (LWO2_surf_adtr *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_adtr) );
		lwo2.adtr->additive = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_ADTR )) != NULL) {
		if (lwo2.adtr == NULL) {
			lwo2.adtr = (LWO2_surf_adtr *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_adtr) );
		}
		lwo2.adtr->envelope = FetchEnvelope( info, envlID );
	}

											/*  SHRP  */
	if ((fval = (*surff->getFlt)( surfID, SURF_SHRP )) != NULL) {
		lwo2.shrp = (LWO2_surf_shrp *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_shrp) );
		lwo2.shrp->sharpness = (F4) fval[0];
	}
	if ((envlID = (*surff->getEnv)( surfID, SURF_SHRP )) != NULL) {
		if (lwo2.shrp == NULL) {
			lwo2.shrp = (LWO2_surf_shrp *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_shrp) );
		}
		lwo2.shrp->envelope = FetchEnvelope( info, envlID );
	}

											/*  SMAN  */
	if ((fval = (*surff->getFlt)( surfID, SURF_SMAN )) != NULL) {
		lwo2.sman = (LWO2_surf_sman *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_sman) );
		lwo2.sman->max_smoothing_angle = (F4) fval[0];
	}

											/*  RSAN  */
	if ((fval = (*surff->getFlt)( surfID, SURF_RSAN )) != NULL) {
		lwo2.rsan = (LWO2_surf_rsan *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_rsan) );
		lwo2.rsan->seam_angle = (F4) fval[0];
	}

											/*  AVAL  */
	if ((fval = (*surff->getFlt)( surfID, SURF_AVAL )) != NULL) {
		lwo2.aval = (LWO2_surf_aval *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_aval) );
		lwo2.aval->value = (F4) fval[0];
	}

											/*  ALPH  */
	if ((fval = (*surff->getFlt)( surfID, SURF_ALPH )) != NULL) {
		lwo2.alph = (LWO2_surf_alph *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_alph) );
		lwo2.alph->value = (F4) fval[0];
	}

											/*  RFOP  */
	ival = (*surff->getInt)( surfID, SURF_RFOP );
	lwo2.rfop = (LWO2_surf_rfop *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_rfop) );
	lwo2.rfop->options = (U2) ival;

											/*  TROP  */
	ival = (*surff->getInt)( surfID, SURF_TROP );
	lwo2.trop = (LWO2_surf_trop *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_trop) );
	lwo2.trop->options = (U2) ival;

											/*  SIDE  */
	ival = (*surff->getInt)( surfID, SURF_SIDE );
	lwo2.side = (LWO2_surf_side *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_side) );
	lwo2.side->sideness = (U2) ival;

											/*  LINE  */
	ival = (*surff->getInt)( surfID, SURF_LINE );
	lwo2.line = (LWO2_surf_line *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_line) );
	lwo2.line->flags = (U2) ival;

											/*  RIMG  */
	if ((imageID = (*surff->getImg)( surfID, SURF_RIMG )) != NULL) {
		lwo2.rimg = (LWO2_surf_rimg *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_rimg) );
		lwo2.rimg->image = FindImage( info, imageID );
	}

											/*  TIMG  */
	if ((imageID = (*surff->getImg)( surfID, SURF_TIMG )) != NULL) {
		lwo2.timg = (LWO2_surf_timg *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_timg) );
		lwo2.timg->image = FindImage( info, imageID );
	}

											/*  VMAP  */
	if ((colorVMap = (*surff->getColorVMap)( surfID )) != NULL) {
		lwo2.vcol = (LWO2_surf_vcol *) MEM_ALLOC( obj->mem, sizeof(LWO2_surf_vcol) );
		strcpy( lwo2.vcol->name, colorVMap );
		lwo2.vcol->type  = FetchColorType( info, colorVMap );
		lwo2.vcol->value = 1.0f;
		if ((fval = (*surff->getFlt)( surfID, SURF_VCOL )) != NULL) {
			lwo2.vcol->value = (F4) fval[0];
		}
	}

	size = LWO2_packSurf( obj, buff, &lwo2 );
	LWO2_freeSurf( obj, &lwo2 );

	return size;
}


static LWID  FetchColorType( StoreInfo *info, const char *colorVMap )
{
	LWObjectFuncs   *objfunc = info->objfunc;
	int             num_lwid, num_vmap, m, n;
	const char      *name;

	static LWID  lwid[] = { VMAP_RGBA, VMAP_RGB };

	num_lwid = sizeof(lwid) / sizeof(lwid[0]);

	for (n = 0; n < num_lwid; n++)
	{
		num_vmap = (*objfunc->numVMaps)( lwid[n] );

		for (m = 0; m < num_vmap; m++)
		{
			name = (*objfunc->vmapName)( lwid[n], m );
			if (strcmp( colorVMap, name ) == 0) {
				return lwid[n];
			}
		}
	}

	return 0L;
}




/*  Store All Points and Polygons for Separate mode  */

static void StoreMeshes( StoreInfo *info )
{
	LWObjectFuncs      *objfunc = info->objfunc;
	LWStateQueryFuncs  *query   = info->query;
	LWMeshInfo         *mesh;
	OBJ2               *obj     = info->obj;
	int                maxLayers, n;
	unsigned int       fg;

	maxLayers = (*objfunc->maxLayers)( info->obj_no );
	fg        = query ? (*query->layerMask)( OPLYR_FG ) : 0xffffffff;

	for (n = 0; n < maxLayers; n++)
	{
		if ((*objfunc->layerExists)( info->obj_no, n ) && ((1<<n)&fg)) 
		{
									/*  New Layer  */
			mesh = (*objfunc->layerMesh)( info->obj_no, n );
			(*obj->newLayer)( obj, NULL );
			obj->layer->layer_no = n;

									/*  Make Hash Table  */
			info->num_pnts  = (*mesh->numPoints)( mesh );
			info->poly_pnts = (PntNO *) calloc( info->num_pnts, sizeof(PntNO) );
			info->hash      = HashMakeTable( info->num_pnts );
			info->mesh      = mesh;
			info->base_pnt  = 0;
									/*  Store Points  */
			(*mesh->scanPoints)( mesh, ScanPoints  , info );
									/*  Store VMaps  */
			StoreVMaps( info );
									/*  Store Polygons  */
			(*mesh->scanPolys) ( mesh, ScanPolygons, info );
									/*  Store discontinuos VMaps  */
			StoreDisconMap( info );
									/*  Bounding Box  */
			(*obj->calcBBox)( obj );
									/*  Clean Up  */
			(*mesh->destroy)( mesh );
			HashFreeTable( info->hash );
			free( info->poly_pnts );
			info->hash      = NULL;
			info->poly_pnts = NULL;
			(*obj->loadPSet)( obj, LWPOLTYPE_FACE );
		}
	}

	(*obj->setLayer)( obj, 0 );
}



/*  Store All Points of the Layer  */

static void StorePoints( StoreInfo *info )
{
	LWObjectFuncs      *objfunc = info->objfunc;
	LWStateQueryFuncs  *query   = info->query;
	LWMeshInfo         *mesh;
	OBJ2               *obj     = info->obj;
	int                maxLayers, n;
	unsigned int       fg;

	info->num_pnts = 0;
	maxLayers = (*objfunc->maxLayers)( info->obj_no );
	fg        = query ? (*query->layerMask)( OPLYR_FG ) : 0xffffffff;

									/*  New Layer  */
	(*obj->newLayer)( obj, NULL );

	for (n = 0; n < maxLayers; n++)
	{
		if ((*objfunc->layerExists)( info->obj_no, n ) && ((1<<n)&fg)) 
		{
			mesh = (*objfunc->layerMesh)( info->obj_no, n );
			info->num_pnts += (*mesh->numPoints)( mesh );
			(*mesh->destroy)( mesh );
		}
	}

	info->poly_pnts = (PntNO *) calloc( info->num_pnts, sizeof(PntNO) );
	info->hash      = HashMakeTable( info->num_pnts );
	info->base_pnt  = 0;

	for (n = 0; n < maxLayers; n++)
	{
		if ((*objfunc->layerExists)( info->obj_no, n ) && ((1<<n)&fg)) 
		{
			mesh = (*objfunc->layerMesh)( info->obj_no, n );
			info->mesh = mesh;
			(*mesh->scanPoints)( mesh, ScanPoints, info );
			StoreVMaps( info );
			info->base_pnt += (*mesh->numPoints)( mesh );
			(*mesh->destroy)( mesh );
		}
	}
									/*  Bounding Box  */
	(*obj->calcBBox)( obj );
}


static int ScanPoints( StoreInfo *info, LWPntID pntID )
{
	OBJ2            *obj     = info->obj;
	LWObjectFuncs   *objfunc = info->objfunc;
	LWMeshInfo      *mesh    = info->mesh;
	PntNO            pnt;
	LWFVector        pos;
	double           position[3];

	(*mesh->pntBasePos)( mesh, pntID, pos );
	position[0] = (double) pos[0];
	position[1] = (double) pos[1];
	position[2] = (double) pos[2];
	pnt = (*obj->addPoint)( obj, position );

	(*info->hash->addPoint)( info->hash, pntID, pnt );

	return EDERR_NONE;
}


static void StoreVMaps( StoreInfo *info )
{
	OBJ2            *obj     = info->obj;
	LWObjectFuncs   *objfunc = info->objfunc;
	LWMeshInfo      *mesh    = info->mesh;
	void            *vmap;
	int              num_lwid, num_vmap, m, n;

	static LWID  lwid[] = { VMAP_PICK, VMAP_WGHT, VMAP_MNVW, 
							VMAP_TXUV, VMAP_MORF, VMAP_SPOT, 
							VMAP_RGBA, VMAP_RGB };

	num_lwid = sizeof(lwid) / sizeof(lwid[0]);

	for (n = 0; n < num_lwid; n++)
	{
		num_vmap = (*objfunc->numVMaps)( lwid[n] );

		for (m = 0; m < num_vmap; m++)
		{
			info->lwid = lwid[n];
			info->name = (*objfunc->vmapName)( info->lwid, m );
			info->ndim = (*objfunc->vmapDim) ( info->lwid, m );
			vmap       = (*mesh->pntVLookup) ( mesh, info->lwid, info->name );

			if ((*mesh->pntVSelect)( mesh, vmap ))
			{
				info->pnt = info->base_pnt;
				(*mesh->scanPoints)( mesh, ScanVMaps, info );
			}
		}
	}
}


static int ScanVMaps( StoreInfo *info, LWPntID pntID )
{
	OBJ2            *obj     = info->obj;
	LWObjectFuncs   *objfunc = info->objfunc;
	LWMeshInfo      *mesh    = info->mesh;
	PntNO            pnt     = info->pnt++;
	VMapData        *data;
	VMapNO           no;
	float            vector[128];

	if ((*mesh->pntVGet)( mesh, pntID, vector)) 
	{
		if ((data = (*obj->getVMap)( obj, info->lwid, (char *)info->name )) != NULL) {
			no = data->idx;
		} else {
			no = (*obj->newVMap)( obj, info->lwid, (char *)info->name, info->ndim );
		}
		(*obj->addVMap)( obj, no, pnt, vector );
	}

	return EDERR_NONE;
}


static void StoreDisconMap( StoreInfo *info )
{
	OBJ2            *obj     = info->obj;
	LWObjectFuncs   *objfunc = info->objfunc;
	LWMeshInfo      *mesh    = info->mesh;
	void            *vmap;
	int              num_lwid, num_vmap, m, n;

	static LWID  lwid[] = { VMAP_PICK, VMAP_WGHT, VMAP_MNVW, 
							VMAP_TXUV, VMAP_MORF, VMAP_SPOT, 
							VMAP_RGBA, VMAP_RGB };

	(*obj->loadPSet)( obj, LWPOLTYPE_FACE );

	num_lwid = sizeof(lwid) / sizeof(lwid[0]);

	for (n = 0; n < num_lwid; n++)
	{
		num_vmap = (*objfunc->numVMaps)( lwid[n] );

		for (m = 0; m < num_vmap; m++)
		{
			info->lwid = lwid[n];
			info->name = (*objfunc->vmapName)( info->lwid, m );
			info->ndim = (*objfunc->vmapDim) ( info->lwid, m );
			vmap       = (*mesh->pntVLookup) ( mesh, info->lwid, info->name );

			if ((*mesh->pntVSelect)( mesh, vmap ))
			{
				info->pol = 0;
				(*mesh->scanPolys)( mesh, ScanDisconMap, info );
			}
		}
	}

	(*obj->savePSet)( obj );
}


static int ScanDisconMap( StoreInfo *info, LWPolID polID )
{
	OBJ2            *obj     = info->obj;
	LWObjectFuncs   *objfunc = info->objfunc;
	LWMeshInfo      *mesh    = info->mesh;
	PolNO            pol     = info->pol;
	PolygonData     *poly;
	VMadData        *data;
	VMadNO           no;
	float            vector[128];
	int              m, numPnts;
	LWPntID          pntID;
	LWID             type;

	type = (*mesh->polType)( mesh, polID );

	if (info->flags & MESH_FLAG_PATCH) {
		if (type != LWPOLTYPE_FACE && type != LWPOLTYPE_PTCH) return EDERR_NONE;
	} else {
		if (type != LWPOLTYPE_FACE) return EDERR_NONE;
	}

	numPnts = (*mesh->polSize)( mesh, polID );
	poly    = (*obj->polyInfo)( obj, pol );
	if (poly == NULL) return EDERR_NONE;

	for (m = 0; m < numPnts; m++)
	{
		pntID = (*mesh->polVertex)( mesh, polID, m );

		if ((*mesh->pntVPGet)( mesh, pntID, polID, vector)) 
		{
			if ((data = (*obj->getVMad)( obj, info->lwid, (char *)info->name )) != NULL) {
				no = data->idx;
			} else {
				no = (*obj->newVMad)( obj, info->lwid, (char *)info->name, info->ndim );
			}
			(*obj->addVMad)( obj, no, poly->points[m], poly->idx, vector );
		}
	}

	info->pol++;

	return EDERR_NONE;
}




/*  Store All Polygons of the Layer  */

static void StorePolygons( StoreInfo *info )
{
	LWObjectFuncs      *objfunc = info->objfunc;
	LWStateQueryFuncs  *query   = info->query;
	LWMeshInfo         *mesh;
	OBJ2               *obj  = info->obj;
	int                maxLayers, n;
	unsigned int       fg;

	maxLayers = (*objfunc->maxLayers)( info->obj_no );
	fg        = query ? (*query->layerMask)( OPLYR_FG ) : 0xffffffff;

	for (n = 0; n < maxLayers; n++)
	{
		if ((*objfunc->layerExists)( info->obj_no, n ) && ((1<<n)&fg))
		{
			mesh = (*objfunc->layerMesh)( info->obj_no, n );
			info->mesh = mesh;
			(*mesh->scanPolys)( mesh, ScanPolygons, info );
			StoreDisconMap( info );
			(*mesh->destroy)( mesh );
			(*obj->loadPSet)( obj, LWPOLTYPE_FACE );
		}
	}
}


static int ScanPolygons( StoreInfo *info, LWPolID polID )
{
	LWMeshInfo      *mesh = info->mesh;
	OBJ2            *obj  = info->obj;
	LWPntID          pntID;
	LWID             type;
	PolygonData     *poly;
	PolNO            pol;
	int              n, num_pnts, num_lwid;
	const char      *surface, *name;

	static LWID  lwid[] = { PTAG_PART, PTAG_RGBA, PTAG_RGB, 
							PTAG_BONE, PTAG_BNWT, PTAG_BNUP, 
							PTAG_TXUV };

	num_pnts = (*mesh->polSize)( mesh, polID );
	type     = (*mesh->polType)( mesh, polID );
	surface  = (*mesh->polTag) ( mesh, polID, LWPTAG_SURF );

	for (n = 0; n < num_pnts; n++)
	{
		pntID = (*mesh->polVertex)( mesh, polID, n );
		info->poly_pnts[n] = FetchPoint( info, pntID );
	}

	if (info->flags & MESH_FLAG_PATCH) {
		if (type == LWPOLTYPE_PTCH) type = LWPOLTYPE_FACE;
	}

	(*obj->loadPSet)( obj, type );

	pol  = (*obj->addPoly) ( obj, num_pnts, info->poly_pnts, (char *)surface );
	poly = (*obj->polyInfo)( obj, pol );

									/*  OTHER PTAGS  */

	num_lwid = sizeof(lwid) / sizeof(lwid[0]);

	for (n = 0; n < num_lwid; n++)
	{
		if ((name  = (*mesh->polTag)( mesh, polID, lwid[n] )) != NULL) {
			(*obj->addPTag)( obj, pol, lwid[n], (char *)name );
		}
	}

	(*obj->savePSet)( obj );

	return EDERR_NONE;
}


static PntNO  FetchPoint( StoreInfo *info, LWPntID pntID )
{
	HashPoint  *point;

	point = (*info->hash->searchPoint)( info->hash, pntID );
	if (point) {
		return point->no;
	}

	return 0;
}



/**************   HASH FUNCTIONS   ***************/

static int primeNumber( int n )
{
	int    i, limit;

	for (;;n++)
	{
		limit = (int) sqrt(n);
		for (i = limit; i > 0; i--) {
			if (n%i == 0) break;
		}
		if (i == 1) return n;
	}
	return 0;
}

static HashPoint *addPoint( HashTable *hash, LWPntID id, int no )
{
	HashPoint  *point;

	point     = (*hash->getEntry)( hash, id );
	point->id = id;
	point->no = no;
	return point;
}

static HashPoint *getEntry( HashTable *hash, LWPntID id )
{
	HashPoint  *point;
	unsigned int idx;

	idx   = (unsigned int) id % hash->size;
	if (hash->points[idx].id == NULL) {
		return &(hash->points[idx]);
	}
	point = (HashPoint *) calloc( sizeof(HashPoint), 1 );
	if (hash->points[idx].sibling) {
		point->sibling = hash->points[idx].sibling;
	}
	hash->points[idx].sibling = point;

	return point;
}

static HashPoint *searchPoint( HashTable *hash, LWPntID id )
{
	HashPoint  *point;
	unsigned int idx;

	idx   = (unsigned int) id % hash->size;
	point = &(hash->points[idx]);
	while (point) {
		if (point->id == id) return point;
		point = point->sibling;
	}
	return NULL;
}

static HashTable *HashMakeTable( int num_pnts )
{
	HashTable  *hash;

	hash = (HashTable *) calloc( sizeof(HashTable), 1 );
	hash->size        = primeNumber( (int)(num_pnts * 1.5) );
	hash->points      = (HashPoint *) calloc( sizeof(HashPoint), hash->size );
	hash->addPoint    = addPoint;
	hash->searchPoint = searchPoint;
	hash->getEntry    = getEntry;

	return hash;
}


static void HashFreeTable( HashTable *hash )
{
	HashPoint  *point, *sibling;
	int         i;

	if (hash == NULL) return;

	for (i = 0; i < hash->size; i++) {
		point = hash->points[i].sibling;
		while (point) {
			sibling = point->sibling;
			free( point );
			point = sibling;
		}
	}
	if (hash) {
		if (hash->points) free( hash->points );
		free( hash );
	}
}




/////////////////////////////////////////  CLEAR VMAP

EDError  Mesh_ClearVMap( GlobalFunc *global, LWModCommand *local, VMapLWID *vmid )
{
	MeshEditOp             *op;
	ClearInfo              *info;
	void                   *vmap = NULL;

	if (global == NULL) return EDERR_BADARGS;

	info = (ClearInfo *) calloc( 1, sizeof(ClearInfo) );
	op   = info->op = (*local->editBegin)( 0, 0, OPSEL_GLOBAL );

	while (vmid->lwid != 0L)
	{
		if ((vmap = (*op->pointVSet)( op->state, vmap, vmid->lwid, vmid->name )) != NULL)
		{
			switch (vmid->lwid) {
			case VMAP_PICK:
				info->ndim = 0; break;
			case VMAP_WGHT:
			case VMAP_MNVW:
				info->ndim = 1; break;
			case VMAP_TXUV:
				info->ndim = 2; break;
			case VMAP_MORF:
			case VMAP_SPOT:
			case VMAP_RGB:
				info->ndim = 3; break;
			case VMAP_RGBA:
				info->ndim = 4; break;
			}
			info->lwid = vmid->lwid;
			info->name = vmid->name;
			(*op->polyScan) ( op->state, (EDPolyScanFunc  *) VMadClearFunc, info, OPLYR_FG );
			(*op->pointScan)( op->state, (EDPointScanFunc *) VMapClearFunc, info, OPLYR_FG );
		}
		vmid++;
	}

	(*op->done)( op->state, 0, 0 );

	free( info );

	return EDERR_NONE;
}



static EDError VMapClearFunc( ClearInfo *info, const EDPointInfo *point )
{
	MeshEditOp         *op   = info->op;

	(*op->pntVMap)( op->state, point->pnt, info->lwid, info->name, info->ndim, NULL );

	return EDERR_NONE;
}



static EDError VMadClearFunc( ClearInfo *info, const EDPolygonInfo *poly )
{
	MeshEditOp         *op = info->op;
	int                n;

	for (n = 0; n < poly->numPnts; n++)	{
		(*op->pntVPMap)( op->state, poly->points[n], poly->pol, info->lwid, info->name, info->ndim, NULL );
	}

	return EDERR_NONE;
}




/////////////////////////////////////////  CLEAR PTAG


EDError  Mesh_ClearPTag( GlobalFunc *global, LWModCommand *local, PTagLWID *ptid )
{
	MeshEditOp             *op;
	ClearInfo              *info;

	if (global == NULL) return EDERR_BADARGS;

	info = (ClearInfo *) calloc( 1, sizeof(ClearInfo) );
	info->ptid = ptid;

	op = info->op = (*local->editBegin)( 0, 0, OPSEL_GLOBAL );
	(*op->polyScan)( op->state, (EDPolyScanFunc *) PTagClearFunc, info, OPLYR_FG );
	(*op->done)( op->state, 0, 0 );

	free( info );

	return EDERR_NONE;
}


static EDError PTagClearFunc( ClearInfo *info, const EDPolygonInfo *poly )
{
	MeshEditOp         *op   = info->op;
	PTagLWID           *ptid = info->ptid;
	const char         *name;

	while (ptid->lwid != 0L)
	{
		if (poly->type == ptid->type)
		{
			if ((name = (*op->polyTag)( op->state, poly->pol, ptid->lwid )) != NULL) {
				(*op->polTag)( op->state, poly->pol, ptid->lwid, NULL );
			}
		}
		ptid++;
	}

	return EDERR_NONE;
}





/////////////////////////////////////////  SAVE VMAP

EDError Mesh_SaveVMap( GlobalFunc *global, LWModCommand *local, VMapLWID *vmid, OBJ2 *obj, int obj_no, int flags )
{
	LWObjectFuncs      *objfunc;
	LWStateQueryFuncs  *query;
	MeshEditOp         *op;
	LWMeshInfo         *mesh;

	SaveInfo           *info;
	VMapLWID           *vmip;
	VMapData           *vmap = NULL;
	VMadData           *vmad = NULL;

	LWCommandCode      ccSetLayer, ccSetBLayer;
	DynaValue          dval;
	int                maxLayers, n;
	int                num_pnts = 0;
	unsigned int       bg;
	unsigned int       fg;

	query    = (*global)( LWSTATEQUERYFUNCS_GLOBAL, GFUSE_TRANSIENT );
	objfunc  = (*global)( LWOBJECTFUNCS_GLOBAL    , GFUSE_TRANSIENT );

	info = (SaveInfo *) calloc( 1, sizeof(SaveInfo) );
	info->obj   = obj;
	info->type  = 0L;
	info->flags = flags;

	ccSetLayer  = (*local->lookup)( local->data, "setlayer" );
	ccSetBLayer = (*local->lookup)( local->data, "setblayer" );
	maxLayers   = (*objfunc->maxLayers)( obj_no );
	fg          = (*query->layerMask)  ( OPLYR_FG );
	bg          = (*query->layerMask)  ( OPLYR_BG );

	for (n = 0; n < maxLayers; n++)
	{
		if ((*objfunc->layerExists)( obj_no, n ) && ((1<<n)&fg)) 
		{
			dval.type = DY_LAYERS;
			dval.intv.value = 1 << n;
			(*local->execute)( local->data, ccSetLayer, 1, &dval, OPSEL_GLOBAL, NULL );

			info->op = op = (*local->editBegin)( 0, 0, OPSEL_GLOBAL );
			vmip = vmid;

			while (vmip->lwid != 0L)
			{
				if ((vmap = (*obj->getVMap)( obj, vmip->lwid, (char *) vmip->name )) != NULL)
				{
					info->vmap    = vmap;
					info->pnt_idx = num_pnts;
					(*op->pointScan)( op->state, SetVertexMaps, info, OPLYR_PRIMARY );
				}
				if ((vmad = (*obj->getVMad)( obj, vmip->lwid, (char *) vmip->name )) != NULL)
				{
					info->vmad    = vmad;
					info->pol_idx[PSET_TYPE_FACE] = 0;
					info->pol_idx[PSET_TYPE_CURV] = 0;
					info->pol_idx[PSET_TYPE_PTCH] = 0;
					info->pol_idx[PSET_TYPE_MBAL] = 0;
					info->pol_idx[PSET_TYPE_BONE] = 0;
					(*op->polyScan) ( op->state, SetDisconMaps, info, OPLYR_PRIMARY );
				}
				vmip++;
			}

			(*op->done)( op->state, EDERR_NONE, 0 );

			mesh = (*objfunc->layerMesh)( obj_no, n );
			num_pnts += (*mesh->numPoints)( mesh );
			(*mesh->destroy)( mesh );
		}
	}

	dval.type = DY_LAYERS;
	dval.intv.value = fg;
	(*local->execute)( local->data, ccSetLayer, 1, &dval, OPSEL_GLOBAL, NULL );

	dval.type = DY_LAYERS;
	dval.intv.value = bg;
	(*local->execute)( local->data, ccSetBLayer, 1, &dval, OPSEL_GLOBAL, NULL );

	(*obj->loadPSet)( obj, OBJ_POL_FACE );

	if (info) free( info );

	return EDERR_NONE;
}


static EDError
SetVertexMaps( SaveInfo *info, const EDPointInfo *point )
{
	MeshEditOp         *op   = info->op;
	OBJ2               *obj  = info->obj;
	PntNO              idx   = info->pnt_idx++;
	VMapData           *vmap = info->vmap;
	F4                 *vals;
	EDError            err;

	(*op->pntVMap)( op->state, point->pnt, vmap->type, vmap->name, vmap->dim, NULL );

	if ((vals = (*obj->getVMapData)( obj, vmap, idx )) != NULL)
	{
		err = (*op->pntVMap)( op->state, point->pnt, vmap->type, vmap->name, vmap->dim, vals );
		if (err != EDERR_NONE) return err;
	}

	return EDERR_NONE;
}



static EDError
SetDisconMaps( SaveInfo *info, const EDPolygonInfo *poly )
{
	MeshEditOp         *op   = info->op;
	OBJ2               *obj  = info->obj;
	PntNO              idx;
	PolygonData        *polygon;
	VMadData           *vmad = info->vmad;
	F4                 *vals;
	int                n;
	U4                 type;

	type = poly->type;

	if (info->flags & MESH_FLAG_PATCH) {
		if (poly->type == LWPOLTYPE_PTCH) type = LWPOLTYPE_FACE;
	}

	switch (type) {
	case LWPOLTYPE_FACE:
		idx = info->pol_idx[PSET_TYPE_FACE]++; break;
	case LWPOLTYPE_CURV:
		idx = info->pol_idx[PSET_TYPE_CURV]++; break;
	case LWPOLTYPE_PTCH:
		idx = info->pol_idx[PSET_TYPE_PTCH]++; break;
	case LWPOLTYPE_MBAL:
		idx = info->pol_idx[PSET_TYPE_MBAL]++; break;
	case LWPOLTYPE_BONE:
		idx = info->pol_idx[PSET_TYPE_BONE]++; break;
	}

	if (info->type != type) {
		(*obj->loadPSet)( obj, type );
	}

	polygon = (*obj->polyInfo)( obj, idx );

	for (n = 0; n < poly->numPnts; n++)
	{
		(*op->pntVPMap)( op->state, poly->points[n], poly->pol, vmad->type, vmad->name, vmad->dim, NULL );
		if ((vals = (*obj->getVMadData)( obj, vmad, polygon->points[n], idx )) != NULL) {
			(*op->pntVPMap)( op->state, poly->points[n], poly->pol, vmad->type, vmad->name, vmad->dim, vals );
		}
	}

	if (info->type != type) {
		(*obj->savePSet)( obj );
		info->type = type;
	}

	return EDERR_NONE;
}

/////////////////////////////////////////  SAVE PTAG

EDError Mesh_SavePTag( GlobalFunc *global, LWModCommand *local, PTagLWID *ptid, OBJ2 *obj, int obj_no, int flags )
{
	LWObjectFuncs      *objfunc;
	LWStateQueryFuncs  *query;
	SaveInfo           *info;
	MeshEditOp         *op;
	LWCommandCode      ccSetLayer, ccSetBLayer;
	DynaValue          dval;
	int                maxLayers, n;
	unsigned int       bg;
	unsigned int       fg;

	query    = (*global)( LWSTATEQUERYFUNCS_GLOBAL, GFUSE_TRANSIENT );
	objfunc  = (*global)( LWOBJECTFUNCS_GLOBAL    , GFUSE_TRANSIENT );

	info = (SaveInfo *) calloc( 1, sizeof(SaveInfo) );
	info->obj     = obj;
	info->ptid    = ptid;
	info->pnt_idx = 0;
	info->type    = 0L;
	info->flags   = flags;

	info->pol_idx[PSET_TYPE_FACE] = 0;
	info->pol_idx[PSET_TYPE_CURV] = 0;
	info->pol_idx[PSET_TYPE_PTCH] = 0;
	info->pol_idx[PSET_TYPE_MBAL] = 0;
	info->pol_idx[PSET_TYPE_BONE] = 0;

	ccSetLayer  = (*local->lookup)( local->data, "setlayer" );
	ccSetBLayer = (*local->lookup)( local->data, "setblayer" );
	maxLayers   = (*objfunc->maxLayers)( obj_no );
	fg          = (*query->layerMask)  ( OPLYR_FG );
	bg          = (*query->layerMask)  ( OPLYR_BG );

	for (n = 0; n < maxLayers; n++)
	{
		if ((*objfunc->layerExists)( obj_no, n ) && ((1<<n)&fg)) 
		{
			dval.type = DY_LAYERS;
			dval.intv.value = 1 << n;
			(*local->execute)( local->data, ccSetLayer, 1, &dval, OPSEL_GLOBAL, NULL );
			info->op  = op = (*local->editBegin)( 0, 0, OPSEL_GLOBAL );
			(*op->polyScan)( op->state, SetPolygonTags, info, OPLYR_PRIMARY );
			(*op->done)( op->state, EDERR_NONE, 0 );
		}
	}

	dval.type = DY_LAYERS;
	dval.intv.value = fg;
	(*local->execute)( local->data, ccSetLayer, 1, &dval, OPSEL_GLOBAL, NULL );

	dval.type = DY_LAYERS;
	dval.intv.value = bg;
	(*local->execute)( local->data, ccSetBLayer, 1, &dval, OPSEL_GLOBAL, NULL );

	(*obj->loadPSet)( obj, OBJ_POL_FACE );

	if (info) free( info );

	return EDERR_NONE;
}




static EDError
SetPolygonTags( SaveInfo  *info, const EDPolygonInfo *poly )
{
	MeshEditOp         *op   = info->op;
	PTagLWID           *ptid = info->ptid;
	OBJ2               *obj  = info->obj;
	PntNO              idx;
	PTagData           *ptag;
	TagData            *tag;
	U4                 type;

	type = poly->type;

	if (info->flags & MESH_FLAG_PATCH) {
		if (poly->type == LWPOLTYPE_PTCH) type = LWPOLTYPE_FACE;
	}

	switch (type) {
	case LWPOLTYPE_FACE:
		idx = info->pol_idx[PSET_TYPE_FACE]++; break;
	case LWPOLTYPE_CURV:
		idx = info->pol_idx[PSET_TYPE_CURV]++; break;
	case LWPOLTYPE_PTCH:
		idx = info->pol_idx[PSET_TYPE_PTCH]++; break;
	case LWPOLTYPE_MBAL:
		idx = info->pol_idx[PSET_TYPE_MBAL]++; break;
	case LWPOLTYPE_BONE:
		idx = info->pol_idx[PSET_TYPE_BONE]++; break;
	}

	if (info->type != type) {
		(*obj->loadPSet)( obj, type );
	}

	while (ptid->lwid != 0L)
	{
		if (type == ptid->type)
		{
			if ((ptag = (*obj->ptagSelect)( obj, NULL, idx, ptid->lwid )) != NULL) {
				if ((tag = (*obj->ptagGetTag)( obj, ptag, idx)) != NULL) {
					(*op->polTag)( op->state, poly->pol, ptid->lwid, tag->name );
				}
			}
		}
		ptid++;
	}

	if (info->type != type) {
		(*obj->savePSet)( obj );
		info->type = type;
	}

	return EDERR_NONE;
}





