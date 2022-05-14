 static inline int mount_entry_on_systemfs(struct mntent *mntent)
 {
  return mount_entry_on_generic(mntent, mntent->mnt_dir, NULL);
 }
