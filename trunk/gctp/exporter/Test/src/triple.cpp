#include "triple.hpp"

/**********  TRIPLE  **********/


#define FP_EQUAL(a,b)			(fabs(a-b) < EPSILON)
#define FP_GREAT(a,b)			(((a)-(b)) > EPSILON)
#define FP_LESS(a,b)			(((b)-(a)) > EPSILON)
#define FP_POINT(x1,y1,x2,y2)	(((x1-x2)*(x1-x2)+ \
								  (y1-y2)*(y1-y2)) < EPSILON*EPSILON)
#define FP3_POINT(p0,p1)		(((p0[0]-p1[0])*(p0[0]-p1[0])+ \
								  (p0[1]-p1[1])*(p0[1]-p1[1])+ \
								  (p0[2]-p1[2])*(p0[2]-p1[2])) < EPSILON*EPSILON)

#define MEM_ALLOC(m,s)			(calloc((s),1))
#define MEM_REALLOC(m, p, s)	(realloc((p), (s)))
#define MEM_DISPOSE(m,p)		{ if ((p)) free((p)); (p) = NULL; }

#define RIGHT_SIDE		0
#define LEFT_SIDE		1
#define ERROR_SIDE		(-1)

//#define DEBUG       	1
//#define _TRANSTEST      1

bool Trianguler::debug = false;

Trianguler::Trianguler(LWW::MeshInfo lwmesh, LWPolID p, bool tri_all) : lwmesh(lwmesh)
{
	polys.resize(1);
	polys[0].idx = 0;
	polys[0].points.resize(lwmesh.polSize(p));
	pnts.resize(lwmesh.polSize(p));
	lwmesh.polBaseNormal(p, polys[0].face);
	for(size_t i = 0; i < pnts.size(); i++) {
		lwmesh.pntBasePos(lwmesh.polVertex(p, i), pnts[i].position);
		polys[0].points[i] = i;
	}
	//normals.resize(lwmesh.polSize(polygon));
	//for(size_t i = 0; i < normals.size(); i++) lwmesh.pntBaseNormal(polygon, lwmesh.polVertex(polygon, i), &normals[i].x);

	PolygonData *polygon = &polys[0];
	PointData    *point;
	TriList       tri;
	Node         *prev;
	float        f[3], pos[3];
	tMat4         m;

	polygon = point_merge( polygon );
	//if(polygon->points.size() < 4) return OBJ_NO_ERROR;
	if(polygon->points.size() < 4) return;
	polygon->getFace(f);
	//(*obj->getFace)( obj, polygon->idx, f );

						/*  MATRIX TO TRANSLATE FROM FACE NORMAL TO Z-AXIS  */
	rotate_F2Z( f, m );

	memset( &tri, 0x00, sizeof(tri) );
	tri.pol     = polygon->idx;
	tri.numPnts = polygon->points.size();
	tri.nodes   = node_new( tri.numPnts );
	tri.p       = poly_new();
	tri.d       = NULL;
	tri.queue   = NULL;

	prev = NULL;

	for (int i = 0; i < tri.numPnts; i++)
	{
		point = &pnts[polygon->points[i]];
		mat4_transv( m, point->position, pos );
		node_xy( tri.nodes, i, pos[0], pos[1], polygon->points[i] );
		debug_printf("[%d] XY<%f,%f> POS<%f,%f,%f>\n", i, tri.nodes[i].x, tri.nodes[i].y, point->position[0], point->position[1], point->position[2]);
#ifdef _TRANSTEST
		point->position[0] = pos[0];
		point->position[1] = pos[1];
		point->position[2] = pos[2];
#endif
	}

#ifdef _TRANSTEST
	return;
#endif

									/*  MAKE POLYGON LIST  */
	MakePolygonList( &tri );

	if (tri.convex == TRUE) {
									/*  OUTPUT CONVEX POLYGON AS TRIANGLES  */
		if (tri_all) {
			OutputConvex( &tri );
		}

	} else {
									/*  MAKE MONOTONE POLYGONS  */
		MakeMonotone   ( &tri );
									/*  MAKE FACE LINKS WITH NEW BRIDGES  */
		MakeFaceLinks  ( &tri );
									/*  TRIANGULATE MONOTONE POLYGONS  */
		TripleMonotones( &tri );
									/*  MAKE FACE LINKS WITH NEW BRIDGES  */
		MakeFaceLinks  ( &tri );
									/*  OUTPUT ALL OF TRIANGLES  */
		OutputTriangles( &tri );
	}

	free_list( &tri );
}



/**********  FREE MEMORIES OF THE LIST  **********/

void Trianguler::free_list( TriList *tri )
{
	Poly  *p = tri->p;
	Edge  *e, *next_e;
	Face  *f, *next_f;
	int      i;

	if (tri->p) {
		for (i = 0, e = p->edges; i < p->numEdges; i++) {
			next_e = e->next;
			MEM_DISPOSE( obj->mem, e );
			e = next_e;
		}
		for (i = 0, f = p->faces; i < p->numFaces; i++) {
			next_f = f->next;
			MEM_DISPOSE( obj->mem, f );
			f = next_f;
		}
	}
	MEM_DISPOSE( obj->mem, tri->p );
	MEM_DISPOSE( obj->mem, tri->d );
	MEM_DISPOSE( obj->mem, tri->nodes );
	tri->p     = NULL;
	tri->d     = NULL;
	tri->nodes = NULL;
	if (tri->queue) queue_free( &tri->queue );
}



/**********  MAKE POLYGON LIST  ************/

void Trianguler::MakePolygonList( TriList *tri )
{
	Poly   *p = tri->p;
	Edge   *edge;
	Face   *f0;
	Node   *nd0, *nd1;
	int       convex, i, n0, n1;

	char *tt[] = {"REGULAR", "START", "SPLIT", "END", "MERGE"};

	debug_printf("//////////////////////////////  MakePolygonList\n");

	tri->convex = TRUE;

	if (node_flip( tri->numPnts, tri->nodes ) == TRUE)
	{
		debug_printf("++++++++++  FLIP NODE\n");
		for (i = n0 = 0; i < tri->numPnts; i++, n0--)
		{
			if (n0<0) n0=tri->numPnts+n0;
			n1 = n0-1;  
			if (n1<0) n1=tri->numPnts+n1;
			nd0  = node_get( tri->nodes, n0 );
			nd1  = node_get( tri->nodes, n1 );
			edge = poly_edge_add( tri->p, nd0, nd1 );
			add_line(tri->pol, nd0->idx, nd1->idx);
		}
	}
	else
	{
		for (i = n0 = 0; i < tri->numPnts; i++, n0++)
		{
			n1 = n0+1;  
			if (n1==tri->numPnts) n1=0;
			nd0  = node_get( tri->nodes, n0 );
			nd1  = node_get( tri->nodes, n1 );
			edge = poly_edge_add( tri->p, nd0, nd1 );
			add_line(tri->pol, nd0->idx, nd1->idx);
		}
	}
	edge->next = tri->p->edges;
	tri->p->edges->prev = edge;

	/*  TEST SELF INTERSECTIONS */
	if (SelfIntersect( tri ) == FALSE) {
		tri->convex = TRUE;
		return;
	}

	/*  DEFINE VERTEX TYPES  */
	for (i = 0; i < p->numEdges; i++)
	{
		edge   = edge_get( p->edges, i );
		nd0    = edge->sp;
		convex = node_type( nd0, edge->ep, edge->prev->sp );
		if (convex == FALSE) {
			tri->convex = FALSE;
		}
		debug_printf("EDGE[%d] CONVEX<%d> NODE<%d> POS<%f,%f> PREV[%d] [%s]\n", i, convex, nd0->idx, nd0->x, nd0->y, edge->prev->idx, tt[nd0->type]);
		edge->origin = edge->next;
		edge->face_o = 0;
	}

	/*  MAKE THE PRIMARY FACE  */
	f0        = poly_face_new( p );
	f0->num   = p->numEdges;
	f0->edges = p->edges;
	f0->sp    = p->edges->sp;
}


