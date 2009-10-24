/*********************************************************************/
/*                                                                   */
/* FILE :        mathlib.c                                           */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Nov 13, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#include "mathlib.hpp"

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_identify                                    */
/*                                                                   */
/* DESCRIPTION :    initialize a transformation matrix               */
/*                                                                   */
/* PARAMETERS :     m   =   transformation matrix                    */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_identify( tMat4 m )
{
    static tMat4 initmatrix = {
        { 1.0, 0.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 0.0, 1.0 },
    };
    memcpy( m, initmatrix, sizeof(tMat4) );
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_mult                                        */
/*                                                                   */
/* DESCRIPTION :    premultiplies the current transformation matrix. */
/*                  if I is the current matrix, mat4_mult replace    */
/*                  I with m*I.                                      */
/*                                                                   */
/* PARAMETERS :     m   =   expects the matrix that is to multiply   */
/*                          the current top matrix of the transfor-  */
/*                          mation stack.                            */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_mult( tMat4 dst, tMat4 m )
{
    float  t[4];
    int    i, j, k;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0, t[j] = 0.0; k < 4; k++) {
                t[j] += m[j][k] * dst[k][i];
            }
        }
        for (j = 0; j < 4; j++) dst[j][i] = t[j];
    }
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_transv                                      */
/*                                                                   */
/* DESCRIPTION :    matrix trnasformation the vector                 */
/*                                                                   */
/* PARAMETERS :     vi[3]  =   input vector                          */
/*                  vo[3]  =   translated vector                     */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_transv( tMat4 m, float *vi, float *vo )
{
    int  i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0, vo[i] = 0.0; j < 3; j++) {
            vo[i] += vi[j] * m[j][i];
        }
        vo[i] += m[3][i];
    }
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_transv4                                      */
/*                                                                   */
/* DESCRIPTION :    matrix trnasformation the vector                 */
/*                                                                   */
/* PARAMETERS :     vi[4]  =   input vector                          */
/*                  vo[4]  =   translated vector                     */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_transv4( tMat4 m, float *vi, float *vo )
{
    int  i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0, vo[i] = 0.0; j < 4; j++) {
            vo[i] += vi[j] * m[j][i];
        }
    }
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_scale                                       */
/*                                                                   */
/* DESCRIPTION :    scale and mirror objectsransformation matrix     */
/*                                                                   */
/* PARAMETERS :     x  =  expects the scaling of the object in X     */
/*                  y  =  expects the scaling of the object in Y     */
/*                  z  =  expects the scaling of the object in Z     */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_scale( tMat4 m, float x, float y, float z )
{
    tMat4  t;
    mat4_identify( t );
    t[0][0] = x;
    t[1][1] = y;
    t[2][2] = z;
    t[3][3] = 1.0;
    mat4_mult( m, t );
}

/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_translate                                   */
/*                                                                   */
/* DESCRIPTION :    translate graphical primitive                    */
/*                                                                   */
/* PARAMETERS :     x  =  expects the x coordinate of a point        */
/*                  y  =  expects the y coordinate of a point        */
/*                  z  =  expects the z coordinate of a point        */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_translate( tMat4 m, float x, float y, float z )
{
    tMat4  t;
    mat4_identify( t );
    t[3][0] = x;
    t[3][1] = y;
    t[3][2] = z;
    mat4_mult( m, t );
}

/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_rotate                                      */
/*                                                                   */
/* DESCRIPTION :    rotate graphical primitive (LEFT-HAND)           */
/*                                                                   */
/* PARAMETERS :     a    =  expects the angle of rotation (radian)   */
/*                  axis =  expects the relative axis of rotation    */
/*                          'x'  indicate the X-axis                 */
/*                          'y'  indicate the Y-axis                 */
/*                          'z'  indicate the Z-axis                 */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_rotate( tMat4 m, float a, char axis )
{
    tMat4  t;

    mat4_identify( t );
    switch (axis)
    {
      case 'x':
      case 'p':
          t[1][1] = t[2][2] = cos((float)a);
          t[1][2] = sin((float)a);
          t[2][1] = t[1][2]*(-1);
          break;
      case 'y':
      case 'h':
          t[0][0] = t[2][2] = cos((float)a);
          t[2][0] = sin((float)a);
          t[0][2] = t[2][0]*(-1);
          break;
      case 'z':
      case 'b':
          t[0][0] = t[1][1] = cos((float)a);
          t[0][1] = sin((float)a);
          t[1][0] = t[0][1]*(-1);
          break;
    }
    mat4_mult( m, t );
}



