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
#include <gctp/vector.hpp>
#include <gctp/dbgout.hpp>

#include "dxexp.h"
#include "dxvec.h"
#include "triple.hpp"

#define COMP_VECS(p,q)	(fabs((p)->vector[0]-(q)->vector[0]) < EPSILON_F && \
						 fabs((p)->vector[1]-(q)->vector[1]) < EPSILON_F && \
						 fabs((p)->vector[2]-(q)->vector[2]) < EPSILON_F)

#define COMP_MAPS(p,q)	(fabs((p)->u-(q)->u) < EPSILON_F && \
						 fabs((p)->v-(q)->v) < EPSILON_F)

#define COMP_COLS(p,q)	(fabs((p)->r-(q)->r) < EPSILON_F && \
						 fabs((p)->g-(q)->g) < EPSILON_F && \
						 fabs((p)->b-(q)->b) < EPSILON_F && \
						 fabs((p)->a-(q)->a) < EPSILON_F)

#define COMP_VECS2(p,q)	(fabs((p)[0]-(q)[0]) < EPSILON_F && \
						 fabs((p)[1]-(q)[1]) < EPSILON_F && \
						 fabs((p)[2]-(q)[2]) < EPSILON_F)

#define COMP_MAPS2(p,q)	(fabs((p)[0]-(q)[0]) < EPSILON_F && \
						 fabs((p)[1]-(q)[1]) < EPSILON_F)

#define COMP_COLS2(p,q)	(fabs((p)[0]-(q)[0]) < EPSILON_F && \
						 fabs((p)[1]-(q)[1]) < EPSILON_F && \
						 fabs((p)[2]-(q)[2]) < EPSILON_F && \
						 fabs((p)[3]-(q)[3]) < EPSILON_F)

/******  PROTOTYPES  ******************************/

static void DX_DumpValences ( DX_Valence* dx_val );

/*
 *
 *  MakeValences   - make valences from the object
 *
 */

DX_Valence* dxExporter::MakeValences(LWW::MeshInfo mesh, EntryType type)
{
	DX_PointEntry*      pointEntry;
	DX_VertexEntry*     vertexEntry;

	DX_Valence*  dx_val = new DX_Valence;
	dx_val->lwmesh = mesh;
	dx_val->nEntries = 0;
	dx_val->nIndicies = 0;

	int num_morph_target = globals().getObjectFuncs()->numVMaps(LWVMAP_MORF); // 相対モーフ
	for(int i = 0; i < num_morph_target; i++) {
		const char *name = globals().getObjectFuncs()->vmapName(LWVMAP_MORF, i);
		dx_val->rmorph[name] = mesh.pntVLookup(LWVMAP_MORF, name);
	}
	//vmap = mesh.pntVLookup(LWVMAP_MORF, vmap_name);
	// Mesh ____MorfTarget__

	num_morph_target = globals().getObjectFuncs()->numVMaps(LWVMAP_SPOT); // 絶対モーフ
	for(int i = 0; i < num_morph_target; i++) {
		const char *name = globals().getObjectFuncs()->vmapName(LWVMAP_SPOT, i);
		dx_val->amorph[name] = mesh.pntVLookup(LWVMAP_SPOT, name);
	}

	LWW::MeshInfo::PolygonItr politr(dx_val->lwmesh);
	while(politr.next()) {
		if(dx_val->lwmesh.polType(politr.current()) == LWPOLTYPE_FACE) {
			int vn = dx_val->lwmesh.polSize(politr.current());
			if((type == ET_LINEPOLY && vn == 2) || (type == ET_TQPOLY && vn > 2)) {
				//dx_val->polid_to_idx[politr.current()] = (int)dx_val->polids.size();
				Trianguler tri(dx_val->lwmesh, politr.current(), _options->tripleAll != 0);
				for(size_t j = 0; j < tri.polys.size(); j++) {
					DX_PolygonInfo polinfo;
					polinfo.id = politr.current();
					polinfo.pnum = (int)tri.polys[j].points.size();
					for(int i = 0; i < polinfo.pnum; i++) polinfo.ivert[i] = tri.polys[j].points[i];
					dx_val->polids.push_back(polinfo);
					for(int i = 0; i < polinfo.pnum; i++) {
						LWPntID pid = dx_val->lwmesh.polVertex(politr.current(), tri.polys[j].points[i]);
						if(dx_val->pntid_to_idx.find(pid) == dx_val->pntid_to_idx.end()) {
							dx_val->pntid_to_idx[pid] = (int)dx_val->pntids.size();
							dx_val->pntids.push_back(pid);
							dx_val->shareinfo.push_back(VShareInfo());
							dx_val->shareinfo.back().pols.push_back(dx_val->polids.size()-1);
							dx_val->shareinfo.back().ivert.push_back(tri.polys[j].points[i]);
						}
						else {
							VShareInfo &shr = dx_val->shareinfo[dx_val->pntid_to_idx[pid]];
							shr.pols.push_back(dx_val->polids.size()-1);
							shr.ivert.push_back(tri.polys[j].points[i]);
						}
					}
				}
			}
		}
	}

	dx_val->nPoints = dx_val->pntids.size();
	dx_val->points = new DX_PointEntry[dx_val->nPoints];

	for(size_t i = 0; i < dx_val->pntids.size(); i++) {
		pointEntry = dx_val->points + i;

		VShareInfo &shr = dx_val->shareinfo[i];
		pointEntry->vertices = NULL;

		for(size_t val_no = 0; val_no < shr.pols.size(); val_no++) {
			vertexEntry = new DX_VertexEntry;
			vertexEntry->pnt_no = i;
			vertexEntry->pol_no = shr.pols[val_no];
			vertexEntry->ivert  = shr.ivert[val_no];
			vertexEntry->next   = NULL;

			EntryValence( dx_val, pointEntry, vertexEntry );
		}
	}
	DX_DumpValences( dx_val );
	return dx_val;
}


