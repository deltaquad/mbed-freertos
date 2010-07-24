#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <reent.h>
#include "lib/syscalls/syscalls_util.h"

int _swistat(int fd, struct stat * st)
{
	struct fdent *pfd;
	int res;

	pfd = findslot(fd);
	if (pfd == NULL)
	{
		errno = EBADF;
		return -1;
	}

	/* Always assume a character device,
	   with 1024 byte blocks. */
	st->st_mode |= S_IFCHR;
	st->st_blksize = 1024;
	res = checkerror(do_AngelSWI (AngelSWI_Reason_FLen, &pfd->handle));

	if (res == -1)
		return -1;
	/* Return the file size. */
	st->st_size = res;
	return 0;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat * st)
{
	memset(st, 0, sizeof(*st));
	return _swistat(fd, st);
}
