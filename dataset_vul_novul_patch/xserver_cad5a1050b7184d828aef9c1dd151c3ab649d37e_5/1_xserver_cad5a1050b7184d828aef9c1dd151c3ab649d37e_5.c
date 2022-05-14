ProcRenderSetPictureFilter(ClientPtr client)
{
    REQUEST(xRenderSetPictureFilterReq);
    PicturePtr pPicture;
    int result;
    xFixed *params;
    int nparams;
    char *name;

    REQUEST_AT_LEAST_SIZE(xRenderSetPictureFilterReq);
    VERIFY_PICTURE(pPicture, stuff->picture, client, DixSetAttrAccess);
     name = (char *) (stuff + 1);
     params = (xFixed *) (name + pad_to_int32(stuff->nbytes));
     nparams = ((xFixed *) stuff + client->req_len) - params;
     result = SetPictureFilter(pPicture, name, stuff->nbytes, params, nparams);
     return result;
 }
