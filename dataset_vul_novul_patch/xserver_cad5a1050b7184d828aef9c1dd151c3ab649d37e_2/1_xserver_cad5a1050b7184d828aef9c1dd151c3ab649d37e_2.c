SProcXResQueryResourceBytes (ClientPtr client)
{
    REQUEST(xXResQueryResourceBytesReq);
    int c;
    xXResResourceIdSpec *specs = (void*) ((char*) stuff + sizeof(*stuff));
     int c;
     xXResResourceIdSpec *specs = (void*) ((char*) stuff + sizeof(*stuff));
 
    swapl(&stuff->numSpecs);
     REQUEST_AT_LEAST_SIZE(xXResQueryResourceBytesReq);
     REQUEST_FIXED_SIZE(xXResQueryResourceBytesReq,
                        stuff->numSpecs * sizeof(specs[0]));
    }
