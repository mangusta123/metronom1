// 	File Template-gpio-led-w.c
// A simple character driver with four user LEDs in Beaglebone Ubuntu


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
#include <linux/interrupt.h>


// AM33xx specific register definitions
#include <plat/am33xx.h>
#include <plat/gpio.h>
#include <plat/dmtimer.h>


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

int    stopped  = 1;

int    tempoval = 100;


int    loopnum = 12;

int    P[] = {7, 0, 1, 0, 1, 0, 3, 0, 1, 0, 1, 0};  //default


int    curr = 0;


struct omap_dm_timer *timer_ptr;
                 int  timer_irq;

static irqreturn_t timer_irq_handler(int, void *, struct pt_regs *);

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

//do something with curr depending on pattern and sleepvalue

        
         



  	return (0);
}

static ssize_t gpio_read(struct file *file, char *buf, size_t count,loff_t *ptr)
{
printk("hellodev1_read: returning zero bytes\n");
return 0;
}


/////////////////////////////////////////////////////////////////////////
//////the following three static functions were copied from dmtimer.c in order
//////to keep dmtimer.c intact (we could simply remove keyword 'static' from their declarations)
/////////////////////////////////////////////////////////////////////////

static void omap_dm_timer_write_reg(struct omap_dm_timer *timer, u32 reg,
						u32 value)
{
	WARN_ON((reg & 0xff) < _OMAP_TIMER_WAKEUP_EN_OFFSET);
	__omap_dm_timer_write(timer, reg, value, timer->posted);
}

static void omap_dm_timer_wait_for_reset(struct omap_dm_timer *timer)
{
	int c;

	if (!timer->sys_stat)
		return;

	c = 0;
	while (!(__raw_readl(timer->sys_stat) & 1)) {
		c++;
		if (c > 100000) {
			printk(KERN_ERR "Timer failed to reset\n");
			return;
		}
	}
}


static void omap_dm_timer_reset(struct omap_dm_timer *timer)
{
	omap_dm_timer_enable(timer);
	if (timer->pdev->id != 1) {
		omap_dm_timer_write_reg(timer, OMAP_TIMER_IF_CTRL_REG, 0x06);
		omap_dm_timer_wait_for_reset(timer);
	}

	__omap_dm_timer_reset(timer, 0, 0);
	omap_dm_timer_disable(timer);
	timer->posted = 1;
}

////////////////////////////////////////////////////////////////////////////

// GPIO open function for open() call
static int gpio_open(struct inode *inode, struct file *filp)
{

        unsigned int *diraddr;

                 int  ret;
          struct clk *gt_fclk; 
       unsigned long  gt_rate;


	// Check & set GPIO1 usage flag
	if( gpio1_usage != 0 ) return -EBUSY;
  	gpio1_usage = 1;    

        
//set GPIO1 LED direction

        diraddr = gpio1_vbase + GPIO_OE_OFFSET;
        
       *diraddr = 0xFC1FFFFF; 

        
        printk("direction has been set\n");
       

//request DMTimer

        timer_ptr = omap_dm_timer_request();

        
        printk("DMTimer has been requested\n");

    if(!timer_ptr)
        printk("DMTimer has not been allocated!!!!\n");

        


//set DMTimer clock source and prescale

        omap_dm_timer_set_source(timer_ptr, OMAP_TIMER_SRC_SYS_CLK);

        omap_dm_timer_set_prescaler(timer_ptr, 0);


        printk("DMTimer clock source and prescale have been set\n");

//set DMTimer ISR

        timer_irq = omap_dm_timer_get_irq(timer_ptr);

              ret = request_irq(timer_irq, timer_irq_handler, IRQF_DISABLED | IRQF_TIMER , "gptimer", timer_irq_handler);
         

        printk("DMTimer ISR has been set\n");

//set DMTimer preload and autoreload
       
        gt_fclk = omap_dm_timer_get_fclk(timer_ptr);        

        gt_rate = clk_get_rate(gt_fclk);

        omap_dm_timer_set_load(timer_ptr, 1, 0xFFFFFFFF - (3 * gt_rate));

        
        printk("DMTimer preload and autoreload have been set\n");

//interrupt enable DMTimer

        omap_dm_timer_set_int_enable(timer_ptr, OMAP_TIMER_INT_OVERFLOW);

        printk("DMTimer interrupt has been enabled\n");


	return 0;           
}


