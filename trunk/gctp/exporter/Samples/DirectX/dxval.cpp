/*********************************************************************/
/*                                                                   */
/* FILE :        dxval.c                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#include <gctp/dbgout.hpp>
#endif

#include "dxexp.h"
#include "dxvec.h"

#define COMP_VECS(p,q)	(fabs((p)->vector[0]-(q)->vector[0]) < EPSILON_F && \
						 fabs((p)->vector[1]-(q)->vector[1]) < EPSILON_F && \
						 fabs((p)->vector[2]-(q)->vector[2]) < EPSILON_F)

#define COMP_MAPS(p,q)	(fabs((p)->u-(q)->u) < EPSILON_F && \
						 fabs((p)->v-(q)->v) < EPSILON_F)

#define COMP_COLS(p,q)	(fabs((p)->r-(q)->r) < EPSILON_F && \
						 fabs((p)->g-(q)->g) < EPSILON_F && \
						 fabs((p)->b-(q)->b) < EPSILON_F && \
						 fabs((p)->a-(q)->a) < EPSILON_F)


/******  PROTOTYPES  ******************************/

static int  DX_EntryValence ( DX_Valence* dx_val, DX_PointEntry* pointEntry, DX_VertexEntry* newEntry );
static void DX_DumpValences ( DX_Valence* dx_val, FILE* fp );
static void DX_DumpValences ( DX_Valence* dx_val );

/*
 *
 *  MakeValences   - make valences from the object
 *
 */

DX_Valence* dxExporter::MakeValences( OBJ2_ID obj )
{
	DX_PointEntry*      pointEntry;
	DX_VertexEntry*     vertexEntry;
	ShareInfo*          share;
	PointData*          point;
	int                 pnt_no, val_no;

	DX_Valence*  dx_val = new DX_Valence;
	dx_val->obj      = obj;
	dx_val->nEntries = 0;
	memset(dx_val->nIndicies, 0, sizeof(dx_val->nIndicies));
	dx_val->nPoints  = (*obj->pointCount)( obj );
	dx_val->points   = new DX_PointEntry[dx_val->nPoints];

	(*obj->pntShareInfo)( obj );

	for (pnt_no = 0; pnt_no < dx_val->nPoints; pnt_no++)
	{
		pointEntry = dx_val->points + pnt_no;
		point      = (*obj->pointInfo)( obj, pnt_no );
		share      = (ShareInfo *) point->userData;

		pointEntry->vertices = NULL;

		for (val_no = 0; val_no < share->numPols; val_no++)
		{
			vertexEntry = new DX_VertexEntry;
			vertexEntry->pnt_no = pnt_no;
			vertexEntry->pol_no = share->pols [val_no];
			vertexEntry->ivert  = share->ivert[val_no];
			vertexEntry->next   = NULL;

			DX_EntryValence( dx_val, pointEntry, vertexEntry );
		}
	}
//	DX_DumpValences( dx_val, stdout );
	DX_DumpValences( dx_val );
	return dx_val;
}


/*
 *
 *  ValenceIndex   - get the EntryNo with PolNO and VertexIndex
 *
 */

DX_DWORD dxExporter::ValenceIndex( DX_Valence* dx_val, PolNO pol_no, int ivert )
{
	OBJ2_ID              obj = dx_val->obj;
	DX_VertexEntry*      vertexEntry;
	PolygonData*         poly;
	PntNO                pnt_no;

	poly   = (*obj->polyInfo)( obj, pol_no );
	pnt_no = poly->points[ ivert ];

	vertexEntry = dx_val->points[ pnt_no ].vertices;

	while (vertexEntry) {
		if (vertexEntry->pol_no == pol_no) return vertexEntry->index;
		vertexEntry = vertexEntry->next;
	}

	vertexEntry = dx_val->points[ pnt_no ].vertices;
	return vertexEntry->index;
}


/*
 *
 *  ValenceGetEntried   - get the EntryNo with PolNO and VertexIndex
 *
 */

