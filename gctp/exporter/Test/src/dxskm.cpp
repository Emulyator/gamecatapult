/*********************************************************************/
/*                                                                   */
/* FILE :        dxskm.cpp                                           */
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
#include "dxvec.h"


/******  TYPE AND STRUCTURES  *********************/

typedef struct st_DX_FaceEntry {

	DX_DWORD                      number;
	struct st_DX_FaceEntry*       next;

} DX_FaceEntry;

typedef struct st_DX_SharedFaces {

	DX_DWORD                      nVertices;
	DX_FaceEntry*                 entries;

} DX_SharedFaces;

typedef struct st_DX_Weight {

	DX_INT                        index;
	DX_FLOAT                      w;

} DX_Weight;

typedef struct st_DX_PointWeight {

	LWPntID id;
	DX_Weight*                    weights;

} DX_PointWeight;

typedef struct st_DX_WeightMap {

	DX_Valence *dx_val;
	DX_INT                        nPnts;
	DX_INT                        nPols;
	DX_INT                        nWeights;
	DX_PointWeight*               points;

	int find(LWPntID pnt)
	{
		for(int i = 0; i < nPnts; i++) if(points[i].id == pnt) return i;
		return 0;
	}
} DX_WeightMap;


/******  PROTOTYPES  ******************************/


static DX_WeightMap*   DX_AllocateWeightMap      ( DX_Valence *dx_val, DX_INT nWeights );
static void            DX_DisposeWeightMap       ( DX_WeightMap** weightmap );
static void            DX_ClampWeightMapPerFace  ( DX_WeightMap* weightmap, DX_INT nMaxSkinWeightsPerFace );
static void            DX_ClampWeightMapPerVertex( DX_WeightMap* weightmap, DX_INT nMaxSkinWeightsPerVertex );

static DX_SharedFaces* DX_MakeSharedFaces        ( DX_Mesh* dx_msh );
static void            DX_DisposeSharedFaces     ( DX_SharedFaces **dx_shf );
static double          DX_WeightPoint2Line       ( float *p, float *s, float *e );
static double          DX_WeightGetFalloff       ( double dist, int falloff );

static int             DX_CompareWeights         ( const void *s, const void *d );
static void            DX_ClampSkinWeights       ( DX_Weight* weights, DX_INT nWeights, DX_INT nMaxSkinWeightsPerVertex );
static void            DX_NormalizeSkinWeights   ( DX_Weight* weights, DX_INT nWeights );
static void            DX_MeshSkinWeights        ( DX_Weight* weights, DX_INT nWeights );
static DX_DOUBLE       DX_GetSkinWeights         ( DX_Weight* weights, DX_INT index, DX_INT nWeights );


/*
 *
 *  MakeXSkinMesh   - make the skined mesh
 *
 */

