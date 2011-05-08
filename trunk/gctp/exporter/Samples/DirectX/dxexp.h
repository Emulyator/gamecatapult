/*********************************************************************/
/*                                                                   */
/* FILE :        dxexp.h                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _DXEXP_H_
#define _DXEXP_H_

#include "dxopt.h"
#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
//#define _WIN32_IE	0x0550
//#include <atlbase.h>
//#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#ifdef _MBCS
# define UNICODE 1
# ifdef _TCHAR
#  undef _TCHAR
# endif
# define _TCHAR wchar_t
#endif
#include <gctp/pointer.hpp>
#include <gctp/xfile.hpp>
#ifdef _MBCS
# undef _TCHAR
# define _TCHAR char
# undef UNICODE
#endif


#define DX_TERM(n,size)			((n == size-1) ? ';' : ',' )

									//  Error Definitions

typedef enum {
	DX_ERR_NOERROR = 0,
	DX_ERR_OBJFILE = 1,
	DX_ERR_IMGFILE = 2,
	DX_ERR_SCNFILE = 3
} DX_ErrorCode;

typedef char           DX_ErrorFile[255];

									//  Materials

typedef struct st_DX_TextureFilename {

	DX_STRING                     filename;

} DX_TextureFilename;

typedef struct st_DX_MaterialData {
	DX_ColorRGBA                  faceColor;
	DX_FLOAT                      power;
	DX_ColorRGB                   specularColor;
	DX_ColorRGB                   emissiveColor;
} DX_MaterialData;

typedef struct st_DX_Material {

	int                           idx;
	OBJ2_ID                       obj;

	DX_STRING                     surfName;
	DX_STRING                     nodeName;
	DX_MaterialData               mtrl;
	DX_TextureFilename            textureFilename;

	struct st_DX_Material*        next;

} DX_Material;



									//  MeshFace

typedef struct st_DX_MeshFace {

	DX_DWORD                      nFaceVertexIndices;
	DX_DWORD*                     faceVertexIndices;

} DX_MeshFace;



									//  MeshNormals

typedef struct st_DX_MeshNormals {

	DX_DWORD                      nNormals;
	DX_Vector*                    normals;
	DX_DWORD                      nFaceNormals;
	DX_MeshFace*                  faceNormals;

} DX_MeshNormals;



									//  MeshTextureCoords

typedef struct st_DX_MeshTextureCoords {

	DX_DWORD                      nTextureCoords;
	DX_Coord2d*                   textureCoords;

} DX_MeshTextureCoords;



									//  VertexDuplicationIndices

typedef struct st_DX_VertexDuplicationIndices {

	DX_DWORD                      nIndices;
	DX_DWORD                      nOriginalVertices;
	DX_DWORD*                     indices;

} DX_VertexDuplicationIndices;



									//  MeshFaceWraps

typedef struct st_DX_MeshFaceWraps {

	DX_DWORD                      nFaceWrapValues;
	DX_Boolean2d*                 faceWrapValues;

} DX_MeshFaceWraps;



									//  VertexColors

typedef struct st_DX_VertexColors {

	DX_DWORD                      nVertexColors;
	DX_IndexColor*                vertexColors;

} DX_VertexColors;



									//  MeshMaterialList

typedef struct st_DX_MeshMaterialList {

	DX_DWORD                      nMaterials;
	DX_DWORD                      nFaceIndexes;
	DX_DWORD*                     faceIndexes;
	DX_Material*                  materials;

} DX_MeshMaterialList;



									//  XSkinMeshHeader

typedef struct st_DX_XSkinMeshHeader {

	DX_WORD                       nMaxSkinWeightsPerVertex;
	DX_WORD                       nMaxSkinWeightsPerFace;
	DX_WORD                       nBones;

} DX_XSkinMeshHeader;



									//  SkinWeights

typedef struct st_DX_SkinWeights {

	int                           idx;
	void*                         work;
	DX_STRING                     transformNodeName;
	DX_DWORD                      nWeights;
	DX_DWORD*                     vertexIndices;
	DX_FLOAT*                     weights;
	DX_Matrix4x4                  matrixOffset;

	DX_Matrix4x4                  boneTransform;
	DX_FLOAT                      boneRestStart[3];
	DX_FLOAT                      boneRestEnd[3];

	struct st_DX_SkinWeights*     next;

} DX_SkinWeights;


									//  Valences

enum EntryType {
	ET_POINTPOLY,
	ET_LINEPOLY,
	ET_TQPOLY,
	ET_NPOLY,
	ET_MAX,
};

inline EntryType PointNumToType(int pn)
{
	if(pn < 2) return ET_POINTPOLY;
	else if(pn == 2) return ET_LINEPOLY;
	else if(2 < pn && pn < 5) return ET_TQPOLY;
	return ET_NPOLY;
}

typedef struct st_DX_VertexEntry {

	PntNO                         pnt_no;
	PolNO                         pol_no;
	short                         ivert;
	short                         num_pnts;
	int                           entried;
	DX_DWORD                      index;

	struct st_DX_VertexEntry*     next;

} DX_VertexEntry;

typedef struct st_DX_PointEntry {

	DX_VertexEntry*               vertices;

} DX_PointEntry;

typedef struct st_DX_Valence {

	OBJ2_ID                       obj;
	int                           nEntries;
	int                           nIndicies[ET_MAX];
	int                           nPoints;
	DX_PointEntry*                points;

} DX_Valence;

									//  Mesh

typedef struct st_DX_Mesh {

	int                           idx;
	OBJ2_ID                       obj;
	int                           layer;

	DX_Valence*                   valences;

	DX_STRING                     nodeName;
	DX_DWORD                      nVertices;
	DX_Vector*                    vertices;
	DX_DWORD                      nFaces;
	DX_MeshFace*                  faces;

	DX_MeshNormals*               meshNormals;
	DX_MeshTextureCoords*         meshTextureCoords;
	DX_VertexDuplicationIndices*  vertexDuplicationIndices;
	DX_MeshFaceWraps*             meshFaceWraps;
	DX_VertexColors*              meshVertexColors;
	DX_MeshMaterialList*          meshMaterialList;
	DX_XSkinMeshHeader*           xskinMeshHeader;
	DX_SkinWeights*               skinWeights;

	struct st_DX_Mesh*            next;
#ifdef GCTP_USE_XFILEAPI
	std::string                   wire_name;
	gctp::xext::Wire             *wire;
	DX_MeshTextureCoords*         wireTextureCoords;
	DX_MeshFaceWraps*             wireFaceWraps;
	DX_VertexColors*              wireVertexColors;
	DX_MeshMaterialList*          wireMaterialList;
#endif
} DX_Mesh;

									//  Frames

typedef enum {
	DX_FRAME_ROOT   = 0,
	DX_FRAME_NULL   = 1,
	DX_FRAME_MESH   = 2,
	DX_FRAME_PIVOT  = 3,
	DX_FRAME_LIGHT  = 4,
	DX_FRAME_CAMERA = 5,
	DX_FRAME_BONE   = 6
} DX_FrameType;


typedef struct st_DX_FrameTransformMatrix {

	DX_Matrix4x4                  frameMatrix;

} DX_FrameTransformMatrix;


typedef struct st_DX_Frame {

	int                           idx;
	SCNItemID                     item;

	DX_STRING                     nodeName;
	DX_FrameType                  frameType;
	DX_FrameTransformMatrix*      frameTransformMatrix;
	DX_Mesh*                      mesh;

	struct st_DX_Frame*           parent;
	struct st_DX_Frame*           child;
	struct st_DX_Frame*           sibling;
	struct st_DX_Frame*           next;

} DX_Frame;


typedef struct st_DX_FloatKeys {

	DX_DWORD                      nValues;
	DX_FLOAT                      values[16];

} DX_FloatKeys;


typedef struct st_DX_TimedFloatKeys {

	DX_DWORD                      time;
	DX_FloatKeys                  tfkeys;

} DX_TimedFloatKeys;


									//  Animations


#define DX_ANIM_ROTATEKEY         0
#define DX_ANIM_SCALEKEY          1
#define DX_ANIM_POSITIONKEY       2
#define DX_ANIM_MATRIXKEY         4

typedef struct st_DX_AnimationKey {

	DX_DWORD                      keyType;
	DX_DWORD                      nKeys;
	DX_TimedFloatKeys*            keys;

	struct st_DX_AnimationKey*    next;

} DX_AnimationKey;


typedef struct st_DX_AnimationOptions {

	DX_DWORD                      openclosed;
	DX_DWORD                      positionquality;

} DX_AnimationOptions;


typedef struct st_DX_Animation {

	int                           idx;

	DX_STRING                     nodeName;
	DX_AnimationKey*              animationKey;
	DX_AnimationOptions*          animationOptions;
	DX_Frame*                     frame;

	struct st_DX_Animation*       next;

} DX_Animation;


typedef struct st_DX_AnimationSet {

	DX_STRING                     nodeName;
	DX_Animation*                 animations;

} DX_AnimationSet;


									//  Scene Graph

class dxExporter
{
public:
	dxExporter ( dxOptions* opts, GlobalFunc* global );
	~dxExporter();

	bool   ExportScene  ( SCN3_ID scn );
	bool   ExportObject ( OBJ2_ID obj );
	bool   IsNewScene   ();

	DX_ErrorCode                  _errCode;
	DX_ErrorFile                  _errFile;

private:
	//  Scene (dxexp.c)
	void                          MakeScene ( SCN3_ID scn );
	void                          MakeObject( OBJ2_ID obj );
	DX_ErrorCode                  OpenScene ();
	DX_ErrorCode                  CloseScene();

	//  Frame (dxfrm.c)
	DX_Frame*                     MakeRootFrame   ();
	DX_Frame*                     MakeObjectFrame ( OBJ2_ID obj );
	DX_Frame*                     MakeItemFrame   ( SCNItemID item );
	void                          LinkFrames      ();
	void                          DisposeFrames   ( DX_Frame **dx_frms );
	void                          CalcFrameMatrix ( DX_Frame* dx_frm );
	DX_Frame*                     ItemHiererchy   ( DX_Frame* dx_frm, SCNItemID item );
	DX_Frame*                     SiblingHiererchy( DX_Frame* dx_frm, DX_Frame* parent );
	DX_FrameTransformMatrix*      MakeFrameMatrix ( DX_Frame* dx_frm );

	//  Animation (dxani.c)
	DX_AnimationSet*              MakeAnimationSet    ();
	DX_Animation*                 MakeAnimation       ( DX_Frame* frame );
	DX_AnimationOptions*          MakeAnimationOptions();
	DX_AnimationKey*              MakeAnimationKey    ( DX_Animation* dx_ani );
	DX_AnimationKey*              MakeRotateKey       ( DX_Animation* dx_ani );
	DX_AnimationKey*              MakeScaleKey        ( DX_Animation* dx_ani );
	DX_AnimationKey*              MakePositionKey     ( DX_Animation* dx_ani );
	int                           GetMatrixValue      ( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals );
	int                           GetScaleValue       ( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals );
	int                           GetPositionValue    ( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals );
	int                           GetRotateValue      ( SCNItemID item, DX_DWORD frame, DX_FLOAT* vals );
	DX_AnimationKey*              MakeMatrixKey       ( DX_Animation *dx_ani );
	DX_DWORD                      CountKeyframe       ( SCNItemID item, SCNChannelNo* ch, int nch );
	bool                          TestChannel         ( SCNChannelID channel, SCNChannelNo* ch, int nch );
	bool                          TestKeyframe        ( SCNItemID item, int frame, SCNChannelNo* ch, int nch );
	Quat                          GetTargetRotation   ( SCNItemID item, SCNItemID target, SCNTime ti );
	Quat                          PathAlignLookAhead  ( SCNItemID item, SCNTime ti );
	bool                          UsableAnimationKey  ( DX_AnimationKey* dx_key );

	//  Material (dxmat.c)
	DX_Material*                  MakeMaterials     ( DX_Material* dx_mats, OBJ2_ID obj, bool *uselist = NULL );
	void                          DisposeMaterials  ( DX_Material **dx_mats );
	void                          ConvertTextureName( char* outfile, char* infile );

	//  Mesh (dxmsh.c)
	DX_Mesh*                      MakeMesh                    ( OBJ2_ID obj, int layer_no );
	void                          DisposeMeshes               ( DX_Mesh **dx_mshs );
	void                          DisposeMaterialList         ( DX_MeshMaterialList **meshMaterialList );
	DX_Vector*                    MakeVertices                ( DX_Mesh* dx_msh );
	DX_MeshFace*                  MakeFaces                   ( DX_Mesh* dx_msh );
	DX_MeshNormals*               MakeNormals                 ( DX_Mesh* dx_msh );
	DX_MeshTextureCoords*         MakeTextureCoords           ( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
	DX_VertexDuplicationIndices*  MakeVertexDuplicationIndices( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
	DX_VertexColors*              MakeVertexColors            ( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
	DX_MeshMaterialList*          MakeMaterialList            ( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
#ifdef GCTP_USE_XFILEAPI
	gctp::xext::Wire *			  MakeWire                    ( DX_Mesh *dx_msh );
#endif

	//  SkinnedMesh (dxskm.c)
	DX_XSkinMeshHeader*           MakeXSkinMesh       ( DX_Frame* dx_frm );
	DX_SkinWeights*               MakeSkinWeights     ( DX_Frame* dx_frm );
	DX_DWORD                      CountBones          ( DX_Frame* dx_frm );
	DX_SkinWeights*               AllocSkinWeight     ( DX_SkinWeights* dx_skms, DX_Frame* dx_frm, DX_Frame* dx_fbn );
	void                          BuildSkinWeights    ( DX_SkinWeights* dx_skms, DX_Frame* dx_frm );
	void                          MakeRestTransform   ( SCNItemID item, DX_Matrix4x4* restTransform );
	void                          MakeInvRestTransform( SCNItemID item, DX_Matrix4x4* invRestTransform );
	void                          MakeBonePositions   ( SCNItemID item, DX_SkinWeights* dx_skm );

	//  Valences (dxval.c)
	DX_Valence*                   MakeValences     ( OBJ2_ID obj );
	DX_DWORD                      ValenceIndex     ( DX_Valence* dx_val, PolNO pol_no, int ivert );
	DX_VertexEntry*               ValenceGetEntried( DX_Valence* dx_val, DX_VertexEntry* entry, EntryType type );
	DX_DWORD                      ValenceOriginal  ( DX_Valence* dx_val, DX_VertexEntry* entry );
	PointData*                    ValenceVertex    ( DX_Valence* dx_val, DX_VertexEntry* entry );
	NormalData*                   ValenceNormal    ( DX_Valence* dx_val, DX_VertexEntry* entry );
	MapData*                      ValenceUVCoord   ( DX_Valence* dx_val, DX_VertexEntry* entry );
	ColorData*                    ValenceColor     ( DX_Valence* dx_val, DX_VertexEntry* entry );
	void                          DisposeValences  ( DX_Valence **dx_val );

#ifdef GCTP_USE_XFILEAPI
	//  Write Scene (wrscn.c)
	void                          WriteScene         ();
	DX_Material*                  SharedMaterial     ( DX_Material* pr_mat );

	//  Write Animation (wrani.c)
	void                          WriteAnimTicksPerSecond( gctp::XSaver &cur );
	void                          WriteAnimationSet    ( DX_AnimationSet*     dx_set, gctp::XSaver &cur );
	void                          WriteAnimation       ( DX_Animation*        dx_ani, gctp::XSaver &cur );
	void                          WriteAnimationOptions( DX_AnimationOptions* dx_opt, gctp::XSaver &cur );
	void                          WriteAnimationKey    ( DX_AnimationKey*     dx_key, gctp::XSaver &cur );

	//  Write Frame (wrfrm.c)
	void                          WriteFrames    ( DX_Frame *dx_frms, gctp::XSaver &cur );

	//  Write Material (wrmat.c)
	void                          WriteMaterial    ( DX_Material* dx_mat, gctp::XSaver &cur );

	//  Write Mesh (wrmsh.c)
	void                          WriteMesh                    ( DX_Mesh* dx_msh, gctp::XSaver &cur );
	void                          WriteMeshNormals             ( DX_Mesh* dx_msh, gctp::XSaver &cur );
	void                          WriteMeshTextureCoords       ( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire = false );
	void                          WriteVertexDuplicationIndices( DX_Mesh* dx_msh, gctp::XSaver &cur );
	void                          WriteMeshFaceWraps           ( DX_Mesh* dx_msh, gctp::XSaver &cur );
	void                          WriteMeshVertexColor         ( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire = false );
	void                          WriteMeshMaterialList        ( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire = false );
	void                          WriteXSkinMeshHeader         ( DX_Mesh* dx_msh, gctp::XSaver &cur );
	void                          WriteSkinWeights             ( DX_Mesh* dx_msh, gctp::XSaver &cur );

#else
	//  Write Template (wrtmp.c)
	void                          WriteTemplates    ();
	//  Write Scene (wrscn.c)
	void                          WriteScene         ();
	DX_Material*                  SharedMaterial     ( DX_Material* pr_mat );
	void                          WriteFileHeader    ();
	void                          TXT_WriteFileHeader();
	void                          BIN_WriteFileHeader();
	void                          WriteHeader        ();
	void                          TXT_WriteHeader    ();
	void                          BIN_WriteHeader    ();

	//  Write Animation (wrani.c)
	void                          WriteAnimationSet        ( DX_AnimationSet* dx_set, int tabs );
	void                          TXT_WriteAnimationSet    ( DX_AnimationSet*     dx_set, int tabs );
	void                          TXT_WriteAnimation       ( DX_Animation*        dx_ani, int tabs );
	void                          TXT_WriteAnimationOptions( DX_AnimationOptions* dx_opt, int tabs );
	void                          TXT_WriteAnimationKey    ( DX_AnimationKey*     dx_key, int tabs );
	void                          BIN_WriteAnimationSet    ( DX_AnimationSet*     dx_set );
	void                          BIN_WriteAnimation       ( DX_Animation*        dx_ani );
	void                          BIN_WriteAnimationOptions( DX_AnimationOptions* dx_opt );
	void                          BIN_WriteAnimationKey    ( DX_AnimationKey*     dx_key );

	//  Write Frame (wrfrm.c)
	void                          WriteFrames    ( DX_Frame *dx_frms, int tabs );
	void                          TXT_WriteFrames( DX_Frame *dx_frms, int tabs );
	void                          BIN_WriteFrames( DX_Frame *dx_frms );

	//  Write Material (wrmat.c)
	void                          WriteMaterial    ( DX_Material* dx_mat, int tabs );
	void                          TXT_WriteMaterial( DX_Material* dx_mat, int tabs );
	void                          BIN_WriteMaterial( DX_Material *dx_mat );

	//  Write Mesh (wrmsh.c)
	void                          WriteMesh                        ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteMesh                    ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteMeshNormals             ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteMeshTextureCoords       ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteVertexDuplicationIndices( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteMeshFaceWraps           ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteMeshVertexColor         ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteMeshMaterialList        ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteXSkinMeshHeader         ( DX_Mesh* dx_msh, int tabs );
	void                          TXT_WriteSkinWeights             ( DX_Mesh* dx_msh, int tabs );
	void                          BIN_WriteMesh                    ( DX_Mesh* dx_msh );
	void                          BIN_WriteMeshNormals             ( DX_Mesh* dx_msh );
	void                          BIN_WriteMeshTextureCoords       ( DX_Mesh* dx_msh );
	void                          BIN_WriteVertexDuplicationIndices( DX_Mesh* dx_msh );
	void                          BIN_WriteMeshFaceWraps           ( DX_Mesh* dx_msh );
	void                          BIN_WriteMeshVertexColor         ( DX_Mesh* dx_msh );
	void                          BIN_WriteMeshMaterialList        ( DX_Mesh* dx_msh );
	void                          BIN_WriteXSkinMeshHeader         ( DX_Mesh* dx_msh );
	void                          BIN_WriteSkinWeights             ( DX_Mesh* dx_msh );

	//  Write Template (wrtmp.c)
	void                          WriteTemplates    ();
	void                          TXT_WriteTemplates();
	void                          BIN_WriteTemplates();

	//  Write Functions (dxffp.c)
	void                          ffp             ( int tabs, char *buf, ... );
	int                           ffp_bytes       ( DX_BYTE  *vals, int num );
	int                           ffp_words       ( DX_WORD  *vals, int num );
	int                           ffp_dwords      ( DX_DWORD *vals, int num );
	int                           ffp_floats      ( DX_FLOAT *vals, int num );
	int                           ffp_word        ( DX_WORD  vals );
	int                           ffp_dword       ( DX_DWORD vals );
	int                           ffp_float       ( DX_FLOAT vals );
	void                          ffp_TOKEN       ( int token );
	void                          ffp_NAME        ( char *name );
	void                          ffp_STRING      ( char *string, int token );
	void                          ffp_INTEGER_LIST( DX_DWORD *list, int num );
	void                          ffp_FLOAT_LIST  ( DX_FLOAT *list, int num );
#endif

	//  Private Members
#ifdef GCTP_USE_XFILEAPI
	gctp::XFileEnv                _xenv;
	gctp::XFileWriter             _xfile;
#else
	FILE*                         _fp;
#endif
	DX_Frame*                     _sceneRoot;
	DX_AnimationSet*              _animationSet;
	DX_DWORD                      _ticksPerSec;

	OBJ2_ID                       _obj;
	SCN3_ID                       _scn;
	dxOptions*                    _options;
	GlobalFunc*                   _global;
};

#endif /* _DXEXP_H_  */

