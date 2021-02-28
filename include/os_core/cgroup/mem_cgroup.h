#ifndef _MEM_CGROUP_H
#define _MEM_CGROUP_H

#include <os_core/cgroup/cgroup.h>

struct mem_cgroup {
	struct cgroup_subsys_state css;
};

#endif