static MagickBooleanType WriteCALSImage(const ImageInfo *image_info,
  Image *image)
{
  char
    header[MaxTextExtent];

  Image
    *group4_image;

  ImageInfo
    *write_info;

  MagickBooleanType
    status;

  register ssize_t
    i;

  size_t
    density,
    length,
    orient_x,
    orient_y;

  ssize_t
    count;

  unsigned char
    *group4;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  /*
    Create standard CALS header.
  */
  count=WriteCALSRecord(image,"srcdocid: NONE");
  (void) count;
  count=WriteCALSRecord(image,"dstdocid: NONE");
  count=WriteCALSRecord(image,"txtfilid: NONE");
  count=WriteCALSRecord(image,"figid: NONE");
  count=WriteCALSRecord(image,"srcgph: NONE");
  count=WriteCALSRecord(image,"doccls: NONE");
  count=WriteCALSRecord(image,"rtype: 1");
  orient_x=0;
  orient_y=0;
  switch (image->orientation)
  {
    case TopRightOrientation:
    {
      orient_x=180;
      orient_y=270;
      break;
    }
    case BottomRightOrientation:
    {
      orient_x=180;
      orient_y=90;
      break;
    }
    case BottomLeftOrientation:
    {
      orient_y=90;
      break;
    }
    case LeftTopOrientation:
    {
      orient_x=270;
      break;
    }
    case RightTopOrientation:
    {
      orient_x=270;
      orient_y=180;
      break;
    }
    case RightBottomOrientation:
    {
      orient_x=90;
      orient_y=180;
      break;
    }
    case LeftBottomOrientation:
    {
      orient_x=90;
      break;
    }
    default:
    {
      orient_y=270;
      break;
    }
  }
  (void) FormatLocaleString(header,sizeof(header),"rorient: %03ld,%03ld",
    (long) orient_x,(long) orient_y);
  count=WriteCALSRecord(image,header);
  (void) FormatLocaleString(header,sizeof(header),"rpelcnt: %06lu,%06lu",
    (unsigned long) image->columns,(unsigned long) image->rows);
  count=WriteCALSRecord(image,header);
  density=200;
  if (image_info->density != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      (void) ParseGeometry(image_info->density,&geometry_info);
      density=(size_t) floor(geometry_info.rho+0.5);
    }
  (void) FormatLocaleString(header,sizeof(header),"rdensty: %04lu",
    (unsigned long) density);
  count=WriteCALSRecord(image,header);
  count=WriteCALSRecord(image,"notes: NONE");
  (void) ResetMagickMemory(header,' ',128);
  for (i=0; i < 5; i++)
    (void) WriteBlob(image,128,(unsigned char *) header);
  /*
    Write CALS pixels.
  */
  write_info=CloneImageInfo(image_info);
  (void) CopyMagickString(write_info->filename,"GROUP4:",MaxTextExtent);
  (void) CopyMagickString(write_info->magick,"GROUP4",MaxTextExtent);
   group4_image=CloneImage(image,0,0,MagickTrue,&image->exception);
   if (group4_image == (Image *) NULL)
     {
       (void) CloseBlob(image);
       return(MagickFalse);
     }
  group4=(unsigned char *) ImageToBlob(write_info,group4_image,&length,
    &image->exception);
   group4_image=DestroyImage(group4_image);
   if (group4 == (unsigned char *) NULL)
     {
       (void) CloseBlob(image);
       return(MagickFalse);
     }
  write_info=DestroyImageInfo(write_info);
  if (WriteBlob(image,length,group4) != (ssize_t) length)
    status=MagickFalse;
  group4=(unsigned char *) RelinquishMagickMemory(group4);
  (void) CloseBlob(image);
  return(status);
}
