#include <linux/module.h>
#include <linux/fs.h>      // File operations
#include <linux/uaccess.h> // copy_to/from_user
#include <linux/proc_fs.h>

#include "bit_macros.h"

#define MAIN_UBUF_SIZE 4096
#define UBUF_REPRESENT_SIZE ((MAIN_UBUF_SIZE * 8 + 5) + \
	(MAIN_UBUF_SIZE / 4)) // the broadest representation BITS representation

extern const struct proc_ops mask_ops;
extern u16 conversion_mask;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IvanRia");
MODULE_DESCRIPTION("Char Device Converter from/to ascii, bin, octal, hex");

struct proc_dir_entry *entry;

static int __init converter_init(void)
{
	conversion_mask = 0x420; // Set mask to "Input: RAW, Outpu ASCII"

	pr_info("Char Device Converter module is loaded\n");
	entry = proc_create("byte_conv_mask", 0666, NULL, &mask_ops);
	if (!entry) {
		pr_warn("Not enough memory\n");
		return -ENOMEM;
	}
	return 0;
}

static void __exit converter_exit(void)
{
	pr_info("Char Device Converter module is unloaded\n");
}

// write callback (echo "..." > /dev/byte_converter)
static ssize_t dev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    return count;
}

// read callback (cat /dev/byte_converter)
static ssize_t dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    // Тут смотрим маску из /proc
    // Форматируем данные в строку
    // Используем copy_to_user(buf, formatted_string, len);
    return count; // Сколько байт реально отдали пользователю
}

// open/close callback
static int dev_open(struct inode *inode, struct file *file) {
    return 0;
}

static char *print_buf_as_bits(const void *buf, const size_t size)
{


static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .read    = dev_read,
    .write   = dev_write,
    .open    = dev_open,
    // .release = dev_release,
};

module_init(converter_init);
module_exit(converter_exit);