DX_XSkinMeshHeader* dxExporter::MakeXSkinMesh( DX_Frame* dx_frm )
{
	if (dx_frm->mesh == NULL) return NULL;

	DX_DWORD nBones = CountBones( dx_frm );
	if (nBones == 0) return NULL;

	DX_XSkinMeshHeader*  dx_xsk = new DX_XSkinMeshHeader;
	dx_xsk->nMaxSkinWeightsPerVertex = 0;
	dx_xsk->nMaxSkinWeightsPerFace   = 0;
	dx_xsk->nBones                   = 0;

	if (dx_frm->mesh->skinWeights)
	{
		DX_Mesh*  dx_msh   = dx_frm->mesh;
		DX_DWORD  nArray   = dx_msh->nVertices > dx_msh->nFaces ? dx_msh->nVertices : dx_msh->nFaces;
		DX_DWORD* nWeights = new DX_DWORD[nArray];
		DX_DWORD  idx, n;

		DX_SkinWeights*  dx_skm = dx_frm->mesh->skinWeights;

		memset( nWeights, 0, sizeof(DX_DWORD) * nArray );

		while (dx_skm)
		{
			for (n = 0; n < dx_skm->nWeights; n++) {
				idx = dx_skm->vertexIndices[n];
				nWeights[ idx ] += 1;
				if (nWeights[ idx ] > dx_xsk->nMaxSkinWeightsPerVertex) {
					dx_xsk->nMaxSkinWeightsPerVertex = nWeights[ idx ];
				}
			}
			if (dx_skm->nWeights > 0) {
				dx_xsk->nBones++;
			}
			dx_skm = dx_skm->next;
		}

		DX_SharedFaces* dx_shf = DX_MakeSharedFaces( dx_msh );
		DX_DWORD*       nFaces = new DX_DWORD[dx_msh->nFaces];
		memset( nWeights, 0x00, dx_msh->nFaces * sizeof(DX_DWORD) );

		DX_FaceEntry*    entry;
		dx_skm = dx_frm->mesh->skinWeights;

		while (dx_skm)
		{
			memset( nFaces, 0x00, dx_msh->nFaces * sizeof(DX_DWORD) );

			for (n = 0; n < dx_skm->nWeights; n++) {
				idx   = dx_skm->vertexIndices[n];
				entry = dx_shf->entries[idx].next;
				while (entry)
				{
					if (nFaces[ entry->number ] == FALSE)
					{
						nFaces  [ entry->number ] = TRUE;
						nWeights[ entry->number ] += 1;
						if (nWeights[ entry->number ] > dx_xsk->nMaxSkinWeightsPerFace) {
							dx_xsk->nMaxSkinWeightsPerFace = nWeights[ entry->number ];
						}
					}
					entry = entry->next;
				}
			}
			dx_skm = dx_skm->next;
		}

		DX_DisposeSharedFaces( &dx_shf );
		delete nWeights;
		delete nFaces;
	}
	else
	{
		dx_xsk->nMaxSkinWeightsPerVertex = nBones + 1;
		dx_xsk->nMaxSkinWeightsPerFace   = nBones + 1;
		dx_xsk->nBones                   = nBones + 1;
	}

	dx_frm->mesh->xskinMeshHeader = dx_xsk;

	return dx_xsk;
}


/*
 *
 *  MakeSkinWeights   - make skin weights for the mesh
 *
 */

DX_SkinWeights* dxExporter::MakeSkinWeights( DX_Frame* dx_frm )
{
	if (dx_frm->mesh == NULL) return NULL;

	DX_DWORD nBones = CountBones( dx_frm );
	if (nBones == 0) return NULL;

										//  SkinWeights for the Mesh frame
	DX_SkinWeights*  dx_skms = AllocSkinWeight( NULL, dx_frm, dx_frm );
	DX_Frame*        dx_fbn  = dx_frm->next;
	DX_SkinWeights*  dx_skm;

	while (dx_fbn)
	{
		if (dx_fbn->frameType == DX_FRAME_BONE) 
		{
										//  SkinWeights for the Bone frame
			dx_skm = AllocSkinWeight( dx_skms, dx_frm, dx_fbn );
			dx_fbn = dx_fbn->next;
		}
		else {
			dx_fbn = NULL;
		}
	}

	if (dx_skms != NULL) {
		BuildSkinWeights( dx_skms, dx_frm );
	}
	dx_frm->mesh->skinWeights = dx_skms;

	return dx_skms;
}



/*
 *
 *  CountBones   - make the root frame
 *
 */

DX_DWORD dxExporter::CountBones( DX_Frame* dx_frm )
{
	DX_DWORD                nBones = 0;

	if (dx_frm->mesh == NULL) return nBones;

	dx_frm = dx_frm->next;

	while (dx_frm)
	{
		if (dx_frm->frameType == DX_FRAME_BONE) {
			nBones++;
		} else {
			return nBones;
		}
		dx_frm = dx_frm->next;
	}

	return nBones;
}


/*
 *
 *  AllocSkinWeight   - allocate a skin weight for the bone
 *
 */

