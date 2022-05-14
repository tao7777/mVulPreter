static Image *ExtractPostscript(Image *image,const ImageInfo *image_info,
  MagickOffsetType PS_Offset,ssize_t PS_Size,ExceptionInfo *exception)
{
  char
    postscript_file[MaxTextExtent];

  const MagicInfo
    *magic_info;

  FILE
    *ps_file;

  ImageInfo
    *clone_info;

  Image
    *image2;

  unsigned char
    magick[2*MaxTextExtent];


  if ((clone_info=CloneImageInfo(image_info)) == NULL)
    return(image);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;

  /* Obtain temporary file */
  (void) AcquireUniqueFilename(postscript_file);
  ps_file=fopen_utf8(postscript_file,"wb");
  if (ps_file == (FILE *) NULL)
    goto FINISH;

  /* Copy postscript to temporary file */
  (void) SeekBlob(image,PS_Offset,SEEK_SET);
  (void) ReadBlob(image, 2*MaxTextExtent, magick);

  (void) SeekBlob(image,PS_Offset,SEEK_SET);
  while(PS_Size-- > 0)
    {
      (void) fputc(ReadBlobByte(image),ps_file);
    }
  (void) fclose(ps_file);

    /* Detect file format - Check magic.mgk configuration file. */
  magic_info=GetMagicInfo(magick,2*MaxTextExtent,exception);
  if(magic_info == (const MagicInfo *) NULL) goto FINISH_UNL;
  /*     printf("Detected:%s  \n",magic_info->name); */
   if(exception->severity != UndefinedException) goto FINISH_UNL;
   if(magic_info->name == (char *) NULL) goto FINISH_UNL;
 
  (void) strncpy(clone_info->magick,magic_info->name,MaxTextExtent-1);
 
     /* Read nested image */
   /*FormatString(clone_info->filename,"%s:%s",magic_info->name,postscript_file);*/
  FormatLocaleString(clone_info->filename,MaxTextExtent,"%s",postscript_file);
  image2=ReadImage(clone_info,exception);

  if (!image2)
    goto FINISH_UNL;

  /*
    Replace current image with new image while copying base image
    attributes.
  */
  (void) CopyMagickString(image2->filename,image->filename,MaxTextExtent);
  (void) CopyMagickString(image2->magick_filename,image->magick_filename,MaxTextExtent);
  (void) CopyMagickString(image2->magick,image->magick,MaxTextExtent);
  image2->depth=image->depth;
  DestroyBlob(image2);
  image2->blob=ReferenceBlob(image->blob);

  if ((image->rows == 0) || (image->columns == 0))
    DeleteImageFromList(&image);

  AppendImageToList(&image,image2);

 FINISH_UNL:
  (void) RelinquishUniqueFileResource(postscript_file);
 FINISH:
  DestroyImageInfo(clone_info);
  return(image);
}
