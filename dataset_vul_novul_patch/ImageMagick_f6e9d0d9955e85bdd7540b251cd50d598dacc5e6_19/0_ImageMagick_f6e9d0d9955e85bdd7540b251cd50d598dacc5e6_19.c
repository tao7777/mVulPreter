static Image *ReadFPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  FPXColorspace
    colorspace;

  FPXImageComponentDesc
    *alpha_component,
    *blue_component,
    *green_component,
    *red_component;

  FPXImageDesc
    fpx_info;

  FPXImageHandle
    *flashpix;

  FPXStatus
    fpx_status;

  FPXSummaryInformation
    summary_info;

  Image
    *image;

  IndexPacket
    index;

  MagickBooleanType
    status;

  register IndexPacket
    *indexes;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *a,
    *b,
    *g,
    *r;

  size_t
    memory_limit;

  ssize_t
    y;

  unsigned char
    *pixels;

  unsigned int
    height,
    tile_width,
    tile_height,
    width;

  size_t
    scene;

  /*
    Open image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  (void) CloseBlob(image);
  /*
    Initialize FPX toolkit.
  */
  fpx_status=FPX_InitSystem();
  if (fpx_status != FPX_OK)
    ThrowReaderException(CoderError,"UnableToInitializeFPXLibrary");
  memory_limit=20000000;
  fpx_status=FPX_SetToolkitMemoryLimit(&memory_limit);
  if (fpx_status != FPX_OK)
    {
      FPX_ClearSystem();
      ThrowReaderException(CoderError,"UnableToInitializeFPXLibrary");
    }
  tile_width=64;
  tile_height=64;
  flashpix=(FPXImageHandle *) NULL;
  {
#if defined(macintosh)
    FSSpec
      fsspec;

    FilenameToFSSpec(image->filename,&fsspec);
    fpx_status=FPX_OpenImageByFilename((const FSSpec &) fsspec,(char *) NULL,
#else
    fpx_status=FPX_OpenImageByFilename(image->filename,(char *) NULL,
#endif
      &width,&height,&tile_width,&tile_height,&colorspace,&flashpix);
  }
  if (fpx_status == FPX_LOW_MEMORY_ERROR)
    {
      FPX_ClearSystem();
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  if (fpx_status != FPX_OK)
    {
      FPX_ClearSystem();
      ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
        image->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if (colorspace.numberOfComponents == 0)
    {
      FPX_ClearSystem();
      ThrowReaderException(CorruptImageError,"ImageTypeNotSupported");
    }
  if (image_info->view == (char *) NULL)
    {
      float
        aspect_ratio;

      /*
        Get the aspect ratio.
      */
      aspect_ratio=(float) width/height;
      fpx_status=FPX_GetImageResultAspectRatio(flashpix,&aspect_ratio);
      if (fpx_status != FPX_OK)
        ThrowReaderException(DelegateError,"UnableToReadAspectRatio");
      if (width != (size_t) floor((aspect_ratio*height)+0.5))
        Swap(width,height);
    }
  fpx_status=FPX_GetSummaryInformation(flashpix,&summary_info);
  if (fpx_status != FPX_OK)
    {
      FPX_ClearSystem();
      ThrowReaderException(DelegateError,"UnableToReadSummaryInfo");
    }
  if (summary_info.title_valid)
    if ((summary_info.title.length != 0) &&
        (summary_info.title.ptr != (unsigned char *) NULL))
      {
        char
          *label;

        /*
          Note image label.
        */
        label=(char *) NULL;
        if (~summary_info.title.length >= (MaxTextExtent-1))
          label=(char *) AcquireQuantumMemory(summary_info.title.length+
            MaxTextExtent,sizeof(*label));
        if (label == (char *) NULL)
          {
            FPX_ClearSystem();
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) CopyMagickString(label,(char *) summary_info.title.ptr,
          summary_info.title.length+1);
        (void) SetImageProperty(image,"label",label);
        label=DestroyString(label);
      }
  if (summary_info.comments_valid)
    if ((summary_info.comments.length != 0) &&
        (summary_info.comments.ptr != (unsigned char *) NULL))
      {
        char
          *comments;

        /*
          Note image comment.
        */
        comments=(char *) NULL;
        if (~summary_info.comments.length >= (MaxTextExtent-1))
          comments=(char *) AcquireQuantumMemory(summary_info.comments.length+
            MaxTextExtent,sizeof(*comments));
        if (comments == (char *) NULL)
          {
            FPX_ClearSystem();
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) CopyMagickString(comments,(char *) summary_info.comments.ptr,
          summary_info.comments.length+1);
        (void) SetImageProperty(image,"comment",comments);
        comments=DestroyString(comments);
      }
  /*
    Determine resolution by scene specification.
  */
  for (i=1; ; i++)
    if (((width >> i) < tile_width) || ((height >> i) < tile_height))
      break;
  scene=i;
  if (image_info->number_scenes != 0)
    while (scene > image_info->scene)
    {
      width>>=1;
      height>>=1;
      scene--;
    }
  if (image_info->size != (char *) NULL)
    while ((width > image->columns) || (height > image->rows))
    {
      width>>=1;
      height>>=1;
      scene--;
    }
  image->depth=8;
  image->columns=width;
  image->rows=height;
  if ((colorspace.numberOfComponents % 2) == 0)
    image->matte=MagickTrue;
  if (colorspace.numberOfComponents == 1)
    {
      /*
        Create linear colormap.
      */
      if (AcquireImageColormap(image,MaxColormapSize) == MagickFalse)
        {
          FPX_ClearSystem();
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
    }
  if (image_info->ping != MagickFalse)
    {
      (void) FPX_CloseImage(flashpix);
       FPX_ClearSystem();
       return(GetFirstImageInList(image));
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   /*
     Allocate memory for the image and pixel buffer.
   */
  pixels=(unsigned char *) AcquireQuantumMemory(image->columns,(tile_height+
    1UL)*colorspace.numberOfComponents*sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    {
      FPX_ClearSystem();
      (void) FPX_CloseImage(flashpix);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Initialize FlashPix image description.
  */
  fpx_info.numberOfComponents=colorspace.numberOfComponents;
  for (i=0; i < 4; i++)
  {
    fpx_info.components[i].myColorType.myDataType=DATA_TYPE_UNSIGNED_BYTE;
    fpx_info.components[i].horzSubSampFactor=1;
    fpx_info.components[i].vertSubSampFactor=1;
    fpx_info.components[i].columnStride=fpx_info.numberOfComponents;
    fpx_info.components[i].lineStride=image->columns*
      fpx_info.components[i].columnStride;
    fpx_info.components[i].theData=pixels+i;
  }
  fpx_info.components[0].myColorType.myColor=fpx_info.numberOfComponents > 2 ?
    NIFRGB_R : MONOCHROME;
  red_component=(&fpx_info.components[0]);
  fpx_info.components[1].myColorType.myColor=fpx_info.numberOfComponents > 2 ?
    NIFRGB_G : ALPHA;
  green_component=(&fpx_info.components[1]);
  fpx_info.components[2].myColorType.myColor=NIFRGB_B;
  blue_component=(&fpx_info.components[2]);
  fpx_info.components[3].myColorType.myColor=ALPHA;
  alpha_component=(&fpx_info.components[fpx_info.numberOfComponents-1]);
  FPX_SetResampleMethod(FPX_LINEAR_INTERPOLATION);
  /*
    Initialize image pixels.
  */
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetAuthenticIndexQueue(image);
    if ((y % tile_height) == 0)
      {
        /*
          Read FPX image tile (with or without viewing affine)..
        */
        if (image_info->view != (char *) NULL)
          fpx_status=FPX_ReadImageRectangle(flashpix,0,y,image->columns,y+
            tile_height-1,scene,&fpx_info);
        else
          fpx_status=FPX_ReadImageTransformRectangle(flashpix,0.0F,
            (float) y/image->rows,(float) image->columns/image->rows,
            (float) (y+tile_height-1)/image->rows,(ssize_t) image->columns,
            (ssize_t) tile_height,&fpx_info);
        if (fpx_status == FPX_LOW_MEMORY_ERROR)
          {
            pixels=(unsigned char *) RelinquishMagickMemory(pixels);
            (void) FPX_CloseImage(flashpix);
            FPX_ClearSystem();
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
      }
    /*
      Transfer a FPX pixels.
    */
    r=red_component->theData+(y % tile_height)*red_component->lineStride;
    g=green_component->theData+(y % tile_height)*green_component->lineStride;
    b=blue_component->theData+(y % tile_height)*blue_component->lineStride;
    a=alpha_component->theData+(y % tile_height)*alpha_component->lineStride;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (fpx_info.numberOfComponents > 2)
        {
          SetPixelRed(q,ScaleCharToQuantum(*r));
          SetPixelGreen(q,ScaleCharToQuantum(*g));
          SetPixelBlue(q,ScaleCharToQuantum(*b));
        }
      else
        {
          index=ScaleCharToQuantum(*r);
          SetPixelIndex(indexes+x,index);
          SetPixelRed(q,index);
          SetPixelGreen(q,index);
          SetPixelBlue(q,index);
        }
      SetPixelOpacity(q,OpaqueOpacity);
      if (image->matte != MagickFalse)
        SetPixelAlpha(q,ScaleCharToQuantum(*a));
      q++;
      r+=red_component->columnStride;
      g+=green_component->columnStride;
      b+=blue_component->columnStride;
      a+=alpha_component->columnStride;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  (void) FPX_CloseImage(flashpix);
  FPX_ClearSystem();
  return(GetFirstImageInList(image));
}
