#include <TCB.h>
#include <myassert.h>
#include <myMicroLIB.h>
#include <task_state.h>
#include <os_TCB_list.h>
#include <os_error.h>
#include <os_cpu_stm32.h>
#include <os_ready.h>
#include <os_suspend.h>
#include <os_delay.h>
#include <os_schedule.h>
#include <module.h>
#include <export.h>
#include <proc.h>
#include <printf_debug.h>

/**
 * the function in this file are used to manage TCB
 */

volatile struct task_struct *OSTCBCurPtr;
volatile struct task_struct *OSTCBHighRdyPtr;
volatile int g_interrupt_count = 0;
volatile int g_task_num = 0;

static struct proc_dir_entry *task_info_proc_root;

void get_task(struct task_struct *task)
{
	++task->ref;
}

void put_task(struct task_struct *task)
{
	ASSERT(task->ref > 0, ASSERT_INPUT);
	if ((--task->ref == 0) && (TCB_IS_CREATED(task)))
		ka_free(task);
}

/**
 * @Author      kaka
 * @DateTime    2019-04-21
 * @description : the addr of this function will be the value of the return register
 */
void delete_myself(void)
{
	task_delete((struct task_struct *)OSTCBCurPtr);
}

static int save_task_struct(struct file *file_ptr)
{
	const char *name = file_ptr->f_den->d_parent->name;
	struct task_struct *TCB_ptr = find_TCB_with_name(name);
	if (NULL == TCB_ptr)
		return -ERROR_LOGIC;
	file_ptr->private_data = TCB_ptr;
	/*we do not have dentry_ptr->d_inode->inode_ops->get_size
	  so change file_len here */
	file_ptr->file_len = 1;
	return 0;
}

static int task_info_open(struct file *file_ptr)
{
	return save_task_struct(file_ptr);
}

static int task_prio_read(struct file *file_ptr, void *buffer, unsigned int len, unsigned int offset)
{
	struct task_struct *TCB_ptr = file_ptr->private_data;
	pr_shell("prio: %u\n", TCB_ptr->prio);
	return len;
}

static struct file_operations proc_task_info_prio_fops = {
	.open = task_info_open,
	.read = task_prio_read,
};

static int task_dump_stack_open(struct file *file_ptr)
{
	return save_task_struct(file_ptr);
}

static int task_dump_stack_read(struct file *file_ptr, void *buffer, unsigned int len, unsigned int offset)
{
	struct task_struct *task_struct_ptr = file_ptr->private_data;
	backtrace_call_stack((UINT32)task_struct_ptr->stack_end, task_struct_ptr->stack_size,
	                     (UINT32)task_struct_ptr->stack);
	return len;
}

static struct file_operations proc_task_dump_stack_fops = {
	.open = task_dump_stack_open,
	.read = task_dump_stack_read,
};

static int create_task_info(struct task_struct *TCB_ptr, struct proc_dir_entry *parent)
{
	struct proc_dir_entry *ret = proc_creat(parent, "prio", &proc_task_info_prio_fops);
	if (IS_ERR(ret))
		return PTR_ERR(ret);
	struct proc_dir_entry *dump_stack_entry = proc_creat(parent, "dump_stack", &proc_task_dump_stack_fops);
	if (IS_ERR(dump_stack_entry)) {
		remove_proc_entry(ret);
		return PTR_ERR(dump_stack_entry);
	}
	return 0;
}

static int create_task_proc(struct task_struct *TCB_ptr)
{
	struct proc_dir_entry *TCB_entry;
	int ret;
	TCB_entry = proc_mkdir(task_info_proc_root, TCB_ptr->name);
	if (IS_ERR(TCB_entry)) {
		pr_info("proc_mkdir fail\n");
		return PTR_ERR(TCB_entry);
	}
	set_proc_entry_private(TCB_entry, TCB_ptr);
	ret = create_task_info(TCB_ptr, TCB_entry);
	if (ret < 0) {
		pr_info("create_task_info fail\n");
		remove_proc_entry(TCB_entry);
		return ret;
	}
	return 0;
}

