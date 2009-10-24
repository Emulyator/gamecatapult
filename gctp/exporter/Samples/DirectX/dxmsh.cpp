/*********************************************************************/
/*                                                                   */
/* FILE :        dxmsh.cpp                                           */
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
#endif
#include "dxexp.h"

/******  PROTOTYPES  ******************************/

static void DX_DisposeNormals                 ( DX_MeshNormals **meshNormals );
static void DX_DisposeTextureCoords           ( DX_MeshTextureCoords **meshTextureCoords );
static void DX_DisposeVertexDuplicationIndices( DX_VertexDuplicationIndices **vertexDuplicationIndices );
static void DX_DisposeFaceWraps               ( DX_MeshFaceWraps **meshFaceWraps );
static void DX_DisposeVertexColors            ( DX_VertexColors **meshVertexColors );
static void DX_DisposeXSkinMeshHeader         ( DX_XSkinMeshHeader **xskinMeshHeader );
static void DX_DisposeSkinWeights             ( DX_SkinWeights **skinWeights );

static void      DX_SetMeshLayer   ( OBJ2_ID obj, int layer_no );
static DX_DWORD  DX_NumNodeTQVertices( DX_Mesh *dx_msh );
static DX_DWORD  DX_NumNodeTQFaces   ( DX_Mesh *dx_msh );
#ifdef GCTP_USE_XFILEAPI
static DX_DWORD          DX_NumNodeLineVertices( DX_Mesh *dx_msh );
static DX_DWORD          DX_NumNodeWires       ( DX_Mesh *dx_msh );
#endif

/*
 *
 *  MakeMesh    - make a new mesh
 *
 */

DX_Mesh* dxExporter::MakeMesh( OBJ2_ID obj, int layer_no )
{
	DX_STRING     nodeName, tempName;

	DX_SetMeshLayer( obj, layer_no );

	(*obj->setTxtLayrs)( obj, MAT_CHAN_NONE, 0 );
	(*obj->setTxtLayrs)( obj, MAT_CHAN_DIFF, 0 );
	(*obj->setTxtLayrs)( obj, MAT_CHAN_COLR, 0 );

	DX_Mesh*  dx_msh = new DX_Mesh;
	dx_msh->idx                      = dx_msh ? dx_msh->idx+1 : 0;
	dx_msh->obj                      = obj;
	dx_msh->layer                    = 0;
	dx_msh->valences                 = MakeValences( obj );
	dx_msh->nVertices                = DX_NumNodeTQVertices( dx_msh );
	dx_msh->nFaces                   = DX_NumNodeTQFaces   ( dx_msh );
	dx_msh->vertices                 = MakeVertices                ( dx_msh );
	dx_msh->faces                    = MakeFaces                   ( dx_msh );
	dx_msh->meshNormals              = MakeNormals                 ( dx_msh );
	dx_msh->meshTextureCoords        = MakeTextureCoords           ( dx_msh );
	dx_msh->vertexDuplicationIndices = MakeVertexDuplicationIndices( dx_msh );
	dx_msh->meshFaceWraps            = NULL;
	dx_msh->meshVertexColors         = MakeVertexColors            ( dx_msh );
	dx_msh->meshMaterialList         = MakeMaterialList            ( dx_msh );
	dx_msh->xskinMeshHeader          = NULL;
	dx_msh->skinWeights              = NULL;
	dx_msh->next                     = NULL;
#ifdef GCTP_USE_XFILEAPI
	dx_msh->wire                     = MakeWire                    ( dx_msh );
	dx_msh->wireTextureCoords        = MakeTextureCoords           ( dx_msh, ET_LINEPOLY );
	dx_msh->wireVertexColors         = MakeVertexColors            ( dx_msh, ET_LINEPOLY );
	dx_msh->wireMaterialList         = MakeMaterialList            ( dx_msh, ET_LINEPOLY );
#endif

	DX_FileUniqName( tempName, obj->name );
	if ((*obj->layerCount)(obj) > 1) {
		sprintf( nodeName, "Mesh_%s_Layer%d", tempName, layer_no+1 );
	} else {
		sprintf( nodeName, "Mesh_%s", tempName );
	}
	DX_MakeNodeName( dx_msh->nodeName, nodeName );

#ifdef GCTP_USE_XFILEAPI
	DX_DWORD wirenum = DX_NumNodeWires( dx_msh );
	if ((*obj->layerCount)(obj) > 1) {
		sprintf( nodeName, "Wire_%s_Layer%d", tempName, layer_no+1 );
	} else {
		sprintf( nodeName, "Wire_%s", tempName );
	}
	dx_msh->wire_name = nodeName;
#endif
	return dx_msh;
}


