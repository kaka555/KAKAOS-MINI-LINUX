#ifndef _KOBJECT_H
#define _KOBJECT_H

#include <double_linked_list.h>

struct kobject
{
	unsigned int count;
	struct kobject *parent;
	struct list_head child;
	struct list_head sibling;
};

int kobject_init(struct kobject *kobj);
struct kobject *kobject_alloc(void);
int kobject_add(struct kobject *kobj, struct kobject *parent_ptr);
void kobject_get(struct kobject *kobj_ptr);
void kobject_put(struct kobject *kobj);

#endif