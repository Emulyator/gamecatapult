/*********************************************************************/
/*                                                                   */
/* FILE :        wrmsh.c                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to eS Export                           */
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

/******  PROTOTYPES  ******************************/

#include <gctp/bbstream.hpp>
#include <rmxfguid.h>
#include <d3dx9mesh.h>

/*
 *
 *  WriteMesh   - write a Layer to Mesh
 *
 */

void dxExporter::WriteMesh( DX_Mesh* dx_msh, gctp::XSaver &cur )
{
	DX_MeshFace*   face;
	DX_DWORD       n, m;

	if(dx_msh->nVertices > 0 && dx_msh->nFaces > 0) {
		/*
		*  Vertices
		*/
		gctp::bbstream strm;
		strm << dx_msh->nVertices;
		for (n = 0; n < dx_msh->nVertices; n++) {
			strm << static_cast<float>(dx_msh->vertices[n].x * _options->scale)
				<< static_cast<float>(dx_msh->vertices[n].y * _options->scale)
				<< static_cast<float>(dx_msh->vertices[n].z * _options->scale);
		}
		/*
		*  Faces
		*/
		strm << dx_msh->nFaces;
		for (n = 0; n < dx_msh->nFaces; n++) {
			face = dx_msh->faces + n;
			strm << face->nFaceVertexIndices;
			bool notzero = false;
			for (m = 0; m < face->nFaceVertexIndices; m++) {
				strm << face->faceVertexIndices[m];
				if(face->faceVertexIndices[m]) notzero = true;
			}
			assert(notzero);
		}

		gctp::XSaveData _mesh = cur.newData(TID_D3DRMMesh, strm.pcount(), strm.buf(), dx_msh->nodeName);

		/*
		*  MeshNormals
		*/
		WriteMeshNormals( dx_msh, _mesh );

		/*
		*  MeshTextureCoords
		*/
		WriteMeshTextureCoords( dx_msh, _mesh );

		/*
		*  VertexDuplicationIndices
		*/
		WriteVertexDuplicationIndices( dx_msh, _mesh );

		/*
		*  MeshVertexColor
		*/
		WriteMeshVertexColor( dx_msh, _mesh );

		/*
		*  MeshMaterialList
		*/
		WriteMeshMaterialList( dx_msh, _mesh );

		/*
		*  XSkinMeshHeader
		*/
		WriteXSkinMeshHeader( dx_msh, _mesh );

		/*
		*  SkinWeights
		*/
		WriteSkinWeights( dx_msh, _mesh );
	}

	if(dx_msh->wire && dx_msh->wire->vert && dx_msh->wire->vert->num > 0 && dx_msh->wire->idx && dx_msh->wire->idx->num > 0) {
		/*
		*  gctp::Wire
		*/
		gctp::XSaveData _wire = dx_msh->wire->write(dx_msh->wire_name.c_str(), cur);
		WriteMeshTextureCoords( dx_msh, _wire, true );
		WriteMeshVertexColor( dx_msh, _wire, true );
		WriteMeshMaterialList( dx_msh, _wire, true );
	}
}


/*
 *
 *  WriteMeshNormals   - write MeshNormals
 *
 */

void dxExporter::WriteMeshNormals( DX_Mesh* dx_msh, gctp::XSaver &cur )
{
	DX_MeshNormals*    meshNormals = dx_msh->meshNormals;
	DX_MeshFace*       face;
	DX_DWORD           n, m;

	if (meshNormals == NULL) return;
	if (_options->outputMeshNormals == FALSE) return;

	/*
	 *  Normals
	 */
	gctp::bbstream strm;
	strm << meshNormals->nNormals;
	for(n = 0; n < meshNormals->nNormals; n++) {
		strm << meshNormals->normals[n].x
			 << meshNormals->normals[n].y
			 << meshNormals->normals[n].z;
	}
	/*
	 *  FaceNormals
	 */
	strm << meshNormals->nFaceNormals;
	for(n = 0; n < meshNormals->nFaceNormals; n++) {
		face = meshNormals->faceNormals + n;
		strm << face->nFaceVertexIndices;
		for (m = 0; m < face->nFaceVertexIndices; m++) {
			strm << face->faceVertexIndices[m];
		}
	}

	cur.newData(TID_D3DRMMeshNormals, strm.pcount(), strm.buf());
}


/*
 *
 *  WriteMeshTextureCoords   - write MeshTextureCoords
 *
 */

void dxExporter::WriteMeshTextureCoords( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire )
{
	DX_MeshTextureCoords*  meshTextureCoords = is_wire ? dx_msh->wireTextureCoords : dx_msh->meshTextureCoords;
	DX_DWORD               n;
	DX_FLOAT               u, v;

	if (meshTextureCoords == NULL) return;
	if (_options->outputTextureCoords == FALSE) return;

	/*
	 *  TextureCoords
	 */
	gctp::bbstream strm;
	strm << meshTextureCoords->nTextureCoords;
	for (n = 0; n < meshTextureCoords->nTextureCoords; n++) 
	{
		u = meshTextureCoords->textureCoords[n].u;
		v = meshTextureCoords->textureCoords[n].v;

		if (_options->reverseUVCoords == TRUE) {
			v = 1.0f - v;
		}

		strm << u << v;
	}
	
	cur.newData(TID_D3DRMMeshTextureCoords, strm.pcount(), strm.buf());
}


/*
 *
 *  TXT_WriteVertexDuplicationIndices   - write VertexDuplicationIndices
 *
 */

