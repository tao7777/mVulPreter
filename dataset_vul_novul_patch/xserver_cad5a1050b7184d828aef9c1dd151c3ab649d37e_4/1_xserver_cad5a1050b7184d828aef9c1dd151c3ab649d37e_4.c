ProcPseudoramiXGetScreenSize(ClientPtr client)
{
    REQUEST(xPanoramiXGetScreenSizeReq);
    WindowPtr pWin;
    xPanoramiXGetScreenSizeReply rep;
    register int rc;
 
     TRACE;
 
     if (stuff->screen >= pseudoramiXNumScreens)
       return BadMatch;
 
    REQUEST_SIZE_MATCH(xPanoramiXGetScreenSizeReq);
     rc = dixLookupWindow(&pWin, stuff->window, client, DixGetAttrAccess);
     if (rc != Success)
         return rc;
    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    /* screen dimensions */
    rep.width = pseudoramiXScreens[stuff->screen].w;
    rep.height = pseudoramiXScreens[stuff->screen].h;
    rep.window = stuff->window;
    rep.screen = stuff->screen;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.width);
        swapl(&rep.height);
        swapl(&rep.window);
        swapl(&rep.screen);
    }
    WriteToClient(client, sizeof(xPanoramiXGetScreenSizeReply),&rep);
    return Success;
}
