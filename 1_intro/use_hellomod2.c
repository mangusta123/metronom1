/*
File hellodev1.c
A simple character device driver with file system operations
*/


#include <linux/module.h> /* Needed by all modules */
#include <linux/fs.h>

# define HELLO_MAJOR 234

static int debug_enable = 0;

module_param(debug_enable, int, 0);

MODULE_PARM_DESC(debug_enable, "Enable module debug mode.");

struct file_operations hellodev1_fops;

static int hellodev1_open(struct inode *inode, struct file *file)
{
printk("hellodev1_open: successful\n");
return 0;
}

static int hellodev1_release(struct inode *inode, struct file *file)
{
printk("hellodev1_release: successful\n");
return 0;
}

static ssize_t hellodev1_read(struct file *file, char *buf, size_t count,loff_t *ptr)
{
printk("hellodev1_read: returning zero bytes\n");
return 0;
}

static ssize_t hellodev1_write(struct file *file, const char *buf, size_t count,loff_t *ppos)
{
printk("hellodev1_write: accepting zero bytes\n");
return 0;
}

static long hellodev1_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
printk("hellodev1_ioctl: cmd=%d, arg=%ld\n", cmd, arg);
return 0;
}


static int __init hellodev1_init(void)
{
int ret;
printk("Hellodev1 Init - debug mode is %s\n", debug_enable ? "enabled" : "disabled" );
ret = register_chrdev(HELLO_MAJOR, "use_hellodev2", &hellodev1_fops);

if (ret < 0) {
printk("Error registering use_hellodev2 fops\n");
return ret;
}

printk("use_hellodev2: registered successfully!\n");

/* Init processing here ...
*/

return 0;
}

static void __exit hellodev1_exit(void)
{
unregister_chrdev(HELLO_MAJOR, "use_hellodev2");
printk("Goodbye, use_hellodev2\n");
}

struct file_operations hellodev1_fops = {
owner:       THIS_MODULE,
read:        hellodev1_read,
write:       hellodev1_write,
compat_ioctl:hellodev1_ioctl,
open:        hellodev1_open,
release:     hellodev1_release,
};

module_init(hellodev1_init);
module_exit(hellodev1_exit);
MODULE_AUTHOR("Christoper Hallinan");
MODULE_DESCRIPTION("Hello Dev 1 Example");
MODULE_LICENSE("GPL");









