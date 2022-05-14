static MagickBooleanType WriteSIXELImage(const ImageInfo *image_info,Image *image)
{
  ExceptionInfo
    *exception;

  MagickBooleanType
    status;

  register const IndexPacket
    *indexes;

  register ssize_t
    i,
    x;

  ssize_t
    opacity,
    y;

  sixel_output_t
    *output;

  unsigned char
    sixel_palette[256 * 3],
    *sixel_pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  exception=(&image->exception);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    (void) TransformImageColorspace(image,sRGBColorspace);
  opacity=(-1);
  if (image->matte == MagickFalse)
    {
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) SetImageType(image,PaletteType);
    }
  else
    {
      MagickRealType
        alpha,
        beta;

      /*
        Identify transparent colormap index.
      */
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) SetImageType(image,PaletteBilevelMatteType);
      for (i=0; i < (ssize_t) image->colors; i++)
        if (image->colormap[i].opacity != OpaqueOpacity)
          {
            if (opacity < 0)
              {
                opacity=i;
                continue;
              }
            alpha=(MagickRealType) image->colormap[i].opacity;
            beta=(MagickRealType) image->colormap[opacity].opacity;
            if (alpha > beta)
              opacity=i;
          }
      if (opacity == -1)
        {
          (void) SetImageType(image,PaletteBilevelMatteType);
          for (i=0; i < (ssize_t) image->colors; i++)
            if (image->colormap[i].opacity != OpaqueOpacity)
              {
                if (opacity < 0)
                  {
                    opacity=i;
                    continue;
                  }
                alpha=(MagickRealType) image->colormap[i].opacity;
                beta=(MagickRealType) image->colormap[opacity].opacity;
                if (alpha > beta)
                  opacity=i;
              }
        }
      if (opacity >= 0)
        {
          image->colormap[opacity].red=image->transparent_color.red;
          image->colormap[opacity].green=image->transparent_color.green;
          image->colormap[opacity].blue=image->transparent_color.blue;
        }
    }
  /*
    SIXEL header.
  */
  for (i=0; i < (ssize_t) image->colors; i++)
  {
    sixel_palette[i * 3 + 0] = ScaleQuantumToChar(image->colormap[i].red);
    sixel_palette[i * 3 + 1] = ScaleQuantumToChar(image->colormap[i].green);
    sixel_palette[i * 3 + 2] = ScaleQuantumToChar(image->colormap[i].blue);
  }

  /*
     Define SIXEL pixels.
   */
   output = sixel_output_create(image);
  sixel_pixels =(unsigned char *) AcquireQuantumMemory(image->columns , image->rows);
   for (y=0; y < (ssize_t) image->rows; y++)
   {
     (void) GetVirtualPixels(image,0,y,image->columns,1,exception);
    indexes=GetVirtualIndexQueue(image);
    for (x=0; x < (ssize_t) image->columns; x++)
      sixel_pixels[y * image->columns + x] = (unsigned char) ((ssize_t) GetPixelIndex(indexes + x));
  }
  status = sixel_encode_impl(sixel_pixels, image->columns, image->rows,
                          sixel_palette, image->colors, -1,
                          output);
  sixel_pixels =(unsigned char *) RelinquishMagickMemory(sixel_pixels);
  output = (sixel_output_t *) RelinquishMagickMemory(output);
  (void) CloseBlob(image);
  return(status);
}
