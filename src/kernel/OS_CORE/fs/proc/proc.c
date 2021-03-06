#include <proc.h>
#include <printf_debug.h>
#include <os_error.h>
#include <myMicroLIB.h>
#include <myassert.h>
#include <bsp.h>

/*
	all proc_dir_entry should be allocated except proc root
 */

static struct proc_dir_entry proc_root = {
	.mode = PROC_DIR,
	.fops = &default_file_operations,
	.parent = NULL,
	.name = "proc",
};

static void proc_dir_entry_init(long mode,
                                struct proc_dir_entry *proc_dir_entry_ptr,
                                struct file_operations *fops,
                                struct proc_dir_entry *parent,
                                const char *name)
{
	ASSERT(proc_dir_entry_ptr != NULL, ASSERT_INPUT);
	ASSERT(fops != NULL, ASSERT_INPUT);
	if (!parent)
		parent = &proc_root;
	set_proc_entry_mode(proc_dir_entry_ptr, mode);
	proc_dir_entry_ptr->fops = fops;
	proc_dir_entry_ptr->parent = parent;
	proc_dir_entry_ptr->name = (char *)(proc_dir_entry_ptr + 1);
	ka_strcpy(proc_dir_entry_ptr->name, name);
	proc_dir_entry_ptr->name[ka_strlen(name)] = '\0';
}

struct proc_dir_entry *proc_mkdir(struct proc_dir_entry *parent, const char *name)
{
	struct proc_dir_entry *proc_dir_entry_ptr = ka_malloc(sizeof(struct proc_dir_entry) + ka_strlen(name) + 1);
	if (NULL == proc_dir_entry_ptr)
		return ERR_PTR(-ERROR_NO_MEM);
	proc_dir_entry_init(PROC_DIR, proc_dir_entry_ptr,
	                    &default_file_operations,
	                    parent, name);
	struct dentry *dentry_ptr;
	if (parent)
		dentry_ptr = ___add_folder(parent->proc_dentry, name, &default_file_operations);
	else
		dentry_ptr = ___add_folder(proc_root.proc_dentry, name, &default_file_operations);
	if (IS_ERR(dentry_ptr)) {
		ka_free(proc_dir_entry_ptr);
		return (void *)dentry_ptr;
	}
	proc_dir_entry_ptr->proc_dentry = dentry_ptr;
	return proc_dir_entry_ptr;
}

struct proc_dir_entry *proc_creat(struct proc_dir_entry *parent, const char *name, struct file_operations *fops)
{
	struct proc_dir_entry *proc_file_entry_ptr = ka_malloc(sizeof(struct proc_dir_entry) + ka_strlen(name) + 1);
	if (NULL == proc_file_entry_ptr)
		return ERR_PTR(-ERROR_NO_MEM);
	proc_dir_entry_init(PROC_FILE, proc_file_entry_ptr,
	                    fops, parent, name);
	struct dentry *dentry_ptr;
	if (parent)
		dentry_ptr = ___add_file(parent->proc_dentry, name, fops);
	else
		dentry_ptr = ___add_file(proc_root.proc_dentry, name, fops);
	if (IS_ERR(dentry_ptr)) {
		ka_free(proc_file_entry_ptr);
		return (void *)dentry_ptr;
	}
	proc_file_entry_ptr->proc_dentry = dentry_ptr;
	return proc_file_entry_ptr;
}

void remove_proc_entry(struct proc_dir_entry *entry)
{
	if (entry->mode == PROC_FILE) {
		__delete_file(entry->proc_dentry);
	}
	else {
		ASSERT(entry->mode == PROC_DIR, ASSERT_PARA_AFFIRM);
		remove_all_dentry(entry->proc_dentry);
	}
}

void __INIT proc_init(void)
{
	int i;
	const struct bsp_device *bsp_device_ptr;
	struct dentry *dentry_ptr = ___add_folder(&root_dentry, proc_root.name, proc_root.fops);
	if (IS_ERR(dentry_ptr)) {
		panic("proc create fail\n");
		ASSERT(1, "should never go here\n");
	}
	proc_root.proc_dentry = dentry_ptr;
	for_each_bsp_device(i, bsp_device_ptr) {
		if (DEV_MEM == bsp_device_ptr->head.type)
			proc_meminfo_register(bsp_device_ptr);
	}
}
INIT_FUN(proc_init, 2);