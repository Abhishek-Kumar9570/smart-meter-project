#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "virtual_meter"
#define VM_MAX_INPUT 32
static unsigned long pulse_count;
static DEFINE_MUTEX(pulse_lock);

/*
 * Read the current pulse count from the virtual meter device.
 */
static ssize_t virtual_meter_read(struct file *file,
                                  char __user *buffer,
                                  size_t length,
                                  loff_t *offset)
{
    char data[VM_MAX_INPUT];
    int data_length;

    if (*offset != 0)
        return 0;

    mutex_lock(&pulse_lock);

    data_length = scnprintf(data, sizeof(data),
                            "%lu\n", pulse_count);

    mutex_unlock(&pulse_lock);

    if (length < data_length)
        return -EINVAL;

    if (copy_to_user(buffer, data, data_length))
        return -EFAULT;

    *offset += data_length;

    return data_length;
}

/*
 * Write a new pulse count to the virtual meter device.
 */
static ssize_t virtual_meter_write(struct file *file,
                                   const char __user *buffer,
                                   size_t length,
                                   loff_t *offset)
{
    char data[VM_MAX_INPUT];
    unsigned long new_count;

    if (length == 0 || length >= sizeof(data))
        return -EINVAL;

    if (copy_from_user(data, buffer, length))
        return -EFAULT;

    data[length] = '\0';

    if (kstrtoul(data, 10, &new_count))
        return -EINVAL;

    mutex_lock(&pulse_lock);
    pulse_count = new_count;
    mutex_unlock(&pulse_lock);

    pr_info("virtual_meter: pulse count updated to %lu\n",
            new_count);

    return length;
}

static const struct file_operations virtual_meter_fops = {
    .owner = THIS_MODULE,
    .read = virtual_meter_read,
    .write = virtual_meter_write,
};

static struct miscdevice virtual_meter_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &virtual_meter_fops,
    .mode = 0666,
};

static int __init virtual_meter_init(void)
{
    int ret;

    pulse_count = 0;

    ret = misc_register(&virtual_meter_device);

    if (ret) {
        pr_err("virtual_meter: failed to register device\n");
        return ret;
    }

    pr_info("virtual_meter: driver loaded successfully\n");
    pr_info("virtual_meter: device available at /dev/%s\n",
            DEVICE_NAME);

    return 0;
}

static void __exit virtual_meter_exit(void)
{
    misc_deregister(&virtual_meter_device);

    pr_info("virtual_meter: driver unloaded\n");
}

module_init(virtual_meter_init);
module_exit(virtual_meter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhishek Kumar");
MODULE_DESCRIPTION("Virtual Smart Meter Pulse Counter Linux Character Device Driver");
MODULE_VERSION("1.0");
