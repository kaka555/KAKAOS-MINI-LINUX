#include <device.h>
#include <cdev.h>
#include <platform.h>
#include <myassert.h>
#include <os_error.h>
#include <export.h>
#include <printf_debug.h>
#include <sys_init_fun.h>

#define PLATFORM_DEV_NAME "platform_"

/*
	value can be in range [0,35]
	corresponding char is ['0'-'9','a'-'z']
 */
static int device_index = 0;

static int _device_register(const char *dev_name, struct file_operations *file_operations_ptr, UINT32 flag)
{
	ASSERT((NULL != dev_name) && (NULL != file_operations_ptr), ASSERT_INPUT);
	struct dentry *dev_dentry_ptr = _find_dentry(DEFAULT_DEVICE_PATH);
	ASSERT(NULL != dev_dentry_ptr, "dev dentry should exist\n");
	ASSERT(is_folder(dev_dentry_ptr), "should be folder\n");
	if (has_same_name_file(dev_dentry_ptr, dev_name))
	{
		return -ERROR_LOGIC;
	}
	struct inode *inode_ptr = _inode_alloc_and_init(NULL, file_operations_ptr, FLAG_INODE_DEVICE | flag);
	if (NULL == inode_ptr)
	{
		return -ERROR_NO_MEM;
	}
	struct dentry *buffer = _file_dentry_alloc_and_init(dev_dentry_ptr, inode_ptr, dev_name, FLAG_DENTRY_DEFAULT);
	if (NULL == buffer)
	{
		ka_free(inode_ptr);
		return -ERROR_NO_MEM;
	}
	return 0;
}

/**
 * @Author      kaka
 * @DateTime    2019-04-21
 * @description : used this function to register a device into VFS,
 * user must offer the struct file_operations which contain the operation pointer
 * including open, close and so on, the path of the device is /device/dev_name
 * @param       dev_name
 * @param       file_operations_ptr
 * @param       flag: read or write ?
 * @return      error code
 */
int device_register(const struct device *dev, UINT32 flag)
{
	if ((NULL == dev->dev_name) || (NULL == dev->driver_ptr->fops))
	{
		return -ERROR_NULL_INPUT_PTR;
	}
	return _device_register(dev->dev_name, dev->driver_ptr->fops, flag);
}
EXPORT_SYMBOL(device_register);

static int _device_unregister(const char *dev_name)
{
	ASSERT(NULL != dev_name, ASSERT_INPUT);
	struct dentry *dev_dentry_ptr = _find_dentry(DEFAULT_DEVICE_PATH);
	ASSERT(NULL != dev_dentry_ptr, ASSERT_PARA_AFFIRM);
	ASSERT(is_folder(dev_dentry_ptr), "should be folder\n");
	struct dentry *buffer_ptr;
	struct list_head *head = &dev_dentry_ptr->subdirs;
	list_for_each_entry(buffer_ptr, head, child)
	{
		if (0 == ka_strcmp(dev_name, buffer_ptr->name)) /* check every dentry's name */
		{
			ASSERT((!dentry_not_releasse(buffer_ptr)) && is_file(buffer_ptr)
			       && inode_is_soft(buffer_ptr->d_inode) && inode_is_dev(buffer_ptr->d_inode), ASSERT_PARA_AFFIRM);
			__delete_file(buffer_ptr);
			KA_WARN(DEBUG_TYPE_VFS, "remove device %s\n", dev_name);
			return 0;
		}
	}
	return -ERROR_LOGIC;
}

/**
 * @Author      kaka
 * @DateTime    2019-04-21
 * @description : remove a device from VFS
 * @param       dev_name   [description]
 * @return      error code
 */
int device_unregister(const struct device *dev)
{
	if (NULL == dev)
	{
		return -ERROR_NULL_INPUT_PTR;
	}
	return _device_unregister(dev->dev_name);
}
EXPORT_SYMBOL(device_unregister);

/*
	A platform device must be a char device
	So we have to use char device's interface
	to register it
 */

static int platflom_match(struct device *dev, struct device_driver *drv)
{
	return ka_strcmp(dev->dev_name, drv->name);
}

static inline char index_to_c(int num)
{
	if (num < 10)
		return i2c(num);
	return ('a' + num - 10);
}

static inline int index_ok(void)
{
	return (device_index < 36);
}

static int platflom_probe(struct device *dev)
{
	int ret;
	if (!index_ok())
		return -ERROR_SYS;
	unsigned int len = ka_strlen(PLATFORM_DEV_NAME) + ka_strlen(dev->dev_name) + 1;
	struct platform_device *platform_device_ptr = container_of(dev, struct platform_device, device);
	platform_device_ptr->platform_device_name = ka_malloc(len + 1);
	if (NULL == platform_device_ptr->platform_device_name)
		return -ERROR_NO_MEM;
	ka_strcpy(platform_device_ptr->platform_device_name, PLATFORM_DEV_NAME);
	ka_strcpy(platform_device_ptr->platform_device_name + sizeof(PLATFORM_DEV_NAME) - 1, dev->dev_name);
	ASSERT((sizeof(PLATFORM_DEV_NAME) - 1 + ka_strlen(dev->dev_name) ==
	        (len - 1)), ASSERT_PARA_AFFIRM);
	platform_device_ptr->platform_device_name[len - 1] = index_to_c(device_index);
	platform_device_ptr->platform_device_name[len] = '\0';
	ret = _device_register(platform_device_ptr->platform_device_name,
	                       dev->driver_ptr->fops, dev->rw);
	if (ret) {
		ka_free(platform_device_ptr->platform_device_name);
		return ret;
	}
	ret = dev->driver_ptr->probe(dev);
	if (ret) {
		device_unregister(dev);
		return ret;
	}
	return 0;
}

static int platflom_remove(struct device *dev)
{
	struct platform_device *platform_device_ptr = container_of(dev, struct platform_device, device);
	_device_unregister(platform_device_ptr->platform_device_name);
	dev->driver_ptr->remove(dev);
	return 0;
}

struct bus_type platform_bus = {
	.match = platflom_match,
	.probe = platflom_probe,
	.remove = platflom_remove,
};

void __INIT platform_bus_register(void)
{
	bus_init(&platform_bus, "platform_bus", NULL);
}
INIT_FUN(platform_bus_register, 3);

int platform_driver_register(struct platform_driver *drv, const char *driver_name)
{
	int ret;
	drv->driver.bus_ptr = &platform_bus;
	ret = driver_init(&drv->driver, driver_name);
	if (ret < 0)
		return ret;
	return driver_add(&drv->driver);
}

int platform_driver_unregister(struct platform_driver *drv)
{
	// any other things to do?
	return driver_rm(&drv->driver);
}

int platform_device_register(struct platform_device *dev, const char *dev_name, UINT32 flag)
{
	int ret = device_init(&dev->device, dev_name, flag);
	if (ret < 0)
		return ret;
	dev->device.bus_ptr = &platform_bus;
	ret = device_add(&dev->device);
	return ret;
}

int platform_device_unregister(struct platform_device *dev)
{
	// any other things to do?
	return device_rm(&dev->device);
}