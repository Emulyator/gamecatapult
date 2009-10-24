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
#include <gctp/vector.hpp>
#include "dxexp.h"

/******  PROTOTYPES  ******************************/

static void DX_DisposeNormals                 ( DX_MeshNormals **meshNormals );
static void DX_DisposeTextureCoords           ( DX_MeshTextureCoords **meshTextureCoords );
static void DX_DisposeVertexDuplicationIndices( DX_VertexDuplicationIndices **vertexDuplicationIndices );
static void DX_DisposeVertexColors            ( DX_VertexColors **meshVertexColors );
static void DX_DisposeXSkinMeshHeader         ( DX_XSkinMeshHeader **xskinMeshHeader );
static void DX_DisposeSkinWeights             ( DX_SkinWeights **skinWeights );

static DX_DWORD  DX_NumNodeTQVertices( DX_Mesh *dx_msh );
static DX_DWORD  DX_NumNodeTQFaces   ( DX_Mesh *dx_msh );
static DX_DWORD          DX_NumNodeLineVertices( DX_Mesh *dx_msh );
static DX_DWORD          DX_NumNodeWires       ( DX_Mesh *dx_msh );

/*
 *
 *  MakeMesh    - make a new mesh
 *
 */

DX_Mesh* dxExporter::MakeMesh( LWW::MeshInfo mesh )
{
	DX_STRING     nodeName, tempName;

	DX_Mesh*  dx_msh = new DX_Mesh;
	//dx_msh->lwmesh                   = mesh;
	dx_msh->valences                 = MakeValences( mesh, ET_TQPOLY );
	dx_msh->nVertices                = DX_NumNodeTQVertices( dx_msh );
	dx_msh->nFaces                   = DX_NumNodeTQFaces   ( dx_msh );
	dx_msh->vertices                 = MakeVertices                ( dx_msh );
	dx_msh->faces                    = MakeFaces                   ( dx_msh );
	dx_msh->meshNormals              = MakeNormals                 ( dx_msh );
	dx_msh->meshTextureCoords        = MakeTextureCoords           ( dx_msh );
	dx_msh->vertexDuplicationIndices = MakeVertexDuplicationIndices( dx_msh );
	dx_msh->meshVertexColors         = MakeVertexColors            ( dx_msh );
	dx_msh->meshMaterialList         = MakeMaterialList            ( dx_msh );
	dx_msh->xskinMeshHeader          = NULL;
	dx_msh->skinWeights              = NULL;
	dx_msh->wirevalences             = MakeValences( mesh, ET_LINEPOLY );
	dx_msh->wire                     = MakeWire                    ( dx_msh );
	dx_msh->wireTextureCoords        = MakeTextureCoords           ( dx_msh, ET_LINEPOLY );
	dx_msh->wireVertexColors         = MakeVertexColors            ( dx_msh, ET_LINEPOLY );
	dx_msh->wireMaterialList         = MakeMaterialList            ( dx_msh, ET_LINEPOLY );
	dx_msh->next                     = NULL;

	DX_FileUniqName( tempName, mesh.filename() );
	if (mesh.globals()->maxLayers(mesh.objIdx()) > 1) {
		sprintf( nodeName, "Mesh_%s_%s", tempName, mesh.name() );
	} else {
		sprintf( nodeName, "Mesh_%s", tempName );
	}
	DX_MakeNodeName( dx_msh->nodeName, nodeName );

	DX_DWORD wirenum = DX_NumNodeWires( dx_msh );
	if (mesh.globals()->maxLayers(mesh.objIdx()) > 1) {
		sprintf( nodeName, "Wire_%s_%s", tempName, mesh.name() );
	} else {
		sprintf( nodeName, "Wire_%s", tempName );
	}
	dx_msh->wire_name = nodeName;

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
		DX_DisposeVertexColors            ( &dx_msh->meshVertexColors );
		DX_DisposeXSkinMeshHeader         ( &dx_msh->xskinMeshHeader );
		DX_DisposeSkinWeights             ( &dx_msh->skinWeights );
		delete dx_msh->wire;
		DX_DisposeTextureCoords           ( &dx_msh->wireTextureCoords );
		DX_DisposeVertexColors            ( &dx_msh->wireVertexColors );
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
	VPointData          point;

	DX_Vector*  vertices = new DX_Vector[dx_msh->nVertices];

	while ((entry = ValenceGetEntried( dx_val, entry )))
	{
		point = ValenceVertex( dx_val, entry );

		vertices[nVertices].x = (DX_FLOAT) point.position[0];
		vertices[nVertices].y = (DX_FLOAT) point.position[1];
		vertices[nVertices].z = (DX_FLOAT) point.position[2];
		nVertices++;
	}

	return vertices;
}

/*
 *  MakeFaces   - make faces
 */

