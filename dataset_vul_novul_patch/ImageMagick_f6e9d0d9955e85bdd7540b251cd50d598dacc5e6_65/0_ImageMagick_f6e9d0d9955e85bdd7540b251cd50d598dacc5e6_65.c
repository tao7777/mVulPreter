static Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent],
    *text;

  const char
    *Text = (char *)
      "abcdefghijklmnopqrstuvwxyz\n"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
      "0123456789.:,;(*!?}^)#${%^&-+@\n";

  const TypeInfo
    *type_info;

  DrawInfo
    *draw_info;

  Image
    *image;

  MagickBooleanType
    status;

  PixelPacket
    background_color;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  ssize_t
    y;

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
  image=AcquireImage(image_info);
  image->columns=800;
  image->rows=480;
  type_info=GetTypeInfo(image_info->filename,exception);
  if ((type_info != (const TypeInfo *) NULL) &&
      (type_info->glyphs != (char *) NULL))
    (void) CopyMagickString(image->filename,type_info->glyphs,MaxTextExtent);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
       image=DestroyImageList(image);
       return((Image *) NULL);
     }
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   /*
     Color canvas with background color
   */
  background_color=image_info->background_color;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
      *q++=background_color;
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
  (void) CopyMagickString(image->magick,image_info->magick,MaxTextExtent);
  (void) CopyMagickString(image->filename,image_info->filename,MaxTextExtent);
  /*
    Prepare drawing commands
  */
  y=20;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->font=AcquireString(image->filename);
  ConcatenateString(&draw_info->primitive,"push graphic-context\n");
  (void) FormatLocaleString(buffer,MaxTextExtent," viewbox 0 0 %.20g %.20g\n",
    (double) image->columns,(double) image->rows);
  ConcatenateString(&draw_info->primitive,buffer);
  ConcatenateString(&draw_info->primitive," font-size 18\n");
  (void) FormatLocaleString(buffer,MaxTextExtent," text 10,%.20g '",(double) y);
  ConcatenateString(&draw_info->primitive,buffer);
  text=EscapeString(Text,'"');
  ConcatenateString(&draw_info->primitive,text);
  text=DestroyString(text);
  (void) FormatLocaleString(buffer,MaxTextExtent,"'\n");
  ConcatenateString(&draw_info->primitive,buffer);
  y+=20*(ssize_t) MultilineCensus((char *) Text)+20;
  for (i=12; i <= 72; i+=6)
  {
    y+=i+12;
    ConcatenateString(&draw_info->primitive," font-size 18\n");
    (void) FormatLocaleString(buffer,MaxTextExtent," text 10,%.20g '%.20g'\n",
      (double) y,(double) i);
    ConcatenateString(&draw_info->primitive,buffer);
    (void) FormatLocaleString(buffer,MaxTextExtent," font-size %.20g\n",
      (double) i);
    ConcatenateString(&draw_info->primitive,buffer);
    (void) FormatLocaleString(buffer,MaxTextExtent," text 50,%.20g "
      "'That which does not destroy me, only makes me stronger.'\n",(double) y);
    ConcatenateString(&draw_info->primitive,buffer);
    if (i >= 24)
      i+=6;
  }
  ConcatenateString(&draw_info->primitive,"pop graphic-context");
  (void) DrawImage(image,draw_info);
  /*
    Relinquish resources.
  */
  draw_info=DestroyDrawInfo(draw_info);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
