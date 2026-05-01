#include <linux/proc_fs.h>

#include "bit_macros.h"

u16 conversion_mask;

static ssize_t mask_read(struct file *file,
		char __user *ubuf,
		size_t count,
		loff_t *ppos) 
{
	char buf[4096];
	size_t len = 0;

	len += scnprintf(buf + len, sizeof(buf) - len, "Current mask is:\n%s",
			print_buf_as_bits(&conversion_mask,
				sizeof(conversion_mask)));

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t mask_write(struct file *file,
		const char __user *ubuf,
		size_t count,
		loff_t *ppos) 
{
	ssize_t err = 0;
	char buf[8];
	if (count > sizeof(buf) - 1) {
		err = -EINVAL;
		goto ERR;
	}
	if (copy_from_user(buf, ubuf, count)) {
		err = -EFAULT;
		goto ERR_FAULT;
	}

	buf[count] = '\0';

	// kstrtou8 convert string to digit
	if (kstrtou16(buf, 0, &conversion_mask)) {
		err = -EINVAL;
		goto ERR;
	}

	if (conversion_mask >= 2048) {
		err = -EINVAL;
		goto ERR;
	}

	if (hweight16(conversion_mask >> 6) > 1) {
		err = -EINVAL;
		goto ERR;
	}

	return count;
ERR:
	pr_warn("Ivalid bit mask in /proc/byte_conv_mask\n");
	return err;
ERR_FAULT:
	pr_warn("Cant read user buffer\n");
	return err;
}

const struct proc_ops mask_ops = {
	.proc_read  = mask_read,
	.proc_write = mask_write,
};

