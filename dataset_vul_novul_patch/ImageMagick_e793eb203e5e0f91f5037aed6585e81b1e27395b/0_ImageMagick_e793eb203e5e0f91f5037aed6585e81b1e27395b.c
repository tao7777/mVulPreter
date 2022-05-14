static MagickBooleanType ProcessMSLScript(const ImageInfo *image_info,
  Image **image,ExceptionInfo *exception)
{
  char
    message[MagickPathExtent];

  Image
    *msl_image;

  int
    status;

  ssize_t
    n;

  MSLInfo
    msl_info;

  xmlSAXHandler
    sax_modules;

  xmlSAXHandlerPtr
    sax_handler;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image != (Image **) NULL);
  msl_image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,msl_image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
        msl_image->filename);
      msl_image=DestroyImageList(msl_image);
      return(MagickFalse);
    }
  msl_image->columns=1;
  msl_image->rows=1;
  /*
    Parse MSL file.
  */
  (void) ResetMagickMemory(&msl_info,0,sizeof(msl_info));
  msl_info.exception=exception;
  msl_info.image_info=(ImageInfo **) AcquireMagickMemory(
    sizeof(*msl_info.image_info));
  msl_info.draw_info=(DrawInfo **) AcquireMagickMemory(
    sizeof(*msl_info.draw_info));
  /* top of the stack is the MSL file itself */
  msl_info.image=(Image **) AcquireMagickMemory(sizeof(*msl_info.image));
  msl_info.attributes=(Image **) AcquireMagickMemory(
    sizeof(*msl_info.attributes));
  msl_info.group_info=(MSLGroupInfo *) AcquireMagickMemory(
    sizeof(*msl_info.group_info));
  if ((msl_info.image_info == (ImageInfo **) NULL) ||
      (msl_info.image == (Image **) NULL) ||
      (msl_info.attributes == (Image **) NULL) ||
      (msl_info.group_info == (MSLGroupInfo *) NULL))
    ThrowFatalException(ResourceLimitFatalError,"UnableToInterpretMSLImage");
  *msl_info.image_info=CloneImageInfo(image_info);
  *msl_info.draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  *msl_info.attributes=AcquireImage(image_info,exception);
  msl_info.group_info[0].numImages=0;
  /* the first slot is used to point to the MSL file image */
  *msl_info.image=msl_image;
  if (*image != (Image *) NULL)
    MSLPushImage(&msl_info,*image);
  (void) xmlSubstituteEntitiesDefault(1);
  (void) ResetMagickMemory(&sax_modules,0,sizeof(sax_modules));
  sax_modules.internalSubset=MSLInternalSubset;
  sax_modules.isStandalone=MSLIsStandalone;
  sax_modules.hasInternalSubset=MSLHasInternalSubset;
  sax_modules.hasExternalSubset=MSLHasExternalSubset;
  sax_modules.resolveEntity=MSLResolveEntity;
  sax_modules.getEntity=MSLGetEntity;
  sax_modules.entityDecl=MSLEntityDeclaration;
  sax_modules.notationDecl=MSLNotationDeclaration;
  sax_modules.attributeDecl=MSLAttributeDeclaration;
  sax_modules.elementDecl=MSLElementDeclaration;
  sax_modules.unparsedEntityDecl=MSLUnparsedEntityDeclaration;
  sax_modules.setDocumentLocator=MSLSetDocumentLocator;
  sax_modules.startDocument=MSLStartDocument;
  sax_modules.endDocument=MSLEndDocument;
  sax_modules.startElement=MSLStartElement;
  sax_modules.endElement=MSLEndElement;
  sax_modules.reference=MSLReference;
  sax_modules.characters=MSLCharacters;
  sax_modules.ignorableWhitespace=MSLIgnorableWhitespace;
  sax_modules.processingInstruction=MSLProcessingInstructions;
  sax_modules.comment=MSLComment;
  sax_modules.warning=MSLWarning;
  sax_modules.error=MSLError;
  sax_modules.fatalError=MSLError;
  sax_modules.getParameterEntity=MSLGetParameterEntity;
  sax_modules.cdataBlock=MSLCDataBlock;
  sax_modules.externalSubset=MSLExternalSubset;
  sax_handler=(&sax_modules);
  msl_info.parser=xmlCreatePushParserCtxt(sax_handler,&msl_info,(char *) NULL,0,
    msl_image->filename);
  while (ReadBlobString(msl_image,message) != (char *) NULL)
  {
    n=(ssize_t) strlen(message);
    if (n == 0)
      continue;
    status=xmlParseChunk(msl_info.parser,message,(int) n,MagickFalse);
    if (status != 0)
      break;
    (void) xmlParseChunk(msl_info.parser," ",1,MagickFalse);
    if (msl_info.exception->severity >= ErrorException)
      break;
   }
   if (msl_info.exception->severity == UndefinedException)
     (void) xmlParseChunk(msl_info.parser," ",1,MagickTrue);
  /*
    Free resources.
  */
   xmlFreeParserCtxt(msl_info.parser);
   (void) LogMagickEvent(CoderEvent,GetMagickModule(),"end SAX");
   msl_info.group_info=(MSLGroupInfo *) RelinquishMagickMemory(
     msl_info.group_info);
   if (*image == (Image *) NULL)
     *image=(*msl_info.image);
  *msl_info.image_info=DestroyImageInfo(*msl_info.image_info);
  msl_info.image_info=(ImageInfo **) RelinquishMagickMemory(
    msl_info.image_info);
  *msl_info.draw_info=DestroyDrawInfo(*msl_info.draw_info);
  msl_info.draw_info=(DrawInfo **) RelinquishMagickMemory(msl_info.draw_info);
  msl_info.image=(Image **) RelinquishMagickMemory(msl_info.image);
  *msl_info.attributes=DestroyImage(*msl_info.attributes);
  msl_info.attributes=(Image **) RelinquishMagickMemory(msl_info.attributes);
  msl_info.group_info=(MSLGroupInfo *) RelinquishMagickMemory(
    msl_info.group_info);
   if (msl_info.exception->severity != UndefinedException)
     return(MagickFalse);
   return(MagickTrue);
}
