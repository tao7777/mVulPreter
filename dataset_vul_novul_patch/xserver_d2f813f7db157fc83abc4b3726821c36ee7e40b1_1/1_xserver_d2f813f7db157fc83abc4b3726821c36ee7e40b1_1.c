fbPictureInit (ScreenPtr pScreen, PictFormatPtr formats, int nformats)
{

 	srcRepeat = FALSE;
     if (maskTransform)
 	maskRepeat = FALSE;
    if (!miComputeCompositeRegion (&region, pSrc, pMask, pDst, xSrc, ySrc,
				   xMask, yMask, xDst, yDst, width, height))
        return;
 
    n = REGION_NUM_RECTS (&region);
    pbox = REGION_RECTS (&region);
    while (n--)
    {
	h = pbox->y2 - pbox->y1;
	y_src = pbox->y1 - yDst + ySrc;
	y_msk = pbox->y1 - yDst + yMask;
	y_dst = pbox->y1;
	while (h)
	{
	    h_this = h;
	    w = pbox->x2 - pbox->x1;
	    x_src = pbox->x1 - xDst + xSrc;
	    x_msk = pbox->x1 - xDst + xMask;
	    x_dst = pbox->x1;
	    if (maskRepeat)
	    {
		y_msk = mod (y_msk - pMask->pDrawable->y, pMask->pDrawable->height);
		if (h_this > pMask->pDrawable->height - y_msk)
		    h_this = pMask->pDrawable->height - y_msk;
		y_msk += pMask->pDrawable->y;
	    }
	    if (srcRepeat)
	    {
		y_src = mod (y_src - pSrc->pDrawable->y, pSrc->pDrawable->height);
		if (h_this > pSrc->pDrawable->height - y_src)
		    h_this = pSrc->pDrawable->height - y_src;
		y_src += pSrc->pDrawable->y;
	    }
	    while (w)
	    {
		w_this = w;
		if (maskRepeat)
		{
		    x_msk = mod (x_msk - pMask->pDrawable->x, pMask->pDrawable->width);
		    if (w_this > pMask->pDrawable->width - x_msk)
			w_this = pMask->pDrawable->width - x_msk;
		    x_msk += pMask->pDrawable->x;
		}
		if (srcRepeat)
		{
		    x_src = mod (x_src - pSrc->pDrawable->x, pSrc->pDrawable->width);
		    if (w_this > pSrc->pDrawable->width - x_src)
			w_this = pSrc->pDrawable->width - x_src;
		    x_src += pSrc->pDrawable->x;
		}
		(*func) (op, pSrc, pMask, pDst,
			 x_src, y_src, x_msk, y_msk, x_dst, y_dst,
			 w_this, h_this);
		w -= w_this;
		x_src += w_this;
		x_msk += w_this;
		x_dst += w_this;
	    }
	    h -= h_this;
	    y_src += h_this;
	    y_msk += h_this;
	    y_dst += h_this;
	}
	pbox++;
    }
    REGION_UNINIT (pDst->pDrawable->pScreen, &region);
 }
