#ifndef _PROC_H
#define _PROC_H

#include <vfs.h>
#include <double_linked_list.h>
#include <sys_init_fun.h>
#include <bsp.h>

struct device;

#define PROC_FILE 0
#define PROC_DIR  1

/* a proc_dir_entry represent a file or dir in proc filesystem */
struct proc_dir_entry
{
	union {
		void *private;
		long mode; /* a dir or file */
	};
	struct file_operations *fops;
	struct proc_dir_entry *parent;
	struct dentry *proc_dentry;
	char *name;
};

struct proc_dir_entry *proc_mkdir(struct proc_dir_entry *parent, const char *name);
struct proc_dir_entry *proc_creat(struct proc_dir_entry *parent, const char *name, struct file_operations *fops);
void remove_proc_entry(struct proc_dir_entry *entry);
int proc_meminfo_register(const struct bsp_device *mem_dev);

void __INIT proc_init(void);

static inline void mark_dir_proc_entry(struct proc_dir_entry *entry)
{
	entry->mode |= 0x01;
}

static inline void mark_file_proc_entry(struct proc_dir_entry *entry)
{
	entry->mode &= (~0x01);
}

static inline void set_proc_entry_mode(struct proc_dir_entry *entry, long mode)
{
	if (mode == PROC_FILE)
		mark_file_proc_entry(entry);
	else
		mark_dir_proc_entry(entry);
}

static inline long get_proc_entry_mode(struct proc_dir_entry *entry)
{
	long mode = entry->mode & 0x01;
	return mode;
}

static inline void set_proc_entry_private(struct proc_dir_entry *entry, void *private)
{
	long ptr = (long)private;
	ptr |= get_proc_entry_mode(entry);
	entry->private = (void *)ptr;
}

static inline void *get_proc_entry_private(struct proc_dir_entry *entry)
{
	return (void *)((long)(entry->private) & (~0x03));
}

#endif