DX_SkinWeights* dxExporter::AllocSkinWeight( DX_SkinWeights* dx_skms, DX_Frame* dx_frm, DX_Frame* dx_fbn )
{

	DX_SkinWeights*  dx_lst = dx_skms;
	if (dx_lst) {
		while (dx_lst->next) { dx_lst = dx_lst->next; }
	}

	DX_Mesh*         dx_msh = dx_frm->mesh;
	DX_SkinWeights*  dx_skm = new DX_SkinWeights;
	dx_skm->idx           = dx_lst ? (dx_lst->idx + 1) : 0;
	dx_skm->work          = dx_fbn;
	dx_skm->nWeights      = 0;
	dx_skm->vertexIndices = new DX_DWORD[dx_msh->nVertices];
	dx_skm->weights       = new DX_FLOAT[dx_msh->nVertices];
	dx_skm->next          = NULL;
	strcpy( dx_skm->transformNodeName, dx_fbn->nodeName );

	MakeInvRestTransform( dx_fbn->item, &dx_skm->matrixOffset );
	MakeRestTransform   ( dx_fbn->item, &dx_skm->boneTransform );
	MakeBonePositions   ( dx_fbn->item, dx_skm );

	if (dx_lst) dx_lst->next = dx_skm;

	return dx_skm;
}


/*
 *
 *  BuildSkinWeights   - build the skin weight for the bones
 *
 */

