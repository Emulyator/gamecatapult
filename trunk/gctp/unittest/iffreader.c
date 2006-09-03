/** @file
 *
 * csv2iffで吐き出したデータを読み込むタスクプロシージャ
 * とかいいつつTXDも読む。
 * @author osamu takasugi
 * @date 2002年1月3日
 * $Id: Binloader.c,v 1.30 2003/08/13 11:36:18 xaxuser Exp $
 */
#include <stddef.h>
#include <ctype.h>
#include <memory.h>
#include "iffreader.h"

#define _SWAPCHUNK(_cnk) (((_cnk)&0x000000ff)<<24|((_cnk)&0x0000ff00)<<8|((_cnk)&0x00ff0000)>>8|((_cnk)&0xff000000)>>24)
#define _ROP_TYPE_PAD   0x00
#define _ROP_TYPE_CHAR  0x40
#define _ROP_TYPE_SHORT 0x80
#define _ROP_TYPE_LONG  0xc0
#define _ROP_CHAR_MASK  0xc0
#define _ROP_DATA_MASK  0x3f

//======================================================
// POF0チャンクを解決
// 返り値	無し
//
//	written by	O.Takasugi
//======================================================
static void parseRops(
	void *target,		// ポインタの解決をするデータ部分のポインタ
	void *rops,			// ROPSチャンクデータ部分のポインタ
	unsigned int size	// ROPSチャンクのサイズ
) {
	unsigned char *current, *tmp, buf[4];
	unsigned int offset, offset_from_base = 0;
	int *addr;
	
	if(NULL == target || NULL == rops) return;
	tmp = (unsigned char *)target; current = (unsigned char *)rops;
	while(size > 0){
		buf[0] = *current++; size--;
		switch(buf[0]&_ROP_CHAR_MASK){
		case _ROP_TYPE_PAD:
			break;
		case _ROP_TYPE_CHAR:
			offset = (unsigned int)(buf[0]&_ROP_DATA_MASK);
			goto _ADD_BASE_ADDRESS_;
		case _ROP_TYPE_SHORT:
			buf[1] = *current++; size--;
			offset = (unsigned int)((buf[0]&_ROP_DATA_MASK)<<8|buf[1]);
			goto _ADD_BASE_ADDRESS_;
		case _ROP_TYPE_LONG:
			buf[1] = *current++; size--;
			buf[2] = *current++; size--;
			buf[3] = *current++; size--;
			offset = (unsigned int)((buf[0]&_ROP_DATA_MASK)<<24|(buf[1]<<16)|(buf[2]<<8)|buf[3]);
		_ADD_BASE_ADDRESS_:
			offset *= 4;
			offset_from_base += offset;
			addr = (int *)(tmp+offset_from_base);
			if(*addr) {
				// *addrが-の場合は解決済みと判断
				// これはＤＣの場合。
				// Windowsでは、0x000FFFFF以上が解決済みと判断
#ifdef WIN32
				if(((unsigned int)*addr)<=0x000FFFFF) *addr += (int)tmp;
#else
				if(*addr>0) *addr += (int)tmp;
#endif
			}
			break;
		}
	}
}

/** チャンクデータ渡り（反復子初期化）
 *
 * @param data  読み取るデータ
 * @param fpos  現在どこまで読み取ったかを保存しておくための領域
 *　　　　　　　（先頭からのオフセット。最初は０で初期化しておくこと）
 * @param ptype	 チャンクの種類がセットされて返される
 * @sample u_int fpos, type, siz; for(chunk = afIffChunkFirst(data, &fpos, &type, &size); fpos < filesize && type; chunk = afIffChunkNext(data, &fpos, &type, &size)) { chunkに対する処理; }
 * @author osamu takasugi
 * @date 2003年2月13日
 * $Id: Binloader.c,v 1.30 2003/08/13 11:36:18 xaxuser Exp $
 */
void *iffChunkFirst(void *data, unsigned int *fpos, unsigned int *ptype, unsigned int *psize)
{
	*fpos = 0; memcpy(ptype, data, 4); memcpy(psize, (char *)data + 4, 4);
	*ptype = _SWAPCHUNK(*ptype);
	return (char *)data + 8;
}

/** チャンクデータ渡り（反復子インクリメント）
 *
 * @param data  読み取るデータ
 * @param fpos  現在どこまで読み取ったかを保存しておくための領域
 *　　　　　　　（先頭からのオフセット。最初は０で初期化しておくこと）
 * @param ptype	 チャンクの種類がセットされて返される
 * @param psize	 チャンクのサイズがセットされて返される
 * @sample u_int fpos, type, size; for(chunk = afIffChunkFirst(data, &fpos, &type, &size); fpos < filesize && type; chunk = afIffChunkNext(data, &fpos, &type, &size)) { chunkに対する処理; }
 * @author osamu takasugi
 * @date 2003年2月13日
 * $Id: Binloader.c,v 1.30 2003/08/13 11:36:18 xaxuser Exp $
 */
void *iffChunkNext(void *data, unsigned int *fpos, unsigned int *ptype, unsigned int *psize)
{
	*fpos += 8 + *psize;
	memcpy(ptype, (char *)data + *fpos, 4);
	memcpy(psize, (char *)data + *fpos + 4, 4);
	*ptype = _SWAPCHUNK(*ptype);
	return (char *)data + *fpos + 8;
}

void iffParseROPS(void *data, unsigned int datasize)
{
	void *prechunk, *chunk; unsigned int fpos, type, size;
	for(chunk = iffChunkFirst(data, &fpos, &type, &size), prechunk = 0; fpos+size < datasize && type; chunk = iffChunkNext(data, &fpos, &type, &size)) {
		if(prechunk && chunk && (type == 'POF0'|| type == 'ROPS') && size) {
			parseRops(prechunk, chunk, size);
		}
		prechunk = chunk;
	}
}
