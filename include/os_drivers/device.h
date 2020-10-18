#ifndef _DEVICE_H
#define _DEVICE_H

#include <vfs.h>
#include <double_linked_list.h>

#define DEFAULT_DEVICE_PATH "/dev"

#define DEVICE_FLAG_ONLINE (0x01 << 0)

struct device
{
	const char *dev_name;
	struct device_driver *driver_ptr;
	struct list_head driver;
	void *device_data;
	struct bus_type *bus_ptr;
	struct list_head bus;
	int flag;
	unsigned int count;
	UINT32 rw;
};

struct device_driver
{
	struct file_operations *fops;
	const char *name;
	int (*probe)(struct device *dev);
	int (*remove)(struct device *dev);
	struct bus_type *bus_ptr;
	struct list_head bus;
	struct list_head device_list;
};

struct bus_type
{
	const char *name; /* The name of the bus. */
	const char *dev_name; /* Used for subsystems to enumerate devices like ("foo%u", dev->id) */
	int (*match)(struct device *dev, struct device_driver *drv);
	int (*probe)(struct device *dev);
	int (*remove)(struct device *dev);
	struct list_head drivers;
	struct list_head devices;
};

static inline void device_online(struct device *dev_ptr)
{
	dev_ptr->flag |= DEVICE_FLAG_ONLINE;
	++(dev_ptr->count);
}

static inline void device_offline(struct device *dev_ptr)
{
	--(dev_ptr->count);
	if (0 == dev_ptr->count)
		dev_ptr->flag &= ~DEVICE_FLAG_ONLINE;
}

static inline int device_is_online(struct device *dev_ptr)
{
	return (dev_ptr->flag & DEVICE_FLAG_ONLINE);
}

static inline int device_init(struct device *dev_ptr, const char *dev_name, UINT32 flag)
{
	dev_ptr->dev_name = dev_name;
	dev_ptr->driver_ptr = NULL;
	INIT_LIST_HEAD(&dev_ptr->driver);
	INIT_LIST_HEAD(&dev_ptr->bus);
	dev_ptr->flag = 0;
	dev_ptr->rw = flag;
	return 0;
}

static inline int readonly_device_init(struct device *dev_ptr, const char *dev_name)
{
	return device_init(dev_ptr, dev_name, FLAG_INODE_READ);
}

static inline int writeonly_device_init(struct device *dev_ptr, const char *dev_name)
{
	return device_init(dev_ptr, dev_name, FLAG_INODE_WRITE);
}

static inline int rdwr_device_init(struct device *dev_ptr, const char *dev_name)
{
	return device_init(dev_ptr, dev_name, FLAG_INODE_READ | FLAG_INODE_WRITE);
}

int bus_init(struct bus_type *bus_ptr, const char *name, const char dev_name);
int driver_init(struct device_driver *driver_ptr,
                const char *driver_name);
int device_add(struct device *device_ptr);
int driver_add(struct device_driver *driver_ptr);
int device_rm(struct device *device_ptr);
int driver_rm(struct device_driver *driver_ptr);

struct device *get_device_ptr(struct dentry *dentry_ptr);

int device_register(const struct device *dev, UINT32 flag);
int device_unregister(const struct device *dev);

static inline int readonly_device_register(const struct device *dev)
{
	return device_register(dev, FLAG_INODE_READ);
}

static inline int writeonly_device_register(const struct device *dev)
{
	return device_register(dev, FLAG_INODE_WRITE);
}

static inline int rdwr_device_register(const struct device *dev)
{
	return device_register(dev, FLAG_INODE_READ | FLAG_INODE_WRITE);
}

static inline int device_has_matched(const struct device *dev)
{
	return (dev->driver_ptr != NULL);
}

#endif
