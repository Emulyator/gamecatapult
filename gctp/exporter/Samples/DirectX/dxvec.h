/*********************************************************************/
/*                                                                   */
/* FILE :        dxvec.h                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _DXVEC_H_
#define _DXVEC_H_

#include <string.h>
#include <math.h>
#include "dxtyp.h"

#define EPSILON_F  				1e-6f
#define EPSILON_D  				1e-15
#define MAX_FLT					1e+38f
#define MAX_DBL					1e+308

#define DVEC_SET(v,x,y,z)		((v)[0]=(x),(v)[1]=(y),(v)[2]=(z))
#define DVEC_COPY(vo,vi)		((vo)[0]=(vi)[0],(vo)[1]=(vi)[1],(vo)[2]=(vi)[2])
#define DVEC_SUB(vo,v1,v2)		(DVEC_SET((vo),(v1)[0]-(v2)[0],(v1)[1]-(v2)[1],(v1)[2]-(v2)[2]))
#define DVEC_LENV(v)			(sqrt((v)[0]*(v)[0]+(v)[1]*(v)[1]+(v)[2]*(v)[2]))
#define DVEC_SCAL(vo,vi,d)		((vo)[0]=(vi)[0]*(d),(vo)[1]=(vi)[1]*(d),(vo)[2]=(vi)[2]*(d))
#define DVEC_INNP(v1,v2)		((v1)[0]*(v2)[0]+(v1)[1]*(v2)[1]+(v1)[2]*(v2)[2])

extern void      DX_MatrixIdentity    ( DX_FLOAT *pOut );
extern void      DX_MatrixMultiply    ( DX_FLOAT *pOut, DX_FLOAT *pM1, DX_FLOAT *pM2 );
extern void      DX_MatrixTranslation ( DX_FLOAT *pOut, DX_FLOAT x, DX_FLOAT y, DX_FLOAT z );
extern void      DX_MatrixRoationX    ( DX_FLOAT *pOut, DX_FLOAT angle );
extern void      DX_MatrixRoationY    ( DX_FLOAT *pOut, DX_FLOAT angle );
extern void      DX_MatrixRoationZ    ( DX_FLOAT *pOut, DX_FLOAT angle );
extern void      DX_MatrixScaling     ( DX_FLOAT *pOut, DX_FLOAT x, DX_FLOAT y, DX_FLOAT z );
extern DX_FLOAT *DX_Vec3TransformCoord( DX_FLOAT *pOut, DX_FLOAT *pV, DX_FLOAT *pM );
extern void      DX_QuaternionToMatrix( DX_FLOAT *q, DX_FLOAT *m );
extern void      DX_MatrixToQuaternion( DX_FLOAT *m, DX_FLOAT *q );

extern DX_DOUBLE DVEC_UNIT ( DX_DOUBLE *vo, DX_DOUBLE *vi );
extern void      DVEC_OUTP ( DX_DOUBLE *vo, DX_DOUBLE *v1, DX_DOUBLE *v2 );
extern DX_DOUBLE DVEC_ANG  ( DX_DOUBLE *v1, DX_DOUBLE *v0 );

#endif /* _DXVEC_H_  */

