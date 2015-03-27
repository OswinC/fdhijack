#ifndef _FDHJ_H_
#define _FDHJ_H_

#include <linux/ioctl.h> 

struct fdhj_param {
	pid_t pid1;
	int fd1;
	pid_t pid2;
	int fd2;
};

#define FDHJ_IOC_MAGIC		0xdf
#define FDHJ_IOC_XFD		_IOW(FDHJ_IOC_MAGIC, 0, struct fdhj_param *)
// GFL for getting file status flags from fdhj_param.fd1 of fdhj_param.pid1
#define FDHJ_IOC_GFL		_IOW(FDHJ_IOC_MAGIC, 1, struct fdhj_param *)
#define FDHJ_IOC_GOWN		_IOW(FDHJ_IOC_MAGIC, 2, struct fdhj_param *)
#define FDHJ_IOC_GSIG		_IOW(FDHJ_IOC_MAGIC, 3, struct fdhj_param *)

#define FDHJ_IOC_MAXNR 3

#ifdef __KERNEL__

#include <linux/module.h>
#define PRINTK(msglv, fmt, args...) printk(msglv "[%s] " fmt, THIS_MODULE->name, ## args)

#	ifndef FDHJ_MAJOR
#		define FDHJ_MAJOR 0
#	endif

#endif /* __KERNEL__ */

#endif /* _FDHJ_H_ */
