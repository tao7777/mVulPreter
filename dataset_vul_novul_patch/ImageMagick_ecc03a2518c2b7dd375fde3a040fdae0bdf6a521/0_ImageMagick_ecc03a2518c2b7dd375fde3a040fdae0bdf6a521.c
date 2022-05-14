static Image *ReadPWPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
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
    magick[MaxTextExtent];

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
  pwp_image=AcquireImage(image_info);
  image=pwp_image;
  status=OpenBlob(image_info,pwp_image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    return((Image *) NULL);
  count=ReadBlob(pwp_image,5,magick);
  if ((count != 5) || (LocaleNCompare((char *) magick,"SFW95",5) != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  read_info=CloneImageInfo(image_info);
  (void) SetImageInfoProgressMonitor(read_info,(MagickProgressMonitor) NULL,
    (void *) NULL);
  SetImageInfoBlob(read_info,(void *) NULL,0);
  unique_file=AcquireUniqueFileResource(read_info->filename);
  for ( ; ; )
  {
    for (c=ReadBlobByte(pwp_image); c != EOF; c=ReadBlobByte(pwp_image))
    {
      for (i=0; i < 17; i++)
        magick[i]=magick[i+1];
      magick[17]=(unsigned char) c;
      if (LocaleNCompare((char *) (magick+12),"SFW94A",6) == 0)
        break;
    }
    if (c == EOF)
      break;
    if (LocaleNCompare((char *) (magick+12),"SFW94A",6) != 0)
      {
        (void) RelinquishUniqueFileResource(read_info->filename);
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
        (void) RelinquishUniqueFileResource(read_info->filename);
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
      (void) fputc(c,file);
    }
    (void) fclose(file);
    next_image=ReadImage(read_info,exception);
    if (next_image == (Image *) NULL)
      break;
    (void) FormatLocaleString(next_image->filename,MaxTextExtent,
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
   (void) RelinquishUniqueFileResource(read_info->filename);
   read_info=DestroyImageInfo(read_info);
   if (EOFBlob(image) != MagickFalse)
     {
       char
        *message;

      message=GetExceptionMessage(errno);
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        "UnexpectedEndOfFile","`%s': %s",image->filename,message);
      message=DestroyString(message);
    }
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
