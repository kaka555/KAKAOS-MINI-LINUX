#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <device.h>

struct platform_device
{
	struct device device;
	char *platform_device_name;
};

struct platform_driver
{
	struct device_driver driver;
};

extern struct bus_type platform_bus;

int platform_driver_register(struct platform_driver *drv, const char *driver_name);
int platform_driver_unregister(struct platform_driver *drv);
int platform_device_register(struct platform_device *dev, const char *dev_name, UINT32 flag);
int platform_device_unregister(struct platform_device *dev);

static inline int ro_platform_device_register(struct platform_device *dev, const char *dev_name)
{
	return platform_device_register(dev, dev_name, FLAG_INODE_READ);
}

static inline int wo_platform_device_register(struct platform_device *dev, const char *dev_name)
{
	return platform_device_register(dev, dev_name, FLAG_INODE_WRITE);
}

static inline int rdwr_platform_device_register(struct platform_device *dev, const char *dev_name)
{
	return platform_device_register(dev, dev_name, FLAG_INODE_READ | FLAG_INODE_WRITE);
}

#endif