/*
 *
 *  DisposeMeshes   - dispose meshes
 *
 */

void dxExporter::DisposeMeshes( DX_Mesh **dx_mshs )
{
	DX_MeshFace*       face;
	DX_DWORD           n;

	DX_Mesh*  dx_msh = *dx_mshs;
	DX_Mesh*  temp;

	while (dx_msh)
	{
		temp = dx_msh->next;
		if (dx_msh->vertices) delete[] dx_msh->vertices;
		if (dx_msh->faces) {
			for (n = 0; n < dx_msh->nFaces; n++) {
				face = dx_msh->faces + n;
				delete[] face->faceVertexIndices;
			}
			delete[] dx_msh->faces;
		}
		DisposeMaterialList               ( &dx_msh->meshMaterialList );
		DX_DisposeNormals                 ( &dx_msh->meshNormals );
		DX_DisposeTextureCoords           ( &dx_msh->meshTextureCoords );
		DX_DisposeVertexDuplicationIndices( &dx_msh->vertexDuplicationIndices );
		DX_DisposeFaceWraps               ( &dx_msh->meshFaceWraps );
		DX_DisposeVertexColors            ( &dx_msh->meshVertexColors );
		DX_DisposeXSkinMeshHeader         ( &dx_msh->xskinMeshHeader );
		DX_DisposeSkinWeights             ( &dx_msh->skinWeights );
#ifdef GCTP_USE_XFILEAPI
		delete dx_msh->wire;
		DX_DisposeTextureCoords           ( &dx_msh->wireTextureCoords );
		DX_DisposeFaceWraps               ( &dx_msh->wireFaceWraps );
		DX_DisposeVertexColors            ( &dx_msh->wireVertexColors );
#endif
		delete dx_msh;
		dx_msh = temp;
	}

	*dx_mshs = NULL;
}


/*
 *
 *  DisposeMaterialList   - dispose MaterialList
 *
 */

void dxExporter::DisposeMaterialList( DX_MeshMaterialList **meshMaterialList )
{
	DX_MeshMaterialList*   temp = *meshMaterialList;

	if (temp) {
		if (temp->faceIndexes) delete[] temp->faceIndexes;
		DisposeMaterials( &temp->materials );
		delete temp;
	}

	*meshMaterialList = NULL;
}


/*
 *  MakeVertices   - make vertices
 */

DX_Vector* dxExporter::MakeVertices( DX_Mesh* dx_msh )
{
	DX_Valence*         dx_val = dx_msh->valences;
	DX_VertexEntry*     entry  = NULL;
	DX_DWORD            nVertices = 0;
	PointData*          point;

	DX_Vector*  vertices = new DX_Vector[dx_msh->nVertices];

	while ((entry = ValenceGetEntried( dx_val, entry, ET_TQPOLY )))
	{
		point = ValenceVertex( dx_val, entry );

		vertices[nVertices].x = (DX_FLOAT) point->position[0];
		vertices[nVertices].y = (DX_FLOAT) point->position[1];
		vertices[nVertices].z = (DX_FLOAT) point->position[2];
		nVertices++;
	}

	return vertices;
}

/*
 *  MakeFaces   - make faces
 */

DX_MeshFace* dxExporter::MakeFaces( DX_Mesh* dx_msh )
{
	OBJ2_ID             obj    = dx_msh->obj;
	DX_Valence*         dx_val = dx_msh->valences;
	DX_VertexEntry*     entry  = NULL;
	DX_DWORD            nFaces = 0;
	PolygonData*        poly;
	int                 n, m;

	DX_MeshFace* faces = new DX_MeshFace[dx_msh->nFaces];

	int num_pols = (*obj->polyCount)( obj );

	for (n = 0; n < num_pols; n++)
	{
		poly = (*obj->polyInfo)( obj, n );

		if (2 < poly->numPnts && poly->numPnts < 5)
		{
			faces[nFaces].nFaceVertexIndices = poly->numPnts;
			faces[nFaces].faceVertexIndices  = new DX_DWORD[poly->numPnts];

			for (m = 0; m < poly->numPnts; m++)
			{
				faces[nFaces].faceVertexIndices[m] = ValenceIndex( dx_val, n, m );
			}
			nFaces++;
		}
	}

	return faces;
}


