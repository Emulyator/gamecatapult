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
#ifdef GCTP_USE_XFILEAPI
#include <gctp/vector.hpp>
#endif

#include "dxexp.h"
#include "dxvec.h"

/******  PROTOTYPES  ******************************/

#ifdef GCTP_USE_XFILEAPI

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
	OBJ2_ID        obj = dx_msh->obj;
	DX_MeshFace*   face;
	DX_DWORD       n, m;

	if (obj == NULL) return;

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
			for (m = 0; m < face->nFaceVertexIndices; m++) {
				strm << face->faceVertexIndices[m];
			}
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
		*  MeshFaceWraps
		*/
		WriteMeshFaceWraps( dx_msh, _mesh );

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

#ifdef GCTP_USE_XFILEAPI
	if(dx_msh->wire && dx_msh->wire->vert && dx_msh->wire->vert->num > 0 && dx_msh->wire->idx && dx_msh->wire->idx->num > 0) {
		/*
		*  gctp::Wire
		*/
		gctp::XSaveData _wire = dx_msh->wire->write(dx_msh->wire_name.c_str(), cur);
		WriteMeshTextureCoords( dx_msh, _wire, true );
		WriteMeshVertexColor( dx_msh, _wire, true );
		WriteMeshMaterialList( dx_msh, _wire, true );
	}
