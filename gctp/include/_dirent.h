#ifndef __DIRENT_H
#define __DIRENT_H

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>

/// �f�B���N�g���G���g���[�\����
struct dirent
{
	char d_name[_MAX_FNAME];
};
/// �f�B���N�g���\����
typedef struct __DIR
{
	char __d_dir_name[MAX_PATH];
	struct dirent __d_dirent;
	off_t __loc;
	HANDLE __handle;
} DIR;

/// �f�B���N�g���G���g���[�\����(���C�h������)
struct wdirent
{
	wchar_t d_name[_MAX_FNAME];
};
/// �f�B���N�g���\����(���C�h������)
typedef struct __WDIR
{
	wchar_t __d_dir_name[MAX_PATH];
	struct wdirent __d_dirent;
	off_t __loc;
	HANDLE __handle;
} WDIR;

#ifdef	__cplusplus
extern	"C"	{
#endif	/*	__cplusplus		*/

DIR *opendir (const char *);
struct dirent *readdir (DIR *);
void rewinddir (DIR *);
int closedir (DIR *);

//int dirfd (DIR *);

WDIR *wopendir (const wchar_t *);
struct wdirent *wreaddir (WDIR *);
void wrewinddir (WDIR *);
int wclosedir (WDIR *);

#ifndef _POSIX_SOURCE
off_t telldir (DIR *);
void seekdir (DIR *, off_t loc);

//int scandir (const char *__dir,
//	     struct dirent ***__namelist,
//	     int (*select) (const struct dirent *),
//	     int (*compar) (const struct dirent **, const struct dirent **));

//int alphasort (const struct dirent **__a, const struct dirent **__b);

off_t wtelldir (WDIR *);
void wseekdir (WDIR *, off_t loc);

#endif /* _POSIX_SOURCE */

#ifdef UNICODE
typedef WDIR TDIR;
typedef struct wdirent tdirent;
WDIR *topendir (const wchar_t *);
struct wdirent *treaddir (WDIR *);
void trewinddir (WDIR *);
int tclosedir (WDIR *);

#ifndef _POSIX_SOURCE
off_t ttelldir (WDIR *);
void tseekdir (WDIR *, off_t loc);
#endif
#else
typedef DIR TDIR;
typedef struct dirent tdirent;
DIR *topendir (const char *);
struct dirent *treaddir (DIR *);
void trewinddir (DIR *);
int tclosedir (DIR *);

#ifndef _POSIX_SOURCE
off_t ttelldir (DIR *);
void tseekdir (DIR *, off_t loc);
#endif
#endif

#ifdef	__cplusplus
}
#endif	/*	__cplusplus		*/

#endif
