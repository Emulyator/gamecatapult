/*********************************************************************/
/*                                                                   */
/* FILE :        dxvec.cpp                                           */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#include "dxvec.h"

/******  PROTOTYPES  ******************************/


#define M(a,b)		((a) * 4 + (b))
#define QW			0
#define QX			1
#define QY			2
#define QZ			3

void DX_MatrixIdentity( DX_FLOAT *pOut )
{
	memset( pOut, 0x00, sizeof(DX_FLOAT) * 16 );
	pOut[ M(0,0) ] = 
	pOut[ M(1,1) ] = 
	pOut[ M(2,2) ] = 
	pOut[ M(3,3) ] = 1.0f;
}

void DX_MatrixTranslation( DX_FLOAT *pOut, DX_FLOAT x, DX_FLOAT y, DX_FLOAT z )
{
	DX_FLOAT       pIn[16];

	DX_MatrixIdentity( pIn );
	pIn[ M(3,0) ] = x;
	pIn[ M(3,1) ] = y;
	pIn[ M(3,2) ] = z;

	DX_MatrixMultiply( pOut, pOut, pIn );
}

void DX_MatrixRoationX( DX_FLOAT *pOut, DX_FLOAT angle )
{
	DX_FLOAT       pIn[16];

	DX_MatrixIdentity( pIn );
	pIn[ M(1,1) ] = (DX_FLOAT)  cos(angle);
	pIn[ M(1,2) ] = (DX_FLOAT)  sin(angle);
	pIn[ M(2,1) ] = (DX_FLOAT) -sin(angle);
	pIn[ M(2,2) ] = (DX_FLOAT)  cos(angle);

	DX_MatrixMultiply( pOut, pOut, pIn );
}

void DX_MatrixRoationY( DX_FLOAT *pOut, DX_FLOAT angle )
{
	DX_FLOAT       pIn[16];

	DX_MatrixIdentity( pIn );
	pIn[ M(0,0) ] = (DX_FLOAT)  cos(angle);
	pIn[ M(0,2) ] = (DX_FLOAT) -sin(angle);
	pIn[ M(2,0) ] = (DX_FLOAT)  sin(angle);
	pIn[ M(2,2) ] = (DX_FLOAT)  cos(angle);

	DX_MatrixMultiply( pOut, pOut, pIn );
}

void DX_MatrixRoationZ( DX_FLOAT *pOut, DX_FLOAT angle )
{
	DX_FLOAT       pIn[16];

	DX_MatrixIdentity( pIn );
	pIn[ M(0,0) ] = (DX_FLOAT)  cos(angle);
	pIn[ M(0,1) ] = (DX_FLOAT)  sin(angle);
	pIn[ M(1,0) ] = (DX_FLOAT) -sin(angle);
	pIn[ M(1,1) ] = (DX_FLOAT)  cos(angle);

	DX_MatrixMultiply( pOut, pOut, pIn );
}

void DX_MatrixScaling( DX_FLOAT *pOut, DX_FLOAT x, DX_FLOAT y, DX_FLOAT z )
{
	DX_FLOAT       pIn[16];

	DX_MatrixIdentity( pIn );
	pIn[ M(0,0) ] = x;
	pIn[ M(1,1) ] = y;
	pIn[ M(2,2) ] = z;

	DX_MatrixMultiply( pOut, pOut, pIn );
}

void DX_MatrixMultiply( DX_FLOAT *pOut, DX_FLOAT *pM1, DX_FLOAT *pM2 )
{
	DX_FLOAT       pIn[16];
	int            i, j, k;

	memset( pIn, 0x00, sizeof(DX_FLOAT) * 16 );
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
			for ( k = 0; k < 4; k++ )
				pIn[ M(i,j) ] += pM1[ M(i,k) ] * pM2[ M(k,j) ];

	memcpy( pOut, pIn, sizeof(DX_FLOAT) * 16 );
}

DX_FLOAT *DX_Vec3TransformCoord( DX_FLOAT *pOut, DX_FLOAT *pV, DX_FLOAT *pM )
{
	pOut[0] = (pV[0] * pM[ M(0,0) ]) + (pV[1] * pM[ M(1,0) ]) + (pV[2] * pM[ M(2,0) ]) + pM[ M(3,0) ];
	pOut[1] = (pV[0] * pM[ M(0,1) ]) + (pV[1] * pM[ M(1,1) ]) + (pV[2] * pM[ M(2,1) ]) + pM[ M(3,1) ];
	pOut[2] = (pV[0] * pM[ M(0,2) ]) + (pV[1] * pM[ M(1,2) ]) + (pV[2] * pM[ M(2,2) ]) + pM[ M(3,2) ];

	return pOut;
}