int _must_check _task_init(
    struct task_struct *TCB_ptr,
    unsigned int stack_size,
    TASK_PRIO_TYPE prio,
    unsigned int timeslice_hope_time,
    const char *name,
    functionptr function,
    void *para,
    TASK_STATE state)
{
	int ret;
	ASSERT(prio < PRIO_MAX, ASSERT_INPUT);
	ASSERT((NULL != name) && (NULL != TCB_ptr) && (NULL != function), ASSERT_INPUT);
	stack_size &= (~0x03);/* according to CPU : 32bit?64bit*/
	TCB_ptr->stack_end = (STACK_TYPE *)ka_malloc(stack_size);
	if (NULL == TCB_ptr->stack_end)
		return -ERROR_NO_MEM;
	TCB_ptr->stack_size = stack_size;
	TCB_ptr->stack = (STACK_TYPE *)TCB_ptr->stack_end + stack_size / 4;
	ASSERT((char *)(TCB_ptr->stack) == (char *)(TCB_ptr->stack_end) + stack_size, ASSERT_PARA_AFFIRM);
	ka_memset(TCB_ptr->stack_end, 0, stack_size);
	set_register((void **)&TCB_ptr->stack, function, delete_myself, para);
	TCB_ptr->prio = prio;
	TCB_ptr->reserve_prio = RESERVED_PRIO;
	TCB_ptr->task_state = state;
	TCB_ptr->attribution = DEFAULT_ATTRIBUTION | TCB_ATTRIBUTION_INIT;
	TCB_ptr->delay_reach_time = 0;
	TCB_ptr->delay_heap_position = 0;
	INIT_LIST_HEAD(&TCB_ptr->same_prio_list);
	INIT_LIST_HEAD(&TCB_ptr->suspend_list);
	TCB_ptr->name = (char *)name;
	TCB_ptr->timeslice_hope_time = timeslice_hope_time;
	TCB_ptr->timeslice_rest_time = timeslice_hope_time;
	TCB_ptr->dynamic_module_ptr = NULL;
	TCB_ptr->run_time = 0;
	_register_in_TCB_list(TCB_ptr);
	ret = create_task_proc(TCB_ptr);
	if (unlikely(ret < 0)) {
		ka_free(TCB_ptr->stack_end);
		return ret;
	}
	TCB_ptr->ref = 1;
	++g_task_num;
	return 0;
}

struct task_struct *_must_check _task_creat(
    unsigned int stack_size,
    TASK_PRIO_TYPE prio,
    unsigned int timeslice_hope_time,
    const char *name,
    functionptr function,
    void *para,
    TASK_STATE state)
{
	ASSERT((prio < PRIO_MAX), ASSERT_INPUT);
	ASSERT((NULL != name) && (NULL != function), ASSERT_INPUT);
	struct task_struct *TCB_ptr = ka_malloc(sizeof(TCB));
	if (NULL == TCB_ptr)
		return NULL;
	if (0 !=
	        _task_init(	TCB_ptr,
	                    stack_size,
	                    prio,
	                    timeslice_hope_time,
	                    name,
	                    function,
	                    para,
	                    state)
	   )
	{
		ka_free(TCB_ptr);
		return NULL;
	}
	TCB_ptr->attribution = DEFAULT_ATTRIBUTION | TCB_ATTRIBUTION_CREATE;
	return TCB_ptr;
}

int _task_change_prio(struct task_struct *TCB_ptr, TASK_PRIO_TYPE prio)
{
	ASSERT(prio < PRIO_MAX, ASSERT_INPUT);
	ASSERT(NULL != TCB_ptr, ASSERT_INPUT);
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	/*1 step: delete it from TCB_list*/
	_delete_from_TCB_list(TCB_ptr);
	/*2 step: according to the task_state,do some changes*/
	switch (TCB_ptr->task_state)
	{
	case STATE_READY:
		_delete_TCB_from_ready(TCB_ptr); break;
	/* add other state's handling code here */
	/*
	 *
	 *
	 *
	 */
	default: break;/* do nothing*/
	}
	/*3 step: change the prio*/
	TCB_ptr->prio = prio;
	/*4 step: register into TCB_list*/
	_register_in_TCB_list(TCB_ptr);
	/*5 step: according to the task_state,do some changes*/
	/*this place should add different code according to the different task_state*/
	switch (TCB_ptr->task_state)
	{
	case STATE_READY:
		_insert_ready_TCB(TCB_ptr); break;
	/* add other state's handling code here */
	/*
	 *
	 *
	 *
	 */
	default: break;/* do nothing*/
	}
	CPU_CRITICAL_EXIT();
	schedule();
	return 0;
}

