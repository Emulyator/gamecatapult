/**@file
 * POSIX.1�������f�B���N�g���֐�
 */
#ifdef _MSC_VER
#pragma once
#endif
//#define WIN32_LEAN_AND_MEAN	// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂��B
#include <_direct.h>
#include <stdio.h>
#include <stdlib.h>

/**
 �v�����FindFirst
 */
DIR *opendir (const char *path)
{
	DIR *ret = malloc(sizeof(DIR));
	if(ret) {
		WIN32_FIND_DATA dat;
		char wstr[MAX_PATH];
		strcpy(wstr, path);
		strcat(wstr, "/*");
		if(SUCCEEDED(ret->__handle = FindFirstFile(wstr, &dat))) {
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
 �v�����FindNext
 */
struct dirent *readdir (DIR *dir)
{
	if(dir && dir->__handle) {
		if(dir->__loc) {
			WIN32_FIND_DATA dat;
			if(FindNextFile(dir->__handle, &dat)) {
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
 �v����Ɉ�[���ĊJ���Ȃ���
 */
void rewinddir (DIR *dir)
{
	if(dir && dir->__handle && dir->__loc) {
		WIN32_FIND_DATA dat;
		FindClose(dir->__handle);
		if(SUCCEEDED(dir->__handle = FindFirstFile(dir->__d_dir_name, &dat))) {
			strcpy(dir->__d_dirent.d_name, dat.cFileName);
			dir->__loc = 0;
		}
	}
}

/**
 �v�����FindClose
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
 ���P�[�V�����Ƃ��ē����J�E���^��Ԃ�
 */
off_t telldir (DIR *dir)
{
	return dir->__loc;
}

/**
 �v����Ɉ�[�����C���h���Ďw��񐔓ǂݔ�΂�
 */
void seekdir (DIR *dir, off_t loc)
{
	for(rewinddir(dir); dir->__loc < loc; readdir(dir));
}
