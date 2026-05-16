/* SPDX-License-Identifier: GPL-3.0 */
#ifndef EXTERNS_H__
#define EXTERNS_H__

extern const struct proc_ops byte_conv_mask_ops;
extern const char * const byte_conv_dev_fname;
extern const char * const byte_conv_proc_fname;
extern u16 byte_conv_mask;
extern spinlock_t byte_conv_mask_lock;

#endif // EXTERNS_H__
