#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>

/*For each probe you need to allocate a kprobe structure*/
static struct kprobe kp;

/*kprobe pre_handler: called just before the probed instruction is executed*/
int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	printk("pre_handler: p->addr=0x%p \n",p->addr);
	dump_stack();
	return 0;
}

/*kprobe post_handler: called after the probed instruction is executed*/
void handler_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	printk("post_handler: p->addr=0x%p \n",p->addr);
	dump_stack();
}

/* fault_handler: this is called if an exception is generated for any
 * instruction within the pre- or post-handler, or when Kprobes
 * single-steps the probed instruction.
 */
int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	printk("fault_handler: p->addr=0x%p, trap #%dn",
		p->addr, trapnr);
	/* Return 0 because we don't handle the fault. */
	return 0;
}

static __init int hello_init(void)
{
	int ret;
	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;
	kp.addr = (kprobe_opcode_t*) kallsyms_lookup_name("omap_gpio_get");
	/* register the kprobe now */
	if (!kp.addr) {
		printk("Couldn't find %s to plant kprobe\n", "omap_gpio_get");
		return -1;
	}
	if ((ret = register_kprobe(&kp) < 0)) {
		printk("register_kprobe failed, returned %d\n", ret);
		return -1;
	}
	printk("kprobe registered\n");
	return 0;
}

static __exit void hello_exit(void)
{
	unregister_kprobe(&kp);
	printk("kprobe unregistered\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("test");
MODULE_DESCRIPTION("KPROBE MODULE");
MODULE_LICENSE("GPL");
