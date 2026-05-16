// SPDX-License-Identifier: GPL-3.0
#include <linux/module.h>
#include <linux/uaccess.h> // copy_to/from_user
#include <linux/fs.h>      // File operations
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include "bit_macros.h"
#include "main.h"
#include "externs.h"
#include "conv_helpers.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IvanRia");
MODULE_DESCRIPTION("Char Device Converter from/to ascii, bin, octal, hex");

// Global visible variables
const char * const byte_conv_dev_fname = "byte_converter";
const char * const byte_conv_proc_fname = "byte_conv_mask";
u16 byte_conv_mask;
spinlock_t byte_conv_mask_lock;

/*******************************************/

// Static visible variables
static struct class *byte_conv_class;
static struct cdev byte_conv_cdev;
static struct device *byte_conv_device;
static dev_t dev_num;
static struct proc_dir_entry *entry;
static struct byte_conv_user_data user_data;

/*******************************************/

// write callback (echo "..." > /dev/byte_converter)
static ssize_t dev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	size_t readed;

	if (count == 0) return 0;
	if (count > INPUT_USER_BUF_SIZE) count = INPUT_USER_BUF_SIZE;
	readed = copy_from_user(user_data.user_data_buf, buf, count);
	user_data.user_data_size = readed;
	// validating input with input bit mask, i.e.: if INPUT in HEX, then
	// check if input (not 0-9 && a-f, A-F)
	// call copy_from_user(kernel_buffer, buf, count);
	return readed; // total converted and writed bytes
}

// read callback (cat /dev/byte_converter)
static ssize_t dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	u16 mask;
	unsigned long flags;
	size_t total_size = 0, input_size = user_data.user_data_size;
	char *kern_buf = NULL, *p = NULL, *input_p = user_data.user_data_buf;
	//char inval_mask_bit_buf[OUT_BUF_SIZE_BIN(sizeof(byte_conv_mask))];
	conv_func_t tasks[7], *curr_task;

	spin_lock_irqsave(&byte_conv_mask_lock, flags);
	mask = byte_conv_mask;
	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);

	if (input_size == 0) {
		pr_warn("user write to /dev/byte_conv NULL bytes\n");
		return 0;
	}

	if (input_size > INPUT_USER_BUF_SIZE) {
		pr_warn("User data size greater than %i, reduce to %i\n",
				(int)INPUT_USER_BUF_SIZE,
				(int)INPUT_USER_BUF_SIZE);
		input_size = INPUT_USER_BUF_SIZE;
	}

	// may be redundancy caution,
	// mask validating is done during write mask
	/*
	if (!is_power_of_2(mask & 0x3f)) {
		print_conv_bin(inval_mask_bit_buf, &mask, sizeof(mask));
		pr_warn("invalid mask\n%s", inval_mask_bit_buf);
		return 0;
	}
	*/

	if (IS_SET_OUTPUT_RAW(mask)) {
		total_size += OUT_BUF_SIZE_RAW(input_size);
		*curr_task++ = print_conv_raw;
	}
	if (IS_SET_OUTPUT_ASCII(mask)) {
		total_size += OUT_BUF_SIZE_ASCII(input_size);
		*curr_task++ = print_conv_ascii;
	}
	if (IS_SET_OUTPUT_BIN(mask)) {
		total_size += OUT_BUF_SIZE_BIN(input_size);
		*curr_task++ = print_conv_bin;
	}
	if (IS_SET_OUTPUT_OCTAL(mask)) {
		total_size += OUT_BUF_SIZE_OCTAL(input_size);
		*curr_task++ = print_conv_octal;
	}
	if (IS_SET_OUTPUT_DEC(mask)) {
		total_size += OUT_BUF_SIZE_DEC(input_size);
		*curr_task++ = print_conv_dec;
	}
	if (IS_SET_OUTPUT_HEX(mask)) {
		total_size += OUT_BUF_SIZE_HEX(input_size);
		*curr_task++ = print_conv_hex;
	}
	*curr_task = NULL;
	kern_buf = kmalloc(total_size, GFP_KERNEL);
	if (!kern_buf) return -ENOMEM;

	for(p = kern_buf, curr_task = tasks; *curr_task;)
		p += (*curr_task++)(p, input_p, input_size);

	copy_to_user(buf, kern_buf, total_size);
	kfree(kern_buf);
	return total_size;
}

static int dev_open(struct inode *inode, struct file *file)
{
	/*
	struct dev_context *ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx) return -ENOMEM;

	ctx->buffer = kmalloc(MAIN_UBUF_SIZE, GFP_KERNEL);
	if (!ctx->buffer) {
		kfree(ctx);
		return -ENOMEM;
	}

	file->private_data = ctx; // Привязываем контекст к файлу
	*/
	return 0;
}
// open/close callback
/*
struct dev_context {
	char *buffer;
	size_t data_size;
};
*/

static const struct file_operations fops = {
	.owner   = THIS_MODULE,
	.read    = dev_read,
	.write   = dev_write,
	.open    = dev_open,
	// .release = dev_release,
};

static int __init converter_init(void)
{
	int ret;

	pr_info("Char Device Converter module is loaded\n");

	byte_conv_mask = 0x40f; // Set mask to "Input: RAW, Outpu ASCII"

	ret = alloc_chrdev_region(&dev_num, 0, 1, byte_conv_dev_fname);
	if (ret < 0) {
		pr_warn("Can't alloc_chrdev_region\n");
		return ret;
	}

	byte_conv_class = class_create(byte_conv_dev_fname);
	if (IS_ERR(byte_conv_class)) {
		pr_warn("Can't create /dev/%s file\n", byte_conv_dev_fname);
		ret = PTR_ERR(byte_conv_class);
		goto ERR_UNREG_REGION;
	}

	cdev_init(&byte_conv_cdev, &fops);
	byte_conv_cdev.owner = THIS_MODULE;
	ret = cdev_add(&byte_conv_cdev, dev_num, 1);
	if (ret < 0) {
		pr_warn("Can't cdev_add() dev_num MAJ: %u, MIN: %u\n",
				MAJOR(dev_num), MINOR(dev_num));
		goto ERR_DESTROY_CLASS;
	}

	byte_conv_device = device_create(byte_conv_class, NULL, dev_num, NULL,
			"%s", byte_conv_dev_fname);
	if (IS_ERR(byte_conv_device)) {
		pr_warn("Can't create /dev/%s file\n", byte_conv_dev_fname);
		goto ERR_CDEV_DEL;
	}

	entry = proc_create(byte_conv_proc_fname,
			0666, NULL, &byte_conv_mask_ops);
	if (!entry) {
		pr_warn("Can't create /proc/%s file\n", byte_conv_proc_fname);
		ret = -ENOMEM;
		goto ERR_DEVICE_DESTROY;
	}

	BC_PR_DEBUG("Major num is: %u, Minor num is: %u\n",
			MAJOR(dev_num), MINOR(dev_num));
	return 0;

ERR_DEVICE_DESTROY:
    device_destroy(byte_conv_class, dev_num);
ERR_CDEV_DEL:
    cdev_del(&byte_conv_cdev);
ERR_DESTROY_CLASS:
    class_destroy(byte_conv_class);
ERR_UNREG_REGION:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit converter_exit(void)
{
	remove_proc_entry(byte_conv_proc_fname, NULL);
	device_destroy(byte_conv_class, dev_num);
	cdev_del(&byte_conv_cdev);
	class_destroy(byte_conv_class);
	unregister_chrdev_region(dev_num, 1);
	pr_info("Char Device Converter module is unloaded\n");
}

module_init(converter_init);
module_exit(converter_exit);

