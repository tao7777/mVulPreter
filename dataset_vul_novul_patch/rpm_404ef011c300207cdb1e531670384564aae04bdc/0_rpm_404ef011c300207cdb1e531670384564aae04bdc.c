static int expandRegular(rpmfi fi, const char *dest, rpmpsm psm, int nodigest, int nocontent)
static int expandRegular(rpmfi fi, const char *dest, rpmpsm psm, int exclusive, int nodigest, int nocontent)
 {
     FD_t wfd = NULL;
     int rc = 0;
 
     /* Create the file with 0200 permissions (write by owner). */
     {
 	mode_t old_umask = umask(0577);
	wfd = Fopen(dest, exclusive ? "wx.ufdio" : "a.ufdio");
 	umask(old_umask);

	/* If reopening, make sure the file is what we expect */
	if (!exclusive && wfd != NULL && !linkSane(wfd, dest)) {
	    rc = RPMERR_OPEN_FAILED;
	    goto exit;
	}
     }
     if (Ferror(wfd)) {
 	rc = RPMERR_OPEN_FAILED;
	goto exit;
    }

    if (!nocontent)
	rc = rpmfiArchiveReadToFilePsm(fi, wfd, nodigest, psm);
exit:
    if (wfd) {
	int myerrno = errno;
	Fclose(wfd);
	errno = myerrno;
    }
    return rc;
}
