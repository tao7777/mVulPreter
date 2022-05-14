static int fsmVerify(const char *path, rpmfi fi)
static int fsmVerify(const char *path, rpmfi fi, const struct stat *fsb)
 {
     int rc;
     int saveerrno = errno;
    struct stat dsb;
    mode_t mode = rpmfiFMode(fi);

    rc = fsmStat(path, 1, &dsb);
    if (rc)
	return rc;

    if (S_ISREG(mode)) {
	/* HP-UX (and other os'es) don't permit unlink on busy files. */
	char *rmpath = rstrscat(NULL, path, "-RPMDELETE", NULL);
	rc = fsmRename(path, rmpath);
	/* XXX shouldn't we take unlink return code here? */
	if (!rc)
	    (void) fsmUnlink(rmpath);
	else
	    rc = RPMERR_UNLINK_FAILED;
	free(rmpath);
        return (rc ? rc : RPMERR_ENOENT);	/* XXX HACK */
     } else if (S_ISDIR(mode)) {
         if (S_ISDIR(dsb.st_mode)) return 0;
         if (S_ISLNK(dsb.st_mode)) {
	    uid_t luid = dsb.st_uid;
             rc = fsmStat(path, 0, &dsb);
             if (rc == RPMERR_ENOENT) rc = 0;
             if (rc) return rc;
             errno = saveerrno;
	    /* Only permit directory symlinks by target owner and root */
            if (S_ISDIR(dsb.st_mode) && (luid == 0 || luid == fsb->st_uid))
		    return 0;
         }
     } else if (S_ISLNK(mode)) {
         if (S_ISLNK(dsb.st_mode)) {
            char buf[8 * BUFSIZ];
            size_t len;
            rc = fsmReadLink(path, buf, 8 * BUFSIZ, &len);
            errno = saveerrno;
            if (rc) return rc;
            if (rstreq(rpmfiFLink(fi), buf)) return 0;
        }
    } else if (S_ISFIFO(mode)) {
        if (S_ISFIFO(dsb.st_mode)) return 0;
    } else if (S_ISCHR(mode) || S_ISBLK(mode)) {
        if ((S_ISCHR(dsb.st_mode) || S_ISBLK(dsb.st_mode)) &&
            (dsb.st_rdev == rpmfiFRdev(fi))) return 0;
    } else if (S_ISSOCK(mode)) {
        if (S_ISSOCK(dsb.st_mode)) return 0;
    }
    /* XXX shouldn't do this with commit/undo. */
    rc = fsmUnlink(path);
    if (rc == 0)	rc = RPMERR_ENOENT;
    return (rc ? rc : RPMERR_ENOENT);	/* XXX HACK */
}