/**********  TEST SELF INTERSECTIONS  **********/

int Trianguler::SelfIntersect( TriList *tri )
{
	Poly   *p = tri->p;
	Node   *nd0, *nd1, *nd2, *nd3;
	Edge   *edge;
	int      i, j, num;

	for (i = 0; i < (p->numEdges-2); i++)
	{
		edge = edge_get( p->edges, i );
		nd0  = edge->sp;
		nd1  = edge->ep;
		num  = (i > 0) ? p->numEdges : (p->numEdges-1);

		for (j = (i+2); j < num; j++)
		{
			edge = edge_get( p->edges, j );
			nd2  = edge->sp;
			nd3  = edge->ep;

			if (intersect_test( nd0->x, nd0->y, nd1->x, nd1->y, 
								nd2->x, nd2->y, nd3->x, nd3->y )) {
				debug_printf("% INTERSECT LINE(%d)-LINE(%d)\n", nd0->idx, nd2->idx);
				return FALSE;
			}
		}
	}

	return TRUE;
}



/**********  MAKE FACE LINKS  ************/

void Trianguler::MakeFaceLinks( TriList *tri )
{
	Face   *f0, *f1;
	Poly   *p = tri->p;
	Poly   *d = tri->d;
	Edge   *ed, *ed_s, *ed_e, *last, *ed_n;
	int       i;

	debug_printf("//////////////////////////////  MakeFaceLinks\n");
	last = edge_get( p->edges, p->numEdges-1 );

	for (i = 0, ed=d->edges; i < d->numEdges; i++)
	{
		f0 = poly_face_get( p, ed->sp, ed->ep );
		if (f0 == NULL) {
			debug_printf("????? NO FACE FOR THE EDGE <%d> ?????\n", ed->idx);
#ifdef DEBUG
			poly_face_dump( p );
#else
			ed = ed->next;
			continue;
#endif
		}
		ed_s         = face_edge_end( f0, ed->sp );
		ed_e         = face_edge_end( f0, ed->ep );
		ed_n         = ed->next;
//		debug_printf("\n+++ NEW BRIDGE D-ED(%d) <%d>-<%d> FACE<%d> P-EDS<%d> P-EDE<%d> +++\n",
//					ed->idx, ed->sp->idx,ed->ep->idx,f0->idx,ed_s->idx,ed_e->idx);

		if (ed_e->face_o == f0->idx) {
			ed->origin   = ed_e->origin;
			ed_e->origin = ed;
		} else {
			ed->origin   = ed_e->twin;
			ed_e->twin   = ed;
		}
		if (ed_s->face_o == f0->idx) {
			ed->twin     = ed_s->origin;
			ed_s->origin = ed;
		} else {
			ed->twin     = ed_s->twin;
			ed_s->twin   = ed;
		}
		f0->edges    = ed_s;
		f0->sp       = (ed_s->face_o == f0->idx) ? ed_s->sp : ed_s->ep;
		f1           = poly_face_new( p );
		f1->edges    = ed_e;
		f1->sp       = (ed_e->face_o == f0->idx) ? ed_e->sp : ed_e->ep;

		if (ed->prev) {
			ed->prev->next = ed->next;
		}
		if (ed->next) {
			ed->next->prev = ed->prev;
		}
		ed->next     = last->next;
		ed->prev     = last;
		last->next   = ed;
		ed->idx      = p->numEdges++;
		ed           = ed_n;

		f0->num      = face_edge_num( f0 );
		f1->num      = face_edge_num( f1 );
#ifdef DEBUG
		poly_face_dump(p);
#endif
	}
	d->numEdges  = 0;
	d->edges     = NULL;
}



/*********  MAKE MONOTONE  ***********/

void Trianguler::MakeMonotone( TriList *tri )
{
	Poly   *p = tri->p;
	Edge   *edge;
	Queue  *queue;
	Tree   *tree;
	Node   *node;

	debug_printf("//////////////////////////////  MakeMonotone\n");

	tri->d = poly_new();
	queue  = queue_make( &p->faces[0], NULL );
	tree   = tree_new();

	while (edge = queue_pop(queue))
	{
		node = edge->sp;

		switch (node->type) {
		case Node::REGULAR:
		  HandleRegularVertex( tri, tree, edge );
		  break;
		case Node::START:
		  HandleStartVertex  ( tri, tree, edge );
		  break;
		case Node::SPLIT:
		  HandleSplitVertex  ( tri, tree, edge );
		  break;
		case Node::END:
		  HandleEndVertex    ( tri, tree, edge );
		  break;
		case Node::MERGE:
		  HandleMergeVertex  ( tri, tree, edge );
		  break;
		}
	}

	tri->queue = queue;
	tree_free ( &tree );
}



/*********  OUTPUT TRIANGLES  ***********/

void Trianguler::OutputTriangles( TriList *tri )
{
	PolygonData   *poly_o, *poly_n;
	//PolygonMap    *polm_o, *polm_n;
	int          pol, *pols;
	int          points[3];
	//int         normals[3];
	//int          colors[3];
	//int          maps[3];
	Poly        *p = tri->p;
	Face        *f;
	Node        *nd0, *nd1, *nd2;
	Edge        *ed;
	int            i, nPols;

	debug_printf("//////////////////////////////  OutputTriangles\n");

	pols = (int *) MEM_ALLOC( obj->mem, sizeof(int) * p->numFaces );
	if (pols == NULL) return;

	for (i = nPols = 0; i < p->numFaces; i++)
	{
		f = face_get( p->faces, i );

		if (f->num != 3) {
			debug_printf("???  FACE<%d> IS NOT TRIAGLE (%d)  ???\n", f->idx, f->num);
			continue;
		}

		//poly_o = (*obj->polyInfo)( obj, tri->pol );
		poly_o = polyInfo( tri->pol );

		nd0 = f->sp;
		ed  = f->edges;
		face_edge_next( ed, nd0, &ed, &nd1 );
		face_edge_next( ed, nd1, &ed, &nd2 );

		points[0] = poly_o->points[ nd0->idx ];
		points[1] = poly_o->points[ nd1->idx ];
		points[2] = poly_o->points[ nd2->idx ];

		if (f->next == NULL) {
			pol    = tri->pol;
			poly_n = poly_o;
			//(*obj->polPnts)( obj, pol, 3, points );
			polPnts( pol, 3, points );
		}
		else if (regular_triangle( points[0], points[1], points[2] ))
		{
			//pol    = (*obj->addPoly)( obj, 3, points, poly_o->surface );
			pol    = addPoly( 3, points );
			//poly_n = (*obj->polyInfo)( obj, pol );
			poly_n = polyInfo( pol );
			//poly_o = (*obj->polyInfo)( obj, tri->pol );
			poly_o = polyInfo( tri->pol );
			//(*obj->copyPTag)( obj, tri->pol, pol );
			pols[nPols++] = pol;
		}
		else {
			continue;
		}

		/*if (poly_o->normals) {
			normals[0] = poly_o->normals[ nd0->idx ];
			normals[1] = poly_o->normals[ nd1->idx ];
			normals[2] = poly_o->normals[ nd2->idx ];
			(*obj->polNorms)( obj, pol, 3, normals );
		}
		if (poly_o->colors) {
			colors[0] = poly_o->colors[ nd0->idx ];
			colors[1] = poly_o->colors[ nd1->idx ];
			colors[2] = poly_o->colors[ nd2->idx ];
			(*obj->polColors)( obj, pol, 3, colors );
		}
		polm_o = poly_o->polms;
		while (polm_o) {
			if (f->next == NULL) {
				polm_n = polm_o;
			} else {
				polm_n = (*obj->addPolymap)( obj, pol, polm_o->chan, polm_o->layr );
			}
			maps[0] = polm_o->maps[ nd0->idx ];
			maps[1] = polm_o->maps[ nd1->idx ];
			maps[2] = polm_o->maps[ nd2->idx ];
			memcpy( polm_n->maps, maps, sizeof(int) * 3 );
			polm_o = polm_o->next;
		}*/
	}

	//triple_vmad( tri->polid, pols, nPols );
	MEM_DISPOSE( obj->mem, pols );
}



