/**@file
 * POSIX.1準拠風ディレクトリ関数
 */
#ifdef _MSC_VER
#pragma once
#endif
//#define WIN32_LEAN_AND_MEAN	// Windows ヘッダーから殆ど使用されないスタッフを除外します。
#include <_direct.h>
#include <stdio.h>
#include <stdlib.h>

/**
 要するにFindFirst
 */
DIR *opendir (const char *path)
{
	DIR *ret = malloc(sizeof(DIR));
	if(ret) {
		WIN32_FIND_DATAA dat;
		char wstr[MAX_PATH];
		strcpy(wstr, path);
		strcat(wstr, "/*");
		if(SUCCEEDED(ret->__handle = FindFirstFileA(wstr, &dat))) {
			strcpy(ret->__d_dirent.d_name, dat.cFileName);
			strcpy(ret->__d_dir_name, wstr);
			ret->__loc = 0;
		}
		else {
			free(ret);
			return NULL;
		}
	}
	return ret;
}

/**
 要するにFindNext
 */
struct dirent *readdir (DIR *dir)
{
	if(dir && dir->__handle) {
		if(dir->__loc) {
			WIN32_FIND_DATAA dat;
			if(FindNextFileA(dir->__handle, &dat)) {
				strcpy(dir->__d_dirent.d_name, dat.cFileName);
			}
			else return NULL;
		}
		dir->__loc++;
		return &dir->__d_dirent;
	}
	return NULL;
}

/**
 要するに一端閉じて開きなおす
 */
void rewinddir (DIR *dir)
{
	if(dir && dir->__handle && dir->__loc) {
		WIN32_FIND_DATAA dat;
		FindClose(dir->__handle);
		if(SUCCEEDED(dir->__handle = FindFirstFileA(dir->__d_dir_name, &dat))) {
			strcpy(dir->__d_dirent.d_name, dat.cFileName);
			dir->__loc = 0;
		}
	}
}

/**
 要するにFindClose
 */
int closedir (DIR *dir)
{
	if(dir) {
		if(dir->__handle) FindClose(dir->__handle);
		free(dir);
	}
	return 1;
}

/**
 ロケーションとして内部カウンタを返す
 */
off_t telldir (DIR *dir)
{
	return dir->__loc;
}

/**
 要するに一端リワインドして指定回数読み飛ばす
 */
void seekdir (DIR *dir, off_t loc)
{
	for(rewinddir(dir); dir->__loc < loc; readdir(dir));
}



/////////////////////////////////////////////////////////
// ワイド版
//

WDIR *wopendir (const wchar_t *path)
{
	WDIR *ret = malloc(sizeof(WDIR));
	if(ret) {
		WIN32_FIND_DATAW dat;
		wchar_t wstr[MAX_PATH];
		wcscpy(wstr, path);
		wcscat(wstr, L"/*");
		if(SUCCEEDED(ret->__handle = FindFirstFileW(wstr, &dat))) {
			wcscpy(ret->__d_dirent.d_name, dat.cFileName);
			wcscpy(ret->__d_dir_name, wstr);
			ret->__loc = 0;
		}
		else {
			free(ret);
			return 0;
		}
	}
	return ret;
}

struct wdirent *wreaddir (WDIR *dir)
{
	if(dir && dir->__handle) {
		if(dir->__loc) {
			WIN32_FIND_DATAW dat;
			if(FindNextFileW(dir->__handle, &dat)) {
				wcscpy(dir->__d_dirent.d_name, dat.cFileName);
			}
			else return NULL;
		}
		dir->__loc++;
		return &dir->__d_dirent;
	}
	return NULL;
}

void wrewinddir (WDIR *dir)
{
	if(dir && dir->__handle && dir->__loc) {
		WIN32_FIND_DATAW dat;
		FindClose(dir->__handle);
		if(SUCCEEDED(dir->__handle = FindFirstFileW(dir->__d_dir_name, &dat))) {
			wcscpy(dir->__d_dirent.d_name, dat.cFileName);
			dir->__loc = 0;
		}
	}
}

/**
 要するにFindClose
 */
int wclosedir (WDIR *dir)
{
	if(dir) {
		if(dir->__handle) FindClose(dir->__handle);
		free(dir);
	}
	return 1;
}

/**
 ロケーションとして内部カウンタを返す
 */
off_t wtelldir (WDIR *dir)
{
	return dir->__loc;
}

/**
 要するに一端リワインドして指定回数読み飛ばす
 */
void wseekdir (WDIR *dir, off_t loc)
{
	for(wrewinddir(dir); dir->__loc < loc; wreaddir(dir));
}









/////////////////////////////////////////////////////////


#ifdef UNICODE
WDIR *topendir(const wchar_t *d)
{
	return wopendir(d);
}
struct wdirent *treaddir(WDIR *d)
{
	return wreaddir(d);
}
void trewinddir (WDIR *d)
{
	wrewinddir(d);
}
int tclosedir (WDIR *d)
{
	return wclosedir(d);
}

#ifndef _POSIX_SOURCE
off_t ttelldir(WDIR *d)
{
	return wtelldir(d);
}
void tseekdir(WDIR *d, off_t loc)
{
	wseekdir(d, loc);
}
#endif
#else
DIR *topendir (const char *d)
{
	return opendir(d);
}
struct dirent *treaddir (DIR *d)
{
	return readdir(d);
}
void trewinddir (DIR *d)
{
	rewinddir(d);
}
int tclosedir (DIR *d)
{
	return closedir(d);
}

#ifndef _POSIX_SOURCE
off_t ttelldir (DIR *d)
{
	return telldir(d);
}
void tseekdir (DIR *d, off_t loc)
{
	seekdir(d);
}
#endif
#endif
