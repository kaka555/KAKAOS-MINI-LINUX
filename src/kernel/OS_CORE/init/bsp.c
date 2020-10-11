#include <bsp.h>
#include <os_time.h>
#include <osinit.h>
#include <proc.h>
#include <dmesg.h>
#include <os_cpu.h>

const struct bsp_device *device_array_ptr = NULL;
unsigned int bsp_device_num = 0;

/**
 * @Author      kaka
 * @DateTime    2019-05-02
 * @description : this function is used for xxx_bsp.c to init the hardware
 * @param       device_array [description]
 * @param       num          [description]
 */
void _bsp_init(const struct bsp_device *device_array, unsigned int num)
{
	unsigned int i;
	if (NULL == device_array_ptr) {
		device_array_ptr = device_array_ptr;
		bsp_device_num = num;
	}
	for (i = 0; i < num; ++i)
	{
		volatile UINT32 tick;
		tick = (UINT32)_get_tick();
		pr_boot("[%u]", tick);
		switch (device_array[i].head.type)
		{
		case DEV_NORMAL:
			device_array[i].u.normal.init_fun();
			pr_boot("type is DEV_NORMAL\n");
			break;
		case DEV_MEM:
			device_array[i].u.mem.init_fun(device_array[i].u.mem.para);
			pr_boot("type is DEV_MEM\n");
			break;
		default :
			pr_boot("error device!!!\n"); break;
		}
		tick = (UINT32)_get_tick();
		pr_boot("[%u]", tick);
		pr_boot("dev_name is %s\n", device_array[i].head.dev_name);
		if (NULL != device_array[i].head.dev_info)
		{
			tick = (UINT32)_get_tick();
			pr_boot("[%u]", tick);
			pr_boot("dev_info is %s\n", device_array[i].head.dev_info);
		}
	}
}
