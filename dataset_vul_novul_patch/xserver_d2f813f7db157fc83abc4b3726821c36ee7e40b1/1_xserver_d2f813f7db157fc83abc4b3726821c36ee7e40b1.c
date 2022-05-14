fbCompositeGeneral (CARD8	op,
		    PicturePtr	pSrc,
		    PicturePtr	pMask,
		    PicturePtr	pDst,
		    INT16	xSrc,
		    INT16	ySrc,
		    INT16	xMask,
		    INT16	yMask,
		    INT16	xDst,
		    INT16	yDst,
 		    CARD16	width,
 		    CARD16	height)
 {
    RegionRec	    region;
    int		    n;
    BoxPtr	    pbox;
    Bool	    srcRepeat = FALSE;
    Bool	    maskRepeat = FALSE;
    int		    w, h;
    CARD32 _scanline_buffer[SCANLINE_BUFFER_LENGTH*3];
    CARD32 *scanline_buffer = _scanline_buffer;
    FbComposeData compose_data;
    if (pSrc->pDrawable)
        srcRepeat = pSrc->repeatType == RepeatNormal && !pSrc->transform
	    && (pSrc->pDrawable->width != 1 || pSrc->pDrawable->height != 1);
    if (pMask && pMask->pDrawable)
	maskRepeat = pMask->repeatType == RepeatNormal && !pMask->transform
	    && (pMask->pDrawable->width != 1 || pMask->pDrawable->height != 1);
    if (op == PictOpOver && !pMask && !pSrc->transform && !PICT_FORMAT_A(pSrc->format) && !pSrc->alphaMap)
        op = PictOpSrc;
    if (!miComputeCompositeRegion (&region,
				   pSrc,
				   pMask,
				   pDst,
				   xSrc,
				   ySrc,
				   xMask,
				   yMask,
				   xDst,
				   yDst,
				   width,
				   height))
	return;
    compose_data.op = op;
    compose_data.src = pSrc;
    compose_data.mask = pMask;
    compose_data.dest = pDst;
    if (width > SCANLINE_BUFFER_LENGTH)
        scanline_buffer = (CARD32 *) malloc(width * 3 * sizeof(CARD32));
    n = REGION_NUM_RECTS (&region);
    pbox = REGION_RECTS (&region);
    while (n--)
    {
	h = pbox->y2 - pbox->y1;
	compose_data.ySrc = pbox->y1 - yDst + ySrc;
	compose_data.yMask = pbox->y1 - yDst + yMask;
	compose_data.yDest = pbox->y1;
	while (h)
	{
	    compose_data.height = h;
	    w = pbox->x2 - pbox->x1;
	    compose_data.xSrc = pbox->x1 - xDst + xSrc;
	    compose_data.xMask = pbox->x1 - xDst + xMask;
	    compose_data.xDest = pbox->x1;
	    if (maskRepeat)
	    {
		compose_data.yMask = mod (compose_data.yMask, pMask->pDrawable->height);
		if (compose_data.height > pMask->pDrawable->height - compose_data.yMask)
		    compose_data.height = pMask->pDrawable->height - compose_data.yMask;
	    }
	    if (srcRepeat)
	    {
		compose_data.ySrc = mod (compose_data.ySrc, pSrc->pDrawable->height);
		if (compose_data.height > pSrc->pDrawable->height - compose_data.ySrc)
		    compose_data.height = pSrc->pDrawable->height - compose_data.ySrc;
	    }
	    while (w)
	    {
		compose_data.width = w;
		if (maskRepeat)
		{
		    compose_data.xMask = mod (compose_data.xMask, pMask->pDrawable->width);
		    if (compose_data.width > pMask->pDrawable->width - compose_data.xMask)
			compose_data.width = pMask->pDrawable->width - compose_data.xMask;
		}
		if (srcRepeat)
		{
		    compose_data.xSrc = mod (compose_data.xSrc, pSrc->pDrawable->width);
		    if (compose_data.width > pSrc->pDrawable->width - compose_data.xSrc)
			compose_data.width = pSrc->pDrawable->width - compose_data.xSrc;
		}
		fbCompositeRect(&compose_data, scanline_buffer);
		w -= compose_data.width;
		compose_data.xSrc += compose_data.width;
		compose_data.xMask += compose_data.width;
		compose_data.xDest += compose_data.width;
	    }
	    h -= compose_data.height;
	    compose_data.ySrc += compose_data.height;
	    compose_data.yMask += compose_data.height;
	    compose_data.yDest += compose_data.height;
	}
	pbox++;
    }
    REGION_UNINIT (pDst->pDrawable->pScreen, &region);
    if (scanline_buffer != _scanline_buffer)
        free(scanline_buffer);
 }