void dxExporter::WriteVertexDuplicationIndices( DX_Mesh* dx_msh, gctp::XSaver &cur )
{
	DX_VertexDuplicationIndices*  vertexDuplicationIndices = dx_msh->vertexDuplicationIndices;
	DX_DWORD                      n;

	if (vertexDuplicationIndices == NULL) return;

	/*
	 *  VertexDuplicationIndices
	 */
	gctp::bbstream strm;
	strm << vertexDuplicationIndices->nIndices;
	strm << vertexDuplicationIndices->nOriginalVertices;
	for (n = 0; n < vertexDuplicationIndices->nIndices; n++) {
		strm << vertexDuplicationIndices->indices[n];
	}

	cur.newData(DXFILEOBJ_VertexDuplicationIndices, strm.pcount(), strm.buf());
}

/*
 *
 *  WriteMeshVertexColor   - write MeshVertexColor
 *
 */

void dxExporter::WriteMeshVertexColor( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire )
{
	DX_VertexColors*  meshVertexColor = is_wire ? dx_msh->meshVertexColors : dx_msh->meshVertexColors;
	DX_DWORD          n;

	if (meshVertexColor == NULL) return;
	if (_options->outputVertexColors == FALSE) return;

	gctp::bbstream strm;
	/*
	 *  MeshVertexColor
	 */
	strm << meshVertexColor->nVertexColors;
	for (n = 0; n < meshVertexColor->nVertexColors; n++) {
		strm << meshVertexColor->vertexColors[n].index
			 << meshVertexColor->vertexColors[n].indexColor.red
			 << meshVertexColor->vertexColors[n].indexColor.green
			 << meshVertexColor->vertexColors[n].indexColor.blue
			 << meshVertexColor->vertexColors[n].indexColor.alpha;
	}

	cur.newData(TID_D3DRMMeshVertexColors, strm.pcount(), strm.buf());
}


/*
 *
 *  WriteMeshMaterialList   - write MeshMaterialList
 *
 */

void dxExporter::WriteMeshMaterialList( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire )
{
	DX_MeshMaterialList*  meshMaterialList = is_wire ? dx_msh->wireMaterialList: dx_msh->meshMaterialList;
	DX_Material*          material;
	DX_DWORD              n;

	if (meshMaterialList == NULL) return;

	gctp::bbstream strm;
	/*
	 *  MeshMaterialList
	 */
	strm << meshMaterialList->nMaterials;
	strm << meshMaterialList->nFaceIndexes;
	for (n = 0; n < meshMaterialList->nFaceIndexes; n++) {
		strm << meshMaterialList->faceIndexes[n];
	}
	gctp::XSaveData mtrllist = cur.newData(TID_D3DRMMeshMaterialList, strm.pcount(), strm.buf());

	material = meshMaterialList->materials;
	for (n = 0; n < meshMaterialList->nMaterials; n++)
	{
		switch (_options->materialType) {
		case DX_MATTYPE_INLINE:
			WriteMaterial( material, mtrllist );
			break;
		case DX_MATTYPE_EXTERN:
			cur.addRefData(material->nodeName);
			break;
		case DX_MATTYPE_SHARED:
			cur.addRefData(material->surfName);
			break;
		}
		material = material->next;
	}
}


/*
 *
 *  TXT_WriteXSkinMeshHeader   - write XSkinMeshHeader
 *
 */

void dxExporter::WriteXSkinMeshHeader( DX_Mesh* dx_msh, gctp::XSaver &cur )
{
	DX_XSkinMeshHeader*  xskinMeshHeader = dx_msh->xskinMeshHeader;

	if (xskinMeshHeader == NULL) return;
	/*
	 *  XSkinMeshHeader
	 */
	cur.newData(DXFILEOBJ_XSkinMeshHeader, sizeof(DX_XSkinMeshHeader), xskinMeshHeader);
}


/*
 *
 *  WriteSkinWeights   - write SkinWeights
 *
 */

void dxExporter::WriteSkinWeights( DX_Mesh* dx_msh, gctp::XSaver &cur )
{
	DX_SkinWeights*  skinWeights = dx_msh->skinWeights;
	DX_DWORD         n;

	while (skinWeights)
	{
		if (skinWeights->nWeights > 0)
		{
			gctp::bbstream strm;
			strm << skinWeights->transformNodeName
			     << skinWeights->nWeights;
			for (n = 0; n < skinWeights->nWeights; n++) {
				strm << skinWeights->vertexIndices[n];
			}
			for (n = 0; n < skinWeights->nWeights; n++) {
				strm << skinWeights->weights[n];
			}

			strm << skinWeights->matrixOffset.matrix[ 0]
				 << skinWeights->matrixOffset.matrix[ 1]
				 << skinWeights->matrixOffset.matrix[ 2]
				 << skinWeights->matrixOffset.matrix[ 3]
				 << skinWeights->matrixOffset.matrix[ 4]
				 << skinWeights->matrixOffset.matrix[ 5]
				 << skinWeights->matrixOffset.matrix[ 6]
				 << skinWeights->matrixOffset.matrix[ 7]
				 << skinWeights->matrixOffset.matrix[ 8]
				 << skinWeights->matrixOffset.matrix[ 9]
				 << skinWeights->matrixOffset.matrix[10]
				 << skinWeights->matrixOffset.matrix[11]
				 << skinWeights->matrixOffset.matrix[12]
				 << skinWeights->matrixOffset.matrix[13]
				 << skinWeights->matrixOffset.matrix[14]
				 << skinWeights->matrixOffset.matrix[15];

			cur.newData(DXFILEOBJ_SkinWeights, strm.pcount(), strm.buf());
		}

		skinWeights = skinWeights->next;
	}
}
