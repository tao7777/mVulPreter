static int fsmMkfile(rpmfi fi, const char *dest, rpmfiles files,
		     rpmpsm psm, int nodigest, int *setmeta,
		     int * firsthardlink)
{
    int rc = 0;
    int numHardlinks = rpmfiFNlink(fi);

    if (numHardlinks > 1) {
 	/* Create first hardlinked file empty */
 	if (*firsthardlink < 0) {
 	    *firsthardlink = rpmfiFX(fi);
	    rc = expandRegular(fi, dest, psm, 1, nodigest, 1);
 	} else {
 	    /* Create hard links for others */
 	    char *fn = rpmfilesFN(files, *firsthardlink);
	    rc = link(fn, dest);
	    if (rc < 0) {
		rc = RPMERR_LINK_FAILED;
	    }
	    free(fn);
	}
    }
    /* Write normal files or fill the last hardlinked (already
        existing) file with content */
     if (numHardlinks<=1) {
 	if (!rc)
	    rc = expandRegular(fi, dest, psm, 1, nodigest, 0);
     } else if (rpmfiArchiveHasContent(fi)) {
 	if (!rc)
	    rc = expandRegular(fi, dest, psm, 0, nodigest, 0);
 	*firsthardlink = -1;
     } else {
 	*setmeta = 0;
    }

    return rc;
}