// GPIO release function for release() call
static int gpio_release(struct inode *inode, struct file *filp)
{
	// Clear GPIO1 usage flag
	gpio1_usage = 0;             

if(!stopped)
        omap_dm_timer_stop(timer_ptr);

        free_irq(timer_irq, timer_irq_handler);

//release timer here

        omap_dm_timer_free(timer_ptr);        

	return 0;
}


//interrupt handler for DMTimer

static irqreturn_t timer_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
    
        unsigned int *clrout;
        unsigned int *setout;

        clrout  = gpio1_vbase + GPIO_CLEARDATAOUT_OFFSET;
        setout  = gpio1_vbase + GPIO_SETDATAOUT_OFFSET;        
         

//reset timer interrupt status

        omap_dm_timer_write_status(timer_ptr, OMAP_TIMER_INT_OVERFLOW);
	omap_dm_timer_read_status(timer_ptr);

//print message
      
        printk("interrupt occured!\n");

//output LED pattern

                    
      switch(P[curr])

       {  

          case 0:
  
           *clrout = 0x1800000;   //pattern for setting LED3 and LED4 off
           *setout = 0x0;

           break;
         
          case 1:

           *clrout = 0x800000;    //pattern for setting LED3 off / LED4 on (weak)
           *setout = 0x1000000;

           break;
          
          case 3:

           *clrout = 0x1000000;   //pattern for setting LED3 on / LED4 off (medium)
           *setout = 0x800000;

           break;
         
          case 7:
  
           *clrout = 0x0;         //pattern for setting LED3 and LED4 on (strong)
           *setout = 0x1800000;

           break;
        
       }  

       if ((loopnum==4 && curr==3)||
           (loopnum==6 && curr==5)||         
           (loopnum==8 && curr==7)|| 
           (loopnum==12&& curr==11))

            curr = 0;
     else

            curr++;

 

    return IRQ_HANDLED;

}


//GPIO ioctl function
int gpio_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{

          struct clk *gt_fclk; 
       unsigned long  gt_rate;

  
  printk("ioctl_num = %d \n", ioctl_num);

         
        if(ioctl_num < 200)

             {
               printk("user entered start !!! \n");

               tempoval = ioctl_num;

               gt_fclk = omap_dm_timer_get_fclk(timer_ptr);        

               gt_rate = clk_get_rate(gt_fclk);
              
               omap_dm_timer_set_load(timer_ptr, 1, 0xFFFFFFFF - ((20 * gt_rate)/tempoval));                 
 

//there are supposed to be tempo/20 interrupts per sec

//or interrupt should occur every 20/tempo seconds


               omap_dm_timer_start(timer_ptr);  
             
               stopped = 0; 


             }
                            
else if(ioctl_num > 200 && ioctl_num < 300)
           
             {
                  printk("user entered time-signature !!! \n"); 

                  if(ioctl_num-200==24)
                     {

                      loopnum = 4;
                      curr    = 0;

                      P[0] = 7;
                      P[1] = 0;
                      P[2] = 1;
                      P[3] = 0;                               
             
                     }

             else if(ioctl_num-200==34)
                     {
 
                     loopnum = 6;
                     curr    = 0; 

                     P[0] = 7;
                     P[1] = 0;
                     P[2] = 1;
                     P[3] = 0;
                     P[4] = 1;
                     P[5] = 0;

                     }

             else if(ioctl_num-200==44)       
                     {
        
                     loopnum = 8;
                     curr    = 0;        

                     P[0] = 7;
                     P[1] = 0;
                     P[2] = 1;
                     P[3] = 0;
                     P[4] = 3;
                     P[5] = 0;
                     P[6] = 1;
                     P[7] = 0;

                     }
             else if(ioctl_num-200==68) 
                     {

                     loopnum = 12;
                     curr    = 0;                 

                     P[0] = 7;
                     P[1] = 0;
                     P[2] = 1;
                     P[3] = 0;
                     P[4] = 1;
                     P[5] = 0;                     
                     P[6] = 3;
                     P[7] = 0;
                     P[8] = 1;
                     P[9] = 0;
                    P[10] = 1;
                    P[11] = 0;

                     }
                     
              }

     else if(ioctl_num==300)
  
              {

               printk("user entered stop !!! \n");

               omap_dm_timer_stop(timer_ptr);
              
               stopped = 1;
                
              }

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
        unlocked_ioctl: gpio_ioctl,
	release:	gpio_release,
};


// Define module init and cleanup functions
module_init(gpio_led_init_module);
module_exit(gpio_led_cleanup_module);

MODULE_AUTHOR("Byung Kook Kim");
MODULE_DESCRIPTION("GPIO-LED Character Driver");
MODULE_LICENSE("GPL");
