/*********************************************************************/
/*                                                                   */
/* FILE :        dxutl.h                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D to DirectX Export                      */
/*                                                                   */
/* HISTORY :     Oct 29, 2000    written by Yoshiaki Tazaki          */
/*               Oct 02, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/

#ifndef _DXUTL_H_
#define _DXUTL_H_

extern void DX_MakeNodeName( char* dst, const char* src );
extern void DX_TextureName ( char* dst, const char* src, const char* ext );
extern void DX_FileBodyName( char* dst, const char* src );
extern void DX_FileUniqName( char* dst, const char* src );

#ifdef __cplusplus
extern "C" {
#endif
extern void dbgprintf(const char *format, ...);
#ifdef __cplusplus
}
#endif

#endif /* _DXUTL_H_  */
