static Image *ReadPWPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MagickPathExtent];

  FILE
    *file;

  Image
    *image,
    *next_image,
    *pwp_image;

  ImageInfo
    *read_info;

  int
    c,
    unique_file;

  MagickBooleanType
    status;

  register Image
    *p;

  register ssize_t
    i;

  size_t
    filesize,
    length;

  ssize_t
    count;

  unsigned char
    magick[MagickPathExtent];

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
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  pwp_image=image;
  memset(magick,0,sizeof(magick));
  count=ReadBlob(pwp_image,5,magick);
  if ((count != 5) || (LocaleNCompare((char *) magick,"SFW95",5) != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  read_info=CloneImageInfo(image_info);
  (void) SetImageInfoProgressMonitor(read_info,(MagickProgressMonitor) NULL,
    (void *) NULL);
  SetImageInfoBlob(read_info,(void *) NULL,0);
  unique_file=AcquireUniqueFileResource(filename);
  (void) FormatLocaleString(read_info->filename,MagickPathExtent,"sfw:%s",
    filename);
  for ( ; ; )
  {
    (void) memset(magick,0,sizeof(magick));
    for (c=ReadBlobByte(pwp_image); c != EOF; c=ReadBlobByte(pwp_image))
    {
      for (i=0; i < 17; i++)
        magick[i]=magick[i+1];
      magick[17]=(unsigned char) c;
      if (LocaleNCompare((char *) (magick+12),"SFW94A",6) == 0)
        break;
    }
    if (c == EOF)
      {
        (void) RelinquishUniqueFileResource(filename);
        read_info=DestroyImageInfo(read_info);
        ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
      }
    if (LocaleNCompare((char *) (magick+12),"SFW94A",6) != 0)
      {
        (void) RelinquishUniqueFileResource(filename);
        read_info=DestroyImageInfo(read_info);
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      }
    /*
      Dump SFW image to a temporary file.
    */
    file=(FILE *) NULL;
    if (unique_file != -1)
      file=fdopen(unique_file,"wb");
    if ((unique_file == -1) || (file == (FILE *) NULL))
      {
        (void) RelinquishUniqueFileResource(filename);
        read_info=DestroyImageInfo(read_info);
        ThrowFileException(exception,FileOpenError,"UnableToWriteFile",
          image->filename);
        image=DestroyImageList(image);
        return((Image *) NULL);
      }
    length=fwrite("SFW94A",1,6,file);
    (void) length;
    filesize=65535UL*magick[2]+256L*magick[1]+magick[0];
    for (i=0; i < (ssize_t) filesize; i++)
    {
       c=ReadBlobByte(pwp_image);
       if (c == EOF)
         break;
      if (fputc(c,file) != c)
        break;
     }
     (void) fclose(file);
     if (c == EOF)
      {
        (void) RelinquishUniqueFileResource(filename);
        read_info=DestroyImageInfo(read_info);
        ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
      }
    next_image=ReadImage(read_info,exception);
    if (next_image == (Image *) NULL)
      break;
    (void) FormatLocaleString(next_image->filename,MagickPathExtent,
      "slide_%02ld.sfw",(long) next_image->scene);
    if (image == (Image *) NULL)
      image=next_image;
    else
      {
        /*
          Link image into image list.
        */
        for (p=image; p->next != (Image *) NULL; p=GetNextImageInList(p)) ;
        next_image->previous=p;
        next_image->scene=p->scene+1;
        p->next=next_image;
      }
    if (image_info->number_scenes != 0)
      if (next_image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    status=SetImageProgress(image,LoadImagesTag,TellBlob(pwp_image),
      GetBlobSize(pwp_image));
    if (status == MagickFalse)
      break;
  }
  if (unique_file != -1)
    (void) close(unique_file);
  (void) RelinquishUniqueFileResource(filename);
  read_info=DestroyImageInfo(read_info);
  if (image != (Image *) NULL)
    {
      if (EOFBlob(image) != MagickFalse)
        {
          char
            *message;

          message=GetExceptionMessage(errno);
          (void) ThrowMagickException(exception,GetMagickModule(),
            CorruptImageError,"UnexpectedEndOfFile","`%s': %s",image->filename,
            message);
          message=DestroyString(message);
        }
      (void) CloseBlob(image);
    }
  return(GetFirstImageInList(image));
}
