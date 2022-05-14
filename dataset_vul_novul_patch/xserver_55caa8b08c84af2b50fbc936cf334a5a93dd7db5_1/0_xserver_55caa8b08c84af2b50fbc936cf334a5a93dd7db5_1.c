 SProcXFixesSelectCursorInput(ClientPtr client)
 {
     REQUEST(xXFixesSelectCursorInputReq);
    REQUEST_SIZE_MATCH(xXFixesSelectCursorInputReq);
 
     swaps(&stuff->length);
     swapl(&stuff->window);
    return (*ProcXFixesVector[stuff->xfixesReqType]) (client);
}