/*
 *
 *  ValenceIndex   - get the EntryNo with PolNO and VertexIndex
 *
 */

DX_DWORD dxExporter::ValenceIndex( DX_Valence* dx_val, int pol_no, int ivert )
{
	int pnt_no = dx_val->pntid_to_idx[dx_val->lwmesh.polVertex(dx_val->polids[pol_no].id, dx_val->polids[pol_no].ivert[ivert])];
	DX_VertexEntry *vertexEntry = dx_val->points[ pnt_no ].vertices;

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

DX_VertexEntry* dxExporter::ValenceGetEntried( DX_Valence *dx_val, DX_VertexEntry *entry )
{
	// 要するにこれは、チェーンリストを深さ優先でトラバースしている
	int pnt_no = 0;

	if(entry) {
		DX_VertexEntry *nextEntry = entry->next;
		while (nextEntry) {
			if(nextEntry->entried) return nextEntry;
			nextEntry = nextEntry->next;
		}
		pnt_no = entry->pnt_no + 1;
	}

	DX_VertexEntry *nextEntry = 0;
	for(;pnt_no < dx_val->nPoints; pnt_no++) {
		DX_PointEntry *point = dx_val->points + pnt_no;
		DX_VertexEntry *nextEntry = point->vertices;
		while (nextEntry) {
			if(nextEntry->entried) return nextEntry;
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

DX_DWORD  dxExporter::ValenceOriginal( DX_Valence *dx_val, DX_VertexEntry *entry )
{
	return dx_val->points[ entry->pnt_no ].vertices->index;
}


/*
 *
 *  ValenceVertex   - get the vertex data
 *
 */

VPointData dxExporter::ValenceVertex( DX_Valence *dx_val, DX_VertexEntry *entry )
{
	VPointData ret;
	dx_val->lwmesh.pntBasePos(dx_val->pntids[entry->pnt_no], ret.position);
	return ret;
}


/*
 *
 *  ValenceNormal   - get the normal data
 *
 */

VNormalData dxExporter::ValenceNormal( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	const char *surfname = dx_val->lwmesh.polTag(dx_val->polids[entry->pol_no].id, LWPTAG_SURF);
	if(surfname) {
		LWW::Surface surf = globals_.getSurface(surfname, dx_val->lwmesh.filename());
		if(surf) {
			double sang = *surf.getFlt(SURF_SMAN); // 符号が+-かでスムースオンかどうかがわかる（ドキュメントに書けよ…）
			if(sang > 0) {
				float cos_sang = cosf((float)sang);
				VNormalData ret;
				LWFVector polnorm;
				dx_val->lwmesh.polBaseNormal(dx_val->polids[entry->pol_no].id, polnorm);
				VShareInfo &shr = dx_val->shareinfo[entry->pnt_no];
				ret.vector[0] = 0;
				ret.vector[1] = 0;
				ret.vector[2] = 0;
				int div = 0;
				for(size_t i = 0; i < shr.pols.size(); i++) {
					LWFVector n;
					dx_val->lwmesh.polBaseNormal(dx_val->polids[shr.pols[i]].id, n);
					if((polnorm[0]*n[0]+polnorm[1]*n[1]+polnorm[2]*n[2]) >= cos_sang) {
						ret.vector[0] += n[0];
						ret.vector[1] += n[1];
						ret.vector[2] += n[2];
						div++;
					}
				}
				if(div) {
					ret.vector[0] /= div;
					ret.vector[1] /= div;
					ret.vector[2] /= div;
					float len = sqrtf(ret.vector[0]*ret.vector[0]+ret.vector[1]*ret.vector[1]+ret.vector[2]*ret.vector[2]);
					ret.vector[0] /= len;
					ret.vector[1] /= len;
					ret.vector[2] /= len;
					return ret;
				}
			}
			/*VNormalData pntnorm, polnorm;
			dx_val->lwmesh.pntBaseNormal(dx_val->polids[entry->pol_no].id, dx_val->pntids[entry->pnt_no], pntnorm.vector);
			dx_val->lwmesh.polBaseNormal(dx_val->polids[entry->pol_no].id, polnorm.vector);
			return (sang > 0 && dot(pntnorm, polnorm) >= cos(sang/2.0)) ? pntnorm : polnorm;*/
		}
	}
	VNormalData polnorm;
	dx_val->lwmesh.polBaseNormal(dx_val->polids[entry->pol_no].id, polnorm.vector);
	return polnorm;
}


/*
 *
 *  ValenceUVCoord   - get the UV coord data
 *
 */

VMapData dxExporter::ValenceUVCoord( DX_Valence *dx_val, DX_VertexEntry *entry )
{
	VMapData ret;
	ret.ismapped = false;
	LWPolID pol = dx_val->polids[entry->pol_no].id;
	const char *surfname = dx_val->lwmesh.polTag(pol, LWPTAG_SURF);
	if(surfname) {
		LWW::Surface surf = globals_.getSurface(surfname, dx_val->lwmesh.filename());
		if(surf) {
			LWW::Texture tex = surf.getTex(SURF_COLR);
			if(tex) {
				LWW::TextureLayer tl = tex.firstLayer();
				while(tl) {
					int projtype;
					if(tl.getParam(TXTAG_PROJ, &projtype)) {
						if(projtype == TXPRJ_UVMAP) {
							void *vmap;
							if(tl.getParam(TXTAG_VMAP, &vmap)) {
								if(vmap) {
									ret.ismapped = dx_val->lwmesh.pntVPIDGet(dx_val->pntids[entry->pnt_no], pol, &ret.u, vmap);
									if(!ret.ismapped) {
										ret.ismapped = dx_val->lwmesh.pntVIDGet(dx_val->pntids[entry->pnt_no], &ret.u, vmap);
									}
								}
							}
						}
						else {
							ret.ismapped = true;
							LWFVector pos, polnorm;
							dx_val->lwmesh.pntBasePos(dx_val->pntids[entry->pnt_no], pos);
							dx_val->lwmesh.polBaseNormal(pol, polnorm);
							int vec;
							if(fabsf(polnorm[0]) > fabsf(polnorm[1])) {
								if(fabsf(polnorm[0]) > fabsf(polnorm[2])) vec = 0;
								else vec = 2;
							}
							else if(fabsf(polnorm[1]) > fabsf(polnorm[2])) vec = 1;
							else vec = 2;
							LWDVector p = {pos[0], pos[1], pos[2]};
							double uv[2];
							tl.evaluateUV(vec, vec, p, p, uv);
							ret.u = (float)uv[0];
							ret.v = (float)uv[1];
							ret.w = 0;
						}
						break;
					}
					tl = tl.next();
				}
			}
		}
	}
	return ret;
}


/*
 *
 *  ValenceColor   - get the vertex color data
 *
 */

VColorData dxExporter::ValenceColor( DX_Valence* dx_val, DX_VertexEntry* entry )
{
	VColorData ret;
	LWPolID pol = dx_val->polids[entry->pol_no].id;
	const char *vcname = dx_val->lwmesh.polTag(pol, LWPTAG_COLR);
	ret.ismapped = dx_val->lwmesh.pntVPIDGet(dx_val->pntids[entry->pnt_no], pol, &ret.r, dx_val->lwmesh.pntVLookup(LWPTAG_COLR, vcname));
	return ret;
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

	if (*dx_val)
	{
		for (int pnt_no = 0; pnt_no < (*dx_val)->nPoints; pnt_no++)
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
int dxExporter::EntryValence( DX_Valence *dx_val, DX_PointEntry *point_entry, DX_VertexEntry *new_entry )
{
	DX_VertexEntry*      vertexEntry, *lastEntry;

	//poly_new    = (*obj->polyInfo)( obj, newEntry->pol_no );
	//newEntry->num_pnts = poly_new->numPnts;
	new_entry->num_pnts = dx_val->polids[new_entry->pol_no].pnum;
	//単なる頂点？この場合はあるのか？
	if(new_entry->num_pnts <= 0) return FALSE;

	if(point_entry->vertices == NULL) {
		dx_val->nEntries++;
		new_entry->index      = dx_val->nIndicies++;
		new_entry->entried    = TRUE;
		point_entry->vertices = new_entry;
		return TRUE;
	}
	else {
		lastEntry = point_entry->vertices;
		while (lastEntry->next) { lastEntry = lastEntry->next; }
	}

	for(vertexEntry = point_entry->vertices; vertexEntry; vertexEntry = vertexEntry->next) {
		//poly = (*obj->polyInfo)( obj, vertexEntry->pol_no );

		{
			VNormalData norm = ValenceNormal(dx_val, vertexEntry);
			VNormalData newnorm = ValenceNormal(dx_val, new_entry);
			if(COMP_VECS2(newnorm.vector, norm.vector) == FALSE) continue;
		}
		{
			LWPolID pol = dx_val->polids[vertexEntry->pol_no].id;
			const char *uvname = dx_val->lwmesh.polTag(pol, LWPTAG_TXUV);
			float uvw[3];
			int ismapped = dx_val->lwmesh.pntVPIDGet(dx_val->pntids[vertexEntry->pnt_no], pol, uvw, dx_val->lwmesh.pntVLookup(LWVMAP_TXUV, uvname));
			LWPolID newpol = dx_val->polids[new_entry->pol_no].id;
			const char *newuvname = dx_val->lwmesh.polTag(newpol, LWPTAG_TXUV);
			float newuvw[3];
			int newismapped = dx_val->lwmesh.pntVPIDGet(dx_val->pntids[new_entry->pnt_no], newpol, newuvw, dx_val->lwmesh.pntVLookup(LWVMAP_TXUV, newuvname));
			if (ismapped != newismapped || COMP_MAPS2(newuvw, uvw) == FALSE) continue;
		}
		{
			LWPolID pol = dx_val->polids[vertexEntry->pol_no].id;
			const char *vcname = dx_val->lwmesh.polTag(pol, LWPTAG_COLR);
			float color[4];
			int ismapped = dx_val->lwmesh.pntVPIDGet(dx_val->pntids[vertexEntry->pnt_no], pol, color, dx_val->lwmesh.pntVLookup(LWVMAP_TXUV, vcname));
			LWPolID newpol = dx_val->polids[new_entry->pol_no].id;
			const char *newvcname = dx_val->lwmesh.polTag(newpol, LWPTAG_COLR);
			float newcolor[4];
			int newismapped = dx_val->lwmesh.pntVPIDGet(dx_val->pntids[new_entry->pnt_no], newpol, newcolor, dx_val->lwmesh.pntVLookup(LWVMAP_TXUV, newvcname));
			if (ismapped != newismapped || COMP_COLS2(newcolor, color) == FALSE) continue;
		}

		/*if (poly_new->normals)
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
		else if (poly->colors) continue;*/

		new_entry->index = vertexEntry->index;
		new_entry->entried  = FALSE;
		lastEntry->next    = new_entry;
		return FALSE;
	}

	dx_val->nEntries++;
	new_entry->index = dx_val->nIndicies++;
	new_entry->entried  = TRUE;
	lastEntry->next    = new_entry;
	return TRUE;
}



/*
 *
 *  DX_DumpValences   - dump the valences
 *
 */

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
			/*int flag;
			LWFVector normal;
			flag = dx_val->lwmesh.pntBaseNormal(dx_val->polids[entry->pol_no], dx_val->pntids[entry->pnt_no], normal);
			PRNN(flag<<"\t"<<normal[0]<<", "<<normal[1]<<", "<<normal[2]);
			flag = dx_val->lwmesh.pntOtherNormal(dx_val->polids[entry->pol_no], dx_val->pntids[entry->pnt_no], normal);
			PRNN(flag<<"\t"<<normal[0]<<", "<<normal[1]<<", "<<normal[2]);
			flag = dx_val->lwmesh.polBaseNormal(dx_val->polids[entry->pol_no], normal);
			PRNN(flag<<"\t"<<normal[0]<<", "<<normal[1]<<", "<<normal[2]);
			flag = dx_val->lwmesh.polOtherNormal(dx_val->polids[entry->pol_no], normal);
			PRNN(flag<<"\t"<<normal[0]<<", "<<normal[1]<<", "<<normal[2]);*/
			entry = entry->next;
		}
	}
}
