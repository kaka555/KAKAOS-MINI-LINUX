#ifndef _DRIVER_H
#define _DRIVER_H

#include <kobject.h>
#include <device.h>
#include <bus.h>

struct driver
{
	struct kobject kobject;
	struct bus *bus_ptr; /* the bus belongs to */
	int (*probe)(struct device *dev);
	int (*release)(struct device *dev);
};

int driver_init(struct driver *dri);
int driver_add(struct driver *dri, struct bus *bus_ptr);
int driver_remove(struct driver *dri);

#endif