SafeAlphaCompositeSolidMask_nx8x8888(
    CARD8      op,
    PicturePtr pSrc,
    PicturePtr pMask,
    PicturePtr pDst,
    INT16      xSrc,
    INT16      ySrc,
    INT16      xMask,
    INT16      yMask,
    INT16      xDst,
    INT16      yDst,
    CARD16     width,
    CARD16     height)
{
    CARD32	src, srca;
    CARD32	*dstLine, *dst, d, dstMask;
    CARD8	*maskLine, *mask, m;
    FbStride	dstStride, maskStride;
    CARD16	w;

    fbComposeGetSolid(pSrc, src, pDst->format);

    dstMask = FbFullMask (pDst->pDrawable->depth);
    srca = src >> 24;
    if (src == 0)
	return;

    fbComposeGetStart (pDst, xDst, yDst, CARD32, dstStride, dstLine, 1);
    fbComposeGetStart (pMask, xMask, yMask, CARD8, maskStride, maskLine, 1);

    if (dstMask == FB_ALLONES && pDst->pDrawable->bitsPerPixel == 32 &&
        width * height > rootless_CompositePixels_threshold &&
        SCREENREC(pDst->pDrawable->pScreen)->imp->CompositePixels)
    {
	void *srcp[2], *destp[2];
	unsigned int dest_rowbytes[2];
	unsigned int fn;

	srcp[0] = &src; srcp[1] = &src;
	/* null rowbytes pointer means use first value as a constant */
	destp[0] = dstLine; destp[1] = dstLine;
	dest_rowbytes[0] = dstStride * 4; dest_rowbytes[1] = dest_rowbytes[0];
	fn = RL_COMPOSITE_FUNCTION(RL_COMPOSITE_OVER, RL_DEPTH_ARGB8888,
                                   RL_DEPTH_A8, RL_DEPTH_ARGB8888);

	if (SCREENREC(pDst->pDrawable->pScreen)->imp->CompositePixels(
                width, height, fn, srcp, NULL,
                maskLine, maskStride,
                destp, dest_rowbytes) == Success)
	{
	    return;
	}
    }

    while (height--)
    {
	dst = dstLine;
	dstLine += dstStride;
	mask = maskLine;
	maskLine += maskStride;
	w = width;

	while (w--)
	{
 }
 
 void
SafeAlphaComposite (CARD8           op,
		    PicturePtr      pSrc,
		    PicturePtr      pMask,
		    PicturePtr      pDst,
		    INT16           xSrc,
		    INT16           ySrc,
		    INT16           xMask,
		    INT16           yMask,
		    INT16           xDst,
		    INT16           yDst,
		    CARD16          width,
		    CARD16          height)
 {
    int oldDepth = pDst->pDrawable->depth;
    int oldFormat = pDst->format;
    
     /*
      * We can use the more optimized fbpict code, but it sets bits above
      * the depth to zero. Temporarily adjust destination depth if needed.
    PicturePtr      pDst,
    INT16           xSrc,
    INT16           ySrc,
    INT16           xMask,
     {
 	pDst->pDrawable->depth = 32;
     }
    
     /* For rootless preserve the alpha in x8r8g8b8 which really is
      * a8r8g8b8
      */
    int		    n;
     {
         pDst->format = PICT_a8r8g8b8;
     }
    
    if (pSrc->pDrawable && pMask->pDrawable &&
	!pSrc->transform && !pMask->transform &&
	!pSrc->alphaMap && !pMask->alphaMap &&
	!pMask->repeat && !pMask->componentAlpha && !pDst->alphaMap &&
	pMask->format == PICT_a8 &&
	pSrc->repeatType == RepeatNormal && 
	pSrc->pDrawable->width == 1 &&
	pSrc->pDrawable->height == 1 &&
	(pDst->format == PICT_a8r8g8b8 ||
	 pDst->format == PICT_x8r8g8b8 ||
	 pDst->format == PICT_a8b8g8r8 ||
	 pDst->format == PICT_x8b8g8r8))
    {
	fbWalkCompositeRegion (op, pSrc, pMask, pDst,
			       xSrc, ySrc, xMask, yMask, xDst, yDst,
			       width, height,
			       TRUE /* srcRepeat */,
			       FALSE /* maskRepeat */,
			       SafeAlphaCompositeSolidMask_nx8x8888);
     }
    else
     {
	fbComposite (op, pSrc, pMask, pDst,
		     xSrc, ySrc, xMask, yMask, xDst, yDst, width, height);
     }
 
    pDst->pDrawable->depth = oldDepth;
     pDst->format = oldFormat;
 }
    }
    /* For rootless preserve the alpha in x8r8g8b8 which really is
     * a8r8g8b8
     */
    if (oldFormat == PICT_x8r8g8b8)
    {
        pDst->format = PICT_a8r8g8b8;
    }



    if (!pSrc->transform && !(pMask && pMask->transform))
    if (!maskAlphaMap && !srcAlphaMap && !dstAlphaMap)
    switch (op) {
    case PictOpSrc:
#ifdef USE_MMX
	if (!pMask && pSrc->format == pDst->format &&
	    pSrc->pDrawable != pDst->pDrawable)
	{
	    func = fbCompositeCopyAreammx;
	}
#endif
	break;
    case PictOpOver:
	if (pMask)
	{
	    if (srcRepeat && 
		pSrc->pDrawable->width == 1 &&
		pSrc->pDrawable->height == 1)
	    {
		srcRepeat = FALSE;
		if (PICT_FORMAT_COLOR(pSrc->format)) {
		    switch (pMask->format) {
		    case PICT_a8:
			switch (pDst->format) {
			case PICT_r5g6b5:
			case PICT_b5g6r5:
#ifdef USE_MMX
			    if (fbHaveMMX())
				func = fbCompositeSolidMask_nx8x0565mmx;
			    else
#endif
			    func = fbCompositeSolidMask_nx8x0565;
			    break;
			case PICT_r8g8b8:
			case PICT_b8g8r8:
			    func = fbCompositeSolidMask_nx8x0888;
			    break;
			case PICT_a8r8g8b8:
			case PICT_x8r8g8b8:
			case PICT_a8b8g8r8:
			case PICT_x8b8g8r8:
			    func = SafeAlphaCompositeSolidMask_nx8x8888;
			    break;
			}
			break;
		    case PICT_a8r8g8b8:
			if (pMask->componentAlpha) {
			    switch (pDst->format) {
			    case PICT_a8r8g8b8:
			    case PICT_x8r8g8b8:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSolidMask_nx8888x8888Cmmx;
				else
#endif
				func = fbCompositeSolidMask_nx8888x8888C;
				break;
			    case PICT_r5g6b5:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSolidMask_nx8888x0565Cmmx;
				else
#endif
				func = fbCompositeSolidMask_nx8888x0565C;
				break;
			    }
			}
			break;
		    case PICT_a8b8g8r8:
			if (pMask->componentAlpha) {
			    switch (pDst->format) {
			    case PICT_a8b8g8r8:
			    case PICT_x8b8g8r8:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSolidMask_nx8888x8888Cmmx;
				else
#endif
				func = fbCompositeSolidMask_nx8888x8888C;
				break;
			    case PICT_b5g6r5:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSolidMask_nx8888x0565Cmmx;
				else
#endif
				func = fbCompositeSolidMask_nx8888x0565C;
				break;
			    }
			}
			break;
		    case PICT_a1:
			switch (pDst->format) {
			case PICT_r5g6b5:
			case PICT_b5g6r5:
			case PICT_r8g8b8:
			case PICT_b8g8r8:
			case PICT_a8r8g8b8:
			case PICT_x8r8g8b8:
			case PICT_a8b8g8r8:
			case PICT_x8b8g8r8:
			    func = fbCompositeSolidMask_nx1xn;
			    break;
			}
			break;
		    }
		}
	    }
	    else /* has mask and non-repeating source */
	    {
		if (pSrc->pDrawable == pMask->pDrawable &&
		    xSrc == xMask && ySrc == yMask &&
		    !pMask->componentAlpha)
		{
		    /* source == mask: non-premultiplied data */
		    switch (pSrc->format) {
		    case PICT_x8b8g8r8:
			switch (pMask->format) {
			case PICT_a8r8g8b8:
			case PICT_a8b8g8r8:
			    switch (pDst->format) {
			    case PICT_a8r8g8b8:
			    case PICT_x8r8g8b8:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSrc_8888RevNPx8888mmx;
#endif
				break;
			    case PICT_r5g6b5:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSrc_8888RevNPx0565mmx;
#endif
				break;
			    }
			    break;
			}
			break;
		    case PICT_x8r8g8b8:
			switch (pMask->format) {
			case PICT_a8r8g8b8:
			case PICT_a8b8g8r8:
			    switch (pDst->format) {
			    case PICT_a8b8g8r8:
			    case PICT_x8b8g8r8:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSrc_8888RevNPx8888mmx;
#endif
				break;
			    case PICT_r5g6b5:
#ifdef USE_MMX
				if (fbHaveMMX())
				    func = fbCompositeSrc_8888RevNPx0565mmx;
#endif
				break;
			    }
			    break;
			}
			break;
		    }
		    break;
	}
	else
	{
		    /* non-repeating source, repeating mask => translucent window */
		    if (maskRepeat &&
			pMask->pDrawable->width == 1 &&
			pMask->pDrawable->height == 1)
		    {
			if (pSrc->format == PICT_x8r8g8b8 &&
			    pDst->format == PICT_x8r8g8b8 &&
			    pMask->format == PICT_a8)
			{
#ifdef USE_MMX
			    if (fbHaveMMX())
				func = fbCompositeSrc_8888x8x8888mmx;
#endif
			}
		    }
		}
	    }
	}
	else /* no mask */
	{
	    if (srcRepeat &&
		pSrc->pDrawable->width == 1 &&
		pSrc->pDrawable->height == 1)
	    {
		/* no mask and repeating source */
	    switch (pSrc->format) {
	    case PICT_a8r8g8b8:
		    switch (pDst->format) {
		    case PICT_a8r8g8b8:
	    case PICT_x8r8g8b8:
#ifdef USE_MMX
			if (fbHaveMMX())
			{
			    srcRepeat = FALSE;
			    func = fbCompositeSolid_nx8888mmx;
			}
#endif
			break;
		    case PICT_r5g6b5:
#ifdef USE_MMX
			if (fbHaveMMX())
			{
			    srcRepeat = FALSE;
			    func = fbCompositeSolid_nx0565mmx;
			}
#endif
			break;
		    }
		    break;
		}
	    }
	    else
	    {
		switch (pSrc->format) {
		case PICT_a8r8g8b8:
		switch (pDst->format) {
		case PICT_a8r8g8b8:
		case PICT_x8r8g8b8:
#ifdef USE_MMX
			if (fbHaveMMX())
			    func = fbCompositeSrc_8888x8888mmx;
			else
#endif
		    func = fbCompositeSrc_8888x8888;
		    break;
		case PICT_r8g8b8:
		    func = fbCompositeSrc_8888x0888;
		    break;
		case PICT_r5g6b5:
		    func = fbCompositeSrc_8888x0565;
		    break;
		}
		break;
		case PICT_x8r8g8b8:
		    switch (pDst->format) {
		    case PICT_a8r8g8b8:
		    case PICT_x8r8g8b8:
#ifdef USE_MMX
			if (fbHaveMMX())
			    func = fbCompositeCopyAreammx;
#endif
			break;
		    }
		case PICT_x8b8g8r8:
		    switch (pDst->format) {
	    case PICT_a8b8g8r8:
	    case PICT_x8b8g8r8:
#ifdef USE_MMX
			if (fbHaveMMX())
			    func = fbCompositeCopyAreammx;
#endif
			break;
		    }
		    break;
		case PICT_a8b8g8r8:
		switch (pDst->format) {
		case PICT_a8b8g8r8:
		case PICT_x8b8g8r8:
#ifdef USE_MMX
			if (fbHaveMMX())
			    func = fbCompositeSrc_8888x8888mmx;
			else
#endif
		    func = fbCompositeSrc_8888x8888;
		    break;
		case PICT_b8g8r8:
		    func = fbCompositeSrc_8888x0888;
		    break;
		case PICT_b5g6r5:
		    func = fbCompositeSrc_8888x0565;
		    break;
		}
		break;
	    case PICT_r5g6b5:
		switch (pDst->format) {
		case PICT_r5g6b5:
		    func = fbCompositeSrc_0565x0565;
		    break;
		}
		break;
	    case PICT_b5g6r5:
		switch (pDst->format) {
		case PICT_b5g6r5:
		    func = fbCompositeSrc_0565x0565;
		    break;
		}
		break;
	    }
	}
	}
	break;
    case PictOpAdd:
	if (pMask == 0)
	{
	    switch (pSrc->format) {
	    case PICT_a8r8g8b8:
		switch (pDst->format) {
		case PICT_a8r8g8b8:
#ifdef USE_MMX
		    if (fbHaveMMX())
			func = fbCompositeSrcAdd_8888x8888mmx;
		    else
#endif
		    func = fbCompositeSrcAdd_8888x8888;
		    break;
		}
		break;
	    case PICT_a8b8g8r8:
		switch (pDst->format) {
		case PICT_a8b8g8r8:
#ifdef USE_MMX
		    if (fbHaveMMX())
			func = fbCompositeSrcAdd_8888x8888mmx;
		    else
#endif
		    func = fbCompositeSrcAdd_8888x8888;
		    break;
		}
		break;
	    case PICT_a8:
		switch (pDst->format) {
		case PICT_a8:
#ifdef USE_MMX
		    if (fbHaveMMX())
			func = fbCompositeSrcAdd_8000x8000mmx;
		    else
#endif
		    func = fbCompositeSrcAdd_8000x8000;
		    break;
		}
		break;
	    case PICT_a1:
		switch (pDst->format) {
		case PICT_a1:
		    func = fbCompositeSrcAdd_1000x1000;
		    break;
		}
		break;
	    }
	}
	break;
    }

    if (!func) {
        /* no fast path, use the general code */
        fbCompositeGeneral(op, pSrc, pMask, pDst, xSrc, ySrc, xMask, yMask, xDst, yDst, width, height);
        pDst->pDrawable->depth = dstDepth;
        pDst->format = oldFormat;
        return;
    }

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
		y_msk = mod (y_msk, pMask->pDrawable->height);
		if (h_this > pMask->pDrawable->height - y_msk)
		    h_this = pMask->pDrawable->height - y_msk;
	    }
	    if (srcRepeat)
	    {
		y_src = mod (y_src, pSrc->pDrawable->height);
		if (h_this > pSrc->pDrawable->height - y_src)
		    h_this = pSrc->pDrawable->height - y_src;
	    }
	    while (w)
	    {
		w_this = w;
		if (maskRepeat)
		{
		    x_msk = mod (x_msk, pMask->pDrawable->width);
		    if (w_this > pMask->pDrawable->width - x_msk)
			w_this = pMask->pDrawable->width - x_msk;
		}
		if (srcRepeat)
		{
		    x_src = mod (x_src, pSrc->pDrawable->width);
		    if (w_this > pSrc->pDrawable->width - x_src)
			w_this = pSrc->pDrawable->width - x_src;
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

    pDst->pDrawable->depth = dstDepth;
    pDst->format = oldFormat;
}
