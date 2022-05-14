static Image *ReadTEXTImage(const ImageInfo *image_info,Image *image,
  char *text,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *p;

  DrawInfo
    *draw_info;

  Image
    *texture;

  MagickBooleanType
    status;

  PointInfo
    delta;

  RectangleInfo
    page;

  ssize_t
    offset;

  TypeMetric
    metrics;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  /*
    Set the page geometry.
  */
  delta.x=DefaultResolution;
  delta.y=DefaultResolution;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      GeometryInfo
        geometry_info;

      MagickStatusType
        flags;

      flags=ParseGeometry(PSDensityGeometry,&geometry_info);
      image->x_resolution=geometry_info.rho;
      image->y_resolution=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->y_resolution=image->x_resolution;
    }
  page.width=612;
  page.height=792;
  page.x=43;
  page.y=43;
  if (image_info->page != (char *) NULL)
    (void) ParseAbsoluteGeometry(image_info->page,&page);
  /*
    Initialize Image structure.
  */
  image->columns=(size_t) floor((((double) page.width*image->x_resolution)/
     delta.x)+0.5);
   image->rows=(size_t) floor((((double) page.height*image->y_resolution)/
     delta.y)+0.5);
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   image->page.x=0;
   image->page.y=0;
   texture=(Image *) NULL;
  if (image_info->texture != (char *) NULL)
    {
      ImageInfo
        *read_info;

      read_info=CloneImageInfo(image_info);
      SetImageInfoBlob(read_info,(void *) NULL,0);
      (void) CopyMagickString(read_info->filename,image_info->texture,
        MaxTextExtent);
      texture=ReadImage(read_info,exception);
      read_info=DestroyImageInfo(read_info);
    }
  /*
    Annotate the text image.
  */
  (void) SetImageBackgroundColor(image);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  (void) CloneString(&draw_info->text,image_info->filename);
  (void) FormatLocaleString(geometry,MaxTextExtent,"0x0%+ld%+ld",(long) page.x,
    (long) page.y);
  (void) CloneString(&draw_info->geometry,geometry);
  status=GetTypeMetrics(image,draw_info,&metrics);
  if (status == MagickFalse)
    ThrowReaderException(TypeError,"UnableToGetTypeMetrics");
  page.y=(ssize_t) ceil((double) page.y+metrics.ascent-0.5);
  (void) FormatLocaleString(geometry,MaxTextExtent,"0x0%+ld%+ld",(long) page.x,
    (long) page.y);
  (void) CloneString(&draw_info->geometry,geometry);
  (void) CopyMagickString(filename,image_info->filename,MaxTextExtent);
  if (*draw_info->text != '\0')
    *draw_info->text='\0';
  p=text;
  for (offset=2*page.y; p != (char *) NULL; )
  {
    /*
      Annotate image with text.
    */
    (void) ConcatenateString(&draw_info->text,text);
    (void) ConcatenateString(&draw_info->text,"\n");
    offset+=(ssize_t) (metrics.ascent-metrics.descent);
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,LoadImageTag,offset,image->rows);
        if (status == MagickFalse)
          break;
      }
    p=ReadBlobString(image,text);
    if ((offset < (ssize_t) image->rows) && (p != (char *) NULL))
      continue;
    if (texture != (Image *) NULL)
      {
        MagickProgressMonitor
          progress_monitor;

        progress_monitor=SetImageProgressMonitor(image,
          (MagickProgressMonitor) NULL,image->client_data);
        (void) TextureImage(image,texture);
        (void) SetImageProgressMonitor(image,progress_monitor,
          image->client_data);
      }
    (void) AnnotateImage(image,draw_info);
    if (p == (char *) NULL)
      break;
    /*
      Page is full-- allocate next image structure.
    */
    *draw_info->text='\0';
    offset=2*page.y;
    AcquireNextImage(image_info,image);
    if (GetNextImageInList(image) == (Image *) NULL)
      {
        image=DestroyImageList(image);
        return((Image *) NULL);
      }
    image->next->columns=image->columns;
    image->next->rows=image->rows;
    image=SyncNextImageInList(image);
    (void) CopyMagickString(image->filename,filename,MaxTextExtent);
    (void) SetImageBackgroundColor(image);
    status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
      GetBlobSize(image));
    if (status == MagickFalse)
      break;
  }
  if (texture != (Image *) NULL)
    {
      MagickProgressMonitor
        progress_monitor;

      progress_monitor=SetImageProgressMonitor(image,
        (MagickProgressMonitor) NULL,image->client_data);
      (void) TextureImage(image,texture);
      (void) SetImageProgressMonitor(image,progress_monitor,image->client_data);
    }
  (void) AnnotateImage(image,draw_info);
  if (texture != (Image *) NULL)
    texture=DestroyImage(texture);
  draw_info=DestroyDrawInfo(draw_info);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