DX_VertexEntry* dxExporter::ValenceGetEntried( DX_Valence* dx_val, DX_VertexEntry* entry, EntryType type )
{
	// 要するにこれは、チェーンリストを深さ優先でトラバースしている
	DX_PointEntry*       point;
	DX_VertexEntry*      nextEntry = NULL;
	PntNO                pnt_no = 0;

	if (entry)
	{
		nextEntry = entry->next;
		while (nextEntry) {
			if (nextEntry->entried && PointNumToType(nextEntry->num_pnts) == type) return nextEntry;
			nextEntry = nextEntry->next;
		}
		pnt_no = entry->pnt_no + 1;
	}

	for (;pnt_no < dx_val->nPoints; pnt_no++) {
		point     = dx_val->points + pnt_no;
		nextEntry = point->vertices;
		while (nextEntry) {
			if (nextEntry->entried && PointNumToType(nextEntry->num_pnts) == type) return nextEntry;
			nextEntry = nextEntry->next;
		}
	}

	return nextEntry;
}


/*
 *
 *  ValenceOriginal   - get the original index.
 *
 */

DX_DWORD  dxExporter::ValenceOriginal( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	PntNO                pnt_no;

	pnt_no = entry->pnt_no;
	return dx_val->points[ pnt_no ].vertices->index;
}


/*
 *
 *  ValenceVertex   - get the vertex data
 *
 */

PointData* dxExporter::ValenceVertex( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	OBJ2_ID              obj = dx_val->obj;

	return (*obj->pointInfo)( obj, entry->pnt_no );
}


/*
 *
 *  ValenceNormal   - get the normal data
 *
 */

NormalData* dxExporter::ValenceNormal( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	OBJ2_ID              obj = dx_val->obj;
	PolygonData*         poly;

	poly = (*obj->polyInfo)( obj, entry->pol_no );
	if (poly->normals) {
		return (*obj->normalInfo)( obj, poly->normals[ entry->ivert ] );
	}
	return NULL;
}


/*
 *
 *  ValenceUVCoord   - get the UV coord data
 *
 */

MapData* dxExporter::ValenceUVCoord( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	OBJ2_ID              obj = dx_val->obj;
	PolygonData*         poly;

	poly = (*obj->polyInfo)( obj, entry->pol_no );
	if (poly->maps) {
		return (*obj->mapInfo)( obj, poly->maps[ entry->ivert ] );
	}
	return NULL;
}


/*
 *
 *  ValenceColor   - get the vertex color data
 *
 */

ColorData* dxExporter::ValenceColor( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	OBJ2_ID              obj = dx_val->obj;
	PolygonData*         poly;

	poly = (*obj->polyInfo)( obj, entry->pol_no );
	if (poly->colors) {
		return (*obj->colorInfo)( obj, poly->colors[ entry->ivert ] );
	}
	return NULL;
}




/*
 *
 *  DisposeValences   - dispose the valences
 *
 */

void dxExporter::DisposeValences( DX_Valence **dx_val )
{
	DX_PointEntry*      pointEntry;
	DX_VertexEntry*     vertexEntry, *temp;
	PntNO               pnt_no;

	if (*dx_val)
	{
		for (pnt_no = 0; pnt_no < (*dx_val)->nPoints; pnt_no++)
		{
			pointEntry  = (*dx_val)->points + pnt_no;
			vertexEntry = pointEntry->vertices;
			while (vertexEntry) {
				temp = vertexEntry->next;
				delete vertexEntry;
				vertexEntry = temp;
			}
		}
		delete[] (*dx_val)->points;
		delete (*dx_val);
		*dx_val = NULL;
	}
}


/*
 *
 *  DX_EntryValence   - entry Vertex to the PointEntry
 *
 */
