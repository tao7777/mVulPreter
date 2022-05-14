ProcPutImage(ClientPtr client)
{
    GC *pGC;
    DrawablePtr pDraw;
    long length;                /* length of scanline server padded */
    long lengthProto;           /* length of scanline protocol padded */
    char *tmpImage;

    REQUEST(xPutImageReq);

    REQUEST_AT_LEAST_SIZE(xPutImageReq);
    VALIDATE_DRAWABLE_AND_GC(stuff->drawable, pDraw, DixWriteAccess);
    if (stuff->format == XYBitmap) {
        if ((stuff->depth != 1) ||
            (stuff->leftPad >= (unsigned int) screenInfo.bitmapScanlinePad))
            return BadMatch;
        length = BitmapBytePad(stuff->width + stuff->leftPad);
    }
    else if (stuff->format == XYPixmap) {
        if ((pDraw->depth != stuff->depth) ||
            (stuff->leftPad >= (unsigned int) screenInfo.bitmapScanlinePad))
            return BadMatch;
        length = BitmapBytePad(stuff->width + stuff->leftPad);
        length *= stuff->depth;
    }
    else if (stuff->format == ZPixmap) {
        if ((pDraw->depth != stuff->depth) || (stuff->leftPad != 0))
            return BadMatch;
        length = PixmapBytePad(stuff->width, stuff->depth);
    }
    else {
        client->errorValue = stuff->format;
        return BadValue;
    }

     tmpImage = (char *) &stuff[1];
     lengthProto = length;
 
    if (lengthProto >= (INT32_MAX / stuff->height))
         return BadLength;
 
     if ((bytes_to_int32(lengthProto * stuff->height) +
         bytes_to_int32(sizeof(xPutImageReq))) != client->req_len)
        return BadLength;

    ReformatImage(tmpImage, lengthProto * stuff->height,
                  stuff->format == ZPixmap ? BitsPerPixel(stuff->depth) : 1,
                  ClientOrder(client));

    (*pGC->ops->PutImage) (pDraw, pGC, stuff->depth, stuff->dstX, stuff->dstY,
                           stuff->width, stuff->height,
                           stuff->leftPad, stuff->format, tmpImage);

    return Success;
}