/*********  OUTPUT CONVEX POLYGON  ***********/

void Trianguler::OutputConvex( TriList *tri )
{
	PolygonData   *poly_o, *poly_n;
	//PolygonMap    *polm_o, *polm_n;
	int          pol, *pols;
	int          points[3];
	//NormNO         normals[3];
	//ColNO          colors[3];
	//MapNO          maps[3];
	int            i, idx, num;
	int            nPols = 0;

	debug_printf("//////////////////////////////  OutputConvex\n");

	//poly_o = (*obj->polyInfo)( obj, tri->pol );
	poly_o = polyInfo( tri->pol );
	//num    = poly_o->numPnts-1;
	num    = poly_o->points.size()-1;
	idx    = 0;

	//pols = (PntNO *) MEM_ALLOC( obj->mem, sizeof(PntNO) * poly_o->numPnts );
	pols = (int *) MEM_ALLOC( obj->mem, sizeof(int) * num );
	if (pols == NULL) return;

	for (i = 1; i < num; i++)
	{
		//poly_o = (*obj->polyInfo)( tri->pol );
		poly_o = polyInfo( tri->pol );

		points[0] = poly_o->points[ idx ];
		points[1] = poly_o->points[ i   ];
		points[2] = poly_o->points[ i+1 ];

		if (i == num-1) {
			pol    = tri->pol;
			poly_n = poly_o;
			//(*obj->polPnts)( pol, 3, points );
			polPnts( pol, 3, points );
		} else {
			//pol    = (*obj->addPoly)( obj, 3, points, poly_o->surface );
			pol    = addPoly( 3, points );
			//poly_n = (*obj->polyInfo)( obj, pol );
			poly_n = polyInfo( pol );
			//poly_o = (*obj->polyInfo)( obj, tri->pol );
			poly_o = polyInfo( tri->pol );
			pols[nPols++] = pol;
		}

		/*if (poly_o->normals) {
			normals[0] = poly_o->normals[ idx ];
			normals[1] = poly_o->normals[ i   ];
			normals[2] = poly_o->normals[ i+1 ];
			(*obj->polNorms)( obj, pol, 3, normals );
		}
		if (poly_o->colors) {
			colors[0] = poly_o->colors[ idx ];
			colors[1] = poly_o->colors[ i   ];
			colors[2] = poly_o->colors[ i+1 ];
			(*obj->polColors)( pol, 3, colors );
		}
		polm_o = poly_o->polms;
		while (polm_o) {
			if (i == num-1) {
				polm_n = polm_o;
			} else {
				polm_n = (*obj->addPolymap)( pol, polm_o->chan, polm_o->layr );
			}
			maps[0] = polm_o->maps[ idx ];
			maps[1] = polm_o->maps[ i   ];
			maps[2] = polm_o->maps[ i+1 ];
			memcpy( polm_n->maps, maps, sizeof(maps) );
			polm_o = polm_o->next;
		}*/
	}

	//triple_vmad( tri->pol, pols, nPols );
	if (pols) MEM_DISPOSE( obj->mem, pols );
}



/*****  HANDLE START VERTEX  *****/

void Trianguler::HandleStartVertex( TriList *tri, Tree *tree, Edge *edge )
{
//	debug_printf("\t\t[START]\n");	edge_print(edge);
	tree_add( tri, tree, edge );
	edge_helper( edge, edge->sp );
	tree_dump(tree);
}


/*****  HANDLE END VERTEX  *****/

void Trianguler::HandleEndVertex( TriList *tri, Tree *tree, Edge *edge )
{
	Poly  *d = tri->d;

//	debug_printf("\t\t[END]\n");	edge_print(edge);

	tree_add( tri, tree, edge );
	edge = edge->prev;
	if (edge->helper && edge->helper->type == Node::MERGE) {
		poly_edge_add( d, edge->ep, edge->helper );
		add_line(tri->pol, edge->ep->idx, edge->helper->idx);
	}
	tree_delete( tree, edge );
	tree_dump(tree);
}


/*****  HANDLE SPLIT VERTEX  *****/

void Trianguler::HandleSplitVertex( TriList *tri, Tree *tree, Edge *edge )
{
	Poly  *d = tri->d;
	Leaf  *lf;
	Edge  *el;

//	debug_printf("\t\t[SPLIT]\n");	edge_print(edge);

	if (lf = tree_get_left( tree, tri->p, edge )) {
		el = lf->edge;
		poly_edge_add( d, edge->sp, el->helper );
		add_line(tri->pol,edge->sp->idx,el->helper->idx);
		edge_helper( el, edge->sp );
	}
	tree_add( tri, tree, edge );
	edge_helper( edge, edge->sp );
	tree_dump(tree);
}


/*****  HANDLE MERGE VERTEX  *****/

void Trianguler::HandleMergeVertex( TriList *tri, Tree *tree, Edge *edge )
{
	Poly  *d = tri->d;
	Leaf  *lf;
	Edge  *el, *prev;

//	debug_printf("\t\t[MERGE]\n");	edge_print(edge);
	prev = edge->prev;

	if (prev->helper && prev->helper->type == Node::MERGE) {
		poly_edge_add( d, edge->sp, prev->helper );
		add_line(tri->pol,edge->sp->idx,prev->helper->idx);
	}
	tree_delete( tree, prev );

	if (lf = tree_get_left( tree, tri->p, edge )) {
		el = lf->edge;
		if (el->helper && el->helper->type == Node::MERGE) {
			poly_edge_add( d, edge->sp, el->helper );
			add_line(tri->pol,edge->sp->idx,el->helper->idx);
		}
		edge_helper( el, edge->sp );
	}
	tree_dump(tree);
}


/*****  HANDLE REGULAR VERTEX  *****/

