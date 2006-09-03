/*********************************************************************/
/*                                                                   */
/* FILE :        memio.h                                             */
/*                                                                   */
/* DESCRIPTION : LightWave 3D Object Library 2                       */
/*                                                                   */
/* HISTORY :     Jan 01, 2000    written by Yoshiaki Tazaki          */
/*               Sep 15, 2000    last update                         */
/*                                                                   */
/* Copyright (C) 2000, D-STORM, Inc.                                 */
/*                                                                   */
/*********************************************************************/

#ifndef _MEMIO_H_
#define _MEMIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <lwio.h>

typedef struct st_MEMSaveState {
    int          ioMode;
    void         *writeData;
    void        (*write)    (void *data, const char  *, int len);

    void        (*writeI1)  (void *data, const char  *, int n);
    void        (*writeI2)  (void *data, const short *, int n);
    void        (*writeI4)  (void *data, const long  *, int n);
    void        (*writeU1)  (void *data, const unsigned char  *, int n);
    void        (*writeU2)  (void *data, const unsigned short *, int n);
    void        (*writeU4)  (void *data, const unsigned long  *, int n);
    void        (*writeFP)  (void *data, const float *, int n);
    void        (*writeStr) (void *data, const char  *);
    void        (*writeID)  (void *data, const LWBlockIdent *);

    void        (*beginBlk) (void *data, const LWBlockIdent *, int leaf);
    void        (*endBlk)   (void *data);
    int         (*depth)    (void *data);
} MEMSaveState;


typedef struct st_MEMLoadState {
    int          ioMode;
    void         *readData;
    int         (*read)    (void *data, char  *, int len);

    int         (*readI1)  (void *data, char  *, int n);
    int         (*readI2)  (void *data, short *, int n);
    int         (*readI4)  (void *data, long  *, int n);
    int         (*readU1)  (void *data, unsigned char  *, int n);
    int         (*readU2)  (void *data, unsigned short *, int n);
    int         (*readU4)  (void *data, unsigned long  *, int n);
    int         (*readFP)  (void *data, float *, int n);
    int         (*readStr) (void *data, char  *, int max);
    LWID        (*readID)  (void *data, const LWBlockIdent *);

    LWID        (*findBlk) (void *data, const LWBlockIdent *);
    void        (*endBlk)  (void *data);
    int         (*depth)   (void *data);
} MEMLoadState;


extern MEMSaveState *MEM_openSave ( void *buff, int buff_len, int ioMode );
extern void          MEM_closeSave( MEMSaveState *save );
extern int           MEM_sizeSave ( MEMSaveState *save );
extern MEMLoadState *MEM_openLoad ( void *buff, int buff_len, int ioMode );
extern void          MEM_closeLoad( MEMLoadState *load );

#ifdef __cplusplus
}
#endif
#endif	/*  _MEMIO_H_  */

