#ifndef _BUS_H
#define _BUS_H

#include <device.h>
#include <driver.h>
#include <sys_init_fun.h>

struct bus
{
	struct kobject kobject;
	int (*probe)(struct device *dev, struct driver *dri);
	int (*match)(struct device *dev, struct driver *dri);
};

int __INIT bus_init(void);

#endif