void dxExporter::BuildSkinWeights( DX_SkinWeights* dx_skms, DX_Frame* dx_frm )
{
	DX_Mesh *dx_msh = dx_frm->mesh;
	DX_Valence *dx_val = dx_msh->valences;
	DX_DWORD vmapCount = 0;
	DX_DWORD nWeights = 0;

	DX_SkinWeights *dx_skm = dx_skms;
	while (dx_skm) {
		DX_Frame *dx_fbn  = (DX_Frame *) dx_skm->work;

		if(dx_fbn->item.type() == LWI_BONE && (dx_fbn->item.flags()&LWBONEF_ACTIVE) ) {
			const char *wmname = dx_fbn->item.toBone().weightMap();
			if(wmname && strlen(wmname) > 0) vmapCount++;
		}
		nWeights++;
		dx_skm  = dx_skm->next;
	}

	DX_WeightMap *weightmap = DX_AllocateWeightMap( dx_val, (DX_INT) nWeights );
	DX_DOUBLE *values = new DX_DOUBLE[ nWeights ];

	for(size_t i = 0; i < dx_val->pntids.size(); i++) {
		LWPntID point = dx_val->pntids[i];
		weightmap->points[i].id = point;
		DX_Weight *weights = weightmap->points[i].weights;
		LWFVector pos;
		dx_val->lwmesh.pntBasePos(point, pos);
		//  Use Weight VMAP
		if (vmapCount > 0) {
			DX_INT wht_no = 0;
			DX_SkinWeights *dx_skm = dx_skms;

			while (dx_skm)
			{
				DX_Frame *dx_fbn  = (DX_Frame *) dx_skm->work;

				weights[wht_no].w     = 0.0f;
				weights[wht_no].index = wht_no;

				if(dx_fbn->item.type() == LWI_BONE && (dx_fbn->item.flags()&LWBONEF_ACTIVE) ) {
					float val;
					void *wght = dx_val->lwmesh.pntVLookup( LWVMAP_WGHT, const_cast<char *>((dx_fbn->item.toBone().weightMap())) );
					if (wght && dx_val->lwmesh.pntVIDGet( point, &val, wght )) {
						weights[wht_no].w = val > EPSILON_F ? val : 0.0f;
					}
				}
				dx_skm = dx_skm->next; wht_no++;
			}
		}
		//  Calculate Bone Weights
		else {
			DX_SkinWeights *dx_skm = dx_skms;
			DX_INT wht_no = 0;
			DX_INT numZero = 0;
			DX_DOUBLE minValue;
			while (dx_skm)
			{
				float pos[3];
				dx_val->lwmesh.pntBasePos(point, pos);
				double dd = DX_WeightPoint2Line( pos, dx_skm->boneRestStart, dx_skm->boneRestEnd );
				weights[wht_no].w     = 0.0f;
				weights[wht_no].index = wht_no;
				if (wht_no == 0) {
					minValue = dd;
				} else if (dd < minValue) {
					minValue = dd;
				}
				values[wht_no] = dd;
				if (dd < DBL_EPSILON) numZero++;
				dx_skm = dx_skm->next; wht_no++;
			}
			if (numZero > 0)
			{
				wht_no   = 0;
				dx_skm   = dx_skms;

				while (dx_skm) {
					weights[wht_no].w = (values[wht_no] < DBL_EPSILON) ? 1.0f : 0.0f;
					dx_skm = dx_skm->next; wht_no++;
				}
			}
			else
			{
				double sum = 0.0;
				wht_no   = 0;
				dx_skm   = dx_skms;

				while (dx_skm) {
					DX_Frame *dx_fbn  = (DX_Frame *) dx_skm->work;
					if(dx_fbn->item.type() == LWI_BONE && (dx_fbn->item.flags()&LWBONEF_ACTIVE) ) {
						double dd = DX_WeightGetFalloff( values[wht_no]/minValue, dx_fbn->item.toBone().falloff() );
						if (dd >= DBL_MAX) numZero++;
						sum           += dd;
						values[wht_no] = dd;
					} else {
						values[wht_no] = 0.0f;
					}
					dx_skm = dx_skm->next; wht_no++;
				}
				if (numZero > 0)
				{
					wht_no   = 0;
					dx_skm   = dx_skms;
					while (dx_skm) {
						weights[wht_no].w = (values[wht_no] >= DBL_MAX) ? 1.0f : 0.0f;
						dx_skm = dx_skm->next; wht_no++;
					}
				}
				else
				{
					wht_no   = 0;
					dx_skm   = dx_skms;
					while (dx_skm) {
						weights[wht_no].w = (float) (values[wht_no] / sum);
						dx_skm = dx_skm->next; wht_no++;
					}
				}
			}
		}
		DX_NormalizeSkinWeights( weights, (DX_INT) nWeights );
	}

	if (_options->useSkinWeightsPerVertex) {
		DX_ClampWeightMapPerFace  ( weightmap, _options->nMaxSkinWeightsPerFace );
		DX_ClampWeightMapPerVertex( weightmap, _options->nMaxSkinWeightsPerVertex );
	}

	DX_DWORD         index;
	DX_DOUBLE        total, w;
	DX_VertexEntry   *entry  = NULL;

	while ((entry = ValenceGetEntried( dx_val, entry )))
	{
		DX_Weight *weights = weightmap->points[ entry->pnt_no ].weights;
		total   = 0.0;
		for(int wht_no = 0; wht_no < weightmap->nWeights; wht_no++) {
			total += weights[wht_no].w;
		}

		if (total < EPSILON_F) {
			DX_MeshSkinWeights( weights, (DX_INT) nWeights );
		} else {
			DX_NormalizeSkinWeights( weights, (DX_INT) nWeights );
		}

		index  = 0;
		DX_SkinWeights *dx_skm = dx_skms;

		while (dx_skm)
		{
			w = DX_GetSkinWeights( weights, index, (DX_INT) nWeights );

			if (w > EPSILON_F)
			{
				dx_skm->vertexIndices[ dx_skm->nWeights ] = entry->index;
				dx_skm->weights      [ dx_skm->nWeights ] = (DX_FLOAT) w;
				dx_skm->nWeights++;
			}

			index++;
			dx_skm  = dx_skm->next;
		}
	}

	DX_DisposeWeightMap( &weightmap );
	delete values;
}


/*
 *
 *  MakeRestTransform   - make the rest transform matrix
 *
 */

