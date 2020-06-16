#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>
#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static long cupid;

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
        .write = proc_write,
};
/* This function is called when the module is loaded. */
static int proc_init(void)
{
    /* creates the /proc/hello entry */
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    return 0;
}
/* This function is called when the module is removed. */
static void proc_exit(void)
{
    /* removes the /proc/hello entry */
    remove_proc_entry(PROC_NAME, NULL);
}

// write
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char *k_mem;
    /* allocate kernel memory */
    k_mem = kmalloc(count, GFP_KERNEL);
    /* copies user space usr buf to kernel memory */
    copy_from_user(k_mem, usr_buf, count);
    /* return kernel memory */
    kstrtol(k_mem, 10, &cupid);
    kfree(k_mem);
    return count;
}

// read
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    struct task_struct *Task_struct;

    if (completed) {
        completed = 0;
        return 0;
    }
    Task_struct = pid_task(find_vpid(cupid), PIDTYPE_PID);
    if (Task_struct == NULL)
        rv = sprintf(buffer, "%d\n", 0);
    else
        rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%ld]\n", Task_struct -> comm,  Task_struct -> pid, Task_struct -> state);
    /* copies kernel space buffer to user space usr buf */
    copy_to_user(usr_buf, buffer, rv);
    completed = 1;
    return rv;
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Proj2");
MODULE_AUTHOR("Xichen Pan");
