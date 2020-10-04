#include <proc.h>
#include <vfs.h>
#include <bsp.h>
#include <os_error.h>
#include <myassert.h>

static char mem_name[] = {'m', 'e', 'm', '0', '\0'};
static struct device *mem_dev_array[10];

static inline unsigned int get_mem_dev_index(const char *name)
{
	return c2i(name[3]);
}

static int meminfo_open(struct file *file_ptr)
{
	ASSERT(NULL == file_ptr->f_den->d_inode, "proc file do not exist in hardware\n");
	unsigned int index = get_mem_dev_index(file_ptr->f_den->name);
	ka_printf("%s start addr is %p\tsize is %u\n",
	          file_ptr->f_den->name,
	          mem_dev_array[index]->u.mem.para->start,
	          mem_dev_array[index]->u.mem.para->size);
	return 0;
}

static struct file_operations proc_meminfo_fops = {
	.open = meminfo_open,
};

int proc_meminfo_register(struct device *mem_dev)
{
	static int mem_num = 0;
	int ret;
	if (mem_num >= 10)
		return -ERROR_SYS;
	mem_name[3] = i2c(mem_num);
	ret = proc_creat(NULL, mem_name, &proc_meminfo_fops);
	if (ret)
		return ret;
	mem_dev_array[mem_num] = mem_dev;
	++mem_num;
	return 0;
}