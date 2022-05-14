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
    return fbComposite (op, pSrc, pMask, pDst,
			xSrc, ySrc, xMask, yMask, xDst, yDst,
			width, height);
 }
