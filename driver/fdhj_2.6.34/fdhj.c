#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/rcupdate.h>
#include <linux/cdev.h>
#include <linux/ptrace.h>
#include <linux/pid.h>
#include <asm/uaccess.h>

#include "fdhj.h"

static int fdhj_major = FDHJ_MAJOR;

module_param(fdhj_major, int, S_IRUGO);

static struct cdev fdhj_cdev;

static int fdhj_exchangefd(pid_t pid1, int fd1, pid_t pid2, int fd2)
{
	if (pid1 < 1 || fd1 < 0 || pid2 < 1 || fd2 < 0) {
		PRINTK(KERN_WARNING, "Invalid parameters\n");
		return -EINVAL;
	}

	rcu_read_lock();
	struct task_struct *task1 = pid_task(find_vpid(pid1), PIDTYPE_PID);
	struct task_struct *task2 = pid_task(find_vpid(pid2), PIDTYPE_PID); 
	rcu_read_unlock();
	if (task1 == NULL) {
		PRINTK(KERN_WARNING, "pid %d doesn't exist\n", pid1);
		return -EINVAL;
	}
	if (task2 == NULL) {
		PRINTK(KERN_WARNING, "pid %d doesn't exist\n", pid2);
		return -EINVAL;
	} 

	spin_lock(&task1->files->file_lock);
	struct fdtable *fdt1 = files_fdtable(task1->files);
	spin_unlock(&task1->files->file_lock);

	spin_lock(&task2->files->file_lock); 
	struct fdtable *fdt2 = files_fdtable(task2->files); 
	spin_unlock(&task2->files->file_lock); 

	if (fdt1->fd[fd1] == NULL) {
		PRINTK(KERN_WARNING, "fd %d for %s [%d] is not opened\n", fd1, task1->comm, task1->pid);
		return -EINVAL;
	}
	if (fdt2->fd[fd2] == NULL) {
		PRINTK(KERN_WARNING, "fd %d for %s [%d] is not opened\n", fd2, task2->comm, task2->pid);
		return -EINVAL;
	} 

	spin_lock(&task1->files->file_lock);
	spin_lock(&task2->files->file_lock); 

	struct file *tmpfilp = fdt1->fd[fd1];
	rcu_assign_pointer(fdt1->fd[fd1], fdt2->fd[fd2]);
	rcu_assign_pointer(fdt2->fd[fd2], tmpfilp); 

	spin_unlock(&task2->files->file_lock);
	spin_unlock(&task1->files->file_lock);

	PRINTK(KERN_INFO, "Exchanged fd %d for %s [%d] and fd %d for %s [%d].\n",
			fd1, task1->comm, task1->pid, fd2, task2->comm, task2->pid);

	return 0;
}

/* this is copied from fcntl.c */
static pid_t fdhj_f_getown(struct file *filp)
{
	pid_t pid;
	read_lock(&filp->f_owner.lock);
	pid = pid_vnr(filp->f_owner.pid);
	if (filp->f_owner.pid_type == PIDTYPE_PGID)
		pid = -pid;
	read_unlock(&filp->f_owner.lock);
	return pid;
}

static int fdhj_getval(unsigned int cmd, pid_t pid, int fd)
{
	int retval = 0;

	if (pid < 1 || fd < 0) {
		PRINTK(KERN_WARNING, "Invalid parameters\n");
		return -EINVAL;
	}

	rcu_read_lock();
	struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	if (task == NULL) {
		PRINTK(KERN_WARNING, "pid %d doesn't exist\n", pid);
		return -EINVAL;
	}

	struct file *filp = fcheck_files(task->files, fd); 
	if (filp == NULL) {
		PRINTK(KERN_WARNING, "fd %d for %s [%d] is not opened\n", fd, task->comm, task->pid);
		return -EINVAL;
	}

	switch (cmd) {
		case FDHJ_IOC_GFL:
			retval = filp->f_flags;
			break;
		case FDHJ_IOC_GOWN:
			retval = (int)fdhj_f_getown(filp);
			force_successful_syscall_return();
			break;
		case FDHJ_IOC_GSIG:
			retval = filp->f_owner.signum;
			break;
		default: 
			retval = -ENOTTY;
			break;
	}
	
	return retval;
} 

static long fdhj_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	long retval = 0;
	struct fdhj_param param;

	if (_IOC_TYPE(cmd) != FDHJ_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > FDHJ_IOC_MAXNR) return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch (cmd) {
		case FDHJ_IOC_XFD:
			if (__copy_from_user(&param, (struct fdhj_param *) arg, sizeof(struct fdhj_param))) {
				return -EFAULT;
			}
			retval = fdhj_exchangefd(param.pid1, param.fd1, param.pid2, param.fd2);
			break;
		case FDHJ_IOC_GFL: case FDHJ_IOC_GOWN: case FDHJ_IOC_GSIG:
			if (__copy_from_user(&param, (struct fdhj_param *) arg, sizeof(struct fdhj_param))) {
				return -EFAULT;
			}
			retval = fdhj_getval(cmd, param.pid1, param.fd1);
			break; 
		default:  /* redundant, as cmd was checked against MAXNR */
			return -ENOTTY;
			break;
	}

	return retval;
} 

static int fdhj_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int fdhj_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations fdhj_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= fdhj_unlocked_ioctl,
	.open			= fdhj_open,
	.release		= fdhj_release,
};

static void __exit fdhj_exit(void)
{
	dev_t dev = MKDEV(fdhj_major, 0);

	cdev_del(&fdhj_cdev); 
	unregister_chrdev_region(dev, 1);

	PRINTK(KERN_INFO, "Cleaned Up.\n");
}

static int __init fdhj_init(void)
{
	int result;
	dev_t dev = 0;

	if (fdhj_major) {
		dev = MKDEV(fdhj_major, 0);
		result = register_chrdev_region(dev, 1, THIS_MODULE->name);
	} else {
		result = alloc_chrdev_region(&dev, 0, 1, THIS_MODULE->name);
		fdhj_major = MAJOR(dev);
	}
	if (result < 0) {
		PRINTK(KERN_WARNING, "Can't get major %d\n", fdhj_major);
		return result;
	}

	cdev_init(&fdhj_cdev, &fdhj_fops);
	fdhj_cdev.owner = THIS_MODULE;
	result = cdev_add(&fdhj_cdev, dev, 1);
	if (result) {
		PRINTK(KERN_WARNING, "Error %d adding fdhj_cdev\n", result);
		goto fail;
	}

	PRINTK(KERN_INFO, "Ready.\n");
	PRINTK(KERN_INFO, "Major number is %d.\n", fdhj_major);

	return 0;

fail:
	fdhj_exit();
	return result;
}

module_init(fdhj_init);
module_exit(fdhj_exit);

MODULE_PARM_DESC(fdhj_major, "Major number for fdhj (default 0 for automatic assignment)");
MODULE_AUTHOR("Vivotek Inc.");
MODULE_LICENSE("GPL");

