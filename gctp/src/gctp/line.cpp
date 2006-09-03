/** @file
 * GameCatapult 直線・線分・レイクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/line.hpp>

using namespace std;

namespace gctp {

	float distance2(const Line &lhs, const Point3f &rhs, float *t)
	{
		Vector diff = rhs - lhs.s;
		float _t = diff*lhs.v/lhs.v.length2();
		diff -= _t*lhs.v;
		if(t) *t = _t;
		return diff.length2();
	}

	float distance2(const RayLine &lhs, const Point3f &rhs, float *t)
	{
		Vector diff = rhs - lhs.s;
		float _t = diff*lhs.v;
		if(_t <= 0.0f) _t = 0.0f;
		else {
			_t /= lhs.v.length2();
			diff -= _t*lhs.v;
		}
		if(t) *t = _t;
		return diff.length2();
	}

	// Magic Software, Inc.
	// http://www.geometrictools.com/
	// Copyright (c) 2004.  All Rights Reserved
	//
	// The Wild Magic Library (WML) source code is supplied under the terms of
	// the license agreement http://www.magic-software.com/License/WildMagic.pdf
	// and may not be copied or disclosed except in accordance with the terms of
	// that agreement.
	float distance2(const LineSeg &rkSegment, const Point3f& rkPoint, float* pfParam)
	{
		Vector kDiff = rkPoint - rkSegment.s;
		Vector segdir = rkSegment.direction();
		float fT = kDiff * segdir;

		if ( fT <= 0.0f ) fT = 0.0f;
		else {
			float fSqrLen = segdir.length2();
			if ( fT >= fSqrLen ) {
				fT = 1.0f;
				kDiff -= segdir;
			}
			else {
				fT /= fSqrLen;
				kDiff -= fT*segdir;
			}
		}

		if( pfParam ) *pfParam = fT;

		return kDiff.length2();
	}

	float distance2(const Line& rkLine0, const Line& rkLine1, float* pfLinP0, float* pfLinP1)
	{
		Vector kDiff = rkLine0.s - rkLine1.s;
		float fA00 = rkLine0.v.length2();
		float fA01 = -(rkLine0.v*rkLine1.v);
		float fA11 = rkLine1.v.length2();
		float fB0 = kDiff * rkLine0.v;
		float fC = kDiff.length2();
		float fDet = fabsf(fA00*fA11-fA01*fA01);
		float fB1, fS, fT, fSqrDist;

		if ( fDet >= FLT_EPSILON ) {
			// lines are not parallel
			fB1 = -kDiff * rkLine1.v;
			float fInvDet = ((float)1.0)/fDet;
			fS = (fA01*fB1-fA11*fB0)*fInvDet;
			fT = (fA01*fB0-fA00*fB1)*fInvDet;
			fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
		}
		else {
			// lines are parallel, select any closest pair of points
			fS = -fB0/fA00;
			fT = 0.0f;
			fSqrDist = fB0*fS+fC;
		}

		if ( pfLinP0 ) *pfLinP0 = fS;
		if ( pfLinP1 ) *pfLinP1 = fT;

		return fabsf(fSqrDist);
	}

	float distance2(const Line &rkLine, const RayLine &rkRay, float* pfLinP, float* pfRayP)
	{
		Vector kDiff = rkLine.s - rkRay.s;
		float fA00 = rkLine.v.length2();
		float fA01 = -(rkLine.v * rkRay.v);
		float fA11 = rkRay.v.length2();
		float fB0 = kDiff * rkLine.v;
		float fC = kDiff.length2();
		float fDet = fabsf(fA00*fA11-fA01*fA01);
		float fB1, fS, fT, fSqrDist;

		if ( fDet >= FLT_EPSILON )
		{
			fB1 = -(kDiff * rkRay.v);
			fT = fA01*fB0-fA00*fB1;

			if ( fT >= 0.0f )
			{
				// two interior points are closest, one on line and one on ray
				float fInvDet = 1.0f/fDet;
				fS = (fA01*fB1-fA11*fB0)*fInvDet;
				fT *= fInvDet;
				fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
			}
			else
			{
				// end point of ray and interior point of line are closest
				fS = -fB0/fA00;
				fT = 0.0f;
				fSqrDist = fB0*fS+fC;
			}
		}
		else
		{
			// lines are parallel, closest pair with one point at ray origin
			fS = -fB0/fA00;
			fT = 0.0f;
			fSqrDist = fB0*fS+fC;
		}

		if ( pfLinP ) *pfLinP = fS;
		if ( pfRayP ) *pfRayP = fT;

		return fabsf(fSqrDist);
	}

	float distance2(const Line &rkLine, const LineSeg &rkSeg, float* pfLinP, float* pfSegP)
	{
		Vector kDiff = rkLine.s - rkSeg.s;
		Vector rkSegDir = rkSeg.direction();
		float fA00 = rkLine.v.length2();
		float fA01 = -(rkLine.v * rkSegDir);
		float fA11 = rkSegDir.length2();
		float fB0 = kDiff * rkLine.v;
		float fC = kDiff.length2();
		float fDet = fabsf(fA00*fA11-fA01*fA01);
		float fB1, fS, fT, fSqrDist;

		if ( fDet >= FLT_EPSILON )
		{
			fB1 = -(kDiff * rkSegDir);
			fT = fA01*fB0-fA00*fB1;

			if ( fT >= 0.0f )
			{
				if ( fT <= fDet )
				{
					// two interior points are closest, one on line and one on
					// segment
					float fInvDet = 1.0f/fDet;
					fS = (fA01*fB1-fA11*fB0)*fInvDet;
					fT *= fInvDet;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
				else
				{
					// end point e1 of segment and interior point of line are
					// closest
					float fTmp = fA01+fB0;
					fS = -fTmp/fA00;
					fT = 1.0f;
					fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
				}
			}
			else
			{
				// end point e0 of segment and interior point of line are closest
				fS = -fB0/fA00;
				fT = 0.0f;
				fSqrDist = fB0*fS+fC;
			}
		}
		else
		{
			// lines are parallel, closest pair with one point at segment origin
			fS = -fB0/fA00;
			fT = 0.0f;
			fSqrDist = fB0*fS+fC;
		}

		if ( pfLinP ) *pfLinP = fS;
		if ( pfSegP ) *pfSegP = fT;

		return fabsf(fSqrDist);
	}

	float distance2(const RayLine &rkRay0, const RayLine &rkRay1, float* pfRayP0, float* pfRayP1)
	{
		Vector kDiff = rkRay0.s - rkRay1.s;
		float fA00 = rkRay0.v.length2();
		float fA01 = -(rkRay0.v * rkRay1.v);
		float fA11 = rkRay1.v.length2();
		float fB0 = kDiff * rkRay0.v;
		float fC = kDiff.length2();
		float fDet = fabsf(fA00*fA11-fA01*fA01);
		float fB1, fS, fT, fSqrDist;

		if ( fDet >= FLT_EPSILON )
		{
			// rays are not parallel
			fB1 = -(kDiff * rkRay1.v);
			fS = fA01*fB1-fA11*fB0;
			fT = fA01*fB0-fA00*fB1;

			if ( fS >= 0.0f )
			{
				if ( fT >= 0.0f )  // region 0 (interior)
				{
					// minimum at two interior points of rays
					float fInvDet = 1.0f/fDet;
					fS *= fInvDet;
					fT *= fInvDet;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
				else  // region 3 (side)
				{
					fT = 0.0f;
					if ( fB0 >= 0.0f )
					{
						fS = 0.0f;
						fSqrDist = fC;
					}
					else
					{
						fS = -fB0/fA00;
						fSqrDist = fB0*fS+fC;
					}
				}
			}
			else
			{
				if ( fT >= 0.0f )  // region 1 (side)
				{
					fS = 0.0f;
					if ( fB1 >= 0.0f )
					{
						fT = 0.0f;
						fSqrDist = fC;
					}
					else
					{
						fT = -fB1/fA11;
						fSqrDist = fB1*fT+fC;
					}
				}
				else  // region 2 (corner)
				{
					if ( fB0 < 0.0f )
					{
						fS = -fB0/fA00;
						fT = 0.0f;
						fSqrDist = fB0*fS+fC;
					}
					else
					{
						fS = 0.0f;
						if ( fB1 >= 0.0f )
						{
							fT = 0.0f;
							fSqrDist = fC;
						}
						else
						{
							fT = -fB1/fA11;
							fSqrDist = fB1*fT+fC;
						}
					}
				}
			}
		}
		else
		{
			// rays are parallel
			if ( fA01 > 0.0f )
			{
				// opposite direction vectors
				fT = 0.0f;
				if ( fB0 >= 0.0f )
				{
					fS = 0.0f;
					fSqrDist = fC;
				}
				else
				{
					fS = -fB0/fA00;
					fSqrDist = fB0*fS+fC;
				}
			}
			else
			{
				// same direction vectors
				if ( fB0 >= 0.0f )
				{
					fB1 = -(kDiff * rkRay1.v);
					fS = 0.0f;
					fT = -fB1/fA11;
					fSqrDist = fB1*fT+fC;
				}
				else
				{
					fS = -fB0/fA00;
					fT = 0.0f;
					fSqrDist = fB0*fS+fC;
				}
			}
		}

		if ( pfRayP0 ) *pfRayP0 = fS;
		if ( pfRayP1 ) *pfRayP1 = fT;

		return fabsf(fSqrDist);
	}

	float distance2(const RayLine &rkRay, const LineSeg &rkSeg, float* pfRayP, float* pfSegP)
	{
		Vector kDiff = rkRay.s - rkSeg.s;
		Vector rkSegDir = rkSeg.direction();
		float fA00 = rkRay.v.length2();
		float fA01 = -(rkRay.v * rkSegDir);
		float fA11 = rkSegDir.length2();
		float fB0 = kDiff * rkRay.v;
		float fC = kDiff.length2();
		float fDet = fabsf(fA00*fA11-fA01*fA01);
		float fB1, fS, fT, fSqrDist, fTmp;

		if ( fDet >= FLT_EPSILON )
		{
			// ray and segment are not parallel
			fB1 = -(kDiff * rkSegDir);
			fS = fA01*fB1-fA11*fB0;
			fT = fA01*fB0-fA00*fB1;

			if ( fS >= 0.0f )
			{
				if ( fT >= 0.0f )
				{
					if ( fT <= fDet )  // region 0
					{
						// minimum at interior points of ray and segment
						float fInvDet = 1.0f/fDet;
						fS *= fInvDet;
						fT *= fInvDet;
						fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
					}
					else  // region 1
					{
						fT = 1.0f;
						if ( fB0 >= -fA01 )
						{
							fS = 0.0f;
							fSqrDist = fA11+2.0f*fB1+fC;
						}
						else
						{
							fTmp = fA01 + fB0;
							fS = -fTmp/fA00;
							fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
						}
					}
				}
				else  // region 5
				{
					fT = 0.0f;
					if ( fB0 >= 0.0f )
					{
						fS = 0.0f;
						fSqrDist = fC;
					}
					else
					{
						fS = -fB0/fA00;
						fSqrDist = fB0*fS+fC;
					}
				}
			}
			else
			{
				if ( fT <= 0.0f )  // region 4
				{
					if ( fB0 < 0.0f )
					{
						fS = -fB0/fA00;
						fT = 0.0f;
						fSqrDist = fB0*fS+fC;
					}
					else
					{
						fS = 0.0f;
						if ( fB1 >= 0.0f )
						{
							fT = 0.0f;
							fSqrDist = fC;
						}
						else if ( -fB1 >= fA11 )
						{
							fT = 1.0f;
							fSqrDist = fA11+2.0f*fB1+fC;
						}
						else
						{
							fT = -fB1/fA11;
							fSqrDist = fB1*fT+fC;
						}
					}
				}
				else if ( fT <= fDet )  // region 3
				{
					fS = 0.0f;
					if ( fB1 >= 0.0f )
					{
						fT = 0.0f;
						fSqrDist = fC;
					}
					else if ( -fB1 >= fA11 )
					{
						fT = 1.0f;
						fSqrDist = fA11+2.0f*fB1+fC;
					}
					else
					{
						fT = -fB1/fA11;
						fSqrDist = fB1*fT+fC;
					}
				}
				else  // region 2
				{
					fTmp = fA01+fB0;
					if ( fTmp < 0.0f )
					{
						fS = -fTmp/fA00;
						fT = 1.0f;
						fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
					}
					else
					{
						fS = 0.0f;
						if ( fB1 >= 0.0f )
						{
							fT = 0.0f;
							fSqrDist = fC;
						}
						else if ( -fB1 >= fA11 )
						{
							fT = 1.0f;
							fSqrDist = fA11+2.0f*fB1+fC;
						}
						else
						{
							fT = -fB1/fA11;
							fSqrDist = fB1*fT+fC;
						}
					}
				}
			}
		}
		else
		{
			// ray and segment are parallel
			if ( fA01 > 0.0f )
			{
				// opposite direction vectors
				fT = 0.0f;
				if ( fB0 >= 0.0f )
				{
					fS = 0.0f;
					fSqrDist = fC;
				}
				else
				{
					fS = -fB0/fA00;
					fSqrDist = fB0*fS+fC;
				}
			}
			else
			{
				// same direction vectors
				fB1 = -(kDiff * rkSegDir);
				fT = 1.0f;
				fTmp = fA01+fB0;
				if ( fTmp >= 0.0f )
				{
					fS = 0.0f;
					fSqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					fS = -fTmp/fA00;
					fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
				}
			}
		}

		if ( pfRayP ) *pfRayP = fS;
		if ( pfSegP ) *pfSegP = fT;

		return fabsf(fSqrDist);
	}

	float distance2(const LineSeg &rkSeg0, const LineSeg &rkSeg1, float* pfSegP0, float* pfSegP1)
	{
		Vector kDiff = rkSeg0.s - rkSeg1.s;
		Vector rkSeg0Dir = rkSeg0.direction();
		Vector rkSeg1Dir = rkSeg1.direction();
		float fA00 = rkSeg0Dir.length2();
		float fA01 = -(rkSeg0Dir * rkSeg1Dir);
		float fA11 = rkSeg1Dir.length2();
		float fB0 = kDiff * rkSeg0Dir;
		float fC = kDiff.length2();
		float fDet = fabsf(fA00*fA11-fA01*fA01);
		float fB1, fS, fT, fSqrDist, fTmp;

		if ( fDet >= FLT_EPSILON )
		{
			// line segments are not parallel
			fB1 = -(kDiff * rkSeg1Dir);
			fS = fA01*fB1-fA11*fB0;
			fT = fA01*fB0-fA00*fB1;
        
			if ( fS >= 0.0f )
			{
				if ( fS <= fDet )
				{
					if ( fT >= 0.0f )
					{
						if ( fT <= fDet )  // region 0 (interior)
						{
							// minimum at two interior points of 3D lines
							float fInvDet = 1.0f/fDet;
							fS *= fInvDet;
							fT *= fInvDet;
							fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
						}
						else  // region 3 (side)
						{
							fT = 1.0f;
							fTmp = fA01+fB0;
							if ( fTmp >= 0.0f )
							{
								fS = 0.0f;
								fSqrDist = fA11+2.0f*fB1+fC;
							}
							else if ( -fTmp >= fA00 )
							{
								fS = 1.0f;
								fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
							}
							else
							{
								fS = -fTmp/fA00;
								fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
							}
						}
					}
					else  // region 7 (side)
					{
						fT = 0.0f;
						if ( fB0 >= 0.0f )
						{
							fS = 0.0f;
							fSqrDist = fC;
						}
						else if ( -fB0 >= fA00 )
						{
							fS = 1.0f;
							fSqrDist = fA00+2.0f*fB0+fC;
						}
						else
						{
							fS = -fB0/fA00;
							fSqrDist = fB0*fS+fC;
						}
					}
				}
				else
				{
					if ( fT >= 0.0f )
					{
						if ( fT <= fDet )  // region 1 (side)
						{
							fS = 1.0f;
							fTmp = fA01+fB1;
							if ( fTmp >= 0.0f )
							{
								fT = 0.0f;
								fSqrDist = fA00+2.0f*fB0+fC;
							}
							else if ( -fTmp >= fA11 )
							{
								fT = 1.0f;
								fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
							}
							else
							{
								fT = -fTmp/fA11;
								fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
							}
						}
						else  // region 2 (corner)
						{
							fTmp = fA01+fB0;
							if ( -fTmp <= fA00 )
							{
								fT = 1.0f;
								if ( fTmp >= 0.0f )
								{
									fS = 0.0f;
									fSqrDist = fA11+2.0f*fB1+fC;
								}
								else
								{
									 fS = -fTmp/fA00;
									 fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
								}
							}
							else
							{
								fS = 1.0f;
								fTmp = fA01+fB1;
								if ( fTmp >= 0.0f )
								{
									fT = 0.0f;
									fSqrDist = fA00+2.0f*fB0+fC;
								}
								else if ( -fTmp >= fA11 )
								{
									fT = 1.0f;
									fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
								}
								else
								{
									fT = -fTmp/fA11;
									fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
								}
							}
						}
					}
					else  // region 8 (corner)
					{
						if ( -fB0 < fA00 )
						{
							fT = 0.0f;
							if ( fB0 >= 0.0f )
							{
								fS = 0.0f;
								fSqrDist = fC;
							}
							else
							{
								fS = -fB0/fA00;
								fSqrDist = fB0*fS+fC;
							}
						}
						else
						{
							fS = 1.0f;
							fTmp = fA01+fB1;
							if ( fTmp >= 0.0f )
							{
								fT = 0.0f;
								fSqrDist = fA00+2.0f*fB0+fC;
							}
							else if ( -fTmp >= fA11 )
							{
								fT = 1.0f;
								fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
							}
							else
							{
								fT = -fTmp/fA11;
								fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
							}
						}
					}
				}
			}
			else
			{
				if ( fT >= 0.0f )
				{
					if ( fT <= fDet )  // region 5 (side)
					{
						fS = 0.0f;
						if ( fB1 >= 0.0f )
						{
							fT = 0.0f;
							fSqrDist = fC;
						}
						else if ( -fB1 >= fA11 )
						{
							fT = 1.0f;
							fSqrDist = fA11+2.0f*fB1+fC;
						}
						else
						{
							fT = -fB1/fA11;
							fSqrDist = fB1*fT+fC;
						}
					}
					else  // region 4 (corner)
					{
						fTmp = fA01+fB0;
						if ( fTmp < 0.0f )
						{
							fT = 1.0f;
							if ( -fTmp >= fA00 )
							{
								fS = 1.0f;
								fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
							}
							else
							{
								fS = -fTmp/fA00;
								fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
							}
						}
						else
						{
							fS = 0.0f;
							if ( fB1 >= 0.0f )
							{
								fT = 0.0f;
								fSqrDist = fC;
							}
							else if ( -fB1 >= fA11 )
							{
								fT = 1.0f;
								fSqrDist = fA11+2.0f*fB1+fC;
							}
							else
							{
								fT = -fB1/fA11;
								fSqrDist = fB1*fT+fC;
							}
						}
					}
				}
				else   // region 6 (corner)
				{
					if ( fB0 < 0.0f )
					{
						fT = 0.0f;
						if ( -fB0 >= fA00 )
						{
							fS = 1.0f;
							fSqrDist = fA00+2.0f*fB0+fC;
						}
						else
						{
							fS = -fB0/fA00;
							fSqrDist = fB0*fS+fC;
						}
					}
					else
					{
						fS = 0.0f;
						if ( fB1 >= 0.0f )
						{
							fT = 0.0f;
							fSqrDist = fC;
						}
						else if ( -fB1 >= fA11 )
						{
							fT = 1.0f;
							fSqrDist = fA11+2.0f*fB1+fC;
						}
						else
						{
							fT = -fB1/fA11;
							fSqrDist = fB1*fT+fC;
						}
					}
				}
			}
		}
		else
		{
			// line segments are parallel
			if ( fA01 > 0.0f )
			{
				// direction vectors form an obtuse angle
				if ( fB0 >= 0.0f )
				{
					fS = 0.0f;
					fT = 0.0f;
					fSqrDist = fC;
				}
				else if ( -fB0 <= fA00 )
				{
					fS = -fB0/fA00;
					fT = 0.0f;
					fSqrDist = fB0*fS+fC;
				}
				else
				{
					fB1 = -(kDiff * rkSeg1Dir);
					fS = 1.0f;
					fTmp = fA00+fB0;
					if ( -fTmp >= fA01 )
					{
						fT = 1.0f;
						fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
					}
					else
					{
						fT = -fTmp/fA01;
						fSqrDist = fA00+2.0f*fB0+fC+fT*(fA11*fT+2.0f*(fA01+fB1));
					}
				}
			}
			else
			{
				// direction vectors form an acute angle
				if ( -fB0 >= fA00 )
				{
					fS = 1.0f;
					fT = 0.0f;
					fSqrDist = fA00+2.0f*fB0+fC;
				}
				else if ( fB0 <= 0.0f )
				{
					fS = -fB0/fA00;
					fT = 0.0f;
					fSqrDist = fB0*fS+fC;
				}
				else
				{
					fB1 = -(kDiff * rkSeg1Dir);
					fS = 0.0f;
					if ( fB0 >= -fA01 )
					{
						fT = 1.0f;
						fSqrDist = fA11+2.0f*fB1+fC;
					}
					else
					{
						fT = -fB0/fA01;
						fSqrDist = fC+fT*(2.0f*fB1+fA11*fT);
					}
				}
			}
		}

		if ( pfSegP0 ) *pfSegP0 = fS;
		if ( pfSegP1 ) *pfSegP1 = fT;

		return fabsf(fSqrDist);
	}

} // namespace gctp