/*
 *  MakeNormals   - make mesh normals
 */

DX_MeshNormals* dxExporter::MakeNormals( DX_Mesh* dx_msh )
{
	if (!_options->outputMeshNormals) return NULL;

	OBJ2_ID             obj    = dx_msh->obj;
	DX_Valence*         dx_val = dx_msh->valences;
	DX_VertexEntry*     entry  = NULL;
	DX_DWORD            nFaceNormals = 0;
	DX_DWORD            nNormals     = 0;
	NormalData*         norm;
	PolygonData*        poly;
	int                 n, m;

	DX_Vector*   normals     = new DX_Vector[dx_msh->nVertices];
	DX_MeshFace* faceNormals = new DX_MeshFace[dx_msh->nFaces];

	while ((entry = ValenceGetEntried( dx_val, entry, ET_TQPOLY )))
	{
		norm = ValenceNormal( dx_val, entry );

		normals[nNormals].x = (DX_FLOAT) norm->vector[0];
		normals[nNormals].y = (DX_FLOAT) norm->vector[1];
		normals[nNormals].z = (DX_FLOAT) norm->vector[2];
		nNormals++;
	}

	int num_pols = (*obj->polyCount)( obj );

	for (n = 0; n < num_pols; n++)
	{
		poly = (*obj->polyInfo)( obj, n );

		if (2 < poly->numPnts && poly->numPnts < 5)
		{
			faceNormals[nFaceNormals].nFaceVertexIndices = poly->numPnts;
			faceNormals[nFaceNormals].faceVertexIndices  = new DX_DWORD[poly->numPnts];

			for (m = 0; m < poly->numPnts; m++) {
				faceNormals[nFaceNormals].faceVertexIndices[m] = ValenceIndex( dx_val, n, m );
			}
			nFaceNormals++;
		}
	}

	DX_MeshNormals*  meshNormals = new DX_MeshNormals;
	meshNormals->nNormals     = dx_msh->nVertices;
	meshNormals->normals      = normals;
	meshNormals->nFaceNormals = dx_msh->nFaces;
	meshNormals->faceNormals  = faceNormals;

	return meshNormals;
}



/*
 *  MakeTextureCoords   - make mesh texture coords
 */

