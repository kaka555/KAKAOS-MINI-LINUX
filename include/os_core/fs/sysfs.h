#ifndef _SYSFS_H
#define _SYSFS_H

#include <kobject.h>
#include <sys_init_fun.h>

struct sysfs_dir_entry
{
	int mode;
	struct kobject kobj;
};

int sysfs_mkdir(struct sysfs_dir_entry *sys_entry, struct sysfs_dir_entry *parent);
int sysfs_creat(struct sysfs_dir_entry *sys_entry, struct sysfs_dir_entry *parent);
int remove_sysfs_entry(struct sysfs_dir_entry *sys_entry);

int __INIT sysfs_init(void);

#endif