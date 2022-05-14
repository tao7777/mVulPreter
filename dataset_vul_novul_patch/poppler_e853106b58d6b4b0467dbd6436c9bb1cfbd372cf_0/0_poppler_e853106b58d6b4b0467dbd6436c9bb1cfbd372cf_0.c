Gfx::Gfx(XRef *xrefA, OutputDev *outA, Dict *resDict, Catalog *catalogA,
	 PDFRectangle *box, PDFRectangle *cropBox,
	 GBool (*abortCheckCbkA)(void *data),
	 void *abortCheckCbkDataA)
 #ifdef USE_CMS
 : iccColorSpaceCache(5)
#endif
{
  int i;

  xref = xrefA;
  catalog = catalogA;
  subPage = gTrue;
  printCommands = globalParams->getPrintCommands();
  profileCommands = globalParams->getProfileCommands();
  textHaveCSPattern = gFalse;
  drawText = gFalse;
   drawText = gFalse;
   maskHaveCSPattern = gFalse;
   mcStack = NULL;
  parser = NULL;
 
   res = new GfxResources(xref, resDict, NULL);
  out = outA;
  state = new GfxState(72, 72, box, 0, gFalse);
  stackHeight = 1;
  pushStateGuard();
  fontChanged = gFalse;
  clip = clipNone;
  ignoreUndef = 0;
  for (i = 0; i < 6; ++i) {
    baseMatrix[i] = state->getCTM()[i];
  }
  formDepth = 0;
  abortCheckCbk = abortCheckCbkA;
  abortCheckCbkData = abortCheckCbkDataA;

  if (cropBox) {
    state->moveTo(cropBox->x1, cropBox->y1);
    state->lineTo(cropBox->x2, cropBox->y1);
    state->lineTo(cropBox->x2, cropBox->y2);
    state->lineTo(cropBox->x1, cropBox->y2);
    state->closePath();
    state->clip();
    out->clip(state);
    state->clearPath();
  }
}
