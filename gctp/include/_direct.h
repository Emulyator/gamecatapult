#ifndef __DIRENT_H
#define __DIRENT_H
/* Posix dirent.h for WIN32.

   Copyright 2001 Red Hat, Inc.

   This software is a copyrighted work licensed under the terms of the
   Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
   details. */

/* Including this file should not require any Windows headers.  */

#include <windows.h>
#include <sys/types.h>

/// ディレクトリエントリー構造体
struct dirent
{
	char d_name[_MAX_FNAME];
};
/// ディレクトリ構造体
typedef struct __DIR
{
	char __d_dir_name[MAX_PATH];
	struct dirent __d_dirent;
	off_t __loc;
	HANDLE __handle;
} DIR;

#ifdef	__cplusplus
extern	"C"	{
#endif	/*	__cplusplus		*/

DIR *opendir (const char *);
struct dirent *readdir (DIR *);
void rewinddir (DIR *);
int closedir (DIR *);

//int dirfd (DIR *);

#ifndef _POSIX_SOURCE
off_t telldir (DIR *);
void seekdir (DIR *, off_t loc);

//int scandir (const char *__dir,
//	     struct dirent ***__namelist,
//	     int (*select) (const struct dirent *),
//	     int (*compar) (const struct dirent **, const struct dirent **));

//int alphasort (const struct dirent **__a, const struct dirent **__b);
#endif /* _POSIX_SOURCE */

#ifdef	__cplusplus
}
#endif	/*	__cplusplus		*/

#endif