void dxExporter::MakeRestTransform( LWW::Item item, DX_Matrix4x4* restTransform )
{
	DX_FLOAT               m[16];
	double                 vec[3];

	if (item.type() != LWI_BONE) {
		DX_MatrixIdentity( restTransform->matrix );
		return;
	}

	DX_MatrixIdentity   ( m );

	item.toBone().restParam( LWIP_ROTATION, vec );
	DX_MatrixRoationZ   ( m, (DX_FLOAT) vec[2] );
	DX_MatrixRoationX   ( m, (DX_FLOAT) vec[1] );
	DX_MatrixRoationY   ( m, (DX_FLOAT) vec[0] );

	item.param( LWIP_PIVOT_ROT, 0.0, vec );
	DX_MatrixRoationZ( m, (DX_FLOAT) vec[2] );
	DX_MatrixRoationX( m, (DX_FLOAT) vec[1] );
	DX_MatrixRoationY( m, (DX_FLOAT) vec[0] );

	item.toBone().restParam( LWIP_POSITION, vec );
	DX_MatrixTranslation( m, (DX_FLOAT) vec[0], 
					   		 (DX_FLOAT) vec[1], 
					   		 (DX_FLOAT) vec[2] );

	while( (item = item.parent()) != NULL && item.type() == LWI_BONE ) {
		item.toBone().restParam( LWIP_ROTATION, vec );
		DX_MatrixRoationZ   ( m, (DX_FLOAT) vec[2] );
		DX_MatrixRoationX   ( m, (DX_FLOAT) vec[1] );
		DX_MatrixRoationY   ( m, (DX_FLOAT) vec[0] );
		item.param( LWIP_PIVOT_ROT, 0.0, vec );
		DX_MatrixRoationZ   ( m, (DX_FLOAT) vec[2] );
		DX_MatrixRoationX   ( m, (DX_FLOAT) vec[1] );
		DX_MatrixRoationY   ( m, (DX_FLOAT) vec[0] );
		item.toBone().restParam( LWIP_POSITION, vec );
		DX_MatrixTranslation( m, (DX_FLOAT) vec[0], 
					   	   		 (DX_FLOAT) vec[1], 
					       		 (DX_FLOAT) vec[2] );
	}

	memcpy( restTransform->matrix, m, sizeof(m) );
}


/*
 *
 *  MakeInvRestTransform   - make the inverse rest transform matrix
 *
 */

static void DX_InvRestTransform( LWW::Item item, DX_FLOAT* m )
{
	LWW::Item              parent;
	double                vec[3];

	parent = item.parent();

	if (parent != NULL && parent.type() == LWI_BONE)
	{
		DX_InvRestTransform( parent, m );
	}

	item.toBone().restParam( LWIP_POSITION, vec );
	DX_MatrixTranslation( m, (DX_FLOAT) -vec[0], 
					   		 (DX_FLOAT) -vec[1], 
					   		 (DX_FLOAT) -vec[2] );

	item.param( LWIP_PIVOT_ROT, 0.0, vec );
	DX_MatrixRoationY   ( m, (DX_FLOAT) -vec[0] );
	DX_MatrixRoationX   ( m, (DX_FLOAT) -vec[1] );
	DX_MatrixRoationZ   ( m, (DX_FLOAT) -vec[2] );

	item.toBone().restParam( LWIP_ROTATION, vec );
	DX_MatrixRoationY   ( m, (DX_FLOAT) -vec[0] );
	DX_MatrixRoationX   ( m, (DX_FLOAT) -vec[1] );
	DX_MatrixRoationZ   ( m, (DX_FLOAT) -vec[2] );
}

void dxExporter::MakeInvRestTransform( LWW::Item item, DX_Matrix4x4* invRestTransform )
{
	DX_FLOAT               m[16];
	
	if (item.type() != LWI_BONE) {
		DX_MatrixIdentity( invRestTransform->matrix );
		return;
	}

	DX_MatrixIdentity( m );
	DX_InvRestTransform( item, m );

	memcpy( invRestTransform->matrix, m, sizeof(m) );
}


/*
 *
 *  MakeBonePositions   - make start and end positions at the Mesh Coordinates
 *
 */

void dxExporter::MakeBonePositions( LWW::Item item, DX_SkinWeights* dx_skm )
{
	DX_FLOAT               V[3];

	if(item.type() != LWI_BONE) return;

	V[0] = 0.0f;
	V[1] = 0.0f;
	V[2] = 0.0f;
	DX_Vec3TransformCoord( dx_skm->boneRestStart, V, dx_skm->boneTransform.matrix );

	V[0] = 0.0f;
	V[1] = 0.0f;
	V[2] = (DX_FLOAT)item.toBone().restLength();
	DX_Vec3TransformCoord( dx_skm->boneRestEnd, V, dx_skm->boneTransform.matrix );
}