void Trianguler::HandleRegularVertex( TriList *tri, Tree *tree, Edge *edge )
{
	Poly  *d = tri->d;
	Leaf  *lf;
	Edge  *el, *prev;

//	debug_printf("\t\t[REGULAR]\n");	edge_print(edge);

	if (compar_node(edge->sp, edge->ep) == (-1))
	{
		prev = edge->prev;	edge_print(prev);

		if (prev->helper && prev->helper->type == Node::MERGE) {
			poly_edge_add( d, edge->sp, prev->helper );
			add_line(tri->pol,edge->sp->idx,prev->helper->idx);
			tree_delete( tree, prev );
			tree_add( tri, tree, edge );
			edge_helper( edge, edge->sp );
		}
		else {
			tree_add( tri, tree, edge );
			edge_helper( edge, edge->sp );
		}
	}
	else if (lf = tree_get_left( tree, tri->p, edge )) {
		el = lf->edge;
		if (el->helper && el->helper->type == Node::MERGE) {
			poly_edge_add( d, edge->sp, el->helper );
			add_line(tri->pol,edge->sp->idx,el->helper->idx);
		}
		edge_helper( el, edge->sp );
	}
	tree_dump(tree);
}



/**********  TRIANGULATE MONOTONE POLYGON  ************/

void Trianguler::TripleMonotones( TriList *tri )
{
	Poly   *p = tri->p;
	Poly   *d = tri->d;
	Queue  *q;
	Stack  *s;
	Face   *f;
	Edge   *e0, *e1, *es, *ed, *er;
	Node   *nd0, *nd1, *nd2;
	int       i, j, convex;

	debug_printf("//////////////////////////////  TriangulateMonotonePolygon\n");
	s = stack_make( p->numEdges );
	q = tri->queue;

	for (i = 0; i < p->numFaces; i++)
	{
		f  = face_get( p->faces, i );
//		debug_printf("*****  NEW FACE <%d>  *****\n",f->idx);
		if (f->num == 3) continue;
		q  = queue_make( f, q );
		e0 = queue_pop( q );
		stack_push( s, e0 );
		e0 = queue_pop( q );
		stack_push( s, e0 );

		for (j = 2; j < q->num-1; j++)
		{
			e1 = queue_pop( q );
			es = stack_ref( s );
//			debug_printf("\n@@@@  START BASE EDGE <%d>  @@@@\n", e1->idx);
			stack_dump(s,f);

			if (face_edge_side(f,e1) != face_edge_side(f,es))
			{
//				debug_printf("--  MODE A  ---\n");
				nd0 = face_edge_node(f,e1);
				while (es = stack_pop(s)) {
					nd1 = face_edge_node(f,es);
//					debug_printf("{POP} <%d> STACK_NUM<%d>\n",es->idx,s->num);
					if (s->num > 0) {
						poly_edge_add( d, nd0, nd1 );
						add_line(tri->pol,nd0->idx,nd1->idx);
					}
				}
//				debug_printf("{PUSH} <%d> STACK_NUM<%d>\n",e0->idx,s->num);
				stack_push(s, e0);
//				debug_printf("{PUSH} <%d> STACK_NUM<%d>\n",e1->idx,s->num);
				stack_push(s, e1);
			}
			else
			{
//				debug_printf("--  MODE B  ---\n");
				nd0 = face_edge_node(f,e1);
				es  = stack_pop(s);
//				debug_printf("{POP} <%d> STACK_NUM<%d>\n",es->idx,s->num);
				nd2 = face_edge_node(f,es);
//				debug_printf("E1<%d> ES<%d>\n",nd0->idx,nd2->idx);

				while (er  = stack_ref(s))
				{
					nd1 = face_edge_node(f,er);

					if (face_edge_side(f,e1) == LEFT_SIDE) {
//						debug_printf("LEFT_SIDE ");
						convex = node_convex(nd2,nd0,nd1);
					} else {
//						debug_printf("RIGHT_SIDE ");
						convex = node_convex(nd2,nd1,nd0);
					}
					if (convex == FALSE) break;

					es  = stack_pop( s );
//					debug_printf("{POP} <%d> STACK_NUM<%d>\n",es->idx,s->num);
					ed  = poly_edge_add( d, nd0, nd1 );
					add_line(tri->pol,nd0->idx,nd1->idx);
					nd2 = nd1;
				}
//				debug_printf("{PUSH} <%d> STACK_NUM<%d>\n",es->idx,s->num);
				stack_push(s, es);
//				debug_printf("{PUSH} <%d> STACK_NUM<%d>\n",e1->idx,s->num);
				stack_push(s, e1);
			}

			e0 = e1;
		}

		e1  = q->list[q->num-1];
		nd0 = face_edge_node(f,e1);
		es  = stack_pop(s);
//		debug_printf("LAST<%d> STACK_NUM<%d>\n",nd0->idx,s->num);
		while (es = stack_pop(s)) {
//			debug_printf("POP <%d> STACK_NUM<%d>\n",es->idx,s->num);
			if (s->num > 0) {
				nd1 = face_edge_node(f,es);
				poly_edge_add( d, nd0, nd1 );
				add_line(tri->pol,nd0->idx,nd1->idx);
			}
		}
	}

	stack_free( &s );
}



/********** TREE HANDLE FUNCTIONS  ***********/

Trianguler::Tree *Trianguler::tree_new()
{
	Tree  *t;

	t = (Tree *) MEM_ALLOC( obj->mem, sizeof(Tree) );
	return t;
}


Trianguler::Leaf *Trianguler::tree_get( Tree *tree, int idx )
{
	Leaf  *leaf = tree->leaves;

	while (leaf && idx--) { leaf = leaf->next; }
	return leaf;
}


Trianguler::Leaf *Trianguler::tree_add( TriList *tri, Tree *tree, Edge *edge )
{
	Leaf  *leaf, *last;

	if (edge) {
		debug_printf("TREE ADD EDGE<%d>\n", edge->idx);
	} else {
		debug_printf("????? TREE ADD [EDGE IS NULL] ??????\n");
	}
	leaf = (Leaf *) MEM_ALLOC( obj->mem, sizeof(Leaf) );
	leaf->edge = edge;
	leaf->idx  = tree->num;
	leaf->next = NULL;

	if (tree->leaves) {
		last = tree_get( tree, tree->num-1 );
		last->next  = leaf;
		leaf->prev  = last;
	} else {
		tree->leaves = leaf;
	}
	tree->num++;

	return leaf;
}


Trianguler::Leaf *Trianguler::tree_find( Tree *tree, Edge *edge )
{
	Leaf  *leaf = tree->leaves;

	if (edge == NULL) return NULL;

	while (leaf) {
		if (leaf->edge->idx == edge->idx) return leaf;
		leaf = leaf->next;
	}
	return NULL;
}


void Trianguler::tree_dump( Tree *tree )
{
#ifdef DEBUG
	Leaf  *leaf = tree->leaves;

	while (leaf) {
		debug_printf("[%d] LEAF", tree->num);
		if (leaf->edge) {
			debug_printf(" EDGE[%d]", leaf->edge->idx);
		} else {
			debug_printf(" EDGE[none]");
		}
		if (leaf->edge && leaf->edge->helper) {
			debug_printf(" HELPER[%d]\n", leaf->edge->helper->idx);
		} else {
			debug_printf(" HELPER[none]\n");
		}
		leaf = leaf->next;
	}
#endif
}


