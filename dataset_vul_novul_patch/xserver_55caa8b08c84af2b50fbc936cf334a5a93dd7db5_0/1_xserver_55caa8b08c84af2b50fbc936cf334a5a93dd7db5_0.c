SProcXFixesCreatePointerBarrier(ClientPtr client)
{
    REQUEST(xXFixesCreatePointerBarrierReq);
    int i;
     int i;
     CARD16 *in_devices = (CARD16 *) &stuff[1];
 
     swaps(&stuff->length);
     swaps(&stuff->num_devices);
     REQUEST_FIXED_SIZE(xXFixesCreatePointerBarrierReq, pad_to_int32(stuff->num_devices));
    swaps(&stuff->x1);
    swaps(&stuff->y1);
    swaps(&stuff->x2);
    swaps(&stuff->y2);
    swapl(&stuff->directions);
    for (i = 0; i < stuff->num_devices; i++) {
        swaps(in_devices + i);
    }

    return ProcXFixesVector[stuff->xfixesReqType] (client);
}
