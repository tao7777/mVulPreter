SProcXSendExtensionEvent(ClientPtr client)
 {
     CARD32 *p;
     int i;
    xEvent eventT = { .u.u.type = 0 };
     xEvent *eventP;
     EventSwapPtr proc;
    REQUEST(xSendExtensionEventReq);
    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xSendExtensionEventReq);
    swapl(&stuff->destination);
    swaps(&stuff->count);

    if (stuff->length !=
        bytes_to_int32(sizeof(xSendExtensionEventReq)) + stuff->count +
        bytes_to_int32(stuff->num_events * sizeof(xEvent)))
        return BadLength;

    eventP = (xEvent *) &stuff[1];
    for (i = 0; i < stuff->num_events; i++, eventP++) {
        proc = EventSwapVector[eventP->u.u.type & 0177];
        if (proc == NotImplemented)     /* no swapping proc; invalid event type? */
            return BadValue;
        (*proc) (eventP, &eventT);
        *eventP = eventT;
    }

    p = (CARD32 *) (((xEvent *) &stuff[1]) + stuff->num_events);
    SwapLongs(p, stuff->count);
    return (ProcXSendExtensionEvent(client));
}
