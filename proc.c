// SPDX-License-Identifier: GPL-3.0
#include <linux/proc_fs.h>
#include <linux/bitmap-str.h>

#include "bit_macros.h"
//#include "func_defs.h"
#include "main.h"
#include "externs.h"

static ssize_t mask_read(struct file *file,
		char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	char buf[256];
	size_t len = 2;

	len = scnprintf(buf, 256, "Current mask is: %*pb\n", 11,
			&byte_conv_mask);

	//len += scnprintf(buf + len, sizeof(buf) - len, "Current mask is:\n%s",
			//print_buf_as_bits(&byte_conv_mask,
				//sizeof(byte_conv_mask)));

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
		BC_PR_DEBUG("User try to write to %s %zu bytes\n",
				byte_conv_proc_fname, count);
		err = -EINVAL;
		goto ERR;
	}

	if (copy_from_user(buf, ubuf, count)) {

		err = -EFAULT;
		goto ERR_FAULT;
	}

	buf[count] = '\0';

	// kstrtou8 convert string to digit
	if (kstrtou16(buf, 0, &byte_conv_mask)) {
		err = -EINVAL;
		goto ERR;
	}

	if (byte_conv_mask >= 2048) {
		err = -EINVAL;
		goto ERR;
	}

	if (((byte_conv_mask >> 6) == 0) ||
			(is_power_of_2(byte_conv_mask >> 6))) {
		err = -EINVAL;
		goto ERR;
	}

	return count;
ERR:
	pr_warn("Ivalid bit mask in %s\n", byte_conv_proc_fname);
	return err;
ERR_FAULT:
	pr_warn("Cant read user buffer\n");
	return err;
}

const struct proc_ops byte_conv_mask_ops = {
	.proc_read  = mask_read,
	.proc_write = mask_write,
};

