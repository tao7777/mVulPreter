ProcPanoramiXGetScreenSize(ClientPtr client)
{
    REQUEST(xPanoramiXGetScreenSizeReq);
    WindowPtr pWin;
     xPanoramiXGetScreenSizeReply rep;
     int rc;
 
    REQUEST_SIZE_MATCH(xPanoramiXGetScreenSizeReq);

     if (stuff->screen >= PanoramiXNumScreens)
         return BadMatch;
 
     rc = dixLookupWindow(&pWin, stuff->window, client, DixGetAttrAccess);
     if (rc != Success)
         return rc;
    rep = (xPanoramiXGetScreenSizeReply) {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
    /* screen dimensions */
        .width = screenInfo.screens[stuff->screen]->width,
        .height = screenInfo.screens[stuff->screen]->height,
        .window = stuff->window,
        .screen = stuff->screen
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.width);
        swapl(&rep.height);
        swapl(&rep.window);
        swapl(&rep.screen);
    }
    WriteToClient(client, sizeof(xPanoramiXGetScreenSizeReply), &rep);
    return Success;
}
