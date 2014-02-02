// 	File Template-gpio-led-w.c
// A simple character driver with four user LEDs in Beaglebone Ubuntu
// 	Programmed by	Byung Kook Kim, Apr. 9. 2012

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include <linux/init.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/slab.h>

// AM33xx specific register definitions
#include <plat/am33xx.h>
#include <plat/gpio.h>

#define GPIO_LED_MAJOR	0  	// Auto select major number

#define GPIO_OE_OFFSET            0xFFFFFFFF

#define GPIO_CLEARDATAOUT_OFFSET  0x190

#define GPIO_SETDATAOUT_OFFSET    0x194


// GPIO-LED File operations table
struct file_operations gpio_led_fops;

// GPIO1 usage flag
static int gpio1_usage      = 0;

// GPIO1 base virtual address 
static void __iomem * gpio1_vbase=NULL;

// GPIO1 physical address
static unsigned long gpio1_start = AM33XX_GPIO1_BASE;	// GPIO1 Start address
static unsigned long gpio1_len = 0x1000;		// Length of GPIO1 addresses

// Function gpio_write for write() call
// You need to insert your codes into this function.
static ssize_t gpio_write (struct file *filp, const char *wbuf, size_t wcount, loff_t *f_pos)
{
	unsigned int mdata;
	unsigned int oe1, oe1new;
	unsigned int dout1, dout1new;
	
        unsigned int *diraddr;
        unsigned int *clrout;
        unsigned int *setout;

        //struct omap_gpio_reg_offs* vir_base; 

       
	// Get data from app program
	get_user(mdata, wbuf);		// (To var, Src addr)

	// First set GPIO1 21 - 25 direction to output ('0')

        // 

	// Add your code: Read GPIO1 Direction register to oe1
	
        //vir_base = (struct omap_gpio_reg_offs *)gpio1_vbase;

       
        diraddr = gpio1_vbase + GPIO_OE_OFFSET;
        clrout  = gpio1_vbase + GPIO_CLEARDATAOUT_OFFSET;
        setout  = gpio1_vbase + GPIO_SETDATAOUT_OFFSET;       
 
        oe1 = *diraddr;
        
        // Add your code: Set new value of GPIO1 Direction register as oe1new.
	        
        oe1new = 0xFC1FFFFF; 

        // Add your code: Write GPIO1 Direction register
	
        *diraddr = oe1new;

        printk("GPIO1_OE: %08x to %08x\n", oe1, oe1new);

	// Then set gpio data out	
	// Add your code: Set GPIO1 Clear Register
        // in order to clear 
    
    
       switch(wbuf[0])

       {  

          case '0':
  
           *clrout = 0x1800000;   //pattern for setting LED3 and LED4 off
           *setout = 0x0;

           break;
         
          case '1':

           *clrout = 0x800000;    //pattern for setting LED3 off / LED4 on (weak)
           *setout = 0x1000000;

           break;
          
          case '3':

           *clrout = 0x1000000;   //pattern for setting LED3 on / LED4 off (medium)
           *setout = 0x800000;

           break;
         
          case '7':
  
           *clrout = 0x0;         //pattern for setting LED3 and LED4 on (strong)
           *setout = 0x1800000;

           break;
        
       }



  	return (0);
}

static ssize_t gpio_read(struct file *file, char *buf, size_t count,loff_t *ptr)
{
printk("hellodev1_read: returning zero bytes\n");
return 0;
}


// GPIO open function for open() call
static int gpio_open(struct inode *inode, struct file *filp)
{
	// Check & set GPIO1 usage flag
	if( gpio1_usage != 0 ) return -EBUSY;
  	gpio1_usage = 1;    
	
	return 0;           
}

// GPIO release function for release() call
static int gpio_release(struct inode *inode, struct file *filp)
{
	// Clear GPIO1 usage flag
	gpio1_usage = 0;             

	return 0;
}

// GPIO_LED init_module
static int __init gpio_led_init_module (void)
{
	int major, result;
    
	// 1. Register gpio_led as a character device 
	major = register_chrdev( GPIO_LED_MAJOR, "gpio-led", &gpio_led_fops );

	if (major < 0) {
		printk(KERN_WARNING "Error registering gpio_led fops %d\n", major);
		return major;
	}
	printk("Init gpio-led device: The major number is %d\n", major);


	// 2. Check memory region and then request
	result = check_mem_region(gpio1_start, gpio1_len);

	if (result < 0) {
		printk("Allocation for I/O memory range failed\n");
		return(result);
	}
	request_mem_region (gpio1_start, gpio1_len, "gpio1");

	// 3. Physical to virtual memory mapping
	gpio1_vbase = ioremap(gpio1_start, gpio1_len);

	if (!gpio1_vbase) {
		printk("Ioremap failed for GPIO1\n");
		// -2. Release memory region
		release_mem_region (gpio1_start, gpio1_len);
		// -1. Unregister gpio-led device
		unregister_chrdev( GPIO_LED_MAJOR, "gpio-led" ); 
		return(-2);
	}

	// Return success
	return 0; 
}



// GPIO-LED cleanup_module
static void __exit gpio_led_cleanup_module (void)
{
	// -3. Unmap ioaddress
	iounmap(gpio1_vbase);

	// -2. Release memory region
	release_mem_region (gpio1_start, gpio1_len);

	// -1. Unregister gpio-led device
	unregister_chrdev( GPIO_LED_MAJOR, "gpio-led" ); 
	printk("Cleanup gpio-led device: Unregistered.\n");
}


// Define file operations table for gpio-led driver
struct file_operations gpio_led_fops = 
{
	owner:		THIS_MODULE,
	open:		gpio_open,
	read:		gpio_read,
	write:		gpio_write,
	release:	gpio_release,
};


// Define module init and cleanup functions
module_init(gpio_led_init_module);
module_exit(gpio_led_cleanup_module);

MODULE_AUTHOR("Byung Kook Kim");
MODULE_DESCRIPTION("GPIO-LED Character Driver");
MODULE_LICENSE("GPL");
