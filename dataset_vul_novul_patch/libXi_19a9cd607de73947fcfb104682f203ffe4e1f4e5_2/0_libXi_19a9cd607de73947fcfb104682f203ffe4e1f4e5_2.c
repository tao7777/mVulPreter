XIQueryDevice(Display *dpy, int deviceid, int *ndevices_return)
{
    XIDeviceInfo        *info = NULL;
    xXIQueryDeviceReq   *req;
     xXIQueryDeviceReq   *req;
     xXIQueryDeviceReply reply;
     char                *ptr;
    char                *end;
     int                 i;
     char                *buf;
 
    LockDisplay(dpy);
    if (_XiCheckExtInit(dpy, XInput_2_0, extinfo) == -1)
        goto error_unlocked;

    GetReq(XIQueryDevice, req);
    req->reqType  = extinfo->codes->major_opcode;
    req->ReqType  = X_XIQueryDevice;
    req->deviceid = deviceid;

    if (!_XReply(dpy, (xReply*) &reply, 0, xFalse))
        goto error;
     if (!_XReply(dpy, (xReply*) &reply, 0, xFalse))
         goto error;
 
    if (reply.length < INT_MAX / 4)
    {
	*ndevices_return = reply.num_devices;
	info = Xmalloc((reply.num_devices + 1) * sizeof(XIDeviceInfo));
    }
    else
    {
	*ndevices_return = 0;
	info = NULL;
    }

     if (!info)
         goto error;
 
     buf = Xmalloc(reply.length * 4);
     _XRead(dpy, buf, reply.length * 4);
     ptr = buf;
    end = buf + reply.length * 4;
 
     /* info is a null-terminated array */
     info[reply.num_devices].name = NULL;
        nclasses         = wire->num_classes;

        ptr += sizeof(xXIDeviceInfo);

        lib->name = Xcalloc(wire->name_len + 1, 1);
         XIDeviceInfo    *lib = &info[i];
         xXIDeviceInfo   *wire = (xXIDeviceInfo*)ptr;
 
        if (ptr + sizeof(xXIDeviceInfo) > end)
            goto error_loop;

         lib->deviceid    = wire->deviceid;
         lib->use         = wire->use;
         lib->attachment  = wire->attachment;

    Xfree(buf);
 
         ptr += sizeof(xXIDeviceInfo);
 
        if (ptr + wire->name_len > end)
            goto error_loop;

         lib->name = Xcalloc(wire->name_len + 1, 1);
        if (lib->name == NULL)
            goto error_loop;
         strncpy(lib->name, ptr, wire->name_len);
        lib->name[wire->name_len] = '\0';
         ptr += ((wire->name_len + 3)/4) * 4;
 
         sz = size_classes((xXIAnyInfo*)ptr, nclasses);
         lib->classes = Xmalloc(sz);
        if (lib->classes == NULL)
        {
            Xfree(lib->name);
            goto error_loop;
        }
         ptr += copy_classes(lib, (xXIAnyInfo*)ptr, &nclasses);
         /* We skip over unused classes */
         lib->num_classes = nclasses;
}
