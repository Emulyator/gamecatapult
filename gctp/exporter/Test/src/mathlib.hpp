/*********************************************************************/
/*                                                                   */
/* FILE :        mathlib.h                                           */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Nov 13, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _LWMATHLIB_HPP_
#define _LWMATHLIB_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

					/*  Quaternion  */
struct Quat {
	enum Part {X, Y, Z, W};
	float x, y, z, w;
}; 

					/*  Euler Angles (x,y,z)=ang 1,2,3, w=order code  */
struct EulerAngles {
	float x, y, z;
	int    o;
};

typedef float tMat4[4][4];

/*
 *  m[0][0] m[1][0] m[2][0] m[3][0]
 *  m[0][1] m[1][1] m[2][1] m[3][1]
 *  m[0][2] m[1][2] m[2][2] m[3][2]
 *  m[0][3] m[1][3] m[2][3] m[3][3]
 */



/*** Order type constants, constructors, extractors ***/
    /* There are 24 possible conventions, designated by:    */
    /*	  o EulAxI = axis used initially		    */
    /*	  o EulPar = parity of axis permutation		    */
    /*	  o EulRep = repetition of initial axis as last	    */
    /*	  o EulFrm = frame from which axes are taken	    */
    /* Axes I,J,K will be a permutation of X,Y,Z.	    */
    /* Axis H will be either I or K, depending on EulRep.   */
    /* Frame S takes axes from initial static frame.	    */
    /* If ord = (AxI=X, Par=Even, Rep=No, Frm=S), then	    */
    /* {a,b,c,ord} means Rz(c)Ry(b)Rx(a), where Rz(c)v	    */
    /* rotates v around Z by c radians.			    */
#define EulFrmS	     0
#define EulFrmR	     1
#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRepNo     0
#define EulRepYes    1
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulParEven   0
#define EulParOdd    1
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
#define EulSafe	     "\000\001\002\000"
#define EulNext	     "\001\002\000\001"
#define EulAxI(ord)  ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)  ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))
    /* EulGetOrd unpacks all useful information about order simultaneously. */
#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}
    /* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */
#define EulOrd(i,p,r,f)	   (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
    /* Static axes */
#define EulOrdXYZs    EulOrd(Quat::X,EulParEven,EulRepNo,EulFrmS)
#define EulOrdXYXs    EulOrd(Quat::X,EulParEven,EulRepYes,EulFrmS)
#define EulOrdXZYs    EulOrd(Quat::X,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdXZXs    EulOrd(Quat::X,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdYZXs    EulOrd(Quat::Y,EulParEven,EulRepNo,EulFrmS)
#define EulOrdYZYs    EulOrd(Quat::Y,EulParEven,EulRepYes,EulFrmS)
#define EulOrdYXZs    EulOrd(Quat::Y,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdYXYs    EulOrd(Quat::Y,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdZXYs    EulOrd(Quat::Z,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZXZs    EulOrd(Quat::Z,EulParEven,EulRepYes,EulFrmS)
#define EulOrdZYXs    EulOrd(Quat::Z,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdZYZs    EulOrd(Quat::Z,EulParOdd,EulRepYes,EulFrmS)
    /* Rotating axes */
#define EulOrdZYXr    EulOrd(Quat::X,EulParEven,EulRepNo,EulFrmR)
#define EulOrdXYXr    EulOrd(Quat::X,EulParEven,EulRepYes,EulFrmR)
#define EulOrdYZXr    EulOrd(Quat::X,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdXZXr    EulOrd(Quat::X,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdXZYr    EulOrd(Quat::Y,EulParEven,EulRepNo,EulFrmR)
#define EulOrdYZYr    EulOrd(Quat::Y,EulParEven,EulRepYes,EulFrmR)
#define EulOrdZXYr    EulOrd(Quat::Y,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdYXYr    EulOrd(Quat::Y,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdYXZr    EulOrd(Quat::Z,EulParEven,EulRepNo,EulFrmR)
#define EulOrdZXZr    EulOrd(Quat::Z,EulParEven,EulRepYes,EulFrmR)
#define EulOrdXYZr    EulOrd(Quat::Z,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdZYZr    EulOrd(Quat::Z,EulParOdd,EulRepYes,EulFrmR)

#define EulOrdLW3D    EulOrdYXZr


extern void  mat4_identify ( tMat4 m );
extern void  mat4_mult     ( tMat4 dst, tMat4 m );
extern void  mat4_transv   ( tMat4 m, float *vi, float *vo );
extern void  mat4_transv4  ( tMat4 m, float *vi, float *vo );
extern void  mat4_scale    ( tMat4 m, float x, float y, float z );
extern void  mat4_translate( tMat4 m, float x, float y, float z );
extern void  mat4_rotate   ( tMat4 m, float a, char axis );
extern int   mat4_inverse  ( tMat4 in, tMat4 out );

extern EulerAngles mat4_euler( float ai, float aj, float ah, int order );
extern Quat        mat4_eul2quat( EulerAngles ea );
extern void        mat4_eul2matrix( EulerAngles ea, tMat4 M );
extern EulerAngles mat4_matrix2eul( tMat4 M, int order );
extern EulerAngles mat4_quat2eul( Quat q, int order );
extern Quat        mat4_multquat( Quat q1, Quat q2 );

#endif /* _LWMATHLIB_HPP_ */

