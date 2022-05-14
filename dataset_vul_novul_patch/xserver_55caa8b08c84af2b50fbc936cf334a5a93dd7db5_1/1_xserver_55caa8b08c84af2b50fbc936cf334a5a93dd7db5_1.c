 SProcXFixesSelectCursorInput(ClientPtr client)
 {
     REQUEST(xXFixesSelectCursorInputReq);
 
     swaps(&stuff->length);
     swapl(&stuff->window);
    return (*ProcXFixesVector[stuff->xfixesReqType]) (client);
}
