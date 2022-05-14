int rpmPackageFilesInstall(rpmts ts, rpmte te, rpmfiles files,
              rpmpsm psm, char ** failedFile)
{
    FD_t payload = rpmtePayload(te);
    rpmfi fi = rpmfiNewArchiveReader(payload, files, RPMFI_ITER_READ_ARCHIVE);
    rpmfs fs = rpmteGetFileStates(te);
    rpmPlugins plugins = rpmtsPlugins(ts);
    struct stat sb;
    int saveerrno = errno;
    int rc = 0;
    int nodigest = (rpmtsFlags(ts) & RPMTRANS_FLAG_NOFILEDIGEST) ? 1 : 0;
    int nofcaps = (rpmtsFlags(ts) & RPMTRANS_FLAG_NOCAPS) ? 1 : 0;
    int firsthardlink = -1;
    int skip;
    rpmFileAction action;
    char *tid = NULL;
    const char *suffix;
    char *fpath = NULL;

    if (fi == NULL) {
	rc = RPMERR_BAD_MAGIC;
	goto exit;
    }

    /* transaction id used for temporary path suffix while installing */
    rasprintf(&tid, ";%08x", (unsigned)rpmtsGetTid(ts));

    /* Detect and create directories not explicitly in package. */
    rc = fsmMkdirs(files, fs, plugins);

    while (!rc) {
	/* Read next payload header. */
	rc = rpmfiNext(fi);

	if (rc < 0) {
	    if (rc == RPMERR_ITER_END)
		rc = 0;
	    break;
	}

	action = rpmfsGetAction(fs, rpmfiFX(fi));
	skip = XFA_SKIPPING(action);
	suffix = S_ISDIR(rpmfiFMode(fi)) ? NULL : tid;
	if (action != FA_TOUCH) {
	    fpath = fsmFsPath(fi, suffix);
	} else {
	    fpath = fsmFsPath(fi, "");
	}

	/* Remap file perms, owner, and group. */
	rc = rpmfiStat(fi, 1, &sb);

	fsmDebug(fpath, action, &sb);

        /* Exit on error. */
        if (rc)
            break;

	/* Run fsm file pre hook for all plugins */
	rc = rpmpluginsCallFsmFilePre(plugins, fi, fpath,
				      sb.st_mode, action);
	if (rc) {
	    skip = 1;
	} else {
	    setFileState(fs, rpmfiFX(fi));
	}

        if (!skip) {
	    int setmeta = 1;

	    /* Directories replacing something need early backup */
	    if (!suffix) {
		rc = fsmBackup(fi, action);
	    }
	    /* Assume file does't exist when tmp suffix is in use */
	    if (!suffix) {
		rc = fsmVerify(fpath, fi);
	    } else {
		rc = (action == FA_TOUCH) ? 0 : RPMERR_ENOENT;
	    }

            if (S_ISREG(sb.st_mode)) {
		if (rc == RPMERR_ENOENT) {
		    rc = fsmMkfile(fi, fpath, files, psm, nodigest,
				   &setmeta, &firsthardlink);
		}
            } else if (S_ISDIR(sb.st_mode)) {
                if (rc == RPMERR_ENOENT) {
                    mode_t mode = sb.st_mode;
                    mode &= ~07777;
                    mode |=  00700;
                    rc = fsmMkdir(fpath, mode);
                }
            } else if (S_ISLNK(sb.st_mode)) {
		if (rc == RPMERR_ENOENT) {
		    rc = fsmSymlink(rpmfiFLink(fi), fpath);
		}
            } else if (S_ISFIFO(sb.st_mode)) {
                /* This mimics cpio S_ISSOCK() behavior but probably isn't right */
                if (rc == RPMERR_ENOENT) {
                    rc = fsmMkfifo(fpath, 0000);
                }
            } else if (S_ISCHR(sb.st_mode) ||
                       S_ISBLK(sb.st_mode) ||
                       S_ISSOCK(sb.st_mode))
            {
                if (rc == RPMERR_ENOENT) {
                    rc = fsmMknod(fpath, sb.st_mode, sb.st_rdev);
                }
            } else {
                /* XXX Special case /dev/log, which shouldn't be packaged anyways */
                if (!IS_DEV_LOG(fpath))
                    rc = RPMERR_UNKNOWN_FILETYPE;
            }
	    /* Set permissions, timestamps etc for non-hardlink entries */
	    if (!rc && setmeta) {
		rc = fsmSetmeta(fpath, fi, plugins, action, &sb, nofcaps);
	    }
        } else if (firsthardlink >= 0 && rpmfiArchiveHasContent(fi)) {
 	    /* we skip the hard linked file containing the content */
 	    /* write the content to the first used instead */
 	    char *fn = rpmfilesFN(files, firsthardlink);
	    rc = expandRegular(fi, fn, psm, 0, nodigest, 0);
 	    firsthardlink = -1;
 	    free(fn);
 	}

        if (rc) {
            if (!skip) {
                /* XXX only erase if temp fn w suffix is in use */
                if (suffix && (action != FA_TOUCH)) {
		    (void) fsmRemove(fpath, sb.st_mode);
                }
                errno = saveerrno;
            }
        } else {
	    /* Notify on success. */
	    rpmpsmNotify(psm, RPMCALLBACK_INST_PROGRESS, rpmfiArchiveTell(fi));

	    if (!skip) {
		/* Backup file if needed. Directories are handled earlier */
		if (suffix)
		    rc = fsmBackup(fi, action);

		if (!rc)
		    rc = fsmCommit(&fpath, fi, action, suffix);
	    }
	}

	if (rc)
	    *failedFile = xstrdup(fpath);

	/* Run fsm file post hook for all plugins */
	rpmpluginsCallFsmFilePost(plugins, fi, fpath,
				  sb.st_mode, action, rc);
	fpath = _free(fpath);
    }

    rpmswAdd(rpmtsOp(ts, RPMTS_OP_UNCOMPRESS), fdOp(payload, FDSTAT_READ));
    rpmswAdd(rpmtsOp(ts, RPMTS_OP_DIGEST), fdOp(payload, FDSTAT_DIGEST));

exit:

    /* No need to bother with close errors on read */
    rpmfiArchiveClose(fi);
    rpmfiFree(fi);
    Fclose(payload);
    free(tid);
    free(fpath);

    return rc;
}