void Trianguler::tree_delete( Tree *tree, Edge *edge )
{
	Leaf  *prev, *next, *leaf;

//	debug_printf("TREE DELETE EDGE<%d>\n", edge->idx);
	leaf = tree_find( tree, edge );
	if (leaf) {
//		debug_printf("DELETE EDGE<%d>\n", leaf->edge->idx);

		prev = leaf->prev;
		next = leaf->next;

//		if (prev)
//			debug_printf("PREV EDGE<%d>\n", prev->edge->idx);
//		if (next)
//			debug_printf("NEXT EDGE<%d>\n", next->edge->idx);

		if (prev) {
			prev->next   = next;
		} else {
			tree->leaves = next;
		}
		if (next) {
			next->prev = prev;
		}
		MEM_DISPOSE( obj->mem, leaf );
		tree->num--;
		if (!tree->num) tree->leaves = NULL;
		tree_dump(tree);
	}
}


Trianguler::Leaf *Trianguler::tree_get_left( Tree *tree, Poly *p, Edge *edge )
{
	Edge  *ed, *left_edge;
	Leaf  *leaf, *left_leaf;
	Node  *lo, *hi, *nd, *nd1, *nd0;
	float   dt, dl=0.0f, xi, yi;
	int      i;

	leaf = NULL;
	nd   = edge->sp;
	nd0  = edge->ep;
	nd1  = edge->prev->sp;
	ed   = edge->next;
	left_edge = NULL;
	left_leaf = NULL;

	if (nd0->type == Node::START && FP_EQUAL(nd->y,nd0->y) && nd->x > nd0->x) {
		leaf = tree_find( tree, ed );
		return leaf;
	}

	for (i = 0; i < tree->num; i++)
	{
		leaf = tree_get( tree, i );
		ed   = leaf->edge;
		lo   = (ed->sp->y < ed->ep->y) ? ed->sp : ed->ep;
		hi   = (ed->sp->y < ed->ep->y) ? ed->ep : ed->sp;
//		debug_printf("[%d] FIND LEFT EDGE <%d> [%d]-[%d]\n", tree->num, ed->idx, lo->idx, hi->idx);

		if (nd->y <= hi->y && nd->y >= lo->y && 
		   (lo->x <= nd->x || hi->x <= nd->x))
		{
			if (intersect_lines( lo->x, lo->y, hi->x, hi->y, nd->x, nd->y, nd->x+1.0f, nd->y, &xi, &yi ) == TRUE &&
				FP_POINT(xi,yi,nd0->x,nd0->y) == FALSE &&
				FP_POINT(xi,yi,nd1->x,nd1->y) == FALSE)
			{
				dt = xi - nd->x;
//				debug_printf("INTERSECT POINT <%f,%f> DT <%f> EDGE<%d>\n", xi,yi,dt, ed->idx);

				if (dt < EPSILON) {
					if (left_edge) {
						if (FP_EQUAL(dt,dl) &&
							FP_POINT(xi,yi,hi->x,hi->y)) {
							dl   = dt;
							left_edge = ed;
							left_leaf = leaf;
//							debug_printf("******* CASE 1\n");
						}
						else if (dt > dl) {
							dl   = dt;
							left_edge = ed;
							left_leaf = leaf;
//							debug_printf("******* CASE 2\n");
						}
						else if (FP_EQUAL(ed->sp->x,left_edge->ep->x) &&
								 FP_EQUAL(ed->sp->y,left_edge->ep->y) &&
								 FP_EQUAL(ed->ep->x,left_edge->sp->x) &&
								 FP_EQUAL(ed->ep->y,left_edge->sp->y)) {
								left_edge = ed;
//							debug_printf("******* CASE 3\n");
						}
					} else {
						dl   = dt;
						left_edge = ed;
						left_leaf = leaf;
//						debug_printf("******* CASE 4\n");
					}
				}
			}
		}
	}

	if (left_edge) 
	{
		if (face_edge_side(p->faces, left_edge) == RIGHT_SIDE) {
//			debug_printf("EDGE <%d> IS RIGHT SIDE\n", left_edge->idx);
			left_leaf = NULL;
		}
	}
//	if (left_leaf) debug_printf("LEFT EDGE <%d>\n", left_leaf->edge->idx);

	return left_leaf;
}


void Trianguler::tree_free( Tree **t )
{
	Leaf  *leaf, *next;

	if (*t) {
		leaf = (*t)->leaves;
		while (leaf) {
			next = leaf->next;
			MEM_DISPOSE( obj->mem, leaf );
			leaf = next;
		}
		MEM_DISPOSE( obj->mem, (*t) );
		(*t) = NULL;
	}
}



/********** QUEUE HANDLE FUNCTIONS  ***********/

int Trianguler::compar_node( Node *sn, Node *dn )
{
	if (FP_EQUAL(sn->y,dn->y)) {
//		return (sn->x < dn->x) ? -1 : 1;
		return FP_LESS(sn->x,dn->x) ? -1 : 1;
//	} else if (sn->y > dn->y) {
	} else if (FP_GREAT(sn->y,dn->y)) {
		return -1;
	} else {
		return 1;
	}
}

int Trianguler::compar_edge( const void *s, const void *d )
{
	Edge  *sn, *dn;

	sn = *((Edge **)(s));
	dn = *((Edge **)(d));

	if (FP_EQUAL(sn->key->y,dn->key->y)) {
//		return (sn->key->x < dn->key->x) ? -1 : 1;
		return FP_LESS(sn->key->x,dn->key->x) ? -1 : 1;
//	} else if (sn->key->y > dn->key->y) {
	} else if (FP_GREAT(sn->key->y,dn->key->y)) {
		return -1;
	} else {
		return 1;
	}
}

Trianguler::Queue *Trianguler::queue_make( Face *f, Queue *old_q )
{
	Queue  *q;
	Edge   *ed;
	Node   *sp;
	int       i;

	if (old_q) {
		q = old_q;
		if (q->num < f->num)
			q->list = (Edge **) MEM_REALLOC( obj->mem, q->list, sizeof(Edge *) * f->num );
		q->num  = f->num;
		q->cur  = 0;
	} else {
		q = (Queue *) MEM_ALLOC( obj->mem, sizeof(Queue) );
		q->list = (Edge **) MEM_ALLOC( obj->mem, sizeof(Edge *) * f->num );
		q->num  = f->num;
		q->cur  = 0;
	}

	ed = f->edges;
	sp = f->sp;

	for (i = 0; i < f->num; i++) {
		ed->key    = sp;
		q->list[i] = ed;
		face_edge_next( ed, sp, &ed, &sp );
	}
	qsort( q->list, f->num, sizeof(Edge *), compar_edge );

	for (i = 0; i < f->num; i++) {
		debug_printf("QUEUE [%d] <%f,%f>\n", q->list[i]->idx, q->list[i]->key->x, q->list[i]->key->y);
	}

	return q;
}

Trianguler::Edge *Trianguler::queue_pop( Queue *q )
{
	Edge  *edge;

	if (q->cur == q->num) return NULL;
	edge = q->list[q->cur];
	q->cur++;

	return edge;
}

void Trianguler::queue_free( Queue **q )
{
	if (*q) {
		MEM_DISPOSE( obj->mem, (*q)->list );
		MEM_DISPOSE( obj->mem, (*q) );
		(*q) = NULL;
	}
}




/********** STACK HANDLE FUNCTIONS  ***********/

