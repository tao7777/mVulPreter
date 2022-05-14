rpmVerifyAttrs rpmfilesVerify(rpmfiles fi, int ix, rpmVerifyAttrs omitMask)
{
    rpm_mode_t fmode = rpmfilesFMode(fi, ix);
    rpmfileAttrs fileAttrs = rpmfilesFFlags(fi, ix);
    rpmVerifyAttrs flags = rpmfilesVFlags(fi, ix);
    const char * fn = rpmfilesFN(fi, ix);
    struct stat sb;
    rpmVerifyAttrs vfy = RPMVERIFY_NONE;

    /*
     * Check to see if the file was installed - if not pretend all is OK.
     */
    switch (rpmfilesFState(fi, ix)) {
    case RPMFILE_STATE_NETSHARED:
    case RPMFILE_STATE_NOTINSTALLED:
	goto exit;
	break;
    case RPMFILE_STATE_REPLACED:
	/* For replaced files we can only verify if it exists at all */
	flags = RPMVERIFY_LSTATFAIL;
	break;
    case RPMFILE_STATE_WRONGCOLOR:
	/*
	 * Files with wrong color are supposed to share some attributes
	 * with the actually installed file - verify what we can.
	 */
	flags &= ~(RPMVERIFY_FILEDIGEST | RPMVERIFY_FILESIZE |
		   RPMVERIFY_MTIME | RPMVERIFY_RDEV);
	break;
    case RPMFILE_STATE_NORMAL:
    /* File from a non-installed package, try to verify nevertheless */
    case RPMFILE_STATE_MISSING:
	break;
    }

    if (fn == NULL || lstat(fn, &sb) != 0) {
	vfy |= RPMVERIFY_LSTATFAIL;
	goto exit;
     }
 
     /* If we expected a directory but got a symlink to one, follow the link */
    if (S_ISDIR(fmode) && S_ISLNK(sb.st_mode)) {
	struct stat dsb;
	/* ...if it actually points to a directory  */
	if (stat(fn, &dsb) == 0 && S_ISDIR(dsb.st_mode)) {
	    uid_t fuid;
	    /* ...and is by a legit user, to match fsmVerify() behavior */
	    if (sb.st_uid == 0 ||
			(rpmugUid(rpmfilesFUser(fi, ix), &fuid) == 0 &&
			 sb.st_uid == fuid)) {
		sb = dsb; /* struct assignment */
	    }
	}
     }
 
     /* Links have no mode, other types have no linkto */
    if (S_ISLNK(sb.st_mode))
	flags &= ~(RPMVERIFY_MODE);
    else
	flags &= ~(RPMVERIFY_LINKTO);

    /* Not all attributes of non-regular files can be verified */
    if (!S_ISREG(sb.st_mode))
	flags &= ~(RPMVERIFY_FILEDIGEST | RPMVERIFY_FILESIZE |
		   RPMVERIFY_MTIME | RPMVERIFY_CAPS);

    /* Content checks of %ghost files are meaningless. */
    if (fileAttrs & RPMFILE_GHOST)
	flags &= ~(RPMVERIFY_FILEDIGEST | RPMVERIFY_FILESIZE |
		   RPMVERIFY_MTIME | RPMVERIFY_LINKTO);

    /* Don't verify any features in omitMask. */
    flags &= ~(omitMask | RPMVERIFY_FAILURES);


    if (flags & RPMVERIFY_FILEDIGEST) {
	const unsigned char *digest; 
	int algo;
	size_t diglen;

	/* XXX If --nomd5, then prelinked library sizes are not corrected. */
	if ((digest = rpmfilesFDigest(fi, ix, &algo, &diglen))) {
	    unsigned char fdigest[diglen];
	    rpm_loff_t fsize;

	    if (rpmDoDigest(algo, fn, 0, fdigest, &fsize)) {
		vfy |= (RPMVERIFY_READFAIL|RPMVERIFY_FILEDIGEST);
	    } else {
		sb.st_size = fsize;
		if (memcmp(fdigest, digest, diglen))
		    vfy |= RPMVERIFY_FILEDIGEST;
	    }
	} else {
	    vfy |= RPMVERIFY_FILEDIGEST;
	} 
    } 

    if (flags & RPMVERIFY_LINKTO) {
	char linkto[1024+1];
	int size = 0;

	if ((size = readlink(fn, linkto, sizeof(linkto)-1)) == -1)
	    vfy |= (RPMVERIFY_READLINKFAIL|RPMVERIFY_LINKTO);
	else {
	    const char * flink = rpmfilesFLink(fi, ix);
	    linkto[size] = '\0';
	    if (flink == NULL || !rstreq(linkto, flink))
		vfy |= RPMVERIFY_LINKTO;
	}
    } 

    if (flags & RPMVERIFY_FILESIZE) {
	if (sb.st_size != rpmfilesFSize(fi, ix))
	    vfy |= RPMVERIFY_FILESIZE;
    } 

    if (flags & RPMVERIFY_MODE) {
	rpm_mode_t metamode = fmode;
	rpm_mode_t filemode;

	/*
	 * Platforms (like AIX) where sizeof(rpm_mode_t) != sizeof(mode_t)
	 * need the (rpm_mode_t) cast here. 
	 */
	filemode = (rpm_mode_t)sb.st_mode;

	/*
	 * Comparing the type of %ghost files is meaningless, but perms are OK.
	 */
	if (fileAttrs & RPMFILE_GHOST) {
	    metamode &= ~0xf000;
	    filemode &= ~0xf000;
	}

	if (metamode != filemode)
	    vfy |= RPMVERIFY_MODE;

#if WITH_ACL
	/*
	 * For now, any non-default acl's on a file is a difference as rpm
	 * cannot have set them.
	 */
	acl_t facl = acl_get_file(fn, ACL_TYPE_ACCESS);
	if (facl) {
	    if (acl_equiv_mode(facl, NULL) == 1) {
		vfy |= RPMVERIFY_MODE;
	    }
	    acl_free(facl);
	}
#endif 
    }

    if (flags & RPMVERIFY_RDEV) {
	if (S_ISCHR(fmode) != S_ISCHR(sb.st_mode)
	 || S_ISBLK(fmode) != S_ISBLK(sb.st_mode))
	{
	    vfy |= RPMVERIFY_RDEV;
	} else if (S_ISDEV(fmode) && S_ISDEV(sb.st_mode)) {
	    rpm_rdev_t st_rdev = (sb.st_rdev & 0xffff);
	    rpm_rdev_t frdev = (rpmfilesFRdev(fi, ix) & 0xffff);
	    if (st_rdev != frdev)
		vfy |= RPMVERIFY_RDEV;
	} 
    }

#if WITH_CAP
    if (flags & RPMVERIFY_CAPS) {
	/*
 	 * Empty capability set ("=") is not exactly the same as no
 	 * capabilities at all but suffices for now... 
 	 */
	cap_t cap, fcap;
	cap = cap_from_text(rpmfilesFCaps(fi, ix));
	if (!cap) {
	    cap = cap_from_text("=");
	}
	fcap = cap_get_file(fn);
	if (!fcap) {
	    fcap = cap_from_text("=");
	}
	
	if (cap_compare(cap, fcap) != 0)
	    vfy |= RPMVERIFY_CAPS;

	cap_free(fcap);
	cap_free(cap);
    }
#endif

    if ((flags & RPMVERIFY_MTIME) && (sb.st_mtime != rpmfilesFMtime(fi, ix))) {
	vfy |= RPMVERIFY_MTIME;
    }

    if (flags & RPMVERIFY_USER) {
	const char * name = rpmugUname(sb.st_uid);
	const char * fuser = rpmfilesFUser(fi, ix);
	uid_t uid;
	int namematch = 0;
	int idmatch = 0;

	if (name && fuser)
	   namematch =  rstreq(name, fuser);
	if (fuser && rpmugUid(fuser, &uid) == 0)
	    idmatch = (uid == sb.st_uid);

	if (namematch != idmatch) {
	    rpmlog(RPMLOG_WARNING,
		    _("Duplicate username or UID for user %s\n"), fuser);
	}

	if (!(namematch || idmatch))
	    vfy |= RPMVERIFY_USER;
    }

    if (flags & RPMVERIFY_GROUP) {
	const char * name = rpmugGname(sb.st_gid);
	const char * fgroup = rpmfilesFGroup(fi, ix);
	gid_t gid;
	int namematch = 0;
	int idmatch = 0;

	if (name && fgroup)
	    namematch = rstreq(name, fgroup);
	if (fgroup && rpmugGid(fgroup, &gid) == 0)
	    idmatch = (gid == sb.st_gid);

	if (namematch != idmatch) {
	    rpmlog(RPMLOG_WARNING,
		    _("Duplicate groupname or GID for group %s\n"), fgroup);
	}

	if (!(namematch || idmatch))
	    vfy |= RPMVERIFY_GROUP;
    }

exit:
    return vfy;
}
