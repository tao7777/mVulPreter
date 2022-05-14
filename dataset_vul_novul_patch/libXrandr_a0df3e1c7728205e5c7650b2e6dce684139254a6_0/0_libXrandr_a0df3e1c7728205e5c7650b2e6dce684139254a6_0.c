XRRGetOutputInfo (Display *dpy, XRRScreenResources *resources, RROutput output)
{
    XExtDisplayInfo		*info = XRRFindDisplay(dpy);
    xRRGetOutputInfoReply	rep;
    xRRGetOutputInfoReq		*req;
    int				nbytes, nbytesRead, rbytes;
    XRROutputInfo		*xoi;

    RRCheckExtension (dpy, info, NULL);

    LockDisplay (dpy);
    GetReq (RRGetOutputInfo, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRGetOutputInfo;
    req->output = output;
    req->configTimestamp = resources->configTimestamp;

    if (!_XReply (dpy, (xReply *) &rep, OutputInfoExtra >> 2, xFalse))
    {
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
 	return NULL;
     }
 
    if (rep.length > INT_MAX >> 2 || rep.length < (OutputInfoExtra >> 2))
    {
        if (rep.length > (OutputInfoExtra >> 2))
	    _XEatDataWords (dpy, rep.length - (OutputInfoExtra >> 2));
	else
	    _XEatDataWords (dpy, rep.length);
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
    }
     nbytes = ((long) (rep.length) << 2) - OutputInfoExtra;
 
     nbytesRead = (long) (rep.nCrtcs * 4 +
	      rep.nCrtcs * sizeof (RRCrtc) +
	      rep.nModes * sizeof (RRMode) +
	      rep.nClones * sizeof (RROutput) +
	      rep.nameLength + 1);	    /* '\0' terminate name */

    xoi = (XRROutputInfo *) Xmalloc(rbytes);
    if (xoi == NULL) {
	_XEatDataWords (dpy, rep.length - (OutputInfoExtra >> 2));
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
    }

    xoi->timestamp = rep.timestamp;
    xoi->crtc = rep.crtc;
    xoi->mm_width = rep.mmWidth;
    xoi->mm_height = rep.mmHeight;
    xoi->connection = rep.connection;
    xoi->subpixel_order = rep.subpixelOrder;
    xoi->ncrtc = rep.nCrtcs;
    xoi->crtcs = (RRCrtc *) (xoi + 1);
    xoi->nmode = rep.nModes;
    xoi->npreferred = rep.nPreferred;
    xoi->modes = (RRMode *) (xoi->crtcs + rep.nCrtcs);
    xoi->nclone = rep.nClones;
    xoi->clones = (RROutput *) (xoi->modes + rep.nModes);
    xoi->name = (char *) (xoi->clones + rep.nClones);

    _XRead32 (dpy, (long *) xoi->crtcs, rep.nCrtcs << 2);
    _XRead32 (dpy, (long *) xoi->modes, rep.nModes << 2);
    _XRead32 (dpy, (long *) xoi->clones, rep.nClones << 2);

    /*
     * Read name and '\0' terminate
     */
    _XReadPad (dpy, xoi->name, rep.nameLength);
    xoi->name[rep.nameLength] = '\0';
    xoi->nameLen = rep.nameLength;

    /*
     * Skip any extra data
     */
    if (nbytes > nbytesRead)
	_XEatData (dpy, (unsigned long) (nbytes - nbytesRead));

    UnlockDisplay (dpy);
    SyncHandle ();
    return (XRROutputInfo *) xoi;
}
