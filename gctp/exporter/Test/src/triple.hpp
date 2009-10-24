#include <gctp/vector.hpp>
#include <gctp/dbgout.hpp>

#include "mathlib.hpp"
#include "dxexp.h"
#include "dxvec.h"






class Trianguler {
public:
	struct PointData {				/*  POINT DATA STRUCTURE  */
		float position[3];
	};
#if 0
	struct PolygonMap {
		short         chan;					/*  TEXTURE CHANNEL  */
		short         layr;					/*  TEXTURE LAYER  */
		int           *maps;

		struct st_PolygonMap  *next;			/*  NEXT POLYGON MAP  */
	};
#endif
	struct PolygonData {
		//void            *userData;
		//unsigned short	 flags_l;				/*  LWO2 POLYGON FLAGS  */
		//int              flags;					/*  WORKING FLAG  */
		int              idx;					/*  POLYGON INDEX  */
		//const char      *surface;				/*  SURFACE NAME POINTER */
		//int              surf_no;				/*  SURFACE INDEX NUMBER  */
		//int		         key;					/*  SORT KEY  */
		LWFVector        face;					/*  FACE NORMAL VECTOR  */
		std::vector<int> points;				/*  POINT LIST  */
		//std::vector<int> normals;				/*  VERTEX NORMALS  */
		//int             *maps;					/*  UV TEXTURE MAPPING (POINTER FROM polms)  */
		//int             *colors;				/*  VERTEX COLORS  */
		//int             edge_no;				/*  ONE OF EDGES OF THE POLYGON  */
		//PolygonMap      *polms;					/*  POLYGON UV MAP DATAS  */
		void getFace(LWFVector f)
		{
			f[0] = face[0];
			f[1] = face[1];
			f[2] = face[2];
		}
	};

	Trianguler(LWW::MeshInfo lwmesh, LWPolID p, bool tri_all);

	LWW::MeshInfo lwmesh;
	std::vector<PointData> pnts;
	std::vector<PolygonData> polys;

private:
	PolygonData *polyInfo(int pol_no)
	{
		return &polys[pol_no];
	}

	PointData *pointInfo(int pnt_no)
	{
		return &pnts[pnt_no];
	}

	void polySetFace(PolygonData &pol)
	{
		LWFVector e1 = {
			pnts[pol.points[1]].position[0]-pnts[pol.points[0]].position[0],
			pnts[pol.points[1]].position[1]-pnts[pol.points[0]].position[1],
			pnts[pol.points[1]].position[2]-pnts[pol.points[0]].position[2]
		};
		LWFVector e2 = {
			pnts[pol.points[2]].position[0]-pnts[pol.points[1]].position[0],
			pnts[pol.points[2]].position[1]-pnts[pol.points[1]].position[1],
			pnts[pol.points[2]].position[2]-pnts[pol.points[1]].position[2]
		};
		pol.face[0] = e1[1]*e2[2]-e1[2]*e2[1];
		pol.face[1] = e1[2]*e2[0]-e1[0]*e2[2];
		pol.face[2] = e1[0]*e2[1]-e1[1]*e2[0];
	}

	void polPnts(int pol, int size, int *pbuf)
	{
		polys[pol].points.resize(size);
		for(size_t i = 0; i < (size_t)size; i++) polys[pol].points[i] = pbuf[i];
		polySetFace(polys[pol]);
	}

	int addPoly( int numPnts, int *pbuf )
	{
		polys.resize(polys.size()+1);
		polys.back().idx = (int)(polys.size()-1);
		for(int i = 0; i < numPnts; i++) {
			polys.back().points.push_back(pbuf[i]);
		}
		polySetFace(polys.back());
		return polys.back().idx;
	}

	struct Node {
		enum Type {
			REGULAR = 0,
			START,
			SPLIT,
			END,
			MERGE
		};

		int      idx;
		int      pnt_no;
		float    x, y;
		Type     type;
	};

	struct Edge {
		int   idx;
		Node  *sp, *ep;				//  START AND END NODE
		Node  *helper;				//  HELPER NODE
		Node  *key;					//  KEY NODE FOR QUICKSORT
		Edge  *next, *prev;			//  NEXT AND PREV BI-DIRECTIONAL LINK
		Edge  *origin, *twin;			//  NEXT EDGE OF ORIGINAL AND TWIN DIRECTION
		int   face_o, face_t;			//  FACE INDEX FOR ORIGIN AND TWIN
	};

	struct Face {
		int     idx;
		int     num;
		Edge  *edges;
		Node  *sp;
		Face  *next;
	};

	struct Queue {
		int            cur;
		int            num;
		Edge           **list;
	};

	struct Stack {
		int            max;
		int            num;
		Edge           **list;
	};

	struct Leaf {
		int   idx;
		Edge  *l, *r;
		Edge  *edge;
		Leaf  *next, *prev;
	};

