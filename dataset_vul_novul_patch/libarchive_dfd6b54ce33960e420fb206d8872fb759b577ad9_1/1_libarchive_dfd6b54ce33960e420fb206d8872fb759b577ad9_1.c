create_filesystem_object(struct archive_write_disk *a)
{
	/* Create the entry. */
 	const char *linkname;
 	mode_t final_mode, mode;
 	int r;
 
 	/* We identify hard/symlinks according to the link names. */
 	/* Since link(2) and symlink(2) don't handle modes, we're done here. */
	linkname = archive_entry_hardlink(a->entry);
	if (linkname != NULL) {
 #if !HAVE_LINK
 		return (EPERM);
 #else
 		r = link(linkname, a->name) ? errno : 0;
 		/*
 		 * New cpio and pax formats allow hardlink entries
		 * to carry data, so we may have to open the file
		 * for hardlink entries.
		 *
		 * If the hardlink was successfully created and
		 * the archive doesn't have carry data for it,
		 * consider it to be non-authoritative for meta data.
		 * This is consistent with GNU tar and BSD pax.
		 * If the hardlink does carry data, let the last
		 * archive entry decide ownership.
		 */
		if (r == 0 && a->filesize <= 0) {
			a->todo = 0;
			a->deferred = 0;
		} else if (r == 0 && a->filesize > 0) {
			a->fd = open(a->name,
				     O_WRONLY | O_TRUNC | O_BINARY | O_CLOEXEC);
			__archive_ensure_cloexec_flag(a->fd);
			if (a->fd < 0)
				r = errno;
		}
		return (r);
#endif
	}
	linkname = archive_entry_symlink(a->entry);
	if (linkname != NULL) {
#if HAVE_SYMLINK
		return symlink(linkname, a->name) ? errno : 0;
#else
		return (EPERM);
#endif
	}

	/*
	 * The remaining system calls all set permissions, so let's
	 * try to take advantage of that to avoid an extra chmod()
	 * call.  (Recall that umask is set to zero right now!)
	 */

	/* Mode we want for the final restored object (w/o file type bits). */
	final_mode = a->mode & 07777;
	/*
	 * The mode that will actually be restored in this step.  Note
	 * that SUID, SGID, etc, require additional work to ensure
	 * security, so we never restore them at this point.
	 */
	mode = final_mode & 0777 & ~a->user_umask;

	switch (a->mode & AE_IFMT) {
	default:
		/* POSIX requires that we fall through here. */
		/* FALLTHROUGH */
	case AE_IFREG:
		a->fd = open(a->name,
		    O_WRONLY | O_CREAT | O_EXCL | O_BINARY | O_CLOEXEC, mode);
		__archive_ensure_cloexec_flag(a->fd);
		r = (a->fd < 0);
		break;
	case AE_IFCHR:
#ifdef HAVE_MKNOD
		/* Note: we use AE_IFCHR for the case label, and
		 * S_IFCHR for the mknod() call.  This is correct.  */
		r = mknod(a->name, mode | S_IFCHR,
		    archive_entry_rdev(a->entry));
		break;
#else
		/* TODO: Find a better way to warn about our inability
		 * to restore a char device node. */
		return (EINVAL);
#endif /* HAVE_MKNOD */
	case AE_IFBLK:
#ifdef HAVE_MKNOD
		r = mknod(a->name, mode | S_IFBLK,
		    archive_entry_rdev(a->entry));
		break;
#else
		/* TODO: Find a better way to warn about our inability
		 * to restore a block device node. */
		return (EINVAL);
#endif /* HAVE_MKNOD */
	case AE_IFDIR:
		mode = (mode | MINIMUM_DIR_MODE) & MAXIMUM_DIR_MODE;
		r = mkdir(a->name, mode);
		if (r == 0) {
			/* Defer setting dir times. */
			a->deferred |= (a->todo & TODO_TIMES);
			a->todo &= ~TODO_TIMES;
			/* Never use an immediate chmod(). */
			/* We can't avoid the chmod() entirely if EXTRACT_PERM
			 * because of SysV SGID inheritance. */
			if ((mode != final_mode)
			    || (a->flags & ARCHIVE_EXTRACT_PERM))
				a->deferred |= (a->todo & TODO_MODE);
			a->todo &= ~TODO_MODE;
		}
		break;
	case AE_IFIFO:
#ifdef HAVE_MKFIFO
		r = mkfifo(a->name, mode);
		break;
#else
		/* TODO: Find a better way to warn about our inability
		 * to restore a fifo. */
		return (EINVAL);
#endif /* HAVE_MKFIFO */
	}

	/* All the system calls above set errno on failure. */
	if (r)
		return (errno);

	/* If we managed to set the final mode, we've avoided a chmod(). */
	if (mode == final_mode)
		a->todo &= ~TODO_MODE;
	return (0);
}