#endif
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
	for (n = 0; n < meshNormals->nFaceNormals; n++) {
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
#ifndef GCTP_USE_XFILEAPI
	if (_options->version <= DX_VERSION_DIRECTX7) return;
#endif

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
 *  WriteMeshFaceWraps   - write MeshFaceWraps
 *
 */

void dxExporter::WriteMeshFaceWraps( DX_Mesh* dx_msh, gctp::XSaver &cur )
{
	DX_MeshFaceWraps*  meshFaceWraps = dx_msh->meshFaceWraps;
	DX_DWORD           n;

	if (meshFaceWraps == NULL) return;

	gctp::bbstream strm;
	/*
	 *  MeshFaceWraps
	 */
	strm << meshFaceWraps->nFaceWrapValues;
	for (n = 0; n < meshFaceWraps->nFaceWrapValues; n++) {
		strm << meshFaceWraps->faceWrapValues[n].u.truefalse
			 << meshFaceWraps->faceWrapValues[n].v.truefalse;
	}

	cur.newData(TID_D3DRMMeshFaceWraps, strm.pcount(), strm.buf());
}


/*
 *
 *  TXT_WriteMeshVertexColor   - write MeshVertexColor
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
#ifndef GCTP_USE_XFILEAPI
	if (_options->version <= DX_VERSION_DIRECTX7) return;
#endif
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

#ifndef GCTP_USE_XFILEAPI
	if (_options->version <= DX_VERSION_DIRECTX7) return;
#endif
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

#else

/*
 *
 *  WriteMesh   - write a Layer to Mesh
 *
 */

void dxExporter::WriteMesh( DX_Mesh* dx_msh, int tabs )
{
	OBJ2_ID        obj = dx_msh->obj;

	if (obj == NULL) return;

	switch (_options->format) {
	case DX_FORMAT_BINARY:
		BIN_WriteMesh( dx_msh );
		break;
	case DX_FORMAT_TEXT:
		TXT_WriteMesh( dx_msh, tabs );
		break;
	}
}


/*
 *
 *  TXT_WriteMesh   - write a Layer to Mesh
 *
 */

void dxExporter::TXT_WriteMesh( DX_Mesh* dx_msh, int tabs )
{
	OBJ2_ID        obj = dx_msh->obj;
	DX_MeshFace*   face;
	DX_DWORD       n, m;

	if (obj == NULL) return;

	ffp( 0, "\n" );
	ffp( tabs, "Mesh %s {\n", dx_msh->nodeName );

	/*
	 *  Vertices
	 */
	ffp( tabs+1, "%d;\n", dx_msh->nVertices );
	for (n = 0; n < dx_msh->nVertices; n++) {
		ffp( tabs+1, "%f;%f;%f;", dx_msh->vertices[n].x * _options->scale,
									  	  dx_msh->vertices[n].y * _options->scale,
									  	  dx_msh->vertices[n].z * _options->scale );
		ffp( 0, "%c\n", DX_TERM(n, dx_msh->nVertices) );
	}
	/*
	 *  Faces
	 */
	ffp( tabs+1, "%d;\n", dx_msh->nFaces );
	for (n = 0; n < dx_msh->nFaces; n++) {
		face = dx_msh->faces + n;
		ffp( tabs+1, "%d;", face->nFaceVertexIndices );
		for (m = 0; m < face->nFaceVertexIndices; m++) {
			ffp( 0, "%d%c", face->faceVertexIndices[m], DX_TERM(m, face->nFaceVertexIndices) );
		}
		ffp( 0, "%c\n", DX_TERM(n, dx_msh->nFaces) );
	}

	/*
	 *  MeshNormals
	 */
	TXT_WriteMeshNormals( dx_msh, tabs+1 );

	/*
	 *  MeshTextureCoords
	 */
	TXT_WriteMeshTextureCoords( dx_msh, tabs+1 );

	/*
	 *  VertexDuplicationIndices
	 */
	TXT_WriteVertexDuplicationIndices( dx_msh, tabs+1 );

	/*
	 *  MeshFaceWraps
	 */
	TXT_WriteMeshFaceWraps( dx_msh, tabs+1 );

	/*
	 *  MeshVertexColor
	 */
	TXT_WriteMeshVertexColor( dx_msh, tabs+1 );

	/*
	 *  MeshMaterialList
	 */
	TXT_WriteMeshMaterialList( dx_msh, tabs+1 );

	/*
	 *  XSkinMeshHeader
	 */
	TXT_WriteXSkinMeshHeader( dx_msh, tabs+1 );

	/*
	 *  SkinWeights
	 */
	TXT_WriteSkinWeights( dx_msh, tabs+1 );

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteMeshNormals   - write MeshNormals
 *
 */

void dxExporter::TXT_WriteMeshNormals( DX_Mesh* dx_msh, int tabs )
{
	DX_MeshNormals*    meshNormals = dx_msh->meshNormals;
	DX_MeshFace*       face;
	DX_DWORD           n, m;

	if (meshNormals == NULL) return;
	if (_options->outputMeshNormals == FALSE) return;

	ffp( 0, "\n" );
	ffp( tabs, "MeshNormals {\n" );

	/*
	 *  Normals
	 */
	ffp( tabs+1, "%d;\n", meshNormals->nNormals );
	for (n = 0; n < meshNormals->nNormals; n++) {
		ffp( tabs+1, "%f;%f;%f;", meshNormals->normals[n].x,
									      meshNormals->normals[n].y,
									      meshNormals->normals[n].z );
		ffp( 0, "%c\n", DX_TERM(n, meshNormals->nNormals) );
	}
	/*
	 *  FaceNormals
	 */
	ffp( tabs+1, "%d;\n", meshNormals->nFaceNormals );
	for (n = 0; n < meshNormals->nFaceNormals; n++) {
		face = meshNormals->faceNormals + n;
		ffp( tabs+1, "%d;", face->nFaceVertexIndices );
		for (m = 0; m < face->nFaceVertexIndices; m++) {
			ffp( 0, "%d%c", face->faceVertexIndices[m], DX_TERM(m,face->nFaceVertexIndices) );
		}
		ffp( 0, "%c\n", DX_TERM(n, meshNormals->nFaceNormals) );
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteMeshTextureCoords   - write MeshTextureCoords
 *
 */

void dxExporter::TXT_WriteMeshTextureCoords( DX_Mesh* dx_msh, int tabs )
{
	DX_MeshTextureCoords*  meshTextureCoords = dx_msh->meshTextureCoords;
	DX_DWORD               n;
	DX_FLOAT               u, v;

	if (meshTextureCoords == NULL) return;
	if (_options->outputTextureCoords == FALSE) return;

	ffp( 0, "\n" );
	ffp( tabs, "MeshTextureCoords {\n" );

	/*
	 *  TextureCoords
	 */
	ffp( tabs+1, "%d;\n", meshTextureCoords->nTextureCoords );
	for (n = 0; n < meshTextureCoords->nTextureCoords; n++) 
	{
		u = meshTextureCoords->textureCoords[n].u;
		v = meshTextureCoords->textureCoords[n].v;

		if (_options->reverseUVCoords == TRUE) {
			v = 1.0f - v;
		}

		ffp( tabs+1, "%f;%f;", u, v );
		ffp( 0, "%c\n", DX_TERM(n, meshTextureCoords->nTextureCoords) );
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteVertexDuplicationIndices   - write VertexDuplicationIndices
 *
 */

void dxExporter::TXT_WriteVertexDuplicationIndices( DX_Mesh* dx_msh, int tabs )
{
	DX_VertexDuplicationIndices*  vertexDuplicationIndices = dx_msh->vertexDuplicationIndices;
	DX_DWORD                      n;

	if (vertexDuplicationIndices == NULL) return;
	if (_options->version <= DX_VERSION_DIRECTX7) return;

	ffp( 0, "\n" );
	ffp( tabs, "VertexDuplicationIndices {\n" );

	/*
	 *  VertexDuplicationIndices
	 */
	ffp( tabs+1, "%d;\n", vertexDuplicationIndices->nIndices );
	ffp( tabs+1, "%d;\n", vertexDuplicationIndices->nOriginalVertices );
	for (n = 0; n < vertexDuplicationIndices->nIndices; n++) {
		ffp( tabs+1, "%d", vertexDuplicationIndices->indices[n] );
		ffp( 0, "%c\n", DX_TERM(n, vertexDuplicationIndices->nIndices) );
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteMeshFaceWraps   - write MeshFaceWraps
 *
 */

void dxExporter::TXT_WriteMeshFaceWraps( DX_Mesh* dx_msh, int tabs )
{
	DX_MeshFaceWraps*  meshFaceWraps = dx_msh->meshFaceWraps;
	DX_DWORD           n;

	if (meshFaceWraps == NULL) return;

	ffp( 0, "\n" );
	ffp( tabs, "MeshFaceWraps {\n" );

	/*
	 *  MeshFaceWraps
	 */
	ffp( tabs+1, "%d;\n", meshFaceWraps->nFaceWrapValues );
	for (n = 0; n < meshFaceWraps->nFaceWrapValues; n++) {
		ffp( tabs+1, "%d;%d;", meshFaceWraps->faceWrapValues[n].u.truefalse,
								   	   meshFaceWraps->faceWrapValues[n].v.truefalse );
		ffp( 0, "%c\n", DX_TERM(n, meshFaceWraps->nFaceWrapValues) );
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteMeshVertexColor   - write MeshVertexColor
 *
 */

void dxExporter::TXT_WriteMeshVertexColor( DX_Mesh* dx_msh, int tabs )
{
	DX_VertexColors*  meshVertexColor = dx_msh->meshVertexColors;
	DX_DWORD          n;

	if (meshVertexColor == NULL) return;
	if (_options->outputVertexColors == FALSE) return;

	ffp( 0, "\n" );
	ffp( tabs, "MeshVertexColors {\n" );

	/*
	 *  MeshVertexColor
	 */
	ffp( tabs+1, "%d;\n", meshVertexColor->nVertexColors );
	for (n = 0; n < meshVertexColor->nVertexColors; n++) {
		ffp( tabs+1, "%d;%f;%f;%f;%f;", meshVertexColor->vertexColors[n].index,
										 	    meshVertexColor->vertexColors[n].indexColor.red,
										 	    meshVertexColor->vertexColors[n].indexColor.green,
										 	    meshVertexColor->vertexColors[n].indexColor.blue,
										 	    meshVertexColor->vertexColors[n].indexColor.alpha );
		ffp( 0, "%c\n", DX_TERM(n, meshVertexColor->nVertexColors) );
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteMeshMaterialList   - write MeshMaterialList
 *
 */

void dxExporter::TXT_WriteMeshMaterialList( DX_Mesh* dx_msh, int tabs )
{
	DX_MeshMaterialList*  meshMaterialList = dx_msh->meshMaterialList;
	DX_Material*          material;
	DX_DWORD              n;

	if (meshMaterialList == NULL) return;

	ffp( 0, "\n" );
	ffp( tabs, "MeshMaterialList {\n" );

	/*
	 *  MeshMaterialList
	 */
	ffp( tabs+1, "%d;\n", meshMaterialList->nMaterials );
	ffp( tabs+1, "%d;\n", meshMaterialList->nFaceIndexes );
	for (n = 0; n < meshMaterialList->nFaceIndexes; n++) {
		ffp( tabs+1, "%d", meshMaterialList->faceIndexes[n] );
		ffp( 0, "%c\n", DX_TERM(n, meshMaterialList->nFaceIndexes) );
	}
	material = meshMaterialList->materials;
	for (n = 0; n < meshMaterialList->nMaterials; n++)
	{
		switch (_options->materialType) {
		case DX_MATTYPE_INLINE:
			TXT_WriteMaterial( material, tabs+1 );
			break;
		case DX_MATTYPE_EXTERN:
			ffp( tabs+1, "{ %s }\n", material->nodeName );
			break;
		case DX_MATTYPE_SHARED:
			ffp( tabs+1, "{ %s }\n", material->surfName );
			break;
		}
		material = material->next;
	}

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteXSkinMeshHeader   - write XSkinMeshHeader
 *
 */

void dxExporter::TXT_WriteXSkinMeshHeader( DX_Mesh* dx_msh, int tabs )
{
	DX_XSkinMeshHeader*  xskinMeshHeader = dx_msh->xskinMeshHeader;

	if (xskinMeshHeader == NULL) return;
	if (_options->version <= DX_VERSION_DIRECTX7) return;

	ffp( 0, "\n" );
	ffp( tabs, "XSkinMeshHeader {\n" );

	/*
	 *  XSkinMeshHeader
	 */
	ffp( tabs+1, "%d;\n", xskinMeshHeader->nMaxSkinWeightsPerVertex );
	ffp( tabs+1, "%d;\n", xskinMeshHeader->nMaxSkinWeightsPerFace );
	ffp( tabs+1, "%d;\n", xskinMeshHeader->nBones );

	ffp( tabs, "}\n" );
}


/*
 *
 *  TXT_WriteSkinWeights   - write SkinWeights
 *
 */

void dxExporter::TXT_WriteSkinWeights( DX_Mesh* dx_msh, int tabs )
{
	DX_SkinWeights*  skinWeights = dx_msh->skinWeights;
	DX_DWORD         n;

	if (_options->version <= DX_VERSION_DIRECTX7) return;

	while (skinWeights)
	{
		if (skinWeights->nWeights > 0)
		{
			ffp( 0, "\n" );
			ffp( tabs, "SkinWeights {\n" );

			ffp( tabs+1, "\"%s\";\n", skinWeights->transformNodeName );
			ffp( tabs+1, "%d;\n", skinWeights->nWeights );
			for (n = 0; n < skinWeights->nWeights; n++) {
				ffp( tabs+1, "%d", skinWeights->vertexIndices[n] );
				ffp( 0, "%c\n", DX_TERM(n, skinWeights->nWeights) );
			}
			for (n = 0; n < skinWeights->nWeights; n++) {
				ffp( tabs+1, "%f", skinWeights->weights[n] );
				ffp( 0, "%c\n", DX_TERM(n, skinWeights->nWeights) );
			}

			ffp( tabs+1, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f;;\n",
				skinWeights->matrixOffset.matrix[ 0],
				skinWeights->matrixOffset.matrix[ 1],
				skinWeights->matrixOffset.matrix[ 2],
				skinWeights->matrixOffset.matrix[ 3],
				skinWeights->matrixOffset.matrix[ 4],
				skinWeights->matrixOffset.matrix[ 5],
				skinWeights->matrixOffset.matrix[ 6],
				skinWeights->matrixOffset.matrix[ 7],
				skinWeights->matrixOffset.matrix[ 8],
				skinWeights->matrixOffset.matrix[ 9],
				skinWeights->matrixOffset.matrix[10],
				skinWeights->matrixOffset.matrix[11],
				skinWeights->matrixOffset.matrix[12],
				skinWeights->matrixOffset.matrix[13],
				skinWeights->matrixOffset.matrix[14],
				skinWeights->matrixOffset.matrix[15] );

			ffp( tabs, "}\n" );
		}

		skinWeights = skinWeights->next;
	}
}




/*
 *
 *  BIN_WriteMesh   - write a Layer to Mesh
 *
 */

void dxExporter::BIN_WriteMesh( DX_Mesh* dx_msh )
{
	OBJ2_ID        obj = dx_msh->obj;
	DX_MeshFace*   face;
	DX_DWORD       n, m, num;
	DX_FLOAT       vals[3];

	if (obj == NULL) return;

	ffp_NAME  ( "Mesh" );
	ffp_NAME  ( dx_msh->nodeName );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  Vertices
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( dx_msh->nVertices );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( dx_msh->nVertices * 3 );
	for (n = 0; n < dx_msh->nVertices; n++) {
		vals[0] = (DX_FLOAT) (dx_msh->vertices[n].x * _options->scale);
		vals[1] = (DX_FLOAT) (dx_msh->vertices[n].y * _options->scale);
		vals[2] = (DX_FLOAT) (dx_msh->vertices[n].z * _options->scale);
		ffp_floats( vals, 3 );
	}
	/*
	 *  Faces
	 */
	for (n = 0, num = 1; n < dx_msh->nFaces; n++) {
		face  = dx_msh->faces + n;
		num  += face->nFaceVertexIndices + 1;
	}
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( num );
	ffp_dword ( dx_msh->nFaces );
	for (n = 0; n < dx_msh->nFaces; n++) {
		face  = dx_msh->faces + n;
		ffp_dword ( face->nFaceVertexIndices );
		for (m = 0; m < face->nFaceVertexIndices; m++) {
			ffp_dword ( face->faceVertexIndices[m] );
		}
	}

	/*
	 *  MeshNormals
	 */
	BIN_WriteMeshNormals( dx_msh );

	/*
	 *  MeshTextureCoords
	 */
	BIN_WriteMeshTextureCoords( dx_msh );

	/*
	 *  VertexDuplicationIndices
	 */
	BIN_WriteVertexDuplicationIndices( dx_msh );

	/*
	 *  MeshFaceWraps
	 */
	BIN_WriteMeshFaceWraps( dx_msh );

	/*
	 *  MeshVertexColor
	 */
	BIN_WriteMeshVertexColor( dx_msh );

	/*
	 *  MeshMaterialList
	 */
	BIN_WriteMeshMaterialList( dx_msh );

	/*
	 *  XSkinMeshHeader
	 */
	BIN_WriteXSkinMeshHeader( dx_msh );

	/*
	 *  SkinWeights
	 */
	BIN_WriteSkinWeights( dx_msh );

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteMeshNormals   - write MeshNormals
 *
 */

void dxExporter::BIN_WriteMeshNormals( DX_Mesh* dx_msh )
{
	DX_MeshNormals*    meshNormals = dx_msh->meshNormals;
	DX_MeshFace*       face;
	DX_DWORD           n, m, num;
	DX_FLOAT           vals[3];

	if (meshNormals == NULL) return;
	if (_options->outputMeshNormals == FALSE) return;

	ffp_NAME  ( "MeshNormals" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  Normals
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( meshNormals->nNormals );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( meshNormals->nNormals * 3 );
	for (n = 0; n < meshNormals->nNormals; n++) {
		vals[0] = meshNormals->normals[n].x;
		vals[1] = meshNormals->normals[n].y;
		vals[2] = meshNormals->normals[n].z;
		ffp_floats( vals, 3 );
	}
	/*
	 *  FaceNormals
	 */
	for (n = 0, num = 1; n < meshNormals->nFaceNormals; n++) {
		face  = meshNormals->faceNormals + n;
		num  += face->nFaceVertexIndices + 1;
	}
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( num );
	ffp_dword ( meshNormals->nFaceNormals );
	for (n = 0; n < meshNormals->nFaceNormals; n++) {
		face = meshNormals->faceNormals + n;
		ffp_dword ( face->nFaceVertexIndices );
		for (m = 0; m < face->nFaceVertexIndices; m++) {
			ffp_dword ( face->faceVertexIndices[m] );
		}
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteMeshTextureCoords   - write MeshTextureCoords
 *
 */

void dxExporter::BIN_WriteMeshTextureCoords( DX_Mesh* dx_msh )
{
	DX_MeshTextureCoords*  meshTextureCoords = dx_msh->meshTextureCoords;
	DX_DWORD               n;
	DX_FLOAT               vals[3];

	if (meshTextureCoords == NULL) return;
	if (_options->outputTextureCoords == FALSE) return;

	ffp_NAME  ( "MeshTextureCoords" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  TextureCoords
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( meshTextureCoords->nTextureCoords );
	ffp_TOKEN ( TOKEN_FLOAT_LIST );
	ffp_dword ( meshTextureCoords->nTextureCoords * 2 );
	for (n = 0; n < meshTextureCoords->nTextureCoords; n++) {
		vals[0] = meshTextureCoords->textureCoords[n].u;
		vals[1] = meshTextureCoords->textureCoords[n].v;

		if (_options->reverseUVCoords == TRUE) {
			vals[1] = 1.0f - vals[1];
		}
		ffp_floats( vals, 2 );
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteVertexDuplicationIndices   - write VertexDuplicationIndices
 *
 */

void dxExporter::BIN_WriteVertexDuplicationIndices( DX_Mesh* dx_msh )
{
	DX_VertexDuplicationIndices*  vertexDuplicationIndices = dx_msh->vertexDuplicationIndices;
	DX_DWORD                      n;

	if (vertexDuplicationIndices == NULL) return;
	if (_options->version <= DX_VERSION_DIRECTX7) return;

	ffp_NAME  ( "VertexDuplicationIndices" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  VertexDuplicationIndices
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( vertexDuplicationIndices->nIndices );
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( vertexDuplicationIndices->nOriginalVertices );
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( vertexDuplicationIndices->nIndices );
	for (n = 0; n < vertexDuplicationIndices->nIndices; n++) {
		ffp_dword ( vertexDuplicationIndices->indices[n] );
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteMeshFaceWraps   - write MeshFaceWraps
 *
 */

void dxExporter::BIN_WriteMeshFaceWraps( DX_Mesh* dx_msh )
{
	DX_MeshFaceWraps*  meshFaceWraps = dx_msh->meshFaceWraps;
	DX_DWORD           n;

	if (meshFaceWraps == NULL) return;

	ffp_NAME  ( "MeshFaceWraps" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  MeshFaceWraps
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( meshFaceWraps->nFaceWrapValues );
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( meshFaceWraps->nFaceWrapValues * 2 );
	for (n = 0; n < meshFaceWraps->nFaceWrapValues; n++) {
		ffp_dword ( meshFaceWraps->faceWrapValues[n].u.truefalse );
		ffp_dword ( meshFaceWraps->faceWrapValues[n].v.truefalse );
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteMeshVertexColor   - write MeshVertexColor
 *
 */

void dxExporter::BIN_WriteMeshVertexColor( DX_Mesh* dx_msh )
{
	DX_VertexColors*  meshVertexColor = dx_msh->meshVertexColors;
	DX_DWORD          n;

	if (meshVertexColor == NULL) return;
	if (_options->outputVertexColors == FALSE) return;

	ffp_NAME  ( "MeshVertexColors" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  MeshVertexColor
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 1 );
	ffp_dword ( meshVertexColor->nVertexColors );
	for (n = 0; n < meshVertexColor->nVertexColors; n++) {
		ffp_TOKEN ( TOKEN_INTEGER_LIST );
		ffp_dword ( 1 );
		ffp_dword ( meshVertexColor->vertexColors[n].index );
		ffp_TOKEN ( TOKEN_FLOAT_LIST );
		ffp_dword ( 4 );
		ffp_float ( meshVertexColor->vertexColors[n].indexColor.red );
		ffp_float ( meshVertexColor->vertexColors[n].indexColor.green );
		ffp_float ( meshVertexColor->vertexColors[n].indexColor.blue );
		ffp_float ( meshVertexColor->vertexColors[n].indexColor.alpha );
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteMeshMaterialList   - write MeshMaterialList
 *
 */

void dxExporter::BIN_WriteMeshMaterialList( DX_Mesh* dx_msh )
{
	DX_MeshMaterialList*  meshMaterialList = dx_msh->meshMaterialList;
	DX_Material*          material;
	DX_DWORD              n;

	if (meshMaterialList == NULL) return;

	ffp_NAME  ( "MeshMaterialList" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  MeshMaterialList
	 */
	ffp_TOKEN ( TOKEN_INTEGER_LIST );
	ffp_dword ( 2 + meshMaterialList->nFaceIndexes );
	ffp_dword ( meshMaterialList->nMaterials );
	ffp_dword ( meshMaterialList->nFaceIndexes );
	for (n = 0; n < meshMaterialList->nFaceIndexes; n++) {
		ffp_dword ( meshMaterialList->faceIndexes[n] );
	}
	material = meshMaterialList->materials;
	for (n = 0; n < meshMaterialList->nMaterials; n++)
	{
		switch (_options->materialType) {
		case DX_MATTYPE_INLINE:
			BIN_WriteMaterial( material );
			break;
		case DX_MATTYPE_EXTERN:
			ffp_TOKEN ( TOKEN_OBRACE );
			ffp_NAME  ( material->nodeName );
			ffp_TOKEN ( TOKEN_CBRACE );
			break;
		case DX_MATTYPE_SHARED:
			ffp_TOKEN ( TOKEN_OBRACE );
			ffp_NAME  ( material->surfName );
			ffp_TOKEN ( TOKEN_CBRACE );
			break;
		}
		material = material->next;
	}

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteXSkinMeshHeader   - write XSkinMeshHeader
 *
 */

void dxExporter::BIN_WriteXSkinMeshHeader( DX_Mesh* dx_msh )
{
	DX_XSkinMeshHeader*  xskinMeshHeader = dx_msh->xskinMeshHeader;

	if (xskinMeshHeader == NULL) return;
	if (_options->version <= DX_VERSION_DIRECTX7) return;

	ffp_NAME  ( "XSkinMeshHeader" );
	ffp_TOKEN ( TOKEN_OBRACE );

	/*
	 *  XSkinMeshHeader
	 */
	ffp_TOKEN ( TOKEN_WORD );
	ffp_word  ( xskinMeshHeader->nMaxSkinWeightsPerVertex );
	ffp_TOKEN ( TOKEN_WORD );
	ffp_word  ( xskinMeshHeader->nMaxSkinWeightsPerFace );
	ffp_TOKEN ( TOKEN_WORD );
	ffp_word  ( xskinMeshHeader->nBones );

	ffp_TOKEN ( TOKEN_CBRACE );
}


/*
 *
 *  BIN_WriteSkinWeights   - write SkinWeights
 *
 */

void dxExporter::BIN_WriteSkinWeights( DX_Mesh* dx_msh )
{
	DX_SkinWeights*  skinWeights = dx_msh->skinWeights;
	DX_DWORD         n;

	if (_options->version <= DX_VERSION_DIRECTX7) return;

	while (skinWeights)
	{
		if (skinWeights->nWeights > 0)
		{
			ffp_NAME  ( "SkinWeights" );
			ffp_TOKEN ( TOKEN_OBRACE );

			ffp_STRING( skinWeights->transformNodeName, TOKEN_SEMICOLON );
			ffp_TOKEN ( TOKEN_INTEGER_LIST );
			ffp_dword ( skinWeights->nWeights + 1 );
			ffp_dword ( skinWeights->nWeights );
			for (n = 0; n < skinWeights->nWeights; n++) {
				ffp_dword ( skinWeights->vertexIndices[n] );
			}
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( skinWeights->nWeights );
			for (n = 0; n < skinWeights->nWeights; n++) {
				ffp_float ( skinWeights->weights[n] );
			}

			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( skinWeights->matrixOffset.matrix[ 0] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 1] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 2] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 3] );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( skinWeights->matrixOffset.matrix[ 4] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 5] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 6] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 7] );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( skinWeights->matrixOffset.matrix[ 8] );
			ffp_float ( skinWeights->matrixOffset.matrix[ 9] );
			ffp_float ( skinWeights->matrixOffset.matrix[10] );
			ffp_float ( skinWeights->matrixOffset.matrix[11] );
			ffp_TOKEN ( TOKEN_FLOAT_LIST );
			ffp_dword ( 4 );
			ffp_float ( skinWeights->matrixOffset.matrix[12] );
			ffp_float ( skinWeights->matrixOffset.matrix[13] );
			ffp_float ( skinWeights->matrixOffset.matrix[14] );
			ffp_float ( skinWeights->matrixOffset.matrix[15] );

			ffp_TOKEN ( TOKEN_CBRACE );
		}

		skinWeights = skinWeights->next;
	}
}

#endif
