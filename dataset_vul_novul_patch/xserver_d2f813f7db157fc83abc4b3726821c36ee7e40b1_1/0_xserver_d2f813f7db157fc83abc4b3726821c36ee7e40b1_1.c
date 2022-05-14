fbPictureInit (ScreenPtr pScreen, PictFormatPtr formats, int nformats)
{

 	srcRepeat = FALSE;
     if (maskTransform)
 	maskRepeat = FALSE;
 
    fbWalkCompositeRegion (op, pSrc, pMask, pDst, xSrc, ySrc,
			   xMask, yMask, xDst, yDst, width, height,
			   srcRepeat, maskRepeat, func);
 }
