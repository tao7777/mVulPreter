static void WriteDDSInfo(Image *image, const size_t pixelFormat,
  const size_t compression, const size_t mipmaps)
{
  char
    software[MaxTextExtent];

  register ssize_t
    i;

  unsigned int
    format,
    caps,
    flags;

  flags=(unsigned int) (DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT |
    DDSD_PIXELFORMAT | DDSD_LINEARSIZE);
  caps=(unsigned int) DDSCAPS_TEXTURE;
  format=(unsigned int) pixelFormat;

  if (mipmaps > 0)
    {
      flags=flags | (unsigned int) DDSD_MIPMAPCOUNT;
      caps=caps | (unsigned int) (DDSCAPS_MIPMAP | DDSCAPS_COMPLEX);
    }

  if (format != DDPF_FOURCC && image->matte)
    format=format | DDPF_ALPHAPIXELS;

  (void) WriteBlob(image,4,(unsigned char *) "DDS ");
  (void) WriteBlobLSBLong(image,124);
  (void) WriteBlobLSBLong(image,flags);
  (void) WriteBlobLSBLong(image,(unsigned int) image->rows);
  (void) WriteBlobLSBLong(image,(unsigned int) image->columns);
 
   if (compression == FOURCC_DXT1)
     (void) WriteBlobLSBLong(image,
             (unsigned int) (MagickMax(1,(image->columns+3)/4) * 8));
   else
     (void) WriteBlobLSBLong(image,
             (unsigned int) (MagickMax(1,(image->columns+3)/4) * 16));
 
   (void) WriteBlobLSBLong(image,0x00);
   (void) WriteBlobLSBLong(image,(unsigned int) mipmaps+1);
  (void) ResetMagickMemory(software,0,sizeof(software));
  (void) strcpy(software,"IMAGEMAGICK");
  (void) WriteBlob(image,44,(unsigned char *) software);

  (void) WriteBlobLSBLong(image,32);
  (void) WriteBlobLSBLong(image,format);

  if (pixelFormat == DDPF_FOURCC)
    {
      (void) WriteBlobLSBLong(image,(unsigned int) compression);
      for(i=0;i < 5;i++) // bitcount / masks
        (void) WriteBlobLSBLong(image,0x00);
    }
  else
    {
      (void) WriteBlobLSBLong(image,0x00);
      if (image->matte)
        {
          (void) WriteBlobLSBLong(image,32);
          (void) WriteBlobLSBLong(image,0xff0000);
          (void) WriteBlobLSBLong(image,0xff00);
          (void) WriteBlobLSBLong(image,0xff);
          (void) WriteBlobLSBLong(image,0xff000000);
        }
      else
        {
          (void) WriteBlobLSBLong(image,24);
          (void) WriteBlobLSBLong(image,0xff);
          (void) WriteBlobLSBLong(image,0x00);
          (void) WriteBlobLSBLong(image,0x00);
          (void) WriteBlobLSBLong(image,0x00);
        }
    }
  
  (void) WriteBlobLSBLong(image,caps);
  for(i=0;i < 4;i++) // ddscaps2 + reserved region
    (void) WriteBlobLSBLong(image,0x00);
}