Trianguler::Stack *Trianguler::stack_make( int num )
{
	Stack  *s;

	s = (Stack *) MEM_ALLOC( obj->mem, sizeof(Stack) );
	s->list = (Edge **) MEM_ALLOC( obj->mem, sizeof(int) * num );
	s->num  = 0;
	s->max  = num;
	return s;
}


void Trianguler::stack_free( Stack **s )
{
	if (*s) {
		MEM_DISPOSE( obj->mem, (*s)->list );
		MEM_DISPOSE( obj->mem, (*s) );
		(*s) = NULL;
	}
}


Trianguler::Edge *Trianguler::stack_pop( Stack *s )
{
	Edge  *e = NULL;

	if (s->num) {
		e = s->list[s->num-1];
		s->num--;
	}
	return e;
}


Trianguler::Edge *Trianguler::stack_ref( Stack *s )
{
	Edge  *e = NULL;

	if (s->num) {
		e = s->list[s->num-1];
	} else {
		debug_printf("??????  STUCK IS EMPTY (%d) ????????\n", s->num);
	}
	return e;
}


void Trianguler::stack_push( Stack *s, Edge *e )
{
	if (s->num < s->max)
	{
		s->list[s->num++] = e;
	}
}


void Trianguler::stack_dump( Stack *s, Face *f )
{
#ifdef DEBUG
	int      i;
	Edge  *ed;
	Node  *nd;

	for (i = s->num-1; i >= 0; i--)
	{
		ed = s->list[i];
		nd = face_edge_node(f,ed);
		debug_printf("STACK[%d] ED<%d> NODE<%d>\n", i, ed->idx, nd->idx);
	}
#endif
}




/**********  NODE HANDLE FUNCTIONS  ***********/

Trianguler::Node *Trianguler::node_new( int n )
{
	Node   *node;
	int       i;

	node = (Node *) MEM_ALLOC( obj->mem, sizeof(Node) * n );

	for (i = 0; i < n; i++) {
		node[i].idx  = i;
		node[i].type = Node::REGULAR;
	}

	return node;
}


Trianguler::Node *Trianguler::node_get( Node *node, int idx )
{
	return &node[idx];
}


Trianguler::Node *Trianguler::node_xy( Node *node, int idx, float x, float y, int pnt_no )
{
	int   n;

	node[idx].pnt_no = pnt_no;
	node[idx].x      = x;
	node[idx].y      = y;
	if (idx >= 3) {
	for (n = 0; n < idx-1; n++) {
		if (node[idx  ].pnt_no == node[n  ].pnt_no &&
			node[idx-1].pnt_no == node[n+1].pnt_no) {
			line_offset( node[idx-1].x, node[idx-1].y, node[idx].x, node[idx].y, EPSILON*10,
						&node[idx-1].x, &node[idx-1].y, &node[idx].x, &node[idx].y );
			return node;
		}
	}
	}
	return node;
}