/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_inverse                                     */
/*                                                                   */
/* DESCRIPTION :    Computes the inverse of a 3D affine matrix; i.e. */
/*                  a matrix with a dimen-sionality of 4 where the   */
/*                  right column has the entries (0, 0, 0, 1).       */
/*                                                                   */
/*          --        --          --          --                     */
/*          |          | -1       |    -1      |                     */
/*          | A      0 |          |   A      0 |                     */
/*    -1    |          |          |            |                     */
/*   M   =  |          |     =    |     -1     |                     */
/*          | C      1 |          | -C A     1 |                     */
/*          |          |          |            |                     */
/*          --        --          --          --                     */
/*                                                                   */
/* PARAMETERS :     in   =  3D affine matrix                         */
/*                  out  =  inverse of 3D affine matrix              */
/*                                                                   */
/* RETURN VALUE :   TRUE   if input matrix is nonsingular            */
/*                  FALSE  otherwise                                 */
/*                                                                   */
/*********************************************************************/

int mat4_inverse( tMat4 in, tMat4 out )
{
    register  float    det_1;
              float    pos, neg, temp;

#define ACCUMULATE    \
    if (temp >= 0.0)  \
        pos += temp;  \
    else              \
        neg += temp;

#define PRECISION_LIMIT (1.0e-15)
#define ABS(a)		(((a)<0) ? -(a) : (a))

    /*
     * Calculate the determinant of submatrix A and determine if the
     * the matrix is singular as limited by the float precision
     * floating-point data representation.
     */
    pos = neg = 0.0;
    temp =  in[0][0] * in[1][1] * in[2][2];
    ACCUMULATE
    temp =  in[0][1] * in[1][2] * in[2][0];
    ACCUMULATE
    temp =  in[0][2] * in[1][0] * in[2][1];
    ACCUMULATE
    temp = -in[0][2] * in[1][1] * in[2][0];
    ACCUMULATE
    temp = -in[0][1] * in[1][0] * in[2][2];
    ACCUMULATE
    temp = -in[0][0] * in[1][2] * in[2][1];
    ACCUMULATE
    det_1 = pos + neg;

    /* Is the submatrix A singular? */
    if ((det_1 == 0.0) || (ABS(det_1 / (pos - neg)) < PRECISION_LIMIT)) {

        /* Matrix M has no inverse */
        /* fprintf (stderr, "mat4_inverse: singular matrix\n");  */
        return FALSE;
    }

    else {

        /* Calculate inverse(A) = adj(A) / det(A) */
        det_1 = 1.0f / det_1;
        out[0][0] =   ( in[1][1] * in[2][2] -
                                 in[1][2] * in[2][1] )
                               * det_1;
        out[1][0] = - ( in[1][0] * in[2][2] -
                                 in[1][2] * in[2][0] )
                               * det_1;
        out[2][0] =   ( in[1][0] * in[2][1] -
                                 in[1][1] * in[2][0] )
                               * det_1;
        out[0][1] = - ( in[0][1] * in[2][2] -
                                 in[0][2] * in[2][1] )
                               * det_1;
        out[1][1] =   ( in[0][0] * in[2][2] -
                                 in[0][2] * in[2][0] )
                               * det_1;
        out[2][1] = - ( in[0][0] * in[2][1] -
                                 in[0][1] * in[2][0] )
                               * det_1;
        out[0][2] =   ( in[0][1] * in[1][2] -
                                 in[0][2] * in[1][1] )
                               * det_1;
        out[1][2] = - ( in[0][0] * in[1][2] -
                                 in[0][2] * in[1][0] )
                               * det_1;
        out[2][2] =   ( in[0][0] * in[1][1] -
                                 in[0][1] * in[1][0] )
                               * det_1;

        /* Calculate -C * inverse(A) */
        out[3][0] = - ( in[3][0] * out[0][0] +
                                 in[3][1] * out[1][0] +
                                 in[3][2] * out[2][0] );
        out[3][1] = - ( in[3][0] * out[0][1] +
                                 in[3][1] * out[1][1] +
                                 in[3][2] * out[2][1] );
        out[3][2] = - ( in[3][0] * out[0][2] +
                                 in[3][1] * out[1][2] +
                                 in[3][2] * out[2][2] );

        /* Fill in last column */
        out[0][3] = out[1][3] = out[2][3] = 0.0f;
        out[3][3] = 1.0f;

        return TRUE;
    }
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_euler                                       */
/*                                                                   */
/* DESCRIPTION :    Make Euler angles                                */
/*                                                                   */
/* PARAMETERS :     ai, aj, ah  =  (x,y,z)=ang 1,2,3                 */
/*                  order       =  rotation order                    */
/*                                                                   */
/* RETURN VALUE :   euler angle                                      */
/*                                                                   */
/*********************************************************************/

EulerAngles mat4_euler( float ai, float aj, float ah, int order )
{
    EulerAngles ea;
    ea.x = ai; ea.y = aj; ea.z = ah;
    ea.o = order;
    return (ea);
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_eul2quat                                    */
/*                                                                   */
/* DESCRIPTION :    Construct quaternion from Euler angles           */
/*                                                                   */
/* PARAMETERS :     ea   =  (x,y,z)=ang 1,2,3, w=order code          */
/*                                                                   */
/* RETURN VALUE :   quatanion                                        */
/*                                                                   */
/*********************************************************************/

Quat mat4_eul2quat( EulerAngles ea )
{
    Quat   qu;
    float a[3], ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int    i,j,k,h,n,s,f;

    EulGetOrd(ea.o,i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    if (n==EulParOdd) ea.y = -ea.y;

    ti = ea.x*0.5f; tj = ea.y*0.5f; th = ea.z*0.5f;
    ci = cos(ti);  cj = cos(tj);  ch = cos(th);
    si = sin(ti);  sj = sin(tj);  sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
    if (s==EulRepYes) {
		a[i] = cj*(cs + sc);	/* Could speed up with */
		a[j] = sj*(cc + ss);	/* trig identities. */
		a[k] = sj*(cs - sc);
		qu.w = cj*(cc - ss);
    } else {
		a[i] = cj*sc - sj*cs;
		a[j] = cj*ss + sj*cc;
		a[k] = cj*cs - sj*sc;
		qu.w = cj*cc + sj*ss;
    }
    if (n==EulParOdd) a[j] = -a[j];
	qu.x = a[Quat::X]; qu.y = a[Quat::Y]; qu.z = a[Quat::Z];

    return (qu);
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_eul2matrix                                  */
/*                                                                   */
/* DESCRIPTION :    Construct matrix from Euler angles (in radians)  */
/*                                                                   */
/* PARAMETERS :     ea   =  (x,y,z)=ang 1,2,3, w=order code          */
/*                  M    =  matrix                                   */
/*                                                                   */
/* RETURN VALUE :   none                                             */
/*                                                                   */
/*********************************************************************/

void mat4_eul2matrix( EulerAngles ea, tMat4 M )
{
    float ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
    int    i,j,k,h,n,s,f;

    EulGetOrd(ea.o,i,j,k,h,n,s,f);
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = -ea.y; ea.z = -ea.z;}

    ti = ea.x;	  tj = ea.y;	th = ea.z;
    ci = cos(ti); cj = cos(tj); ch = cos(th);
    si = sin(ti); sj = sin(tj); sh = sin(th);
    cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;

    if (s==EulRepYes) {
		M[i][i] = cj;	  M[j][i] =  sj*si;    M[k][i] =  sj*ci;
		M[i][j] = sj*sh;  M[j][j] = -cj*ss+cc; M[k][j] = -cj*cs-sc;
		M[i][k] = -sj*ch; M[j][k] =  cj*sc+cs; M[k][k] =  cj*cc-ss;
    } else {
		M[i][i] = cj*ch; M[j][i] = sj*sc-cs; M[k][i] = sj*cc+ss;
		M[i][j] = cj*sh; M[j][j] = sj*ss+cc; M[k][j] = sj*cs-sc;
		M[i][k] = -sj;	 M[j][k] = cj*si;    M[k][k] = cj*ci;
    }
    M[Quat::X][Quat::W]=M[Quat::Y][Quat::W]=M[Quat::Z][Quat::W]=M[Quat::W][Quat::X]=M[Quat::W][Quat::Y]=M[Quat::W][Quat::Z]=0.0; M[Quat::W][Quat::W]=1.0;
}

/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_matrix2eul                                  */
/*                                                                   */
/* DESCRIPTION :    Convert matrix to Euler angles (in radians)      */
/*                                                                   */
/* PARAMETERS :     M       =  matrix                                */
/*                  order   =  rotation order                        */
/*                                                                   */
/* RETURN VALUE :   euler angle =  (x,y,z)=ang 1,2,3, w=order code   */
/*                                                                   */
/*********************************************************************/

EulerAngles mat4_matrix2eul( tMat4 M, int order )
{
    EulerAngles ea;
    int         i,j,k,h,n,s,f;

    EulGetOrd(order,i,j,k,h,n,s,f);

    if (s==EulRepYes) {
		float sy = sqrt(M[j][i]*M[j][i] + M[k][i]*M[k][i]);
		if (sy > 16*FLT_EPSILON) {
	    	ea.x = atan2(M[j][i], M[k][i]);
	    	ea.y = atan2(sy, M[i][i]);
	    	ea.z = atan2(M[i][j], -M[i][k]);
		} else {
	    	ea.x = atan2(-M[k][j], M[j][j]);
	    	ea.y = atan2(sy, M[i][i]);
	    	ea.z = 0;
		}
    } else {
		float cy = sqrt(M[i][i]*M[i][i] + M[i][j]*M[i][j]);
		if (cy > 16*FLT_EPSILON) {
	    	ea.x = atan2(M[j][k], M[k][k]);
	    	ea.y = atan2(-M[i][k], cy);
	    	ea.z = atan2(M[i][j], M[i][i]);
		} else {
	    	ea.x = atan2(-M[k][j], M[j][j]);
	    	ea.y = atan2(-M[i][k], cy);
	    	ea.z = 0;
		}
    }
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}
    ea.o = order;

    return (ea);
}

/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_quat2eul                                    */
/*                                                                   */
/* DESCRIPTION :    Convert quaternion to Euler angles (in radians)  */
/*                                                                   */
/* PARAMETERS :     M       =  matrix                                */
/*                  order   =  rotation order                        */
/*                                                                   */
/* RETURN VALUE :   euler angle =  (x,y,z)=ang 1,2,3, w=order code   */
/*                                                                   */
/*********************************************************************/

EulerAngles mat4_quat2eul( Quat q, int order )
{
    tMat4  M;
    float Nq = q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    float s = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;
    float xs = q.x*s,	  ys = q.y*s,	 zs = q.z*s;
    float wx = q.w*xs,	  wy = q.w*ys,	 wz = q.w*zs;
    float xx = q.x*xs,	  xy = q.x*ys,	 xz = q.x*zs;
    float yy = q.y*ys,	  yz = q.y*zs,	 zz = q.z*zs;

	M[Quat::X][Quat::X] = 1.0f - (yy + zz); M[Quat::Y][Quat::X] = xy - wz; M[Quat::Z][Quat::X] = xz + wy;
    M[Quat::X][Quat::Y] = xy + wz; M[Quat::Y][Quat::Y] = 1.0f - (xx + zz); M[Quat::Z][Quat::Y] = yz - wx;
    M[Quat::X][Quat::Z] = xz - wy; M[Quat::Y][Quat::Z] = yz + wx; M[Quat::Z][Quat::Z] = 1.0f - (xx + yy);
    M[Quat::X][Quat::W]=M[Quat::Y][Quat::W]=M[Quat::Z][Quat::W]=M[Quat::W][Quat::X]=M[Quat::W][Quat::Y]=M[Quat::W][Quat::Z]=0.0f; M[Quat::W][Quat::W]=1.0f;

    return (mat4_matrix2eul(M, order));
}


/*********************************************************************/
/*                                                                   */
/* NAME :           mat4_multquat                                    */
/*                                                                   */
/* DESCRIPTION :    multiply q1 and q2                               */
/*                                                                   */
/* PARAMETERS :     q1      =  quaternion 1                          */
/*                  q2      =  quaternion 2                          */
/*                                                                   */
/* RETURN VALUE :   q1 * q2                                          */
/*                                                                   */
/*********************************************************************/

Quat mat4_multquat( Quat q1, Quat q2 )
{
	Quat  ret;

	ret.x = q1.y*q2.z - q1.z*q2.y + q1.w*q2.x + q1.x*q2.w;
	ret.y = q1.z*q2.x - q1.x*q2.z + q1.w*q2.y + q1.y*q2.w;
	ret.z = q1.x*q2.y - q1.y*q2.x + q1.w*q2.z + q1.z*q2.w;
	ret.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	return ret;
}