void DX_QuaternionToMatrix( DX_FLOAT *q, DX_FLOAT *m )
{
	DX_FLOAT  norm, s, xx, yy, zz, xy, xz, yz, wx, wy, wz;

	norm = q[QX]*q[QX] + q[QY]*q[QY] + q[QZ]*q[QZ] + q[QW]*q[QW],
	s    = (norm > 0) ? 2/norm : 0,

	xx = q[QX] * q[QX] * s,
	yy = q[QY] * q[QY] * s,
	zz = q[QZ] * q[QZ] * s,
	xy = q[QX] * q[QY] * s,
	xz = q[QX] * q[QZ] * s,
	yz = q[QY] * q[QZ] * s,
	wx = q[QW] * q[QX] * s,
	wy = q[QW] * q[QY] * s,
	wz = q[QW] * q[QZ] * s;

	m[ M(0,0) ] = 1.0f - (yy + zz);
	m[ M(1,0) ] = xy + wz;
	m[ M(2,0) ] = xz - wy;

	m[ M(0,1) ] = xy - wz;
	m[ M(1,1) ] = 1.0f - (xx + zz);
	m[ M(2,1) ] = yz + wx;

	m[ M(0,2) ] = xz + wy;
	m[ M(1,2) ] = yz - wx;
	m[ M(2,2) ] = 1.0f - (xx + yy);

	m[ M(0,3) ] = m[ M(1,3) ] = m[ M(2,3) ] = m[ M(3,0) ] = m[ M(3,1) ] = m[ M(3,2) ] = 0.0f;
	m[ M(3,3) ] = 1.0f;

}

void DX_MatrixToQuaternion( DX_FLOAT *m, DX_FLOAT *q )
{
	DX_FLOAT    Tr, fourD;
	int         i,j,k;

	Tr = m[ M(0,0) ] + m[ M(1,1) ] + m[ M(2,2) ] + m[ M(3,3) ];

	if (Tr >= 1.0f)
	{
		fourD = (DX_FLOAT) (2.0 * sqrt(Tr));
		q[QW] = fourD / 4.0f;
		q[QX] = (m[ M(2,1) ] - m[ M(1,2) ]) / fourD;
		q[QY] = (m[ M(0,2) ] - m[ M(2,0) ]) / fourD;
		q[QZ] = (m[ M(1,0) ] - m[ M(0,1) ]) / fourD;
	}
	else
	{
		if (m[ M(0,0) ] > m[ M(1,1) ])
		{
			i = 0;
		}
		else
		{
			i = 1;
		}
		if (m[ M(2,2) ] > m[ M(i,i) ])
		{
			i = 2;
		}

		j = (i+1)%3;
		k = (j+1)%3;

		fourD = (DX_FLOAT) ( 2.0 * sqrt(m[ M(i,i) ] - m[ M(j,j) ] - m[ M(k,k) ] + 1.0 ) );

		q[i] = fourD / 4.0f;
		
		/*
		Calculate remaining components
		*/
		q[j+1] = (m[ M(i,j) ] + m[ M(j,i) ]) / fourD;
		q[k+1] = (m[ M(i,k) ] + m[ M(k,i) ]) / fourD;
		q[QW]  = (m[ M(k,j) ] - m[ M(j,k) ]) / fourD;
	}
}

#undef M


DX_DOUBLE DVEC_UNIT( DX_DOUBLE *vo, DX_DOUBLE *vi )
{
    DX_DOUBLE  dist;

    dist = DVEC_LENV( vi );
    if (dist < EPSILON_D) return 0.0;
    DVEC_SCAL( vo, vi, 1.0/dist );
    return dist;
}


void DVEC_OUTP( DX_DOUBLE *vo, DX_DOUBLE *v1, DX_DOUBLE *v2 )
{
	DX_DOUBLE  temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	DVEC_COPY( vo, temp );
}


DX_DOUBLE DVEC_ANG( DX_DOUBLE *v1, DX_DOUBLE *v0 )
{
    DX_DOUBLE  dist;

    dist = acos( DVEC_INNP( v0, v1 ) / DVEC_LENV( v0 ) / DVEC_LENV( v1 ) );
    return dist;
}