int Trianguler::node_low( Node *node, Node *t )
{
//	if ((FP_EQUAL(t->y,node->y) && t->x > node->x) || t->y < node->y) {
	if ((FP_EQUAL(t->y,node->y) && FP_GREAT(t->x,node->x)) || FP_LESS(t->y,node->y)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


int Trianguler::node_high( Node *node, Node *t )
{
//	if ((FP_EQUAL(t->y,node->y) && t->x < node->x) || t->y > node->y) {
	if ((FP_EQUAL(t->y,node->y) && FP_LESS(t->x,node->x)) || FP_GREAT(t->y,node->y)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


float Trianguler::node_area( Node *node, Node *next, Node *prev )
{
	float x1, y1, x2, y2, d;

	x1 = node->x - prev->x;
	y1 = node->y - prev->y;
	x2 = next->x - prev->x;
	y2 = next->y - prev->y;
    d  = x1 * y2 - x2 * y1;
//  debug_printf("ANG = %f NODE<%d> NEXT<%d> PREV<%d>\n", d, node->idx, next->idx, prev->idx);
    return d;
}


int Trianguler::node_convex( Node *node, Node *next, Node *prev )
{
	float  d;
	int     convex;
	d = node_area(node, next, prev);
    convex = (d >= 0.0) ? TRUE : FALSE;
//  debug_printf("CONVEX<%d> NODE<%d> NEXT<%d> PREV<%d>\n",convex,node->idx,next->idx,prev->idx);
    return convex;
}


int Trianguler::node_type( Node *node, Node *next, Node *prev )
{
	int  convex;

	convex = node_convex( node, next, prev );

	if (node_low(node, next) && node_low(node, prev))
	{
		node->type = convex ? Node::START : Node::SPLIT;
	}
	else if (node_high(node, next) && node_high(node, prev))
	{
		node->type = convex ? Node::END : Node::MERGE;
	}
	else
	{
		node->type = Node::REGULAR;
	}

	return convex;
}


int Trianguler::node_flip ( int num, Node *node )
{
	Node  *node_s = node;
	Node  *node_t = node+1;
	float   d = 0.0;
	int      i;

	for (i = 0; i < num-2; i++, node_t++)
	{
		d += node_area(node_t, node_t+1, node_s);
	}
	return d < 0.0 ? TRUE : FALSE;
}



/**********  EDGE HADLE FUNCTIONS  ***********/


Trianguler::Edge *Trianguler::edge_get( Edge *edge, int idx )
{
	while (idx--) { edge = edge->next; }
	return edge;
}


Trianguler::Edge *Trianguler::edge_helper( Edge *edge, Node *helper )
{
//	debug_printf("HELPER <%d> EDGE[%d]\n", helper->idx, edge->idx);
	edge->helper = helper;
	return edge;
}


void Trianguler::edge_print( Edge *edge )
{
//	debug_printf("[%d] EDGE - SP<%d> EP<%d>", edge->idx, edge->sp->idx, edge->ep->idx);
	if (edge->helper) {
		debug_printf(" HELP<%d>\n", edge->helper->idx);
	} else {
		debug_printf(" HELP<none>\n");
	}
}




/**********  POLY HADLE FUNCTIONS  ***********/


Trianguler::Poly *Trianguler::poly_new()
{
	return (Poly *) MEM_ALLOC( obj->mem, sizeof(Poly) );
}


Trianguler::Edge *Trianguler::poly_edge_add( Poly *poly, Node *sp, Node *ep )
{
	Edge  *edge, *last;

	edge = (Edge *) MEM_ALLOC( obj->mem, sizeof(Edge) );
	edge->sp     = sp;
	edge->ep     = ep;
	edge->idx    = poly->numEdges;
	edge->helper = NULL;
	edge->next   = NULL;
	edge->prev   = NULL;
	edge->origin = NULL;
	edge->twin   = NULL;
	edge->face_o = (-1);
	edge->face_t = (-1);

	if (poly->edges) {
		last = edge_get( poly->edges, poly->numEdges-1 );
		last->next  = edge;
		edge->prev  = last;
	} else {
		poly->edges = edge;
	}

	poly->numEdges++;
	debug_printf("\t\t----->  POLY_EDGE_ADD[%d] SP<%d> EP<%d>  ***\n", edge->idx, sp->idx, ep->idx);

	return edge;
}


Trianguler::Face *Trianguler::poly_face_new( Poly *p )
{
	Face  *f, *last;

	f = (Face *) MEM_ALLOC( obj->mem, sizeof(Face) );

	if (f) {
		f->idx   = p->numFaces;
		f->num   = 0;
		f->edges = NULL;
		f->sp    = NULL;
		f->next  = NULL;
		if (p->faces) {
			last = face_get(p->faces, p->numFaces-1);
			last->next = f;
		} else {
			p->faces   = f;
		}
		p->numFaces++;
	}
	return f;
}


Trianguler::Face *Trianguler::poly_face_get( Poly *p, Node *sp, Node *ep )
{
	Face  *f = p->faces;
	Edge  *edge_s;
	Node  *node_s;
	int      i, j;
	int      count;

	for (i = 0; i < p->numFaces; i++, f=f->next)
	{
		if (f->num == 3) continue;

		node_s = f->sp;
		edge_s = f->edges;
		count  = 0;

		for (j = 0; j < f->num; j++)
		{
			if (node_s->idx == sp->idx) count++;
			if (node_s->idx == ep->idx) count++;
			face_edge_next( edge_s, node_s, &edge_s, &node_s );
		}
		if (count == 2) {
			return f;
		}
	}
	return NULL;
}


void Trianguler::poly_face_dump( Poly *p )
{
#ifdef DEBUG
	Face  *f = p->faces;
	Edge  *ed;
	Node  *sp;
	int      i;

	while (f)
	{
		debug_printf("FACE[%d] SP<%d> NUM<%d>\n", f->idx, f->sp->idx, f->num);
		ed = f->edges;
		sp = f->sp;
		for (i = 0; i < f->num; i++) {
			debug_printf("\t[%d] EDGE<%d> FSP<%d> FACE_O<%d> FACE_T<%d> ESP<%d> EEP<%d>", 
						i, ed->idx, sp->idx, ed->face_o, ed->face_t, ed->sp->idx, ed->ep->idx);
			if (ed->origin) {
				debug_printf(" ORIGIN<%d>", ed->origin->idx);
			} else {
				debug_printf(" ORIGIN<none>");
			}
			if (ed->twin) {
				debug_printf(" TWIN<%d>\n", ed->twin->idx);
			} else {
				debug_printf(" TWIN<none>\n");
			}
			face_edge_next(ed,sp,&ed,&sp); 
		}
		f = f->next;
	}
#endif
}



/**********  FACE HADLE FUNCTIONS  ***********/


int Trianguler::face_edge_num( Face *face )
{
	Edge  *ed;
	Node  *sp;
	int      num = 0;

	ed = face->edges;
	sp = face->sp;

	do {
		num++;
//		debug_printf("FACE_EDGE_NUM[%d] NUM[%d] EDGE<%d> SP<%d>\n", face->idx, num, ed->idx, sp->idx);
		if (ed->sp->idx == sp->idx) {
			ed->face_o = face->idx;
		} else {
			ed->face_t = face->idx;
		}
		face_edge_next(ed,sp,&ed,&sp);
	} while (ed->idx != face->edges->idx);

	return num;
}


Trianguler::Edge *Trianguler::face_edge_end( Face *face, Node *ep )
{
	Edge  *edge_s;
	Node  *node_s, *node_e;
	int      j;

	node_s = face->sp;
	edge_s = face->edges;

	for (j = 0; j < face->num; j++)
	{
		node_e = (edge_s->sp->idx == node_s->idx) ? edge_s->ep : edge_s->sp;

		if (node_e->idx == ep->idx) {
			return edge_s;
		}
		face_edge_next( edge_s, node_s, &edge_s, &node_s );
	}
	return NULL;
}


void Trianguler::face_edge_next( Edge *edge, Node *sp, Edge **edge_n, Node **sp_n )
{
	if (edge->ep->idx == sp->idx) {
		*sp_n   = edge->sp;
		*edge_n = edge->twin;
	} else if (edge->sp->idx == sp->idx) {
		*sp_n   = edge->ep;
		*edge_n = edge->origin;
	} else {
		*sp_n   = NULL;
		*edge_n = NULL;
	}
}


Trianguler::Face *Trianguler::face_get( Face *face, int idx )
{
	while (idx--) { face = face->next; }
	return face;
}


int Trianguler::face_edge_side( Face *f, Edge *e )
{
	if (e->face_o == f->idx) {
		if ((FP_EQUAL(e->sp->y,e->ep->y) && FP_LESS(e->sp->x,e->ep->x)) ||
			 FP_GREAT(e->sp->y,e->ep->y)) {
//		if ((FP_EQUAL(e->sp->y,e->ep->y) && e->sp->x < e->ep->x) ||
//			(e->sp->y > e->ep->y)) {
			return LEFT_SIDE;
		} else {
			return RIGHT_SIDE;
		}
	}
	else if (e->face_t == f->idx) {
		if ((FP_EQUAL(e->ep->y,e->sp->y) && FP_LESS(e->ep->x,e->sp->x)) ||
			 FP_GREAT(e->ep->y,e->sp->y)) {
//		if ((FP_EQUAL(e->ep->y,e->sp->y) && e->ep->x < e->sp->x) ||
//			(e->ep->y > e->sp->y)) {
			return LEFT_SIDE;
		} else {
			return RIGHT_SIDE;
		}
	}
	return ERROR_SIDE;
}


Trianguler::Node *Trianguler::face_edge_node( Face *f, Edge *e )
{
	if (e->face_o == f->idx) {
		return e->sp;
	}
	else if (e->face_t == f->idx) {
		return e->ep;
	}
	return NULL;
}


/**********  TEST INTERSECTION  ***********/

int Trianguler::intersect_test( float x1, float y1, float x2, float y2, 
                           float x3, float y3, float x4, float y4 )
{
	float  x21, y21, x43, y43, x31, y31, det, detinv, s, t;

	x21 = x2 - x1;
	y21 = y2 - y1;
	x43 = x4 - x3;
	y43 = y4 - y3;
	x31 = x3 - x1;
	y31 = y3 - y1;
	det = x43 * y21 - y43 * x21;

	if (fabs(det) < EPSILON) return FALSE;

	detinv = 1.0f / det;
	s = (x43 * y31 - y43 * x31) * detinv;
	t = (x21 * y31 - y21 * x31) * detinv;

	if (s < 0.0 || s > 1.0 || t < 0.0 || t > 1.0)
	{
	    return FALSE;
	}
	else if (fabs(s) < EPSILON || fabs(1.0-s) < EPSILON ||
	         fabs(t) < EPSILON || fabs(1.0-t) < EPSILON)
	{
	    return FALSE;
	}

	return TRUE;
}


/**********  INTERSECTION OF TWO LINES  ***********/

int Trianguler::intersect_lines( float x1k, float y1k, float x1l, float y1l, 
                            float x2k, float y2k, float x2l, float y2l, 
                            float *xi, float *yi )
{
	float    a1, b1, c1, a2, b2, c2, det;

	a1 = y1k - y1l;
	b1 = x1l - x1k;
	c1 = x1k*y1l - x1l*y1k;
	a2 = y2k - y2l;
	b2 = x2l - x2k;
	c2 = x2k*y2l - x2l*y2k;

	det = a1*b2 - a2*b1;
	if (fabs(det) > EPSILON) {
		*xi = (b1*c2 - b2*c1) / det;
		*yi = (a2*c1 - a1*c2) / det;
		return TRUE;
	}
	return FALSE;
}



void Trianguler::rotate_F2Z( float *f, tMat4 mm )
{
	float cosa, sina, cosb, sinb;
	float xd, yd, zd, xzd, xyzd;

	xd = f[0];
	yd = f[1];
	zd = f[2];

	mat4_identify(mm);
	xzd  =  sqrt(xd*xd+zd*zd);
	xyzd =  sqrt(xd*xd+yd*yd+zd*zd);

	if (xzd >= EPSILON) {
		cosa =  zd / xzd;
		sina = -xd / xzd;
		cosb =  xzd / xyzd;
		sinb =  yd / xyzd;
		mm[0][0] = cosa;
		mm[0][1] = sina*sinb;
		mm[0][2] = -sina*cosb;
		mm[1][1] = cosb;
		mm[1][2] = sinb;
		mm[2][0] = sina;
		mm[2][1] = -cosa*sinb;
		mm[2][2] = cosa*cosb;
	} else {
		mm[0][0] = -f[1];
		mm[1][1] = 0.0;
		mm[1][2] = 1.0;
		mm[2][1] = 1.0;
		mm[2][2] = 0.0;
	}
//	debug_printf("COSA<%f> SINA<%f> COSB<%f> SINB<%f>\n", cosa, sina, cosb, sinb);
}


void Trianguler::line_offset( float  x1, float  y1, float  x2, float  y2, float off,
                         float *x3, float *y3, float *x4, float *y4 )
{
	float  f1, g1, len;

	len = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
	f1  = (x2 - x1) / len;
	g1  = (y2 - y1) / len;
	*x3 = x1 - g1 * off;
	*y3 = y1 + f1 * off;
	*x4 = x2 - g1 * off;
	*y4 = y2 + f1 * off;
}



void Trianguler::add_line( int pol, int idx1, int idx2 )
{
#ifdef DEBUG
	/*PolygonData  *polygon;
	PntNO         points[2];

	polygon = (*obj->polyInfo)( obj, pol );
	points[0] = polygon->points[idx1];
	points[1] = polygon->points[idx2];
	(*obj->addPoly)(obj, 2, points, polygon->surface);*/
#endif
}



/*****  GET ERROR STRING  ******/
void Trianguler::debug_printf( char *buf, ... )
{
	char	buffer[4096];
	va_list 	arglist;

	if (debug)
	{
		va_start (arglist, buf);
		vsprintf (buffer, buf, arglist);
		va_end (arglist);
		PRN(buffer);
	}
}


/********  MERGE DUPLICATE POINTS  ******/

Trianguler::PolygonData *Trianguler::point_merge( PolygonData *polygon )
{
	PointData    *point, *point0;
	//PolygonMap   *polm;
	int          n, num = 1;

	//point0  = (*obj->pointInfo)( obj, polygon->points[0] );
	point0  = pointInfo( polygon->points[0] );

	//for (n = 1; n < polygon->numPnts; n++)
	for (n = 1; n < (int)polygon->points.size(); n++)
	{
		//point  = (*obj->pointInfo)( obj, polygon->points[n] );
		point  = pointInfo( polygon->points[n] );
		if (FP3_POINT(point0->position,point->position)) continue;
		//if (n == polygon->numPnts-1) {
		if (n == (int)(polygon->points.size()-1)) {
			//point0  = (*obj->pointInfo)( obj, polygon->points[0] );
			point0  = pointInfo( polygon->points[0] );
			if (FP3_POINT(point0->position,point->position)) continue;
		}

		polygon->points[num] = polygon->points[n];
		/*if (polygon->normals) {
			polygon->normals[num] = polygon->normals[n];
		}
		if (polygon->colors) {
			polygon->colors[num] = polygon->colors[n];
		}
		polm = polygon->polms;
		while (polm) {
			polm->maps[num] = polm->maps[n];
			polm = polm->next;
		}*/
		num++;
		point0 = point;
	}
	//polygon->numPnts = num;
	polygon->points.resize(num);

	return polygon;
}


/*********  TRIANGULATE DISCONTINUOS VMAP  ***********/

#if 0
void Trianguler::triple_vmad( int pol, int *pols, int nPols )
{
	LayerData      *layer = obj->layer;
	VMadData       *vmad;
	VMadPoint      *vpnt;
	VMadPolygon    *vpol;
	int            vmad_no, pnt_no, vpol_no, n, nVPols, keep;

	for (vmad_no = 0; vmad_no < layer->numVMads; vmad_no++)
	{
		vmad = &layer->vmads[ vmad_no ];

		for (pnt_no = 0; pnt_no < vmad->numPnts; pnt_no++) 
		{
			vpnt   = &vmad->points[ pnt_no ];
			vpol   = vpnt->polygons;
			nVPols = 0;
			while (vpol) {
				nVPols++;
				vpol = vpol->next;
			}
			vpol = vpnt->polygons;
			for (vpol_no = 0; vpol_no < nVPols; vpol_no++, vpol = vpol->next) {
				if (vpol->pol_no == pol) {
					keep = point_in_polygon( obj, vpnt->pnt_no, vpol->pol_no );
					for (n = 0; n < nPols; n++) {
						if (point_in_polygon( obj, vpnt->pnt_no, pols[n] )) {
							if (keep) {
								(*obj->addVMad)( obj, vmad_no, vpnt->pnt_no, pols[n], vpol->vals );
							} else {
								vpol->pol_no = pols[n];
								keep = TRUE;
							}
						}
					}
				}
			}
		}
	}
}
#endif



/*********  RETURN TRUE IF POINT IN THE POLYGON  ***********/

int Trianguler::point_in_polygon( int pnt_no, int pol_no )
{
	PolygonData     *poly;
	int             n;

	//poly = (*obj->polyInfo)( obj, pol_no );
	poly = polyInfo( pol_no );
	//for (n = 0; n < poly->numPnts; n++) {
	for (n = 0; n < (int)poly->points.size(); n++) {
		if (poly->points[n] == pnt_no) return TRUE;
	}
	return FALSE;
}



/*********  RETURN TRUE IF TRIANGLE IS REGULAR  ***********/

int Trianguler::regular_triangle( int p0, int p1, int p2 )
{
	PointData     *pnt0, *pnt1, *pnt2;
	float        v1[3], v2[3], vo[3], dist;

	//pnt0  = (*obj->pointInfo)( obj, p0 );
	pnt0  = pointInfo( p0 );
	//pnt1  = (*obj->pointInfo)( obj, p1 );
	pnt1  = pointInfo( p1 );
	//pnt2  = (*obj->pointInfo)( obj, p2 );
	pnt2  = pointInfo( p2 );
	v1[0] = (pnt1->position[0] - pnt0->position[0]);
	v1[1] = (pnt1->position[1] - pnt0->position[1]);
	v1[2] = (pnt1->position[2] - pnt0->position[2]);
	dist  = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);
	if (dist < EPSILON) return FALSE;
	v1[0] /= dist;
	v1[1] /= dist;
	v1[2] /= dist;
	v2[0] = (pnt2->position[0] - pnt0->position[0]);
	v2[1] = (pnt2->position[1] - pnt0->position[1]);
	v2[2] = (pnt2->position[2] - pnt0->position[2]);
	dist  = sqrt(v2[0]*v2[0] + v2[1]*v2[1] + v2[2]*v2[2]);
	if (dist < EPSILON) return FALSE;
	v2[0] /= dist;
	v2[1] /= dist;
	v2[2] /= dist;
	vo[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	vo[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	vo[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	dist  =  vo[0] * vo[0] + vo[1] * vo[1] + vo[2] * vo[2];
	return (dist < EPSILON) ? FALSE : TRUE;
}
