static ssize_t cm_write(struct file *file, const char __user * user_buf,
			size_t count, loff_t *ppos)
{
	static char *buf;
	static u32 max_size;
	static u32 uncopied_bytes;
	struct acpi_table_header table;
	acpi_status status;
	if (!(*ppos)) {
		/* parse the table header to get the table length */
		if (count <= sizeof(struct acpi_table_header))
			return -EINVAL;
		if (copy_from_user(&table, user_buf,
				   sizeof(struct acpi_table_header)))
			return -EFAULT;
		uncopied_bytes = max_size = table.length;
		buf = kzalloc(max_size, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;
	}
	if (buf == NULL)
		return -EINVAL;
	if ((*ppos > max_size) ||
	    (*ppos + count > max_size) ||
	    (*ppos + count < count) ||
	    (count > uncopied_bytes))
		return -EINVAL;
	if (copy_from_user(buf + (*ppos), user_buf, count)) {
		kfree(buf);
		buf = NULL;
		return -EFAULT;
	}
	uncopied_bytes -= count;
	*ppos += count;
	if (!uncopied_bytes) {
		status = acpi_install_method(buf);
		kfree(buf);
		buf = NULL;
		if (ACPI_FAILURE(status))
			return -EINVAL;
		add_taint(TAINT_OVERRIDDEN_ACPI_TABLE);
	}
	return count;
}
