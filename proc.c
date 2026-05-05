// SPDX-License-Identifier: GPL-3.0
#include <linux/proc_fs.h>
#include <linux/bitmap-str.h>

#include "bit_macros.h"
#include "main.h"
#include "externs.h"

static const char * const input_mods_strings[] = {
	"Input mode is: HEX",
	"Input mode is: DEC",
	"Input mode is: OCTAL",
	"Input mode is: BIN",
	"Input mode is: RAW (Default)",
};

static ssize_t mask_read(struct file *file,
		char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	char buf[256];
	size_t len;

	if (byte_conv_mask >= 2048)
		goto ERR;

	if (!is_power_of_2(byte_conv_mask >> 6))
		goto ERR;

	len = scnprintf(buf, sizeof(buf), "Current mask is: %16pb\n\n",
			&byte_conv_mask);

	len += scnprintf(buf + len, sizeof(buf) - len, "%s\n",
			input_mods_strings[ffs(byte_conv_mask >> 6) - 1]);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
ERR:
	len = scnprintf(buf, sizeof(buf), "Current mask is: %pbl\n\n",
			&byte_conv_mask);
	len += scnprintf(buf + len, sizeof(buf) - len, "Invalid mask\n");

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t mask_write(struct file *file,
		const char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	u16 tmp_mask;
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

	if (kstrtou16(buf, 0, &tmp_mask)) {
		err = -EINVAL;
		goto ERR;
	}

	if (tmp_mask >= 2048) {
		err = -EINVAL;
		goto ERR;
	}

	if (!is_power_of_2(tmp_mask >> 6)) {
		err = -EINVAL;
		goto ERR;
	}

	byte_conv_mask = tmp_mask;
	return count;
ERR:
	pr_warn("Invalid bit mask in %s\n", byte_conv_proc_fname);
	return err;
ERR_FAULT:
	pr_warn("Can't read user buffer\n");
	return err;
}

const struct proc_ops byte_conv_mask_ops = {
	.proc_read  = mask_read,
	.proc_write = mask_write,
};

