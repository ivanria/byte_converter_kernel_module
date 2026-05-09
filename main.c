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
/*
struct dev_context {
	char *buffer;
	size_t data_size;
};
*/

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

// Global visible variables
const char * const byte_conv_dev_fname = "byte_converter";
const char * const byte_conv_proc_fname = "byte_conv_mask";
u16 byte_conv_mask;

/*******************************************/

// Static visible variables
static struct class *byte_conv_class;
static struct cdev byte_conv_cdev;
static struct device *byte_conv_device;
static dev_t dev_num;
static struct proc_dir_entry *entry;

static const struct file_operations fops = {
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

