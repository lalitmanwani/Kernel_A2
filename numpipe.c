#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/sched.h>
MODULE_LICENSE("DUAL BSD/GPL");

static struct miscdevice numpipe;

static int N;

static int charcnt = 100;

static int Device_open;

static struct semaphore full;
static struct semaphore empty;
static struct semaphore rmutex;
static struct semaphore wmutex;


module_param(N, int, 0000);


struct pipe{
char** buff;
int bes;
int rp; 
int wp;
};

struct pipe p;



static int dev_open(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);
static int dev_release(struct inode*, struct file*);


static struct file_operations fops = {
	.open = &dev_open,
	.read = &dev_read,
	.write = &dev_write,
	.release = &dev_release
};



static int dev_open(struct inode* _inode, struct file* _file){

	++Device_open;
	return 0;
}



int init_module(){

	numpipe.name = "numpipe";
	numpipe.minor = MISC_DYNAMIC_MINOR;
	numpipe.fops = &fops;
	

	int register_return_value;
	if((register_return_value = misc_register(&numpipe))){
		printk(KERN_ERR "Could not register the device\n");
		return register_return_value;
	}
		printk(KERN_ALERT "Character device Registered!");	

	int i;
	p.buff = (char**)kmalloc(N*sizeof(char*), GFP_KERNEL);
	for(i = 0;i < N;++i){
		p.buff[i] = (char*)kmalloc((charcnt+1)*sizeof(char), GFP_KERNEL);
		p.buff[charcnt] = '\0';
		
	}


	sema_init(&full, 0);
	sema_init(&empty, N);
	sema_init(&rmutex, 1);
	sema_init(&wmutex, 1);

	p.bes = N;

	Device_open = 0;
	return 0;
}


void cleanup_module(){

	int i = 0;
	while(i < N){
		kfree(p.buff[i]);
	i++;	
	}
	kfree(p.buff);
	misc_deregister(&numpipe);
	printk(KERN_ALERT "Character device de-registered! Buffer removed");
}




static ssize_t dev_read(struct file* _file, char* ubuff, size_t len, loff_t* offset){
	int i = 0;
	
	down_interruptible(&rmutex);
	down_interruptible(&full);

	p.rp = p.rp % N;
	while(i < len){
		if(p.bes < N)
		{	
			copy_to_user(&ubuff[i], &p.buff[p.rp][i], 1);
		}
		else
		{
		break;		
		}
	i++;	
	}
	++p.rp;
	++p.bes;
	up(&empty);
	up(&rmutex);
	return i;
}



static int dev_release(struct inode* _inode, struct file* _file){

	--Device_open;
	return 0;
}



static ssize_t dev_write(struct file* _file, const char* ubuff, size_t len, loff_t* offset){
	
	int i = 0;

	down_interruptible(&wmutex);

	down_interruptible(&empty);

	p.wp = p.wp % N;
	while(i < len){
	if(p.bes > 0)
	{
	copy_from_user(&p.buff[p.wp][i], &ubuff[i], 1);
	}
	else
	{
		break;
	}
	i++;	
	}
	++p.wp;
	--p.bes;
	up(&full);
	up(&wmutex);
	return i;
}

