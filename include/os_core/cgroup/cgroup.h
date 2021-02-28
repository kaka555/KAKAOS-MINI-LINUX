#ifndef _CGROUP_H
#define _CGROUP_H

#include <double_linked_list.h>
#include <os_base/atomic.h>
#include <TCB.h>

enum cgroup_subsys_type
{
	MEM_CGROUP,
	CPU_CGROUP,
	CGROUP_SUBSYS_NUM
};

struct cgroup;
struct cgroup_subsys_state;

/*
 * A css_set is a structure holding pointers to a set of
 * cgroup_subsys_state objects. This saves space in the task struct
 * object and speeds up fork()/exit(), since a single inc/dec and a
 * list_add()/del() can bump the reference count on the entire cgroup
 * set for a task.
 */
struct css_set
{
	atomic_t ref;
	struct cgroup_subsys_state *subsys[CGROUP_SUBSYS_NUM];
};

struct cgroup_subsys
{
	void (*attach)(struct cgroup_taskset *tset);
	int (*css_online)(struct cgroup_subsys_state *css);
	void (*css_offline)(struct cgroup_subsys_state *css);
}

struct cgroup_subsys_state
{
	struct cgroup *cgroup;
	/* the cgroup subsystem that this css is attached to */
	struct cgroup_subsys *ss;
};

struct cgroup
{
	struct cgroup *parent;
	struct list_head children;
	struct list_head sibling;
	struct cgroup_subsystem_state subsys[CGROUP_SUBSYS_NUM];
};

int cgroup_init(struct cgroup *cgrp);
int cgroup_del(struct cgroup *cgrp);
int cgroup_attach(struct cgroup *cgrp, struct css_set *cset);
int cgroup_detach(struct cgroup *cgrp, struct css_set *cset);
int online_cgroup(struct cgroup *cgrp);
int offline_cgroup(struct cgroup *cgrp);

#endif