// いろいろ改造｡ 点ポリゴン、線ポリゴン、３－４ポリゴン、それ以上のポリゴン、でEntryNoを別カウントするように
static int DX_EntryValence( DX_Valence* dx_val, DX_PointEntry* pointEntry, DX_VertexEntry* newEntry )
{
	OBJ2_ID              obj = dx_val->obj;
	DX_VertexEntry*      vertexEntry, *lastEntry;
	PolygonData          *poly, *poly_new;
	NormalData           *norm, *norm_new;
	MapData              *map,  *map_new;
	ColorData            *col,  *col_new;

	poly_new    = (*obj->polyInfo)( obj, newEntry->pol_no );
	newEntry->num_pnts = poly_new->numPnts;
	//単なる頂点？この場合はあるのか？
	if(newEntry->num_pnts <= 0) return FALSE;

	EntryType type = PointNumToType(newEntry->num_pnts);

	if (pointEntry->vertices == NULL)
	{
		dx_val->nEntries++;
		newEntry->index      = dx_val->nIndicies[type]++;
		newEntry->entried    = TRUE;
		pointEntry->vertices = newEntry;
		return TRUE;
	}
	else
	{
		lastEntry = pointEntry->vertices;
		while (lastEntry->next) { lastEntry = lastEntry->next; }
	}

	for (vertexEntry = pointEntry->vertices; vertexEntry; vertexEntry = vertexEntry->next)
	{
		poly = (*obj->polyInfo)( obj, vertexEntry->pol_no );

		if (poly_new->normals)
		{
			if (poly->normals == NULL) continue;
			norm_new = (*obj->normalInfo)( obj, poly_new->normals[ newEntry->ivert ] );
			norm     = (*obj->normalInfo)( obj, poly->normals[ vertexEntry->ivert ] );
			if (norm_new && norm && COMP_VECS(norm_new, norm) == FALSE) continue;
		}

		if (poly_new->maps)
		{
			if (poly->maps == NULL) continue;
			map_new = (*obj->mapInfo)( obj, poly_new->maps[ newEntry->ivert ] );
			map     = (*obj->mapInfo)( obj, poly->maps[ vertexEntry->ivert ] );
			if (COMP_MAPS(map_new, map) == FALSE) continue;
		}
		else if (poly->maps) continue;

		if (poly_new->colors)
		{
			if (poly->colors == NULL) continue;
			col_new = (*obj->colorInfo)( obj, poly_new->colors[ newEntry->ivert ] );
			col     = (*obj->colorInfo)( obj, poly->colors[ vertexEntry->ivert ] );
			if (COMP_COLS(col_new, col) == FALSE) continue;
		}
		else if (poly->colors) continue;

		newEntry->index = vertexEntry->index;
		newEntry->entried  = FALSE;
		lastEntry->next    = newEntry;
		return FALSE;
	}

	dx_val->nEntries++;
	newEntry->index = dx_val->nIndicies[type]++;
	newEntry->entried  = TRUE;
	lastEntry->next    = newEntry;
	return TRUE;
}



/*
 *
 *  DX_DumpValences   - dump the valences
 *
 */

static void DX_DumpValences( DX_Valence *dx_val, FILE *fp )
{
	DX_PointEntry*   point;
	DX_VertexEntry*  entry;
	int              n;

	for (n = 0; n < dx_val->nPoints; n++)
	{
		point = dx_val->points + n;
		fprintf(fp, "PNT [%d]\n", n);
		entry = point->vertices;
		while (entry) {
			fprintf(fp, "\tEntry NO[%d] PNT[%d] POL[%d] IVERT[%d] NUMPNTS[%d] ENTRIED[%d]\n",
					entry->index,
					entry->pnt_no,
					entry->pol_no,
					entry->ivert,
					entry->num_pnts,
					entry->entried );
			entry = entry->next;
		}
	}
}

#ifdef GCTP_USE_XFILEAPI
static void DX_DumpValences( DX_Valence *dx_val )
{
	DX_PointEntry*   point;
	DX_VertexEntry*  entry;
	int              n;

	for (n = 0; n < dx_val->nPoints; n++)
	{
		point = dx_val->points + n;
		PRNN("PNT ["<<n<<"]");
		entry = point->vertices;
		while (entry) {
			PRNN("\tEntry NO["<<entry->index<<"] PNT["<<entry->pnt_no<<"] POL["<<entry->pol_no<<"] IVERT["<<entry->ivert<<"] NUMPNTS["<<entry->num_pnts<<"] ENTRIED["<<entry->entried<<"]");
			entry = entry->next;
		}
	}
}
#endif
