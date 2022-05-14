 SProcXFixesChangeSaveSet(ClientPtr client)
 {
     REQUEST(xXFixesChangeSaveSetReq);
 
     swaps(&stuff->length);
     swapl(&stuff->window);
}
