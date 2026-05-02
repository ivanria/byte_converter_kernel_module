#ifndef FUNC_DEFS_H__
#define FUNC_DEFS_H__

static ssize_t dev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static ssize_t dev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static int dev_open(struct inode *inode, struct file *file);


#endif // FUNC_DEFS_H__


