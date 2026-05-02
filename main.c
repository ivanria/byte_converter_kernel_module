// SPDX-License-Identifier: GPL-3.0
#include <linux/module.h>
#include <linux/fs.h>      // File operations
#include <linux/uaccess.h> // copy_to/from_user
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include "bit_macros.h"
#include "func_defs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IvanRia");
MODULE_DESCRIPTION("Char Device Converter from/to ascii, bin, octal, hex");

static char *dev_file_name = "byte_converter";
static char *proc_file_name = "byte_conv_mask";

extern const struct proc_ops mask_ops;
extern u16 conversion_mask;

static int major;
static struct class *byte_conv_class;
static struct cdev byte_conv_cdev;
static struct device *byte_conv_device;
static dev_t dev_num;

struct proc_dir_entry *entry;


static const struct file_operations fops = {
	.owner   = THIS_MODULE,
	.read    = dev_read,
	.write   = dev_write,
	.open    = dev_open,
	// .release = dev_release,
};

static int __init converter_init(void)
{
	conversion_mask = 0x420; // Set mask to "Input: RAW, Outpu ASCII"

	pr_info("Char Device Converter module is loaded\n");

	major = alloc_chrdev_region(&dev_num, 0, 1, dev_file_name);
	if (major < 0) {
		pr_warn("Can't create /dev/%s file\n", dev_file_name);
		return major;
	}

	pr_warn("dev_num is: %u, major is: %i\n", dev_num, major);

	byte_conv_class = class_create(dev_file_name);
	if (IS_ERR(byte_conv_class)) {
		unregister_chrdev_region(dev_num, 1);
		pr_warn("Can't create /dev/%s file\n", dev_file_name);
		return PTR_ERR(byte_conv_class);
	}

	cdev_init(&byte_conv_cdev, &fops);
	byte_conv_cdev.owner = THIS_MODULE;
	cdev_add(&byte_conv_cdev, dev_num, 1);

	byte_conv_device = device_create(byte_conv_class, NULL, dev_num, NULL,
			"%s", dev_file_name);
	if (IS_ERR(byte_conv_device)) {
		class_destroy(byte_conv_class);
		unregister_chrdev_region(dev_num, 1);
		pr_warn("Can't create /dev/%s file\n", dev_file_name);
		return PTR_ERR(byte_conv_device);
	}

	entry = proc_create(proc_file_name, 0666, NULL, &mask_ops);
	if (!entry) {
		pr_warn("Can't create /proc/%s file\n", proc_file_name);
		return -ENOMEM;
	}

	return 0;
}

static void __exit converter_exit(void)
{
	device_destroy(byte_conv_class, MKDEV(MAJOR(dev_num), MINOR(dev_num)));
	cdev_del(&byte_conv_cdev);
	class_unregister(byte_conv_class);
	class_destroy(byte_conv_class);
	unregister_chrdev_region(dev_num, 1);
	remove_proc_entry(proc_file_name, NULL);
	pr_info("Char Device Converter module is unloaded\n");
}

// write callback (echo "..." > /dev/byte_converter)
static ssize_t dev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	// Тут выделяем память (или используем готовую)
	// Используем copy_from_user(kernel_buffer, buf, count);
	return count;
}

// read callback (cat /dev/byte_converter)
static ssize_t dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	// Тут смотрим маску из /proc
	// Форматируем данные в строку
	// Используем copy_to_user(buf, formatted_string, len);
	return count; // Сколько байт реально отдали пользователю
}

// open/close callback
static int dev_open(struct inode *inode, struct file *file)
{
	return 0;
}

//static char *print_buf_as_bits(char *represent_buf, const size_t size)
//{



module_init(converter_init);
module_exit(converter_exit);

