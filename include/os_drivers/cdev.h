#ifndef _CDEV_H
#define _CDEV_H

#include <kobject.h>
#include <device.h>

typedef int dev_t;

struct cdev
{
	struct kobject obj;
	const struct file_operations *fops;
	dev_t dev;
};

int cdev_add(struct cdev *cdev_ptr, dev_t dev, unsigned int count);
void cdev_del(struct cdev *cdev_ptr);
int alloc_chrdev_region(dev_t *dev_ptr, unsigned int baseminor, unsigned int count,
                        const char *name);
void free_chrdev_region(dev_t dev, unsigned int baseminor, unsigned int count);

struct device *get_device_ptr(struct dentry *dentry_ptr);

#endif
