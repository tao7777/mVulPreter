 SProcXFixesQueryVersion(ClientPtr client)
 {
     REQUEST(xXFixesQueryVersionReq);
 
     swaps(&stuff->length);
     swapl(&stuff->majorVersion);
    return (*ProcXFixesVector[stuff->xfixesReqType]) (client);
}
