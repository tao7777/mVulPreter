ProcXResQueryResourceBytes (ClientPtr client)
{
    REQUEST(xXResQueryResourceBytesReq);

    int                          rc;
     ConstructResourceBytesCtx    ctx;
 
     REQUEST_AT_LEAST_SIZE(xXResQueryResourceBytesReq);
     REQUEST_FIXED_SIZE(xXResQueryResourceBytesReq,
                        stuff->numSpecs * sizeof(ctx.specs[0]));
 
                                       (void*) ((char*) stuff +
                                                sz_xXResQueryResourceBytesReq))) {
        return BadAlloc;
    }

    rc = ConstructResourceBytes(stuff->client, &ctx);

    if (rc == Success) {
        xXResQueryResourceBytesReply rep = {
            .type = X_Reply,
            .sequenceNumber = client->sequence,
            .length = bytes_to_int32(ctx.resultBytes),
            .numSizes = ctx.numSizes
        };

        if (client->swapped) {
            swaps (&rep.sequenceNumber);
            swapl (&rep.length);
            swapl (&rep.numSizes);

            SwapXResQueryResourceBytes(&ctx.response);
        }

        WriteToClient(client, sizeof(rep), &rep);
        WriteFragmentsToClient(client, &ctx.response);
    }

    DestroyConstructResourceBytesCtx(&ctx);

    return rc;
}
