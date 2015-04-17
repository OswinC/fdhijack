#define _GNU_SOURCE	// for Linux-specific F_SETSIG
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "fdhj.h"
#include "libfdhj.h"
#include "ceh.h"

static int fdhj = -1;

int fdhj_open_dev(void)
{
	if (fdhj > -1)
		return fdhj;

	fdhj = open("/dev/fdhj", O_RDWR);
	if (fdhj < 0) 
		throw(FDHJEX_OPEN, fdhj, "fdhj_open_dev()");

	return fdhj;
}

void fdhj_close_dev(void)
{
	close(fdhj);
	fdhj = -1;
}

int fdhj_open_file(const char *pathname, pid_t pid, int fd)
{
	struct fdhj_param param; 
	memset(&param, 0, sizeof(param));
	param.pid1 = pid;
	param.fd1 = fd; 
	int flags = ioctl(fdhj, FDHJ_IOC_GFL, &param);
	if (flags < 0)
		throw(FDHJEX_IOCTL, flags, "fdhj_open_file()"); 

	int fd_open = open(pathname, flags | O_SYNC);
	if (fd_open < 0)
		throw(FDHJEX_OPEN, fd_open, "fdhj_open_file()");

	return fd_open;
}

int fdhj_get_value(pid_t pid, int fd, int request)
{
	struct fdhj_param param; 
	memset(&param, 0, sizeof(param));
	param.pid1 = pid;
	param.fd1 = fd; 
	int val = ioctl(fdhj, request, &param);
	if (val < 0)
		throw(FDHJEX_IOCTL, val, "fdhj_get_value()"); 
	
	return val;
}

int fdhj_fcntlset(int fd_dst, int request, pid_t pid_src, int fd_src)
{
	int request_fdhj = -1; 
	switch (request)
	{
		case F_SETFD:
			request_fdhj = FDHJ_IOC_GFL;
			break;
		case F_SETOWN:
			request_fdhj = FDHJ_IOC_GOWN;
			break;
		case F_SETSIG:
			request_fdhj = FDHJ_IOC_GSIG;
			break;
		default:
			throw(FDHJEX_UNKNOWN, request, "fdhj_fcntlset()"); 
			break;
	} 

	int val = fdhj_get_value(pid_src, fd_src, request_fdhj);
	int ret = fcntl(fd_dst, request, val);
	if (ret < 0)
		throw(FDHJEX_FCNTL, ret, "fdhj_fcntlset()"); 

	return val;
}

void fdhj_exchange_fd(pid_t pid1, int fd1, pid_t pid2, int fd2)
{ 
	struct fdhj_param param; 
	memset(&param, 0, sizeof(param));
	param.pid1 = pid1;
	param.fd1 = fd1;
	param.pid2 = pid2;
	param.fd2 = fd2;
	int ret = ioctl(fdhj, FDHJ_IOC_XFD, &param);
	if (ret < 0)
		throw(FDHJEX_IOCTL, ret, "fdhj_exchange_fd()"); 
}
