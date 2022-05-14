 SProcXFixesQueryVersion(ClientPtr client)
 {
     REQUEST(xXFixesQueryVersionReq);
    REQUEST_SIZE_MATCH(xXFixesQueryVersionReq);
 
     swaps(&stuff->length);
     swapl(&stuff->majorVersion);
    return (*ProcXFixesVector[stuff->xfixesReqType]) (client);
}
