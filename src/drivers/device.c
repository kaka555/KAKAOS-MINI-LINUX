#include <device.h>
#include <vfs.h>
#include <os_error.h>
#include <export.h>
#include <myassert.h>
#include <printf_debug.h>

int bus_init(struct bus_type *bus_ptr, const char *name, const char dev_name)
{
	ASSERT(NULL != bus_ptr, ASSERT_INPUT);
	ASSERT(NULL != name, ASSERT_INPUT);
	ASSERT(NULL != bus_ptr->match, ASSERT_INPUT);
	ASSERT(NULL != bus_ptr->probe, ASSERT_INPUT);
	ASSERT(NULL != bus_ptr->remove, ASSERT_INPUT);
	bus_ptr->name = name;
	INIT_LIST_HEAD(&bus_ptr->drivers);
	INIT_LIST_HEAD(&bus_ptr->devices);
	return 0;
}

int driver_init(struct device_driver *driver_ptr,
                const char *driver_name)
{
	ASSERT(NULL != driver_ptr, ASSERT_INPUT);
	ASSERT(NULL != driver_name, ASSERT_INPUT);
	ASSERT(NULL != driver_ptr->fops, ASSERT_INPUT);
	driver_ptr->name = driver_name;
	INIT_LIST_HEAD(&driver_ptr->device_list);
	INIT_LIST_HEAD(&driver_ptr->bus);
	return 0;
}

int device_add(struct device *device_ptr)
{
	struct device_driver *driver_ptr;
	int ret;
	struct bus_type *bus_ptr = device_ptr->bus_ptr;
	if (NULL == bus_ptr)
		return -ERROR_LOGIC; //a device should attach into a bus
	list_add_tail(&device_ptr->bus, &bus_ptr->devices);
	list_for_each_entry(driver_ptr, &bus_ptr->drivers, bus) {
		ret = bus_ptr->match(device_ptr, driver_ptr);
		if (ret == 0) {
			device_ptr->driver_ptr = driver_ptr;
			bus_ptr->probe(device_ptr);
			break;
		}
		/* of course we allow no match */
	}
	return 0;
}

int driver_add(struct device_driver *driver_ptr)
{
	struct device *device_ptr;
	int ret;
	struct bus_type *bus_ptr = driver_ptr->bus_ptr;
	if (NULL == bus_ptr)
		return -ERROR_LOGIC; //a device should attach into a bus
	list_add_tail(&driver_ptr->bus, &bus_ptr->drivers);
	list_for_each_entry(device_ptr, &bus_ptr->drivers, bus) {
		ret = bus_ptr->match(device_ptr, driver_ptr);
		if (ret == 0) {
			device_ptr->driver_ptr = driver_ptr;
			bus_ptr->probe(device_ptr);
			break;
		}
		/* of course we allow no match */
	}
	return 0;
}

int device_rm(struct device *device_ptr)
{
	int ret;
	if (device_is_online(device_ptr))
		return -ERROR_BUSY;
	ASSERT(device_ptr->bus_ptr, ASSERT_PARA_AFFIRM);
	ret = device_ptr->bus_ptr->remove(device_ptr);
	if (ret < 0)
		return ret;
	list_del(&device_ptr->bus);
	list_del(&device_ptr->driver);
	device_ptr->bus_ptr = NULL;
	return 0;
}

int driver_rm(struct device_driver *driver_ptr)
{
	struct list_head del_list;
	struct device *device_ptr;
	INIT_LIST_HEAD(&del_list);
	list_for_each_entry(device_ptr, &driver_ptr->device_list, driver) {
		if (device_is_online(device_ptr))
			goto fail;
		list_del(&device_ptr->driver);
		list_add_tail(&device_ptr->driver, &del_list);
		driver_ptr->remove(device_ptr); /* let's assume that if device is offline,
		 function remove must be succeed */
	}
	driver_ptr->bus_ptr = NULL;
	return 0;
fail:
	list_for_each_entry(device_ptr, &del_list, driver) {
		list_del(&device_ptr->driver);
		list_add_tail(&device_ptr->driver, &driver_ptr->device_list);
		driver_ptr->bus_ptr->probe(device_ptr);
	}
	return -ERROR_BUSY;
}