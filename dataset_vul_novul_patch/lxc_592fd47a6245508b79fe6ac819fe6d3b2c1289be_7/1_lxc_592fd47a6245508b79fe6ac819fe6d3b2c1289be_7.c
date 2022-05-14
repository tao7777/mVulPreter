 static inline int mount_entry_on_generic(struct mntent *mntent,
                 const char* path)
 {
 	unsigned long mntflags;
 	char *mntdata;
	int ret;
	bool optional = hasmntopt(mntent, "optional") != NULL;

	ret = mount_entry_create_dir_file(mntent, path);

	if (ret < 0)
		return optional ? 0 : -1;

	cull_mntent_opt(mntent);

	if (parse_mntopts(mntent->mnt_opts, &mntflags, &mntdata) < 0) {
		free(mntdata);
		return -1;
 	}
 
 	ret = mount_entry(mntent->mnt_fsname, path, mntent->mnt_type,
			  mntflags, mntdata, optional);
 
 	free(mntdata);
 
	return ret;
}
