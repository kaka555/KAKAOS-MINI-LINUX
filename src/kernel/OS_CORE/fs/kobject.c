#include <kobject.h>
#include <myMicroLIB.h>
#include <myassert.h>
#include <osbase.h>
#include <myassert.h>

int kobject_init(struct kobject *kobj_ptr)
{
	ASSERT(kobj_ptr != NULL, ASSERT_INPUT);
	kobj_ptr->count = 0;
	kobj_ptr->parent = NULL;
	INIT_LIST_HEAD(&kobj_ptr->child);
	kobject_get(kobj_ptr);
	return 0;
}

struct kobject *kobject_alloc(void)
{
	struct kobject *kobject_ptr = ka_malloc(sizeof(struct kobject));
	if (NULL == kobject_ptr)
		return NULL;
	int ret = kobject_init(kobject_ptr);
	if (ret)
		return ERR_PTR(ret);
	return kobject_ptr;
}

int kobject_add(struct kobject *kobj_ptr, struct kobject *parent_ptr)
{
	ASSERT(kobj_ptr->parent == NULL, ASSERT_INPUT);
	ASSERT(kobj_ptr->count == 0, ASSERT_INPUT);
	kobj_ptr->parent = parent_ptr;
	list_add_tail(&kobj_ptr->sibling, &parent_ptr->child);
	kobject_get(kobj_ptr);
	return 0;
}

void kobject_get(struct kobject *kobj_ptr)
{
	struct kobject *kobject_ptr = kobj_ptr->parent;
	while (kobject_ptr != NULL) {
		++kobject_ptr->count;
		kobject_ptr = kobject_ptr->parent;
	}
	++kobj_ptr->count;
}

void kobject_put(struct kobject *kobj_ptr)
{
	ASSERT(kobj_ptr->count != 0, ASSERT_INPUT);
	--kobj_ptr->count;
	if (kobj_ptr->count == 0) {
		ASSERT(list_empty(&kobj_ptr->child), "kobj should have no child in fun kobject_put\n");
		if (kobj_ptr->parent != NULL)
			list_del(&kobj_ptr->sibling);
	}
}