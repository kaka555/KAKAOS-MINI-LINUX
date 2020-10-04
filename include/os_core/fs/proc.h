#ifndef _PROC_H
#define _PROC_H

#include <vfs.h>
#include <double_linked_list.h>
#include <sys_init_fun.h>

struct device;

#define PROC_FILE 0
#define PROC_DIR  1

/* a proc_dir_entry represent a file or dir in proc filesystem */
struct proc_dir_entry
{
	int mode; /* a dir or file */
	struct file_operations *fops;
	struct proc_dir_entry *parent;

//	struct list_head sibling_entry;
//	struct list_head subdir;
	char *name;
	struct dentry *proc_dentry;
};

int proc_mkdir(struct proc_dir_entry *parent, const char *name);
int proc_creat(struct proc_dir_entry *parent, const char *name, struct file_operations *fops);
void remove_proc_entry(struct proc_dir_entry *entry);
int proc_meminfo_register(struct device *mem_dev);

void __INIT proc_init(void);

#endif