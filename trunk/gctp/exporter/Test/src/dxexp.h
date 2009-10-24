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
#include <map>
#include <vector>

#define DX_TERM(n,size)			((n == size-1) ? ';' : ',' )

									//  Error Definitions

typedef enum {
	DX_ERR_NOERROR = 0,
	DX_ERR_OBJFILE = 1,
	DX_ERR_IMGFILE = 2,
	DX_ERR_SCNFILE = 3
} DX_ErrorCode;

typedef char           DX_ErrorFile[255];

struct VPointData {				/*  POINT DATA STRUCTURE  */
	float position[3];
};

struct VNormalData {				/*  NORMAL DATA STRUCTURE  */
	float vector[3];
};

struct VMapData {					/*  UV TEXTURE MAP DATA STRUCTURE  */
	int ismapped;
	float u, v, w;
};

struct VColorData {				/*  VERTEX COLOR DATA STRUCTURE  */
	int ismapped;
	float r, g, b, a;
};

struct VShareInfo {
	std::vector<int> pols;
	std::vector<int> ivert;
};

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

	LWW::Surface lwsurf;

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
	ET_NONE,
	ET_POINTPOLY,
	ET_LINEPOLY,
	ET_TQPOLY,
	ET_NPOLY,
	ET_MAX,
};

typedef struct st_DX_VertexEntry {

	int                           pnt_no;
	int                           pol_no;
	short                         ivert;
	short                         num_pnts;
	int                           entried;
	DX_DWORD                      index;

	struct st_DX_VertexEntry*     next;

} DX_VertexEntry;

typedef struct st_DX_PointEntry {

	DX_VertexEntry*               vertices;  // これはポインタチェーン
	// 名前からまったく想像できないが、その点が使用されているポリゴンの
	// 分だけチェーンでエントリが作られる、点の共有情報を展開したもの

} DX_PointEntry;

struct DX_PolygonInfo {
	LWPolID id;
	int pnum;
	int ivert[4];
};

typedef struct st_DX_Valence {

	LWW::MeshInfo lwmesh;
	std::vector<LWPntID> pntids;
	std::vector<DX_PolygonInfo> polids;
	std::map<LWPntID, int> pntid_to_idx;
	//std::map<LWPolID, int> polid_to_idx;
	std::vector<VShareInfo> shareinfo;
	typedef std::map<const char *, void *> MorphMap;
	MorphMap rmorph;
	MorphMap amorph;

	int                           nEntries;
	int                           nIndicies;
	int                           nPoints;
	DX_PointEntry*                points; // これは配列

} DX_Valence;
									//  Mesh

struct DX_MorphTargetMesh {
	DX_DWORD                      nVertices;
	DX_Vector*                    vertices;
	DX_DWORD                      nFaces;
	DX_MeshFace*                  faces;

	DX_MeshNormals*               meshNormals;
	DX_MeshTextureCoords*         meshTextureCoords;
	DX_VertexDuplicationIndices*  vertexDuplicationIndices;
	DX_VertexColors*              meshVertexColors;
	DX_MeshMaterialList*          meshMaterialList;
};

