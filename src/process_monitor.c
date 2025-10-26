#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kprobes.h>
#include <generated/utsrelease.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vamsee Krishna Kasani");
MODULE_DESCRIPTION("Kernel module for monitoring process termination");
MODULE_VERSION("1.0");

/* Kprobe for process exit */
static struct kprobe kp_exit = {
    .symbol_name = "do_exit",
};

/* Statistics */
static atomic_t exit_count = ATOMIC_INIT(0);

/**
 * Handler called before do_exit (process termination)
 */
static int handler_pre_exit(struct kprobe *p, struct pt_regs *regs)
{
    struct task_struct *task = current;
    
    atomic_inc(&exit_count);
    
    pr_info("PROCESS_MONITOR: Process exiting - PID: %d, Name: %s, Parent PID: %d\n",
            task->pid, task->comm, task->parent->pid);
    
    /* Log CPU time consumed */
    pr_info("PROCESS_MONITOR:   User time: %llu ns, System time: %llu ns\n",
            task->utime, task->stime);
    
    return 0;
}

/**
 * Module initialization
 */
static int __init process_monitor_init(void)
{
    int ret;
    
    pr_info("PROCESS_MONITOR: ========================================\n");
    pr_info("PROCESS_MONITOR: Module loading...\n");
    pr_info("PROCESS_MONITOR: Kernel version: %s\n", UTS_RELEASE);
    
    /* Register kprobe for process exit */
    kp_exit.pre_handler = handler_pre_exit;
    ret = register_kprobe(&kp_exit);
    if (ret < 0) {
        pr_err("PROCESS_MONITOR: Failed to register exit kprobe: %d\n", ret);
        return ret;
    }
    pr_info("PROCESS_MONITOR: Registered exit kprobe at %p\n", kp_exit.addr);
    pr_info("PROCESS_MONITOR: Module loaded successfully\n");
    pr_info("PROCESS_MONITOR: Monitoring process termination events\n");
    pr_info("PROCESS_MONITOR: ========================================\n");
    
    return 0;
}

/**
 * Module cleanup
 */
static void __exit process_monitor_exit(void)
{
    /* Unregister kprobe */
    unregister_kprobe(&kp_exit);
    
    pr_info("PROCESS_MONITOR: ========================================\n");
    pr_info("PROCESS_MONITOR: Module unloaded\n");
    pr_info("PROCESS_MONITOR: Total process exits monitored: %d\n", 
            atomic_read(&exit_count));
    pr_info("PROCESS_MONITOR: ========================================\n");
}

module_init(process_monitor_init);
module_exit(process_monitor_exit);