/* hello.c lkm */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

static int __init setup(void) {
    pr_info("Our LKM successfully loaded.");

    return 0;

}

static void __exit unload(void) {

    pr_info("LKM unloaded.");

    return;
}

module_init(setup);
module_exit(unload);

MODULE_LICENSE("GPL");
