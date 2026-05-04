// SPDX-License-Identifier: GPL-3.0
#include <linux/module.h>
#include <linux/fs.h>      // File operations
#include <linux/uaccess.h> // copy_to/from_user
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include "bit_macros.h"
#include "main.h"
#include "externs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IvanRia");
MODULE_DESCRIPTION("Char Device Converter from/to ascii, bin, octal, hex");

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


// Global visible variables
char *byte_conv_dev_fname = "byte_converter";
char *byte_conv_proc_fname = "byte_conv_mask";
u16 byte_conv_mask;
/*******************************************/


// Static visible variables
static struct class *byte_conv_class;
static struct cdev byte_conv_cdev;
static struct device *byte_conv_device;
static dev_t dev_num;
static struct proc_dir_entry *entry;

static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.read    = dev_read,
	.write   = dev_write,
	.open    = dev_open,
	// .release = dev_release,
};
/*******************************************/


static int __init converter_init(void)
{
	int ret;


	pr_info("Char Device Converter module is loaded\n");

	byte_conv_mask = 0x420; // Set mask to "Input: RAW, Outpu ASCII"

	ret = alloc_chrdev_region(&dev_num, 0, 1, byte_conv_dev_fname);
	if (ret < 0) {
		pr_warn("Can't create /dev/%s file\n", byte_conv_dev_fname);
		return ret;
	}

	BC_PR_DEBUG("Major num is: %u, Minor num is: %u\n",
			MAJOR(dev_num), MINOR(dev_num));

	byte_conv_class = class_create(byte_conv_dev_fname);
	if (IS_ERR(byte_conv_class)) {
		unregister_chrdev_region(dev_num, 1);
		pr_warn("Can't create /dev/%s file\n", byte_conv_dev_fname);
		return PTR_ERR(byte_conv_class);
	}

	cdev_init(&byte_conv_cdev, &fops);
	byte_conv_cdev.owner = THIS_MODULE;
	cdev_add(&byte_conv_cdev, dev_num, 1);

	byte_conv_device = device_create(byte_conv_class, NULL, dev_num, NULL,
			"%s", byte_conv_dev_fname);
	if (IS_ERR(byte_conv_device)) {
		class_destroy(byte_conv_class);
		unregister_chrdev_region(dev_num, 1);
		pr_warn("Can't create /dev/%s file\n", byte_conv_dev_fname);
		return PTR_ERR(byte_conv_device);
	}

	entry = proc_create(byte_conv_proc_fname, 0666, NULL,
			&byte_conv_mask_ops);
	if (!entry) {
		pr_warn("Can't create /proc/%s file\n", byte_conv_proc_fname);
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
	remove_proc_entry(byte_conv_proc_fname, NULL);
	pr_info("Char Device Converter module is unloaded\n");
}

module_init(converter_init);
module_exit(converter_exit);

