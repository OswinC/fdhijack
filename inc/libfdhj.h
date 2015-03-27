#ifndef _LIBFDHJ_H_
#define _LIBFDHJ_H_

#include "ceh.h"

typedef enum {
	FDHJEX_OPEN,
	FDHJEX_IOCTL,
	FDHJEX_FCNTL,
	FDHJEX_UNKNOWN,
} FDHJEX;

int fdhj_open_dev(void);
void fdhj_close_dev(void);
int fdhj_open_file(const char *pathname, pid_t pid, int fd);
int fdhj_get_value(pid_t pid, int fd, int request);
int fdhj_fcntlset(int fd_dst, int request, pid_t pid_src, int fd_src);
void fdhj_exchange_fd(pid_t pid1, int fd1, pid_t pid2, int fd2);

#endif /* _LIBFDHJ_H_ */