/*
 *  DX_AllocateWeightMap      ALLOCATE WEIGHT MAP
 */
static DX_WeightMap* DX_AllocateWeightMap( DX_Valence *dx_val, DX_INT nWeights )
{
	DX_WeightMap*  weightmap = new DX_WeightMap;
	weightmap->dx_val   = dx_val;
	weightmap->nPnts    = (DX_INT) dx_val->pntids.size();
	weightmap->nPols    = (DX_INT) dx_val->polids.size();
	weightmap->nWeights = nWeights;
	weightmap->points   = new DX_PointWeight[weightmap->nPnts];
	for (DX_INT n = 0; n < weightmap->nPnts; n++) {
		weightmap->points[n].weights = new DX_Weight[nWeights];
		memset( weightmap->points[n].weights, 0, sizeof(DX_Weight) * nWeights );
	}
	return weightmap;
}



/*
 *  DX_DisposeWeightMap      DISPOSE WEIGHT MAP
 */
static void DX_DisposeWeightMap( DX_WeightMap** weightmap )
{
	if (*weightmap) {
		for (DX_INT n = 0; n < (*weightmap)->nPnts; n++) {
			delete[] (*weightmap)->points[n].weights;
		}
		delete[] (*weightmap)->points;
		delete   (*weightmap);
		(*weightmap) = NULL;
	}
}



/*
 *  DX_ClampWeightMapPerFace      CLAMP WEIGHT MAP PER FACE
 */
static void DX_ClampWeightMapPerFace( DX_WeightMap *weightmap, DX_INT nMaxSkinWeightsPerFace )
{
	if (nMaxSkinWeightsPerFace > weightmap->nWeights) nMaxSkinWeightsPerFace = weightmap->nWeights;

	DX_Weight *weights = new DX_Weight[weightmap->nWeights];

	for(size_t i = 0; i < weightmap->dx_val->polids.size(); i++) {
		DX_PolygonInfo &polygon = weightmap->dx_val->polids[i];
		for (int wht_no = 0; wht_no < weightmap->nWeights; wht_no++) {
			weights[wht_no].index = wht_no;
			weights[wht_no].w     = 0.0f;
		}
		//LWFVector pos;
		//mesh.pntBasePos(point, pos);
		//polygon = (*obj->polyInfo)( obj, pol_no );
		for (int vtx_no = 0; vtx_no < polygon.pnum; vtx_no++) {
			//pnt_no = polygon->points[vtx_no];
			int pnt_no = weightmap->find(weightmap->dx_val->lwmesh.polVertex(polygon.id, polygon.ivert[vtx_no]));
			for (int seq_no = 0; seq_no < weightmap->nWeights; seq_no++) {
				int wht_no = weightmap->points[pnt_no].weights[seq_no].index;
				weights[wht_no].w += weightmap->points[pnt_no].weights[seq_no].w;
			}
		}
		qsort( weights, weightmap->nWeights, sizeof(DX_Weight), DX_CompareWeights );
		for (int seq_no = nMaxSkinWeightsPerFace; seq_no < weightmap->nWeights; seq_no++) {
			weights[seq_no].w = 0.0f;
		}
		for (int vtx_no = 0; vtx_no < polygon.pnum; vtx_no++) {
			//pnt_no = polygon->points[vtx_no];
			int pnt_no = weightmap->find(weightmap->dx_val->lwmesh.polVertex(polygon.id, polygon.ivert[vtx_no]));
			for (int seq_no = 0; seq_no < weightmap->nWeights; seq_no++) {
				int wht_no = weightmap->points[pnt_no].weights[seq_no].index;
				DX_FLOAT w = (DX_FLOAT) DX_GetSkinWeights( weights, wht_no, weightmap->nWeights );
				if (w == 0.0f) {
					weightmap->points[pnt_no].weights[seq_no].w = 0.0f;
				}
			}
		}
	}

	for(int pnt_no = 0; pnt_no < weightmap->nPnts; pnt_no++) {
		DX_NormalizeSkinWeights( weightmap->points[pnt_no].weights, weightmap->nWeights );
	}

	delete[] weights;
}



