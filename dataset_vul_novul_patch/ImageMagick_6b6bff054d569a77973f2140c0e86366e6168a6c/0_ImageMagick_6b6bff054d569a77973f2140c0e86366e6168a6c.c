static Image *ReadCALSImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MagickPathExtent],
    header[MagickPathExtent],
    message[MagickPathExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *read_info;

  int
    c,
    unique_file;

  MagickBooleanType
    status;

  register ssize_t
    i;

  unsigned long
    density,
    direction,
    height,
    orientation,
    pel_path,
    type,
    width;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read CALS header.
  */
  (void) memset(header,0,sizeof(header));
  density=0;
  direction=0;
  orientation=1;
  pel_path=0;
  type=1;
  width=0;
  height=0;
  for (i=0; i < 16; i++)
  {
    if (ReadBlob(image,128,(unsigned char *) header) != 128)
      break;
    switch (*header)
    {
      case 'R':
      case 'r':
      {
        if (LocaleNCompare(header,"rdensty:",8) == 0)
          {
            (void) sscanf(header+8,"%lu",&density);
            break;
          }
        if (LocaleNCompare(header,"rpelcnt:",8) == 0)
          {
            (void) sscanf(header+8,"%lu,%lu",&width,&height);
            break;
          }
        if (LocaleNCompare(header,"rorient:",8) == 0)
          {
            (void) sscanf(header+8,"%lu,%lu",&pel_path,&direction);
            if (pel_path == 90)
              orientation=5;
            else
              if (pel_path == 180)
                orientation=3;
              else
                if (pel_path == 270)
                  orientation=7;
            if (direction == 90)
              orientation++;
            break;
          }
        if (LocaleNCompare(header,"rtype:",6) == 0)
          {
            (void) sscanf(header+6,"%lu",&type);
            break;
          }
        break;
      }
    }
  }
  /*
    Read CALS pixels.
  */
  file=(FILE *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  if (unique_file != -1)
    file=fdopen(unique_file,"wb");
   if ((unique_file == -1) || (file == (FILE *) NULL))
     ThrowImageException(FileOpenError,"UnableToCreateTemporaryFile");
   while ((c=ReadBlobByte(image)) != EOF)
    if (fputc(c,file) != c)
      break;
   (void) fclose(file);
   (void) CloseBlob(image);
   image=DestroyImage(image);
  read_info=CloneImageInfo(image_info);
  SetImageInfoBlob(read_info,(void *) NULL,0);
  (void) FormatLocaleString(read_info->filename,MagickPathExtent,"group4:%s",
    filename);
  (void) FormatLocaleString(message,MagickPathExtent,"%lux%lu",width,height);
  (void) CloneString(&read_info->size,message);
  (void) FormatLocaleString(message,MagickPathExtent,"%lu",density);
  (void) CloneString(&read_info->density,message);
  read_info->orientation=(OrientationType) orientation;
  image=ReadImage(read_info,exception);
  if (image != (Image *) NULL)
    {
      (void) CopyMagickString(image->filename,image_info->filename,
        MagickPathExtent);
      (void) CopyMagickString(image->magick_filename,image_info->filename,
        MagickPathExtent);
      (void) CopyMagickString(image->magick,"CALS",MagickPathExtent);
    }
  read_info=DestroyImageInfo(read_info);
  (void) RelinquishUniqueFileResource(filename);
  return(image);
}
