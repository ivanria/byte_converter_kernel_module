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
	"Input mode is: OCT",
	"Input mode is: BIN",
	"Input mode is: RAW",
};

static ssize_t ret;

static ssize_t mask_read(struct file *file,
		char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	u32 be_mask;
	char bit_str_mask[OUT_BUF_SIZE_BIN(sizeof(byte_conv_mask))];
	char buf[512];
	unsigned long flags;
	size_t len = 0, bin_len;

	spin_lock_irqsave(&byte_conv_mask_lock, flags);

	if ((byte_conv_mask & 0x3f) == 0)
		goto ERR;

	if (!is_power_of_2(((byte_conv_mask >> 10) & 0x1f)))
		goto ERR;

	be_mask = cpu_to_be32(byte_conv_mask);

	bin_len = print_conv_bin(bit_str_mask, (char *)&be_mask,
			sizeof(be_mask));
	len += scnprintf(buf + len, sizeof(buf) - len, "Current mask is: ");
	len += scnprintf(buf + len, bin_len, "%s", bit_str_mask);

	// Any power of two cannot be zero. [ffs(....) - 1] is safely
	len += scnprintf(buf + len, sizeof(buf) - len, "%s\n",
			input_mods_strings[ffs(byte_conv_mask >> 10) - 1]);

	spin_unlock_irqrestore(&byte_conv_mask_lock, flags);
	ret = simple_read_from_buffer(ubuf, count, ppos, buf, len);
	if (ret != 0) // Prevent double msg in dmesg
		BC_PR_DEBUG("mask bits is: %s", bit_str_mask);
	return ret;

ERR:
	be_mask = cpu_to_be32(byte_conv_mask);

	bin_len = print_conv_bin(bit_str_mask, (char *)&be_mask,
			sizeof(be_mask));

	BC_PR_DEBUG("mask bits is: %s", bit_str_mask);
	len = scnprintf(buf, sizeof(buf), "Mask invalid\n");

	ret = simple_read_from_buffer(ubuf, count, ppos, buf, len);
	if (ret != 0) // Prevent double msg in dmesg
		BC_PR_DEBUG("mask bits is: %s", bit_str_mask);
	return ret;
}

static ssize_t mask_write(struct file *file,
		const char __user *ubuf,
		size_t count,
		loff_t *ppos)
{
	u32 tmp_mask;
	unsigned long flags;
	ssize_t err = 0;
	char buf[256];

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

	if (kstrtou32(buf, 0, &tmp_mask)) {
		err = -EINVAL;
		goto ERR;
	}

	if ((tmp_mask & 0x3f) == 0) {
		err = -EINVAL;
		goto ERR;
	}

	if (!is_power_of_2(((tmp_mask >> 10) & 0x1f))) {
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