/**
 * @Author      kaka
 * @DateTime    2019-05-02
 * @description : use this function to change the priority of the task
 * @param       TCB_ptr
 * @param       prio
 * @return      error code
 */
int task_change_prio(struct task_struct *TCB_ptr, TASK_PRIO_TYPE prio)
{
	if (unlikely(prio >= PRIO_MAX)) {
		OS_ERROR_PARA_MESSAGE_DISPLAY(task_change_prio, prio);
		return -ERROR_USELESS_INPUT;
	}
	if (unlikely(NULL == TCB_ptr)) {
		OS_ERROR_PARA_MESSAGE_DISPLAY(task_change_prio, TCB_ptr);
		return -ERROR_NULL_INPUT_PTR;
	}
	return _task_change_prio(TCB_ptr, prio);
}
EXPORT_SYMBOL(task_change_prio);

int _task_delete(struct task_struct *TCB_ptr)
{
	ASSERT(NULL != TCB_ptr, ASSERT_INPUT);
	CPU_SR_ALLOC();
	CPU_CRITICAL_ENTER();
	_delete_from_TCB_list(TCB_ptr);
	/* free the stack */
	ka_free(TCB_ptr->stack_end);
	switch (TCB_ptr->task_state)
	{
	case STATE_SUSPEND_NORMAL:
	case STATE_WAIT_MCB_FOREVER:
	case STATE_WAIT_MESSAGE_QUEUE_FOREVER:
	case STATE_PUT_MESSAGE_QUEUE_FOREVER:
	case STATE_WAIT_MUTEX_FOREVER:
		_remove_from_suspend_list(TCB_ptr); break;
	case STATE_DELAY:
	case STATE_WAIT_MCB_TIMEOUT:
	case STATE_WAIT_MESSAGE_QUEUE_TIMEOUT:
	case STATE_PUT_MESSAGE_QUEUE_TIMEOUT:
	case STATE_WAIT_MEM_POOL_TIMEOUT:
		_remove_from_delay_heap(TCB_ptr); break;
	case STATE_READY:
		_delete_TCB_from_ready(TCB_ptr); break;
	/*add other state's handling code here*/
	/*
	 *
	 *
	 *
	 */
	default:
		ASSERT(0, ASSERT_BAD_EXE_LOCATION);
		break;
	}
	if (is_module(TCB_ptr)) {
		ASSERT(TCB_ptr->dynamic_module_ptr, ASSERT_PARA_AFFIRM);
		set_module_state(TCB_ptr->dynamic_module_ptr, MODULE_STATE_LOADED);
	}
	if (TCB_ptr == OSTCBCurPtr)
		OSTCBCurPtr = NULL;
	put_task(TCB_ptr);
	CPU_CRITICAL_EXIT();
	--g_task_num;
	schedule();
	return 0;
}

/**
 * @Author      kaka
 * @DateTime    2019-05-02
 * @description : use this function to remove a task from os
 * @param       TCB_ptr    [description]
 * @return                 [description]
 */
int task_delete(struct task_struct *TCB_ptr)
{
	if (NULL == TCB_ptr)
	{
		OS_ERROR_PARA_MESSAGE_DISPLAY(task_delete, TCB_ptr);
		return -ERROR_NULL_INPUT_PTR;
	}
	return _task_delete(TCB_ptr);
}
EXPORT_SYMBOL(task_delete);

static void __INIT TCB_proc_init(void)
{
	task_info_proc_root = proc_mkdir(NULL, "task_info");
	if (IS_ERR(task_info_proc_root))
		panic("task_info proc init fail\n");
}
INIT_FUN(TCB_proc_init, 3);
