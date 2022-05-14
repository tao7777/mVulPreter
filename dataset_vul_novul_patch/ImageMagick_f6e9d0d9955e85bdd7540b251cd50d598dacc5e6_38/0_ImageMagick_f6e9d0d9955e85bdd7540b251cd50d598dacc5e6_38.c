static Image *ReadMVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define BoundingBox  "viewbox"

  DrawInfo
    *draw_info;

  Image
    *image;

  MagickBooleanType
    status;

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
  if ((image->columns == 0) || (image->rows == 0))
    {
      char
        primitive[MaxTextExtent];

      register char
        *p;

      SegmentInfo
        bounds;

      /*
        Determine size of image canvas.
      */
      while (ReadBlobString(image,primitive) != (char *) NULL)
      {
        for (p=primitive; (*p == ' ') || (*p == '\t'); p++) ;
        if (LocaleNCompare(BoundingBox,p,strlen(BoundingBox)) != 0)
          continue;
        (void) sscanf(p,"viewbox %lf %lf %lf %lf",&bounds.x1,&bounds.y1,
          &bounds.x2,&bounds.y2);
        image->columns=(size_t) floor((bounds.x2-bounds.x1)+0.5);
        image->rows=(size_t) floor((bounds.y2-bounds.y1)+0.5);
        break;
      }
    }
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"MustSpecifyImageSize");
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->affine.sx=image->x_resolution == 0.0 ? 1.0 : image->x_resolution/
    DefaultResolution;
  draw_info->affine.sy=image->y_resolution == 0.0 ? 1.0 : image->y_resolution/
     DefaultResolution;
   image->columns=(size_t) (draw_info->affine.sx*image->columns);
   image->rows=(size_t) (draw_info->affine.sy*image->rows);
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
   if (SetImageBackgroundColor(image) == MagickFalse)
     {
       InheritException(exception,&image->exception);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Render drawing.
  */
  if (GetBlobStreamData(image) == (unsigned char *) NULL)
    draw_info->primitive=FileToString(image->filename,~0UL,exception);
  else
    {
      draw_info->primitive=(char *) AcquireMagickMemory(GetBlobSize(image)+1);
      if (draw_info->primitive != (char *) NULL)
        {
          CopyMagickMemory(draw_info->primitive,GetBlobStreamData(image),
            GetBlobSize(image));
          draw_info->primitive[GetBlobSize(image)]='\0';
        }
     }
  (void) DrawImage(image,draw_info);
  draw_info=DestroyDrawInfo(draw_info);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
