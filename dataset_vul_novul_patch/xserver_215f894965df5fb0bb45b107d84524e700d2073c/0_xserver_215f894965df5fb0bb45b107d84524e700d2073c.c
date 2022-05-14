ProcSendEvent(ClientPtr client)
{
    WindowPtr pWin;
    WindowPtr effectiveFocus = NullWindow;      /* only set if dest==InputFocus */
    DeviceIntPtr dev = PickPointer(client);
    DeviceIntPtr keybd = GetMaster(dev, MASTER_KEYBOARD);
    SpritePtr pSprite = dev->spriteInfo->sprite;

    REQUEST(xSendEventReq);

    REQUEST_SIZE_MATCH(xSendEventReq);

    /* libXext and other extension libraries may set the bit indicating
     * that this event came from a SendEvent request so remove it
     * since otherwise the event type may fail the range checks
     * and cause an invalid BadValue error to be returned.
     *
     * This is safe to do since we later add the SendEvent bit (0x80)
     * back in once we send the event to the client */

    stuff->event.u.u.type &= ~(SEND_EVENT_BIT);

    /* The client's event type must be a core event type or one defined by an
       extension. */

    if (!((stuff->event.u.u.type > X_Reply &&
           stuff->event.u.u.type < LASTEvent) ||
          (stuff->event.u.u.type >= EXTENSION_EVENT_BASE &&
           stuff->event.u.u.type < (unsigned) lastEvent))) {
         client->errorValue = stuff->event.u.u.type;
         return BadValue;
     }
    /* Generic events can have variable size, but SendEvent request holds
       exactly 32B of event data. */
    if (stuff->event.u.u.type == GenericEvent) {
        client->errorValue = stuff->event.u.u.type;
        return BadValue;
    }
     if (stuff->event.u.u.type == ClientMessage &&
         stuff->event.u.u.detail != 8 &&
         stuff->event.u.u.detail != 16 && stuff->event.u.u.detail != 32) {
    }

    if (stuff->destination == PointerWindow)
        pWin = pSprite->win;
    else if (stuff->destination == InputFocus) {
        WindowPtr inputFocus = (keybd) ? keybd->focus->win : NoneWin;

        if (inputFocus == NoneWin)
            return Success;

        /* If the input focus is PointerRootWin, send the event to where
           the pointer is if possible, then perhaps propogate up to root. */
        if (inputFocus == PointerRootWin)
            inputFocus = GetCurrentRootWindow(dev);

        if (IsParent(inputFocus, pSprite->win)) {
            effectiveFocus = inputFocus;
            pWin = pSprite->win;
        }
        else
            effectiveFocus = pWin = inputFocus;
    }
    else
        dixLookupWindow(&pWin, stuff->destination, client, DixSendAccess);

    if (!pWin)
        return BadWindow;
    if ((stuff->propagate != xFalse) && (stuff->propagate != xTrue)) {
        client->errorValue = stuff->propagate;
        return BadValue;
    }
    stuff->event.u.u.type |= SEND_EVENT_BIT;
    if (stuff->propagate) {
        for (; pWin; pWin = pWin->parent) {
            if (XaceHook(XACE_SEND_ACCESS, client, NULL, pWin,
                         &stuff->event, 1))
                return Success;
            if (DeliverEventsToWindow(dev, pWin,
                                      &stuff->event, 1, stuff->eventMask,
                                      NullGrab))
                return Success;
            if (pWin == effectiveFocus)
                return Success;
            stuff->eventMask &= ~wDontPropagateMask(pWin);
            if (!stuff->eventMask)
                break;
        }
    }
    else if (!XaceHook(XACE_SEND_ACCESS, client, NULL, pWin, &stuff->event, 1))
        DeliverEventsToWindow(dev, pWin, &stuff->event,
                              1, stuff->eventMask, NullGrab);
    return Success;
}
