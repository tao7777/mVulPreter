void preproc_mount_mnt_dir(void) {
	if (!tmpfs_mounted) {
		if (arg_debug)
			printf("Mounting tmpfs on %s directory\n", RUN_MNT_DIR);
		if (mount("tmpfs", RUN_MNT_DIR, "tmpfs", MS_NOSUID | MS_STRICTATIME,  "mode=755,gid=0") < 0)
			errExit("mounting /run/firejail/mnt");
		tmpfs_mounted = 1;
 		fs_logger2("tmpfs", RUN_MNT_DIR);
 
 #ifdef HAVE_SECCOMP
		create_empty_dir_as_root(RUN_SECCOMP_DIR, 0755);

 		if (arg_seccomp_block_secondary)
 			copy_file(PATH_SECCOMP_BLOCK_SECONDARY, RUN_SECCOMP_BLOCK_SECONDARY, getuid(), getgid(), 0644); // root needed
 		else {
			copy_file(PATH_SECCOMP_32, RUN_SECCOMP_32, getuid(), getgid(), 0644); // root needed
		}
		if (arg_allow_debuggers)
			copy_file(PATH_SECCOMP_DEFAULT_DEBUG, RUN_SECCOMP_CFG, getuid(), getgid(), 0644); // root needed
		else
			copy_file(PATH_SECCOMP_DEFAULT, RUN_SECCOMP_CFG, getuid(), getgid(), 0644); // root needed

		if (arg_memory_deny_write_execute)
			copy_file(PATH_SECCOMP_MDWX, RUN_SECCOMP_MDWX, getuid(), getgid(), 0644); // root needed
		create_empty_file_as_root(RUN_SECCOMP_PROTOCOL, 0644);
		if (set_perms(RUN_SECCOMP_PROTOCOL, getuid(), getgid(), 0644))
			errExit("set_perms");
		create_empty_file_as_root(RUN_SECCOMP_POSTEXEC, 0644);
		if (set_perms(RUN_SECCOMP_POSTEXEC, getuid(), getgid(), 0644))
			errExit("set_perms");
#endif
	}
}