	struct Tree {
		int             num;
		Leaf            *leaves;
	};


	struct Poly {
		int             numEdges;
		int             numFaces;
		Edge            *edges;
		Face            *faces;
	};


	struct TriList {
		int             pol;
		int             convex;
		int             numPnts;
		Node            *nodes;
		Poly            *p, *d;
		Queue           *queue;
	};


	/**********  FUNCTION PROTOTYPES   **********/

	void free_list      ( TriList *tri );
	void MakePolygonList( TriList *tri );
	void MakeMonotone   ( TriList *tri );
	void MakeFaceLinks  ( TriList *tri );
	void TripleMonotones( TriList *tri );
	void OutputTriangles( TriList *tri );
	void OutputConvex   ( TriList *tri );
	int  SelfIntersect  ( TriList *tri );

	void HandleStartVertex  ( TriList *tri, Tree *tree, Edge *edge );
	void HandleSplitVertex  ( TriList *tri, Tree *tree, Edge *edge );
	void HandleEndVertex    ( TriList *tri, Tree *tree, Edge *edge );
	void HandleMergeVertex  ( TriList *tri, Tree *tree, Edge *edge );
	void HandleRegularVertex( TriList *tri, Tree *tree, Edge *edge );

	static Queue *queue_make       ( Face *f, Queue *old_q );
	static Edge  *queue_pop  ( Queue *q );
	static void   queue_free     ( Queue **q );
	static int compar_edge  ( const void *s, const void *d );
	static int compar_node  ( Node *sn, Node *dn );

	static Stack *stack_make     ( int num );
	static void stack_free   ( Stack **s );
	static Edge *stack_pop( Stack *s );
	static Edge *stack_ref( Stack *s );
	static void stack_push( Stack *s, Edge *e );
	static void stack_dump( Stack *s, Face *f );

	static Tree *tree_new();
	static Leaf *tree_get( Tree *tree, int idx );
	static Leaf *tree_add( TriList *tri, Tree *tree, Edge *edge );
	static Leaf *tree_find( Tree *tree, Edge *edge );
	static void tree_dump( Tree *tree );
	static void  tree_delete    ( Tree *tree, Edge *edge );
	static void  tree_free      ( Tree **t );
	static Leaf  *tree_get_left  ( Tree *tree, Poly *p, Edge *edge );

	static Node  *node_new       ( int n );
	static Node  *node_get       ( Node *node, int idx );
	static int      node_low       ( Node *node, Node *t );
	static int      node_high      ( Node *node, Node *t );
	static int      node_convex    ( Node *node, Node *next, Node *prev );
	static float    node_area      ( Node *node, Node *next, Node *prev );
	static int      node_flip      ( int num, Node *node );
	static Node *node_xy(Node *node, int idx, float x, float y, int pnt_no);
	static int node_type( Node *node, Node *next, Node *prev );

	static Edge  *edge_get       ( Edge *edge, int idx );
	static Edge  *edge_helper    ( Edge *edge, Node *helper );
	static void   edge_print     ( Edge *edge );

	static Poly  *poly_new       (  );
	static Edge  *poly_edge_add  ( Poly *poly, Node *sp, Node *ep );
	static Face  *poly_face_new  ( Poly *p );
	static Face  *poly_face_get  ( Poly *p, Node *sp, Node *ep );
	static void     poly_face_dump ( Poly *p );

	static int    face_edge_num  ( Face *face );
	static Edge  *face_edge_end  ( Face *face, Node *ep );
	static void   face_edge_next ( Edge *edge, Node *sp, Edge **edge_n, Node **sp_n );
	static Face  *face_get       ( Face *face, int idx );
	static int    face_edge_side ( Face *f, Edge *e );
	static Node  *face_edge_node ( Face *f, Edge *e );

	static int    intersect_test ( float x1k, float y1k, float x1l, float y1l, 
								   float x2k, float y2k, float x2l, float y2l );
	static int    intersect_lines( float x1k, float y1k, float x1l, float y1l, 
								   float x2k, float y2k, float x2l, float y2l, 
								   float *xi, float *yi );
	static void     rotate_F2Z     ( float *f, tMat4 m );
	static void     line_offset    ( float  x1, float  y1, float  x2, float  y2, float off,
									 float *x3, float *y3, float *x4, float *y4 );

	PolygonData *point_merge( PolygonData *polygon );

	//void triple_vmad     ( int pol, int *pols, int nPols );
	int  point_in_polygon( int pnt_no, int pol_no );
	int  regular_triangle( int p0, int p1, int p2 );

	/**  DEBUG FUNCTIONS  **/
	void     add_line       ( int pol, int idx1, int idx2 );
	static void debug_printf( char *buf, ... );
	static bool debug;
};
