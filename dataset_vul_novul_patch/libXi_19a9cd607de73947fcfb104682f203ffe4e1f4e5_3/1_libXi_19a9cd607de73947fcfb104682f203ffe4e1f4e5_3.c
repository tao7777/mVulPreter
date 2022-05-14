XOpenDevice(
    register Display	*dpy,
    register XID	 id)
{
    register long rlen;	/* raw length */
    xOpenDeviceReq *req;
    xOpenDeviceReply rep;
    XDevice *dev;
    XExtDisplayInfo *info = XInput_find_display(dpy);

    LockDisplay(dpy);
    if (_XiCheckExtInit(dpy, XInput_Initial_Release, info) == -1)
        return NULL;

    GetReq(OpenDevice, req);
    req->reqType = info->codes->major_opcode;
    req->ReqType = X_OpenDevice;
    req->deviceid = id;

    if (!_XReply(dpy, (xReply *) & rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return (XDevice *) NULL;
 	return (XDevice *) NULL;
     }
 
    rlen = rep.length << 2;
    dev = (XDevice *) Xmalloc(sizeof(XDevice) + rep.num_classes *
			      sizeof(XInputClassInfo));
     if (dev) {
 	int dlen;	/* data length */
	    _XEatData(dpy, (unsigned long)rlen - dlen);
    } else
	_XEatDataWords(dpy, rep.length);

    UnlockDisplay(dpy);
    SyncHandle();
    return (dev);
}
