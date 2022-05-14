Status XvMCGetDRInfo(Display *dpy, XvPortID port,
		     char **name, char **busID,
		     int *major, int *minor,
		     int *patchLevel,
		     int *isLocal)
{
    XExtDisplayInfo *info = xvmc_find_display(dpy);
    xvmcGetDRInfoReply rep;
    xvmcGetDRInfoReq  *req;
    CARD32 magic;

#ifdef HAVE_SHMAT
    volatile CARD32 *shMem;
    struct timezone here;
    struct timeval now;
    here.tz_minuteswest = 0;
    here.tz_dsttime = 0;
#endif

    *name = NULL;
    *busID = NULL;

    XvMCCheckExtension (dpy, info, BadImplementation);

    LockDisplay (dpy);
    XvMCGetReq (GetDRInfo, req);

    req->port = port;
    magic = 0;
    req->magic = 0;
#ifdef HAVE_SHMAT
    req->shmKey = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0600);

    /*
     * We fill a shared memory page with a repetitive pattern. If the
     * X server can read this pattern, we probably have a local connection.
     * Note that we can trigger the remote X server to read any shared
     * page on the remote machine, so we shouldn't be able to guess and verify
     * any complicated data on those pages. Thats the explanation of this
     * otherwise stupid-looking pattern algorithm.
     */

    if (req->shmKey >= 0) {
	shMem = (CARD32 *) shmat(req->shmKey, NULL, 0);
	shmctl( req->shmKey, IPC_RMID, NULL);
	if ( shMem ) {

	    register volatile CARD32 *shMemC = shMem;
	    register int i;

	    gettimeofday( &now, &here);
	    magic = now.tv_usec & 0x000FFFFF;
	    req->magic = magic;
	    i = 1024 / sizeof(CARD32);
	    while(i--) {
	        *shMemC++ = magic;
	        magic = ~magic;
	    }
	} else {
	    req->shmKey = -1;
	}
    }
#else
    req->shmKey = 0;
#endif
    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) {
        UnlockDisplay (dpy);
        SyncHandle ();
#ifdef HAVE_SHMAT
	if ( req->shmKey >= 0) {
	    shmdt( (const void *) shMem );
	}
#endif
        return -1;
    }
#ifdef HAVE_SHMAT
    shmdt( (const void *) shMem );
#endif

    if (rep.length > 0) {
	unsigned long realSize = 0;
	char *tmpBuf = NULL;

	if ((rep.length < (INT_MAX >> 2)) &&
	    /* protect against overflow in strncpy below */
	    (rep.nameLen + rep.busIDLen > rep.nameLen)) {
	    realSize = rep.length << 2;
	    if (realSize >= (rep.nameLen + rep.busIDLen)) {
		tmpBuf = Xmalloc(realSize);
		*name = Xmalloc(rep.nameLen);
		*busID = Xmalloc(rep.busIDLen);
	    }
	}

 	if (*name && *busID && tmpBuf) {
 	    _XRead(dpy, tmpBuf, realSize);
 	    strncpy(*name,tmpBuf,rep.nameLen);
	    (*name)[rep.nameLen == 0 ? 0 : rep.nameLen - 1] = '\0';
 	    strncpy(*busID,tmpBuf+rep.nameLen,rep.busIDLen);
	    (*busID)[rep.busIDLen == 0 ? 0 : rep.busIDLen - 1] = '\0';
 	    XFree(tmpBuf);
 	} else {
 	    XFree(*name);
	    *name = NULL;
	    XFree(*busID);
	    *busID = NULL;
	    XFree(tmpBuf);

	    _XEatDataWords(dpy, rep.length);
	    UnlockDisplay (dpy);
	    SyncHandle ();
	    return -1;

	}
    }

    UnlockDisplay (dpy);
    SyncHandle ();
    *major = rep.major;
    *minor = rep.minor;
    *patchLevel = rep.patchLevel;
    *isLocal = (req->shmKey > 0) ? rep.isLocal : 1;
    return (rep.length > 0) ? Success : BadImplementation;
}
