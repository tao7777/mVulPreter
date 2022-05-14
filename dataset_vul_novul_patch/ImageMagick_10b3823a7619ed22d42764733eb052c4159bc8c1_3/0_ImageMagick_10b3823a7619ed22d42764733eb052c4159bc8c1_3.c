static MagickBooleanType WriteGROUP4Image(const ImageInfo *image_info,
  Image *image)
{
  char
    filename[MaxTextExtent];

  FILE
    *file;

  Image
    *huffman_image;

  ImageInfo
    *write_info;

  int
    unique_file;

  MagickBooleanType
    status;

  register ssize_t
    i;

  ssize_t
    count;

  TIFF
    *tiff;

  toff_t
    *byte_count,
    strip_size;

  unsigned char
    *buffer;

  /*
    Write image as CCITT Group4 TIFF image to a temporary file.
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
  huffman_image=CloneImage(image,0,0,MagickTrue,&image->exception);
  if (huffman_image == (Image *) NULL)
    {
      (void) CloseBlob(image);
      return(MagickFalse);
    }
  huffman_image->endian=MSBEndian;
  file=(FILE *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  if (unique_file != -1)
    file=fdopen(unique_file,"wb");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      ThrowFileException(&image->exception,FileOpenError,
        "UnableToCreateTemporaryFile",filename);
      return(MagickFalse);
    }
  (void) FormatLocaleString(huffman_image->filename,MaxTextExtent,"tiff:%s",
    filename);
   (void) SetImageType(huffman_image,BilevelType);
   write_info=CloneImageInfo((ImageInfo *) NULL);
   SetImageInfoFile(write_info,file);
   (void) SetImageDepth(image,1);
  (void) SetImageType(image,BilevelType);
   write_info->compression=Group4Compression;
   write_info->type=BilevelType;
   (void) SetImageOption(write_info,"quantum:polarity","min-is-white");
  status=WriteTIFFImage(write_info,huffman_image);
  (void) fflush(file);
  write_info=DestroyImageInfo(write_info);
  if (status == MagickFalse)
    {
      InheritException(&image->exception,&huffman_image->exception);
      huffman_image=DestroyImage(huffman_image);
      (void) fclose(file);
      (void) RelinquishUniqueFileResource(filename);
      return(MagickFalse);
    }
  tiff=TIFFOpen(filename,"rb");
  if (tiff == (TIFF *) NULL)
    {
      huffman_image=DestroyImage(huffman_image);
      (void) fclose(file);
      (void) RelinquishUniqueFileResource(filename);
      ThrowFileException(&image->exception,FileOpenError,"UnableToOpenFile",
        image_info->filename);
      return(MagickFalse);
    }
  /*
    Allocate raw strip buffer.
  */
  if (TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count) != 1)
    {
      TIFFClose(tiff);
      huffman_image=DestroyImage(huffman_image);
      (void) fclose(file);
      (void) RelinquishUniqueFileResource(filename);
      return(MagickFalse);
    }
  strip_size=byte_count[0];
  for (i=1; i < (ssize_t) TIFFNumberOfStrips(tiff); i++)
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
  buffer=(unsigned char *) AcquireQuantumMemory((size_t) strip_size,
    sizeof(*buffer));
  if (buffer == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      huffman_image=DestroyImage(huffman_image);
      (void) fclose(file);
      (void) RelinquishUniqueFileResource(filename);
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        image_info->filename);
    }
  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  for (i=0; i < (ssize_t) TIFFNumberOfStrips(tiff); i++)
  {
    count=(ssize_t) TIFFReadRawStrip(tiff,(uint32) i,buffer,strip_size);
    if (WriteBlob(image,(size_t) count,buffer) != count)
      status=MagickFalse;
  }
  buffer=(unsigned char *) RelinquishMagickMemory(buffer);
  TIFFClose(tiff);
  huffman_image=DestroyImage(huffman_image);
  (void) fclose(file);
  (void) RelinquishUniqueFileResource(filename);
  (void) CloseBlob(image);
  return(status);
}
