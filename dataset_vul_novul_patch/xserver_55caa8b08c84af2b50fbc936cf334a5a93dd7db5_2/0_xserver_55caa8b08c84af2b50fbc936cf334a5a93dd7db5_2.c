ProcXFixesCopyRegion(ClientPtr client)
{
     RegionPtr pSource, pDestination;
 
     REQUEST(xXFixesCopyRegionReq);
    REQUEST_SIZE_MATCH(xXFixesCopyRegionReq);
 
     VERIFY_REGION(pSource, stuff->source, client, DixReadAccess);
     VERIFY_REGION(pDestination, stuff->destination, client, DixWriteAccess);
    if (!RegionCopy(pDestination, pSource))
        return BadAlloc;

    return Success;
}