DX_MeshFace* dxExporter::MakeFaces( DX_Mesh *dx_msh )
{
	DX_MeshFace *faces = new DX_MeshFace[dx_msh->nFaces];
	for(size_t i = 0; i < dx_msh->nFaces; i++) {
		faces[i].nFaceVertexIndices = dx_msh->valences->polids[i].pnum;
		faces[i].faceVertexIndices  = new DX_DWORD[faces[i].nFaceVertexIndices];
		for (size_t m = 0; m < faces[i].nFaceVertexIndices; m++) {
			faces[i].faceVertexIndices[m] = ValenceIndex( dx_msh->valences, i, m );
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

	DX_Valence*         dx_val = dx_msh->valences;
	DX_VertexEntry      *entry  = NULL;
	DX_DWORD            nNormals     = 0;
	VNormalData         norm;

	DX_Vector *normals = new DX_Vector[dx_msh->nVertices];

	while ((entry = ValenceGetEntried( dx_msh->valences, entry )))
	{
		norm = ValenceNormal( dx_msh->valences, entry );

		normals[nNormals].x = (DX_FLOAT) norm.vector[0];
		normals[nNormals].y = (DX_FLOAT) norm.vector[1];
		normals[nNormals].z = (DX_FLOAT) norm.vector[2];
		nNormals++;
	}

	DX_MeshFace *faceNormals = new DX_MeshFace[dx_msh->nFaces];
	for(size_t i = 0; i < dx_val->polids.size(); i++) {
		faceNormals[i].nFaceVertexIndices = dx_msh->valences->polids[i].pnum;
		faceNormals[i].faceVertexIndices  = new DX_DWORD[faceNormals[i].nFaceVertexIndices];

		for(size_t j = 0; j < faceNormals[i].nFaceVertexIndices; j++) {
			faceNormals[i].faceVertexIndices[j] = ValenceIndex( dx_val, i, j );
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

	if(globals_.getObjectFuncs()->numVMaps( LWVMAP_TXUV ) == 0) return NULL;

	DX_Valence*            dx_val = (type == ET_LINEPOLY)? dx_msh->wirevalences : dx_msh->valences;
	DX_VertexEntry*        entry  = NULL;
	DX_DWORD               nTextureCoords = 0;
	VMapData               map;

	DX_Coord2d*  textureCoords = new DX_Coord2d[(type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices];

	while ((entry = ValenceGetEntried( dx_val, entry )))
	{
		map = ValenceUVCoord( dx_val, entry );

		if (map.ismapped) { // —LŒø‚È’l
			textureCoords[nTextureCoords].u = (DX_FLOAT) map.u;
			textureCoords[nTextureCoords].v = (DX_FLOAT) map.v;
		} else {
			textureCoords[nTextureCoords].u = 0.0f;
			textureCoords[nTextureCoords].v = 0.0f;
		}
		nTextureCoords++;
	}

	DX_MeshTextureCoords *meshTextureCoords = new DX_MeshTextureCoords;
	meshTextureCoords->nTextureCoords = (type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices;
	meshTextureCoords->textureCoords  = textureCoords;

	return meshTextureCoords;
}



/*
 *  MakeVertexDuplicationIndices   - make mesh texture coords
 */

DX_VertexDuplicationIndices* dxExporter::MakeVertexDuplicationIndices( DX_Mesh* dx_msh, EntryType type )
{
	DX_Valence*           dx_val = (type == ET_LINEPOLY)? dx_msh->wirevalences : dx_msh->valences;
	DX_VertexEntry*       entry  = NULL;
	DX_DWORD              nIndices = 0, nPoints = 0;

	DX_VertexDuplicationIndices*  vertexDuplicationIndices = NULL;

	if (dx_msh->nVertices > (DX_DWORD) dx_val->nPoints)
	{
		DX_DWORD*  indices = new DX_DWORD[(type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices];

		while ((entry = ValenceGetEntried( dx_val, entry )))
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
	if (!_options->outputVertexColors) return NULL;
	if(globals_.getObjectFuncs()->numVMaps( LWVMAP_RGB ) == 0 && globals_.getObjectFuncs()->numVMaps( LWVMAP_RGBA ) == 0) return NULL;

	DX_DWORD              nVertexColors = 0;
	VColorData            col;

	DX_IndexColor*  vertexColors = new DX_IndexColor[(type == ET_LINEPOLY)? dx_msh->wire->vert->num : dx_msh->nVertices];

	DX_VertexEntry*       entry  = NULL;
	DX_Valence*           dx_val = (type == ET_LINEPOLY)? dx_msh->wirevalences : dx_msh->valences;
	while ((entry = ValenceGetEntried( dx_val, entry )))
	{
		col = ValenceColor( dx_val, entry );

		if(col.ismapped) {
			vertexColors[nVertexColors].indexColor.red   = (DX_FLOAT) col.r;
			vertexColors[nVertexColors].indexColor.green = (DX_FLOAT) col.g;
			vertexColors[nVertexColors].indexColor.blue  = (DX_FLOAT) col.b;
			vertexColors[nVertexColors].indexColor.alpha = (DX_FLOAT) col.a;
		}
		else {
			const char *surfname = dx_val->lwmesh.polTag(dx_val->polids[entry->pol_no].id, LWPTAG_SURF);
			LWW::Surface surf = globals_.getSurface(surfname, dx_val->lwmesh.filename());
			double *colr = surf.getFlt(SURF_DIFF);
			vertexColors[nVertexColors].indexColor.red   = (DX_FLOAT) colr[0];
			vertexColors[nVertexColors].indexColor.green = (DX_FLOAT) colr[1];
			vertexColors[nVertexColors].indexColor.blue  = (DX_FLOAT) colr[2];
			vertexColors[nVertexColors].indexColor.alpha = (DX_FLOAT) colr[3];
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
	DX_Valence *dx_val = (type == ET_LINEPOLY)? dx_msh->wirevalences : dx_msh->valences;
	std::vector<LWW::Surface> surfs = globals_.getSurfacesByObject(dx_val->lwmesh.filename());
	DX_DWORD nMaterials  = (DX_DWORD) surfs.size();
	DX_DWORD nFaceIndexes = (type == ET_LINEPOLY)? dx_msh->wire->idx->num : dx_msh->nFaces;
	DX_DWORD *faceIndexes = new DX_DWORD[nFaceIndexes];
	bool *uselist = new bool[nMaterials];
	for (DX_DWORD n = 0; n < nMaterials; n++) uselist[n] = false;
	
	for(size_t i = 0; i < dx_val->polids.size() && i < nFaceIndexes; i++) {
		const char *surfname = dx_val->lwmesh.polTag(dx_val->polids[i].id, LWPTAG_SURF);
		std::vector<LWW::Surface> thissurf = dx_val->lwmesh.globals()->getSurfacesByName(surfname, dx_val->lwmesh.filename());
		for(int j = 0; j < (int)surfs.size(); j++) {
			if(thissurf.front().id() == surfs[j].id()) {
				faceIndexes[i] = j;
				uselist[j] = true;
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
	meshMaterialList->materials    = MakeMaterials( NULL, dx_val->lwmesh, uselist );
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
 *  DX_NumNodeTQVertices   - count number of node vertices
 */

static DX_DWORD  DX_NumNodeTQVertices( DX_Mesh *dx_msh )
{
	DX_Valence*         dx_val = dx_msh->valences;

	return dx_val->nIndicies;
}


/*
 *  DX_NumNodeTQFaces   - count number of node faces
 */

static DX_DWORD  DX_NumNodeTQFaces( DX_Mesh* dx_msh )
{
	DX_Valence*         dx_val = dx_msh->valences;
	return dx_val->polids.size();
}

/*
 *  MakeWire    - make wireframe data
 */

gctp::xext::Wire *dxExporter::MakeWire( DX_Mesh *dx_msh )
{
	DX_Valence *dx_val = dx_msh->wirevalences;
	DX_VertexEntry*     entry  = NULL;
	DX_DWORD            nVertices = 0;
	VPointData          point;

	int vnum = DX_NumNodeLineVertices(dx_msh), pnum = DX_NumNodeWires( dx_msh );
	gctp::xext::Wire *ret = new gctp::xext::Wire(vnum, pnum);

	ret->vert->num = vnum;
	while ((entry = ValenceGetEntried( dx_val, entry )))
	{
		point = ValenceVertex( dx_val, entry );

		ret->vert->verticies[nVertices].x = (DX_FLOAT) point.position[0];
		ret->vert->verticies[nVertices].y = (DX_FLOAT) point.position[1];
		ret->vert->verticies[nVertices].z = (DX_FLOAT) point.position[2];
		nVertices++;
	}
	
	ret->idx->num = pnum;

	for(size_t i = 0; i < dx_val->polids.size(); i++) {
		ret->idx->indicies[i].start = ValenceIndex( dx_val, i, 0 );
		ret->idx->indicies[i].end = ValenceIndex( dx_val, i, 1 );
	}
	return ret;
}

/*
 *  DX_NumNodeLineVertices   - count number of node vertices
 */

static DX_DWORD  DX_NumNodeLineVertices( DX_Mesh *dx_msh )
{
	DX_Valence*         dx_val = dx_msh->wirevalences;

	return dx_val->nIndicies;
}

/*
 *  DX_NumNodeWires   - count number of node lines
 */

static DX_DWORD  DX_NumNodeWires( DX_Mesh* dx_msh )
{
	DX_Valence*         dx_val = dx_msh->wirevalences;
	return dx_val->polids.size();
}
