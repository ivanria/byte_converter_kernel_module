// SPDX-License-Identifier: GPL-3.0
#include <linux/proc_fs.h>
#include <linux/bitmap-str.h>

#include "bit_macros.h"
#include "main.h"
#include "externs.h"
#include "conv_helpers.h"

static const char * const input_mods_strings[] = {
	"Input mode is: HEX",
	"Input mode is: DEC",
	"Input mode is: OCTAL",
	"Input mode is: BIN",
	"Input mode is: RAW (Default)", // 29 bytes
};

static ssize_t mask_read(struct file *file,
		char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	char mask_buf[sizeof(byte_conv_mask)];
	char bit_str_mask[OUT_BUF_SIZE_BIN(sizeof(byte_conv_mask))];
	char buf[256];
	unsigned long flags;
	size_t len = 0, bin_len;

	spin_lock_irqsave(&byte_conv_mask_lock, flags);

	if (byte_conv_mask >= 2048)
		goto ERR;

	if (!is_power_of_2(byte_conv_mask >> 6))
		goto ERR;

	mask_buf[0] = (char)((byte_conv_mask >> 8) & 0x0f);
	mask_buf[1] = (char)(byte_conv_mask & 0x0f);

	bin_len = print_conv_bin(bit_str_mask, mask_buf,
			sizeof(byte_conv_mask));
	BC_PR_DEBUG("mask bits is: %s", bit_str_mask);
	len += scnprintf(buf + len, sizeof(buf) - len, "Current mask is: ");
	len += scnprintf(buf + len, bin_len, "%s", bit_str_mask);

	// Any power of two cannot be zero. [ffs(....) - 1] is safely
	len += scnprintf(buf + len, sizeof(buf) - len, "%s\n",
			input_mods_strings[ffs(byte_conv_mask >> 6) - 1]);

	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);
	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
ERR:
	mask_buf[0] = (char)((byte_conv_mask >> 8) & 0x0f);
	mask_buf[1] = (char)(byte_conv_mask & 0x0f);

	bin_len = print_conv_bin(bit_str_mask, mask_buf,
			sizeof(byte_conv_mask));
	BC_PR_DEBUG("mask bits is: %s", bit_str_mask);
	len = scnprintf(buf, sizeof(buf), "Mask invalid\n");

	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);
	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t mask_write(struct file *file,
		const char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	u16 tmp_mask;
	unsigned long flags;
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

	spin_lock_irqsave(&byte_conv_mask_lock, flags);

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
	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);
	return count;
ERR:
	pr_warn("Invalid bit mask in %s\n", byte_conv_proc_fname);
	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);
	return err;
ERR_FAULT:
	pr_warn("Can't read user buffer\n");
	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);
	return err;
}

const struct proc_ops byte_conv_mask_ops = {
	.proc_read  = mask_read,
	.proc_write = mask_write,
};