/*
 *  DX_ClampWeightMapPerVertex      CLAMP WEIGHT MAP PER VERTEX
 */
static void DX_ClampWeightMapPerVertex( DX_WeightMap* weightmap, DX_INT nMaxSkinWeightsPerVertex )
{
	if (nMaxSkinWeightsPerVertex > weightmap->nWeights) nMaxSkinWeightsPerVertex = weightmap->nWeights;

	DX_INT      seq_no, pnt_no;

	for (pnt_no = 0; pnt_no < weightmap->nPnts; pnt_no++) 
	{
		for (seq_no = 0; seq_no < weightmap->nWeights; seq_no++) {
			if (weightmap->points[pnt_no].weights[seq_no].w <= EPSILON_F) {
				weightmap->points[pnt_no].weights[seq_no].w = 0.0f;
			}
		}
		qsort( weightmap->points[pnt_no].weights, weightmap->nWeights, sizeof(DX_Weight), DX_CompareWeights );

		DX_NormalizeSkinWeights( weightmap->points[pnt_no].weights, nMaxSkinWeightsPerVertex );

		for (seq_no = nMaxSkinWeightsPerVertex; seq_no < weightmap->nWeights; seq_no++) {
			weightmap->points[pnt_no].weights[seq_no].w = 0.0f;
		}
	}
}



/*
 *  DX_CompareWeights      QUICK SORT DRIVER FUNCTION
 */
static int DX_CompareWeights( const void *s, const void *d )
{
	if ( ((DX_Weight *)(s))->w == ((DX_Weight *)(d))->w) 
		return 0;
	else if ( ((DX_Weight *)(s))->w > ((DX_Weight *)(d))->w) 
		return -1;
	else 
		return 1;
}


/*
 *
 *  DX_ClampSkinWeights   - clamp a skin weight for the bone
 *
 */
 
static void DX_ClampSkinWeights( DX_Weight* weights, DX_INT nWeights, DX_INT nMaxSkinWeightsPerVertex )
{
	DX_DOUBLE               total = 0.0;
	DX_INT                  n;

	if (nMaxSkinWeightsPerVertex > nWeights) nMaxSkinWeightsPerVertex = nWeights;

	for (n = 0, total = 0.0; n < nWeights; n++) {
		if (weights[n].w > EPSILON_F) total += weights[n].w;
		else                          weights[n].w = 0.0f;
	}

	qsort( weights, nWeights, sizeof(DX_Weight), DX_CompareWeights );

	for (n = 0, total = 0.0; n < nMaxSkinWeightsPerVertex; n++) {
		if (weights[n].w > EPSILON_F) total += weights[n].w;
		else                          weights[n].w = 0.0f;
	}

	for (n = 0; n < nMaxSkinWeightsPerVertex; n++) {
		if (total > EPSILON_F) weights[n].w = (DX_FLOAT) (weights[n].w / total);
		else                   weights[n].w = 0.0f;
	}

	for (n = nMaxSkinWeightsPerVertex; n < nWeights; n++) {
		weights[n].w = 0.0f;
	}
}


/*
 *
 *  DX_NormalizeSkinWeights   - normalize a skin weight for the bone
 *
 */
 
static void DX_NormalizeSkinWeights( DX_Weight* weights, DX_INT nWeights )
{
	DX_DOUBLE               total = 0.0;
	DX_INT                  n;

	for (n = 0; n < nWeights; n++) {
		if (weights[n].w > EPSILON_F) total += weights[n].w;
		else                          weights[n].w = 0.0f;
	}

	for (n = 0; n < nWeights; n++) {
		if (total > EPSILON_F) weights[n].w = (DX_FLOAT) (weights[n].w / total);
		else                   weights[n].w = 0.0f;
	}
}


/*
 *
 *  DX_MeshSkinWeights   - skin weight for the mesh
 *
 */
 
static void DX_MeshSkinWeights( DX_Weight* weights, DX_INT nWeights )
{
	DX_INT                  n;

	weights[0].w = 1.0f;
	for (n = 1; n < nWeights; n++) {
		weights[n].w = 0.0f;
	}
}


