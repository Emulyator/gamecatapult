#ifndef _IFFREADER_H_
#define _IFFREADER_H_
/** @file
 *
 * csv2iffで吐き出したファイルを読み込むタスク
 * @author osamu takasugi
 * @date 2002年1月3日
 * $Id: Binloader.h,v 1.13 2003/08/13 11:36:18 xaxuser Exp $
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
void *iffChunkFirst(void *data, unsigned int *fpos, unsigned int *ptype, unsigned int *psize);
void *iffChunkNext(void *data, unsigned int *fpos, unsigned int *ptype, unsigned int *psize);
void iffParseROPS(void *data, unsigned int datasize);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // _IFFREADER_H_
