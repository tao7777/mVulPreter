SProcXIBarrierReleasePointer(ClientPtr client)
{
    xXIBarrierReleasePointerInfo *info;
    REQUEST(xXIBarrierReleasePointerReq);
    int i;

    swaps(&stuff->length);
     REQUEST_AT_LEAST_SIZE(xXIBarrierReleasePointerReq);
 
     swapl(&stuff->num_barriers);
    if (stuff->num_barriers > UINT32_MAX / sizeof(xXIBarrierReleasePointerInfo))
        return BadLength;
     REQUEST_FIXED_SIZE(xXIBarrierReleasePointerReq, stuff->num_barriers * sizeof(xXIBarrierReleasePointerInfo));
 
     info = (xXIBarrierReleasePointerInfo*) &stuff[1];
        swapl(&info->barrier);
        swapl(&info->eventid);
    }