/*
 *
 *  DX_GetSkinWeights   - get the skin weight for the bone
 *
 */
 
static DX_DOUBLE DX_GetSkinWeights( DX_Weight* weights, DX_INT index, DX_INT nWeights )
{
	DX_INT                  n;

	for (n = 0; n < nWeights; n++) {
		if (weights[n].index == index) return weights[n].w;
	}

	return 0.0;
}


/*
 *
 *  DX_MakeSharedFaces   - make the shared faces for the mesh
 *
 */

static DX_SharedFaces* DX_MakeSharedFaces( DX_Mesh* dx_msh )
{
	DX_SharedFaces*         dx_shf;
	DX_FaceEntry*           entry;
	DX_DWORD                idx, n, m;

	dx_shf = new DX_SharedFaces;
	dx_shf->nVertices = dx_msh->nVertices;
	dx_shf->entries   = new DX_FaceEntry[dx_shf->nVertices];
	memset( dx_shf->entries, 0, sizeof(DX_FaceEntry) * dx_shf->nVertices );

	for (n = 0; n < dx_msh->nFaces; n++)
	{
		for (m = 0; m < dx_msh->faces[n].nFaceVertexIndices; m++)
		{
			idx   = dx_msh->faces[n].faceVertexIndices[m];
			entry = new DX_FaceEntry;
			memset( entry, 0, sizeof(DX_FaceEntry) );
			entry->number = n;
			if (dx_shf->entries[ idx ].next) {
				entry->next = dx_shf->entries[ idx ].next;
				dx_shf->entries[ idx ].next = entry;
			} else {
				entry->next = NULL;
				dx_shf->entries[ idx ].next = entry;
			}
			dx_shf->entries[ idx ].number++;
		}
	}

	return dx_shf;
}


/*
 *
 *  DX_DisposeSharedFaces   - dispose the shared faces
 *
 */

static void DX_DisposeSharedFaces( DX_SharedFaces **dx_shf )
{
	DX_FaceEntry            *face, *temp;
	DX_DWORD                n;

	if (*dx_shf)
	{
		for (n = 0; n < (*dx_shf)->nVertices; n++)
		{
			face = (*dx_shf)->entries[n].next;
			while (face) {
				temp = face->next;
				delete face;
				face = temp;
			}
		}
		delete[] (*dx_shf)->entries;
		delete (*dx_shf);
		*dx_shf = NULL;
	}
}



/*
 *  DX_WeightPoint2Line      CALCULATE THE DISTANCE BETWEEN POINT AND LINE
 */
static double DX_WeightPoint2Line( float *p, float *s, float *e )
{
	const   double accy = EPSILON_D;
	double	dist, xsp, ysp, zsp, xes, yes, zes;
	double	denom, xfac, yfac, zfac, t;

	xsp = s[0] - p[0];
	ysp = s[1] - p[1];
	zsp = s[2] - p[2];
	xes = e[0] - s[0];
	yes = e[1] - s[1];
	zes = e[2] - s[2];

	denom = xes*xes + yes*yes + zes*zes;

	if (denom < accy)
	{
		dist = sqrt(xsp*xsp + ysp*ysp + zsp*zsp);
	} 
	else {
		t = -(xsp*xes + ysp*yes + zsp*zes) / denom;
		if (t > 1.0) t = 1.0;
		if (t < 0.0) t = 0.0;
		xfac = xsp + t * xes;
		yfac = ysp + t * yes;
		zfac = zsp + t * zes;
		dist = sqrt(xfac*xfac + yfac*yfac + zfac*zfac);
	}

	return dist;
}



/*
 *  DX_WeightGetFalloff      GET WEIGHT VALUE WITH FALLOFF
 */
static double DX_WeightGetFalloff( double dist, int falloff )
{
	double    minValue = DBL_MIN * 10.0;
	double    maxValue = DBL_MAX * 0.1;

	while (falloff--) {
		if (dist < minValue) {
			return DBL_MAX;
		} else if (dist > maxValue) {
			return DBL_MIN;
		} else {
			dist *= dist;
		}
	}
	return 1.0 / dist;
}






