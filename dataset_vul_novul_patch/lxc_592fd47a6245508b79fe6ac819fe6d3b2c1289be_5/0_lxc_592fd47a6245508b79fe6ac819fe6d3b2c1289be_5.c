 static int mount_entry(const char *fsname, const char *target,
 		       const char *fstype, unsigned long mountflags,
		       const char *data, int optional, const char *rootfs)
 {
 #ifdef HAVE_STATVFS
 	struct statvfs sb;
 #endif
 
	if (safe_mount(fsname, target, fstype, mountflags & ~MS_REMOUNT, data, rootfs)) {
 		if (optional) {
 			INFO("failed to mount '%s' on '%s' (optional): %s", fsname,
 			     target, strerror(errno));
			return 0;
		}
		else {
			SYSERROR("failed to mount '%s' on '%s'", fsname, target);
			return -1;
		}
	}

	if ((mountflags & MS_REMOUNT) || (mountflags & MS_BIND)) {
		DEBUG("remounting %s on %s to respect bind or remount options",
		      fsname ? fsname : "(none)", target ? target : "(none)");
		unsigned long rqd_flags = 0;
		if (mountflags & MS_RDONLY)
			rqd_flags |= MS_RDONLY;
#ifdef HAVE_STATVFS
		if (statvfs(fsname, &sb) == 0) {
			unsigned long required_flags = rqd_flags;
			if (sb.f_flag & MS_NOSUID)
				required_flags |= MS_NOSUID;
			if (sb.f_flag & MS_NODEV)
				required_flags |= MS_NODEV;
			if (sb.f_flag & MS_RDONLY)
				required_flags |= MS_RDONLY;
			if (sb.f_flag & MS_NOEXEC)
				required_flags |= MS_NOEXEC;
			DEBUG("(at remount) flags for %s was %lu, required extra flags are %lu", fsname, sb.f_flag, required_flags);
			/*
			 * If this was a bind mount request, and required_flags
			 * does not have any flags which are not already in
			 * mountflags, then skip the remount
			 */
			if (!(mountflags & MS_REMOUNT)) {
				if (!(required_flags & ~mountflags) && rqd_flags == 0) {
					DEBUG("mountflags already was %lu, skipping remount",
						mountflags);
					goto skipremount;
				}
			}
			mountflags |= required_flags;
		}
 #endif
 
 		if (mount(fsname, target, fstype,
			  mountflags | MS_REMOUNT, data) < 0) {
 			if (optional) {
 				INFO("failed to mount '%s' on '%s' (optional): %s",
 					 fsname, target, strerror(errno));
				return 0;
			}
			else {
				SYSERROR("failed to mount '%s' on '%s'",
					 fsname, target);
				return -1;
			}
		}
	}

#ifdef HAVE_STATVFS
skipremount:
#endif
	DEBUG("mounted '%s' on '%s', type '%s'", fsname, target, fstype);

	return 0;
}
