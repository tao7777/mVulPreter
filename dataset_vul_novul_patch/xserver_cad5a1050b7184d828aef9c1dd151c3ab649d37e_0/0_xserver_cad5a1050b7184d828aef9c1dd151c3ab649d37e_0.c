ProcScreenSaverUnsetAttributes(ClientPtr client)
{
#ifdef PANORAMIX
    if (!noPanoramiXExtension) {
        REQUEST(xScreenSaverUnsetAttributesReq);
         PanoramiXRes *draw;
         int rc, i;
 
        REQUEST_SIZE_MATCH(xScreenSaverUnsetAttributesReq);

         rc = dixLookupResourceByClass((void **) &draw, stuff->drawable,
                                       XRC_DRAWABLE, client, DixWriteAccess);
         if (rc != Success)
        for (i = PanoramiXNumScreens - 1; i > 0; i--) {
            stuff->drawable = draw->info[i].id;
            ScreenSaverUnsetAttributes(client);
        }

        stuff->drawable = draw->info[0].id;
    }
#endif

    return ScreenSaverUnsetAttributes(client);
}
