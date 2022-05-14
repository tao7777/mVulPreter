static int __init acpi_custom_method_init(void)
{
	if (!acpi_debugfs_dir)
		return -ENOENT;
	cm_dentry = debugfs_create_file("custom_method", S_IWUSR,
					acpi_debugfs_dir, NULL, &cm_fops);
	if (!cm_dentry)
		return -ENODEV;
	return 0;
}
