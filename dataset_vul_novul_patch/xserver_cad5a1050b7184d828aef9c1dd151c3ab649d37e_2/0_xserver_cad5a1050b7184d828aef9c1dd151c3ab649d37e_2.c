SProcXResQueryResourceBytes (ClientPtr client)
{
    REQUEST(xXResQueryResourceBytesReq);
    int c;
    xXResResourceIdSpec *specs = (void*) ((char*) stuff + sizeof(*stuff));
     int c;
     xXResResourceIdSpec *specs = (void*) ((char*) stuff + sizeof(*stuff));
 
     REQUEST_AT_LEAST_SIZE(xXResQueryResourceBytesReq);
    swapl(&stuff->numSpecs);
     REQUEST_FIXED_SIZE(xXResQueryResourceBytesReq,
                        stuff->numSpecs * sizeof(specs[0]));
    }