DX_MeshTextureCoords* dxExporter::MakeTextureCoords( DX_Mesh* dx_msh, EntryType type )
{
	if (!_options->outputTextureCoords) return NULL;

	OBJ2_ID               obj     = dx_msh->obj;
	if ((*obj->mapCount)( obj ) == 0) return NULL;

	DX_Valence*            dx_val = dx_msh->valences;
	DX_VertexEntry*        entry  = NULL;
	DX_DWORD               nTextureCoords = 0;
	MapData*               map;

	DX_Coord2d*  textureCoords = new DX_Coord2d[(type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices];

	while ((entry = ValenceGetEntried( dx_val, entry, type )))
	{
		map = ValenceUVCoord( dx_val, entry );

		if (map) {
			textureCoords[nTextureCoords].u = (DX_FLOAT) map->u;
			textureCoords[nTextureCoords].v = (DX_FLOAT) map->v;
		} else {
			textureCoords[nTextureCoords].u = 0.0f;
			textureCoords[nTextureCoords].v = 0.0f;
		}
		nTextureCoords++;
	}

	DX_MeshTextureCoords*  meshTextureCoords = new DX_MeshTextureCoords;
	meshTextureCoords->nTextureCoords = (type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices;
	meshTextureCoords->textureCoords  = textureCoords;

	return meshTextureCoords;
}



/*
 *  MakeVertexDuplicationIndices   - make mesh texture coords
 */

DX_VertexDuplicationIndices* dxExporter::MakeVertexDuplicationIndices( DX_Mesh* dx_msh, EntryType type )
{
	OBJ2_ID               obj    = dx_msh->obj;
	DX_Valence*           dx_val = dx_msh->valences;
	DX_VertexEntry*       entry  = NULL;
	DX_DWORD              nIndices = 0, nPoints = 0;

	DX_VertexDuplicationIndices*  vertexDuplicationIndices = NULL;

	if (dx_msh->nVertices > (DX_DWORD) dx_val->nPoints)
	{
		DX_DWORD*  indices = new DX_DWORD[(type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices];

		while ((entry = ValenceGetEntried( dx_val, entry, type )))
		{
			indices[nIndices] = ValenceOriginal( dx_val, entry );
			if (indices[nIndices] > nPoints) nPoints = indices[nIndices];
			nIndices++;
		}

		vertexDuplicationIndices = new DX_VertexDuplicationIndices;
		vertexDuplicationIndices->nIndices          = (type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices;
		vertexDuplicationIndices->nOriginalVertices = nPoints + 1;
		vertexDuplicationIndices->indices           = indices;
	}

	return vertexDuplicationIndices;
}



/*
 *  MakeVertexColors   - make mesh vertex colors
 */

DX_VertexColors* dxExporter::MakeVertexColors( DX_Mesh* dx_msh, EntryType type )
{
	OBJ2_ID               obj     = dx_msh->obj;

	if (!_options->outputVertexColors) return NULL;
	if ((*obj->colorCount)( obj ) == 0) return NULL;

	DX_Valence*           dx_val = dx_msh->valences;
	DX_VertexEntry*       entry  = NULL;
	DX_DWORD              nVertexColors = 0;
	ColorData*            col;
	PolygonData*          poly;
	MaterialData*         mat;

	DX_IndexColor*  vertexColors = new DX_IndexColor[(type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices];

	while ((entry = ValenceGetEntried( dx_val, entry, type)))
	{
		col = ValenceColor( dx_val, entry );

		if (col) {
			vertexColors[nVertexColors].indexColor.red   = (DX_FLOAT) col->r;
			vertexColors[nVertexColors].indexColor.green = (DX_FLOAT) col->g;
			vertexColors[nVertexColors].indexColor.blue  = (DX_FLOAT) col->b;
			vertexColors[nVertexColors].indexColor.alpha = (DX_FLOAT) col->a;
		} else {
			poly = (*obj->polyInfo)   ( obj, entry->pol_no );
			mat  = (*obj->getMaterial)( obj, poly->surf_no );
			vertexColors[nVertexColors].indexColor.red   = (DX_FLOAT) mat->colr[0];
			vertexColors[nVertexColors].indexColor.green = (DX_FLOAT) mat->colr[1];
			vertexColors[nVertexColors].indexColor.blue  = (DX_FLOAT) mat->colr[2];
			vertexColors[nVertexColors].indexColor.alpha = (DX_FLOAT) mat->colr[3];
		}
		vertexColors[nVertexColors].index = nVertexColors;
		nVertexColors++;
	}

	DX_VertexColors*  meshVertexColors = new DX_VertexColors;
	meshVertexColors->nVertexColors  = (type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices;
	meshVertexColors->vertexColors   = vertexColors;

	return meshVertexColors;
}



/*
 *  MakeMaterialList   - make mesh material list
 */

DX_MeshMaterialList* dxExporter::MakeMaterialList( DX_Mesh* dx_msh, EntryType type )
{
	OBJ2_ID               obj = dx_msh->obj;
	DX_DWORD              nFaceIndexes = 0;
	PolygonData*          poly;

	DX_DWORD  nMaterials  = (DX_DWORD) (*obj->surfCount)( obj );
	DX_DWORD* faceIndexes = new DX_DWORD[(type == ET_LINEPOLY)? dx_msh->wire->idx->num : dx_msh->nFaces];
	bool *uselist = new bool[nMaterials];
	for (DX_DWORD n = 0; n < nMaterials; n++) uselist[n] = false;

	int num_pols    = (*obj->polyCount)( obj );

	for (int n = 0; n < num_pols; n++)
	{
		poly = (*obj->polyInfo)( obj, n );

		if(type == ET_LINEPOLY) {
			if (poly->numPnts == 2) {
				faceIndexes[nFaceIndexes++] = poly->surf_no;
				uselist[poly->surf_no] = true;
			}
		}
		else {
			if (2 < poly->numPnts && poly->numPnts < 5) {
				faceIndexes[nFaceIndexes++] = poly->surf_no;
				uselist[poly->surf_no] = true;
			}
		}
	}

	DX_DWORD  useMaterials = 0;
	int *translist = new int[nMaterials];
	for (DX_DWORD n = 0; n < nMaterials; n++) {
		if(uselist[n]) translist[n] = useMaterials++;
	}
	for(DX_DWORD n = 0; n < nFaceIndexes; n++) {
		faceIndexes[n] = translist[faceIndexes[n]];
	}
	delete[] translist;

	DX_MeshMaterialList* meshMaterialList = new DX_MeshMaterialList;
	meshMaterialList->nMaterials   = useMaterials;
	meshMaterialList->nFaceIndexes = (type == ET_LINEPOLY)? dx_msh->wire->idx->num : dx_msh->nFaces;
	meshMaterialList->faceIndexes  = faceIndexes;
	meshMaterialList->materials    = MakeMaterials( NULL, obj, uselist );
	delete[] uselist;

	return meshMaterialList;
}


/*
 *
 *  DX_DisposeNormals   - dispose normals
 *
 */

static void DX_DisposeNormals( DX_MeshNormals **meshNormals )
{
	DX_MeshFace*          face;
	DX_MeshNormals*       temp = *meshNormals;

	if (temp) {
		if (temp->normals) delete[] temp->normals;
		if (temp->faceNormals) {
			for (DX_DWORD n = 0; n < temp->nFaceNormals; n++) {
				face = temp->faceNormals + n;
				delete[] face->faceVertexIndices;
			}
			delete[] temp->faceNormals;
		}
		delete temp;
	}

	*meshNormals = NULL;
}


/*
 *
 *  DX_DisposeTextureCoords   - dispose TextureCoords
 *
 */

static void DX_DisposeTextureCoords( DX_MeshTextureCoords **meshTextureCoords )
{
	DX_MeshTextureCoords*  temp = *meshTextureCoords;

	if (temp) {
		if (temp->textureCoords) delete[] temp->textureCoords;
		delete temp;
	}

	*meshTextureCoords = NULL;
}


/*
 *
 *  DX_DisposeVertexDuplicationIndices   - dispose VertexDuplicationIndices
 *
 */

static void DX_DisposeVertexDuplicationIndices( DX_VertexDuplicationIndices **vertexDuplicationIndices )
{
	DX_VertexDuplicationIndices*  temp = *vertexDuplicationIndices;

	if (temp) {
		if (temp->indices) delete[] temp->indices;
		delete temp;
	}

	*vertexDuplicationIndices = NULL;
}


/*
 *
 *  DX_DisposeFaceWraps   - dispose FaceWraps
 *
 */

static void DX_DisposeFaceWraps( DX_MeshFaceWraps **meshFaceWraps )
{
	DX_MeshFaceWraps*  temp = *meshFaceWraps;

	if (temp) {
		if (temp->faceWrapValues) delete[] temp->faceWrapValues;
		delete temp;
	}

	*meshFaceWraps = NULL;
}


/*
 *
 *  DX_DisposeVertexColors   - dispose VertexColors
 *
 */

static void DX_DisposeVertexColors( DX_VertexColors **meshVertexColors )
{
	DX_VertexColors*  temp = *meshVertexColors;

	if (temp) {
		if (temp->vertexColors) delete[] temp->vertexColors;
		delete temp;
	}

	*meshVertexColors = NULL;
}


/*
 *
 *  DX_DisposeXSkinMeshHeader   - dispose XSkinMeshHeader
 *
 */

static void DX_DisposeXSkinMeshHeader( DX_XSkinMeshHeader **xskinMeshHeader )
{
	DX_XSkinMeshHeader*   temp = *xskinMeshHeader;

	if (temp) {
		delete temp;
	}

	*xskinMeshHeader = NULL;
}


/*
 *
 *  DX_DisposeSkinWeights   - dispose SkinWeights
 *
 */

static void DX_DisposeSkinWeights( DX_SkinWeights **skinWeights )
{
	DX_SkinWeights*  dx_skm = *skinWeights;
	DX_SkinWeights*  temp;

	while (dx_skm)
	{
		temp = dx_skm->next;
		if (dx_skm->vertexIndices) delete[] dx_skm->vertexIndices;
		if (dx_skm->weights)       delete[] dx_skm->weights;
		delete dx_skm;
		dx_skm = temp;
	}

	*skinWeights = NULL;
}




/*
 *  DX_SetMeshLayer   - set the specific layer to current
 */

static void DX_SetMeshLayer( OBJ2_ID obj, int layer_no )
{
	LayerData*    layer = obj->layerList;

	while (layer) {
		if (layer->layer_no == layer_no) {
			(*obj->setLayer)( obj, layer->idx );
			return;
		}
		layer = layer->nextLayer;
	}
}


/*
 *  DX_NumNodeTQVertices   - count number of node vertices
 */

static DX_DWORD  DX_NumNodeTQVertices( DX_Mesh *dx_msh )
{
	DX_Valence*         dx_val = dx_msh->valences;

	return dx_val->nIndicies[ET_TQPOLY];
}


/*
 *  DX_NumNodeTQFaces   - count number of node faces
 */

static DX_DWORD  DX_NumNodeTQFaces( DX_Mesh* dx_msh )
{
	OBJ2_ID         obj = dx_msh->obj;
	DX_DWORD        nFaces = 0;
	PolygonData*    poly;

	int num_pols = (*obj->polyCount)( obj );

	for (int n = 0; n < num_pols; n++)
	{
		poly = (*obj->polyInfo)( obj, n );
		if (2 < poly->numPnts && poly->numPnts < 5) nFaces++;
	}

	return nFaces;
}

#ifdef GCTP_USE_XFILEAPI
/*
 *  MakeWire    - make wireframe data
 */

gctp::xext::Wire *dxExporter::MakeWire( DX_Mesh *dx_msh )
{
	OBJ2_ID             obj = dx_msh->obj;
	DX_Valence*         dx_val = dx_msh->valences;
	DX_VertexEntry*     entry  = NULL;
	DX_DWORD            nVertices = 0, nFaces = 0;
	PointData*          point;
	PolygonData*        poly;

	int vnum = DX_NumNodeLineVertices(dx_msh), pnum = DX_NumNodeWires( dx_msh );
	gctp::xext::Wire *ret = new gctp::xext::Wire(vnum, pnum);

	ret->vert->num = vnum;
	while ((entry = ValenceGetEntried( dx_val, entry, ET_LINEPOLY )))
	{
		point = ValenceVertex( dx_val, entry );

		ret->vert->verticies[nVertices].x = (DX_FLOAT) point->position[0];
		ret->vert->verticies[nVertices].y = (DX_FLOAT) point->position[1];
		ret->vert->verticies[nVertices].z = (DX_FLOAT) point->position[2];
		nVertices++;
	}
	
	ret->idx->num = pnum;
	int num_pols = (*obj->polyCount)( obj );
	for (int n = 0; n < num_pols; n++)
	{
		poly = (*obj->polyInfo)( obj, n );

		if (poly->numPnts == 2)
		{
			ret->idx->indicies[nFaces].start = ValenceIndex( dx_val, n, 0 );
			ret->idx->indicies[nFaces].end = ValenceIndex( dx_val, n, 1 );
			nFaces++;
		}
	}

	return ret;
}

/*
 *  DX_NumNodeLineVertices   - count number of node vertices
 */

static DX_DWORD  DX_NumNodeLineVertices( DX_Mesh *dx_msh )
{
	DX_Valence*         dx_val = dx_msh->valences;

	return dx_val->nIndicies[ET_LINEPOLY];
}

/*
 *  DX_NumNodeWires   - count number of node lines
 */

static DX_DWORD  DX_NumNodeWires( DX_Mesh* dx_msh )
{
	OBJ2_ID         obj = dx_msh->obj;
	DX_DWORD        nFaces = 0;
	PolygonData*    poly;

	int num_pols = (*obj->polyCount)( obj );

	for (int n = 0; n < num_pols; n++)
	{
		poly = (*obj->polyInfo)( obj, n );
		if (poly->numPnts == 2) nFaces++;
	}

	return nFaces;
}
#endif