typedef struct st_DX_Mesh {

	//LWW::MeshInfo lwmesh;

	DX_Valence*                   valences;

	DX_STRING                     nodeName;
	DX_DWORD                      nVertices;
	DX_Vector*                    vertices;
	DX_DWORD                      nFaces;
	DX_MeshFace*                  faces;

	DX_MeshNormals*               meshNormals;
	DX_MeshTextureCoords*         meshTextureCoords;
	DX_VertexDuplicationIndices*  vertexDuplicationIndices;
	DX_VertexColors*              meshVertexColors;
	DX_MeshMaterialList*          meshMaterialList;
	DX_XSkinMeshHeader*           xskinMeshHeader;
	DX_SkinWeights*               skinWeights;

	struct st_DX_Mesh*            next;

	std::vector<DX_MorphTargetMesh> rmorphs;
	std::vector<DX_MorphTargetMesh> amorphs;

	DX_Valence*                   wirevalences;

	std::string                   wire_name;
	gctp::xext::Wire             *wire;
	DX_MeshTextureCoords*         wireTextureCoords;
	DX_VertexColors*              wireVertexColors;
	DX_MeshMaterialList*          wireMaterialList;

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
	LWW::Item                     item;

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

	bool   ExportScene  ( );
	bool   ExportObject ( int obj );
	bool   IsNewScene   ();

	DX_ErrorCode                  _errCode;
	DX_ErrorFile                  _errFile;

	LWW::Globals &globals() { return globals_; }

	LWFrame TIME2FRAME(LWTime t) { return ((LWFrame)(_ticksPerSec * (t) + 0.5)); }
	LWTime FRAME2TIME(LWFrame f) { return ((LWTime)(f) / _ticksPerSec); }

	static const char *CHNAME_XPOS;
	static const char *CHNAME_YPOS;
	static const char *CHNAME_ZPOS;
	static const char *CHNAME_HEADING;
	static const char *CHNAME_PITCH;
	static const char *CHNAME_BANK;
	static const char *CHNAME_XSCALE;
	static const char *CHNAME_YSCALE;
	static const char *CHNAME_ZSCALE;

private:
	//  Scene (dxexp.c)
	void                          MakeScene ( );
	void                          MakeObject( int obj );
	DX_ErrorCode                  OpenScene ();
	DX_ErrorCode                  CloseScene();

	//  Frame (dxfrm.c)
	DX_Frame*                     MakeRootFrame   ();
	DX_Frame*                     MakeObjectFrame ( int obj );
	DX_Frame*                     MakeItemFrame   ( LWW::Item item );
	void                          LinkFrames      ();
	void                          DisposeFrames   ( DX_Frame **dx_frms );
	void                          CalcFrameMatrix ( DX_Frame* dx_frm );
	DX_Frame*                     ItemHiererchy   ( DX_Frame* dx_frm, LWW::Item item );
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
	int                           GetMatrixValue      ( LWW::Item item, DX_DWORD frame, DX_FLOAT *vals );
	int                           GetScaleValue       ( LWW::Item item, DX_DWORD frame, DX_FLOAT *vals );
	int                           GetPositionValue    ( LWW::Item item, DX_DWORD frame, DX_FLOAT *vals );
	int                           GetRotateValue      ( LWW::Item item, DX_DWORD frame, DX_FLOAT *vals );
	DX_AnimationKey*              MakeMatrixKey       ( DX_Animation *dx_ani );
	DX_DWORD                      CountKeyframe       ( LWW::Item item, const char **chnames, int nch );
	bool                          TestChannel         ( LWW::Channel channel, const char **chnames, int nch );
	bool                          TestKeyframe        ( LWW::Item item, LWFrame frame, const char **chnames, int nch );
	bool                          UsableAnimationKey  ( DX_AnimationKey* dx_key );

	//  Material (dxmat.c)
	DX_Material*                  MakeMaterials     ( DX_Material *dx_mats, LWW::MeshInfo mesh, bool *uselist = NULL );
	void                          DisposeMaterials  ( DX_Material **dx_mats );
	void                          ConvertTextureName( char* outfile, char* infile );

	//  Mesh (dxmsh.c)
	DX_Mesh*                      MakeMesh                    ( LWW::MeshInfo mesh );
	void                          DisposeMeshes               ( DX_Mesh **dx_mshs );
	void                          DisposeMaterialList         ( DX_MeshMaterialList **meshMaterialList );
	DX_Vector*                    MakeVertices                ( DX_Mesh* dx_msh );
	DX_MeshFace*                  MakeFaces                   ( DX_Mesh* dx_msh );
	DX_MeshNormals*               MakeNormals                 ( DX_Mesh* dx_msh );
	DX_MeshTextureCoords*         MakeTextureCoords           ( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
	DX_VertexDuplicationIndices*  MakeVertexDuplicationIndices( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
	DX_VertexColors*              MakeVertexColors            ( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );
	DX_MeshMaterialList*          MakeMaterialList            ( DX_Mesh* dx_msh, EntryType type = ET_TQPOLY );

	gctp::xext::Wire *			  MakeWire                    ( DX_Mesh *dx_msh );

	//  SkinnedMesh (dxskm.c)
	DX_XSkinMeshHeader*           MakeXSkinMesh       ( DX_Frame* dx_frm );
	DX_SkinWeights*               MakeSkinWeights     ( DX_Frame* dx_frm );
	DX_DWORD                      CountBones          ( DX_Frame* dx_frm );
	DX_SkinWeights*               AllocSkinWeight     ( DX_SkinWeights* dx_skms, DX_Frame* dx_frm, DX_Frame* dx_fbn );
	void                          BuildSkinWeights    ( DX_SkinWeights* dx_skms, DX_Frame* dx_frm );
	void                          MakeRestTransform   ( LWW::Item item, DX_Matrix4x4* restTransform );
	void                          MakeInvRestTransform( LWW::Item item, DX_Matrix4x4* invRestTransform );
	void                          MakeBonePositions   ( LWW::Item item, DX_SkinWeights* dx_skm );

	//  Valences (dxval.c)
	DX_Valence*                   MakeValences     ( LWW::MeshInfo mesh, EntryType type );
	int                           EntryValence     ( DX_Valence* dx_val, DX_PointEntry* pointEntry, DX_VertexEntry* newEntry );
	DX_DWORD                      ValenceIndex     ( DX_Valence* dx_val, int pol_no, int ivert );
	DX_VertexEntry*               ValenceGetEntried( DX_Valence* dx_val, DX_VertexEntry* entry );
	DX_DWORD                      ValenceOriginal  ( DX_Valence* dx_val, DX_VertexEntry* entry );
	VPointData                    ValenceVertex    ( DX_Valence* dx_val, DX_VertexEntry* entry );
	VNormalData                   ValenceNormal    ( DX_Valence* dx_val, DX_VertexEntry* entry );
	VMapData                      ValenceUVCoord   ( DX_Valence* dx_val, DX_VertexEntry* entry );
	VColorData                    ValenceColor     ( DX_Valence* dx_val, DX_VertexEntry* entry );
	void                          DisposeValences  ( DX_Valence **dx_val );


	//  Write Scene (wrscn.c)
	void                          WriteScene         ();
	DX_Material*                  SharedMaterial     ( DX_Material *pr_mat );

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
	void                          WriteMeshVertexColor         ( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire = false );
	void                          WriteMeshMaterialList        ( DX_Mesh* dx_msh, gctp::XSaver &cur, bool is_wire = false );
	void                          WriteXSkinMeshHeader         ( DX_Mesh* dx_msh, gctp::XSaver &cur );
	void                          WriteSkinWeights             ( DX_Mesh* dx_msh, gctp::XSaver &cur );


	//  Private Members
	gctp::XFileEnv                _xenv;
	gctp::XFileWriter             _xfile;
	DX_Frame*                     _sceneRoot;
	DX_AnimationSet*              _animationSet;
	DX_DWORD                      _ticksPerSec;

	dxOptions*                    _options;

	LWW::Globals globals_;
};

#endif /* _DXEXP_H_  */

