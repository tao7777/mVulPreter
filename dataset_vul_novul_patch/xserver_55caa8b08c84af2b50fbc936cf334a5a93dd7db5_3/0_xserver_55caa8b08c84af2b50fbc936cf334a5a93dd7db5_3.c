 SProcXFixesChangeSaveSet(ClientPtr client)
 {
     REQUEST(xXFixesChangeSaveSetReq);
    REQUEST_SIZE_MATCH(xXFixesChangeSaveSetReq);
 
     swaps(&stuff->length);
     swapl(&stuff->window);
}
