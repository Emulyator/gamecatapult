/*********************************************************************/
/*                                                                   */
/* FILE :        meshio.h                                            */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Oct 08, 2001    last update                         */
/*                                                                   */
/* Copyright (C) 2000-2001, D-STORM, Inc.                            */
/*                                                                   */
/*********************************************************************/
 
#ifndef _MESHIO_H_
#define _MESHIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <lwserver.h>
#include <lwcmdseq.h>
#include <lwmeshedt.h>

#define MESH_FLAG_NONE       0x0
#define MESH_FLAG_MERGE      0x1		//  merge object layers
#define MESH_FLAG_PATCH      0x2		//  use sub-patch as faces

typedef struct st_VMapLWID {
	LWID          lwid;
	char          name[255];
} VMapLWID;

typedef struct st_PTagLWID {
	LWID          lwid;
	LWID          type;
} PTagLWID;

extern int      Mesh_Object( GlobalFunc *global );
extern int      Mesh_Find  ( GlobalFunc *global, const char *objname );
extern OBJError Mesh_Load  ( OBJ2 *obj, void *global, int obj_no, int flags );
extern EDError  Mesh_ClearVMap( GlobalFunc *global, LWModCommand *local, VMapLWID *vmid );
extern EDError  Mesh_ClearPTag( GlobalFunc *global, LWModCommand *local, PTagLWID *ptid );
extern EDError  Mesh_SaveVMap ( GlobalFunc *global, LWModCommand *local, VMapLWID *vmid, 
                                OBJ2 *obj, int obj_no, int flags );
extern EDError  Mesh_SavePTag ( GlobalFunc *global, LWModCommand *local, PTagLWID *ptid, 
                                OBJ2 *obj, int obj_no, int flags );

#ifdef __cplusplus
}
#endif
#endif  /* _MESHIO_H_ */

