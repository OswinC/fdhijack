#define _GNU_SOURCE	// for Linux-specific F_SETSIG
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "fdhj.h"

int main(int argc, const char *argv[])
{
	if (argc < 4)
	{
		printf("Usage: %s <pid1> <fd1> <pid2> <fd2>\n", argv[0]);
		return -1;
	}

	int fdhj, fd_new;

	fdhj = open("/dev/fdhj", O_RDWR);
	if (fdhj < 0)
	{
		perror("open /dev/fdhj");
		goto err_openfdhj;
	}

	struct fdhj_param param;
	memset(&param, 0, sizeof(param));
	param.pid1 = atoi(argv[1]);
	param.fd1 = atoi(argv[2]);

	int flags, owner, signum;
	int ret;
	/* get file status flags and use it to open the file */
	flags = ioctl(fdhj, FDHJ_IOC_GFL, &param);
	if (flags < 0)
	{
		perror("FDHJ_IOC_GFL");
		goto err_getfl;
	} 
	fd_new = open(argv[3], flags);
	if (fd_new < 0)
	{
		perror("open <file path>");
		goto err_getfl;
	}

	/* get and set signum */
	signum = ioctl(fdhj, FDHJ_IOC_GSIG, &param);
	if (signum < 0)
	{
		perror("FDHJ_IOC_GSIG");
		goto err_getsig;
	} 
	ret = fcntl(fd_new, F_SETSIG, signum);	// need to define _GNU_SOURCE
	if (ret < 0)
	{
		perror("F_SETSIG");
		goto err_getsig;
	}
	
	/* set owner to the given pid before exchange fd */
	owner = ioctl(fdhj, FDHJ_IOC_GOWN, &param);
	if (owner < 0)
	{
		if (errno > 0)
		{
			/* for the case of process group id */
			owner = errno;
		}
		else
		{
			perror("FDHJ_IOC_GOWN");
			goto err_getsig;
		}
	} 
	ret = fcntl(fd_new, F_SETOWN, owner);
	if (ret < 0)
	{
		perror("F_SETOWN");
		goto err_getsig;
	}
	
	/* exchange fd */
	param.pid1 = atoi(argv[1]);
	param.fd1 = atoi(argv[2]);
	param.pid2 = getpid();
	param.fd2 = fd_new;
	ret = ioctl(fdhj, FDHJ_IOC_XFD, &param);
	if (ret == 0)
	{
		printf("Done.\n");
		errno = 0;
	}

err_getsig:
	close(fd_new);
err_getfl:
	close(fdhj); 
err_openfdhj:
	return errno;
}
