static Image *ReadOneJNGImage(MngInfo *mng_info,
    const ImageInfo *image_info, ExceptionInfo *exception)
{
  Image
    *alpha_image,
    *color_image,
    *image,
    *jng_image;

  ImageInfo
    *alpha_image_info,
    *color_image_info;

  MagickBooleanType
    logging;

  int
    unique_filenames;

  ssize_t
    y;

  MagickBooleanType
    status;

  png_uint_32
    jng_height,
    jng_width;

  png_byte
    jng_color_type,
    jng_image_sample_depth,
    jng_image_compression_method,
    jng_image_interlace_method,
    jng_alpha_sample_depth,
    jng_alpha_compression_method,
    jng_alpha_filter_method,
    jng_alpha_interlace_method;

  register const PixelPacket
    *s;

  register ssize_t
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  unsigned int
    read_JSEP,
    reading_idat;

  size_t
    length;

  jng_alpha_compression_method=0;
  jng_alpha_sample_depth=8;
  jng_color_type=0;
  jng_height=0;
  jng_width=0;
  alpha_image=(Image *) NULL;
  color_image=(Image *) NULL;
  alpha_image_info=(ImageInfo *) NULL;
  color_image_info=(ImageInfo *) NULL;
  unique_filenames=0;

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
    "  Enter ReadOneJNGImage()");

  image=mng_info->image;

  if (GetAuthenticPixelQueue(image) != (PixelPacket *) NULL)
    {
      /*
        Allocate next image structure.
      */
      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "  AcquireNextImage()");

      AcquireNextImage(image_info,image);

      if (GetNextImageInList(image) == (Image *) NULL)
        return(DestroyImageList(image));

      image=SyncNextImageInList(image);
    }
  mng_info->image=image;

  /*
    Signature bytes have already been read.
  */

  read_JSEP=MagickFalse;
  reading_idat=MagickFalse;
  for (;;)
  {
    char
      type[MaxTextExtent];

    unsigned char
      *chunk;

    unsigned int
      count;

    /*
      Read a new JNG chunk.
    */
    status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
      2*GetBlobSize(image));

    if (status == MagickFalse)
      break;

    type[0]='\0';
    (void) ConcatenateMagickString(type,"errr",MaxTextExtent);
    length=ReadBlobMSBLong(image);
    count=(unsigned int) ReadBlob(image,4,(unsigned char *) type);

    if (logging != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Reading JNG chunk type %c%c%c%c, length: %.20g",
        type[0],type[1],type[2],type[3],(double) length);

    if (length > PNG_UINT_31_MAX || count == 0)
      ThrowReaderException(CorruptImageError,"CorruptImage");

    p=NULL;
    chunk=(unsigned char *) NULL;
 
     if (length != 0)
       {
         chunk=(unsigned char *) AcquireQuantumMemory(length+MagickPathExtent,
           sizeof(*chunk));
 
        if (chunk == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

        for (i=0; i < (ssize_t) length; i++)
        {
          int
            c;

          c=ReadBlobByte(image);
          if (c == EOF)
            break;
          chunk[i]=(unsigned char) c;
        }

        p=chunk;
      }

    (void) ReadBlobMSBLong(image);  /* read crc word */

    if (memcmp(type,mng_JHDR,4) == 0)
      {
        if (length == 16)
          {
            jng_width=(size_t) ((p[0] << 24) | (p[1] << 16) |
              (p[2] << 8) | p[3]);
            jng_height=(size_t) ((p[4] << 24) | (p[5] << 16) |
              (p[6] << 8) | p[7]);
            if ((jng_width == 0) || (jng_height == 0))
              ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
            jng_color_type=p[8];
            jng_image_sample_depth=p[9];
            jng_image_compression_method=p[10];
            jng_image_interlace_method=p[11];

            image->interlace=jng_image_interlace_method != 0 ? PNGInterlace :
              NoInterlace;

            jng_alpha_sample_depth=p[12];
            jng_alpha_compression_method=p[13];
            jng_alpha_filter_method=p[14];
            jng_alpha_interlace_method=p[15];

            if (logging != MagickFalse)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    jng_width:      %16lu,    jng_height:     %16lu\n"
                  "    jng_color_type: %16d,     jng_image_sample_depth: %3d\n"
                  "    jng_image_compression_method:%3d",
                  (unsigned long) jng_width, (unsigned long) jng_height,
                  jng_color_type, jng_image_sample_depth,
                  jng_image_compression_method);

                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    jng_image_interlace_method:  %3d"
                  "    jng_alpha_sample_depth:      %3d",
                  jng_image_interlace_method,
                  jng_alpha_sample_depth);

                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    jng_alpha_compression_method:%3d\n"
                  "    jng_alpha_filter_method:     %3d\n"
                  "    jng_alpha_interlace_method:  %3d",
                  jng_alpha_compression_method,
                  jng_alpha_filter_method,
                  jng_alpha_interlace_method);
              }
          }

        if (length != 0)
          chunk=(unsigned char *) RelinquishMagickMemory(chunk);

        continue;
      }


    if ((reading_idat == MagickFalse) && (read_JSEP == MagickFalse) &&
        ((memcmp(type,mng_JDAT,4) == 0) || (memcmp(type,mng_JdAA,4) == 0) ||
         (memcmp(type,mng_IDAT,4) == 0) || (memcmp(type,mng_JDAA,4) == 0)))
      {
        /*
           o create color_image
           o open color_blob, attached to color_image
           o if (color type has alpha)
               open alpha_blob, attached to alpha_image
        */

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Creating color_blob.");

        color_image_info=(ImageInfo *)AcquireMagickMemory(sizeof(ImageInfo));

        if (color_image_info == (ImageInfo *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

        GetImageInfo(color_image_info);
        color_image=AcquireImage(color_image_info);

        if (color_image == (Image *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");

        (void) AcquireUniqueFilename(color_image->filename);
        unique_filenames++;
        status=OpenBlob(color_image_info,color_image,WriteBinaryBlobMode,
          exception);

        if (status == MagickFalse)
          {
            color_image=DestroyImage(color_image);
            return(DestroyImageList(image));
          }

        if ((image_info->ping == MagickFalse) && (jng_color_type >= 12))
          {
            alpha_image_info=(ImageInfo *)
              AcquireMagickMemory(sizeof(ImageInfo));

            if (alpha_image_info == (ImageInfo *) NULL)
              {
                color_image=DestroyImage(color_image);
                ThrowReaderException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }

            GetImageInfo(alpha_image_info);
            alpha_image=AcquireImage(alpha_image_info);

            if (alpha_image == (Image *) NULL)
              {
                alpha_image_info=DestroyImageInfo(alpha_image_info);
                color_image=DestroyImage(color_image);
                ThrowReaderException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }

            if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "    Creating alpha_blob.");

            (void) AcquireUniqueFilename(alpha_image->filename);
            unique_filenames++;
            status=OpenBlob(alpha_image_info,alpha_image,WriteBinaryBlobMode,
              exception);

            if (status == MagickFalse)
              {
                alpha_image=DestroyImage(alpha_image);
                alpha_image_info=DestroyImageInfo(alpha_image_info);
                color_image=DestroyImage(color_image);
                return(DestroyImageList(image));
              }

            if (jng_alpha_compression_method == 0)
              {
                unsigned char
                  data[18];

                if (logging != MagickFalse)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "    Writing IHDR chunk to alpha_blob.");

                (void) WriteBlob(alpha_image,8,(const unsigned char *)
                  "\211PNG\r\n\032\n");

                (void) WriteBlobMSBULong(alpha_image,13L);
                PNGType(data,mng_IHDR);
                LogPNGChunk(logging,mng_IHDR,13L);
                PNGLong(data+4,jng_width);
                PNGLong(data+8,jng_height);
                data[12]=jng_alpha_sample_depth;
                data[13]=0; /* color_type gray */
                data[14]=0; /* compression method 0 */
                data[15]=0; /* filter_method 0 */
                data[16]=0; /* interlace_method 0 */
                (void) WriteBlob(alpha_image,17,data);
                (void) WriteBlobMSBULong(alpha_image,crc32(0,data,17));
              }
          }
        reading_idat=MagickTrue;
      }

    if (memcmp(type,mng_JDAT,4) == 0)
      {
        /* Copy chunk to color_image->blob */

        if (logging != MagickFalse)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "    Copying JDAT chunk data to color_blob.");

        if (length != 0)
          {
            (void) WriteBlob(color_image,length,chunk);
            chunk=(unsigned char *) RelinquishMagickMemory(chunk);
          }

        continue;
      }

    if (memcmp(type,mng_IDAT,4) == 0)
      {
        png_byte
           data[5];

        /* Copy IDAT header and chunk data to alpha_image->blob */

        if (alpha_image != NULL && image_info->ping == MagickFalse)
          {
            if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "    Copying IDAT chunk data to alpha_blob.");

            (void) WriteBlobMSBULong(alpha_image,(size_t) length);
            PNGType(data,mng_IDAT);
            LogPNGChunk(logging,mng_IDAT,length);
            (void) WriteBlob(alpha_image,4,data);
            (void) WriteBlob(alpha_image,length,chunk);
            (void) WriteBlobMSBULong(alpha_image,
              crc32(crc32(0,data,4),chunk,(uInt) length));
          }

        if (length != 0)
          chunk=(unsigned char *) RelinquishMagickMemory(chunk);

        continue;
      }

    if ((memcmp(type,mng_JDAA,4) == 0) || (memcmp(type,mng_JdAA,4) == 0))
      {
        /* Copy chunk data to alpha_image->blob */

        if (alpha_image != NULL && image_info->ping == MagickFalse)
          {
            if (logging != MagickFalse)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "    Copying JDAA chunk data to alpha_blob.");

            (void) WriteBlob(alpha_image,length,chunk);
          }

        if (length != 0)
          chunk=(unsigned char *) RelinquishMagickMemory(chunk);

        continue;
      }

    if (memcmp(type,mng_JSEP,4) == 0)
      {
        read_JSEP=MagickTrue;

        if (length != 0)
          chunk=(unsigned char *) RelinquishMagickMemory(chunk);

        continue;
      }

    if (memcmp(type,mng_bKGD,4) == 0)
      {
        if (length == 2)
          {
            image->background_color.red=ScaleCharToQuantum(p[1]);
            image->background_color.green=image->background_color.red;
            image->background_color.blue=image->background_color.red;
          }

        if (length == 6)
          {
            image->background_color.red=ScaleCharToQuantum(p[1]);
            image->background_color.green=ScaleCharToQuantum(p[3]);
            image->background_color.blue=ScaleCharToQuantum(p[5]);
          }

        chunk=(unsigned char *) RelinquishMagickMemory(chunk);
        continue;
      }

    if (memcmp(type,mng_gAMA,4) == 0)
      {
        if (length == 4)
          image->gamma=((float) mng_get_long(p))*0.00001;

        chunk=(unsigned char *) RelinquishMagickMemory(chunk);
        continue;
      }

    if (memcmp(type,mng_cHRM,4) == 0)
      {
        if (length == 32)
          {
            image->chromaticity.white_point.x=0.00001*mng_get_long(p);
            image->chromaticity.white_point.y=0.00001*mng_get_long(&p[4]);
            image->chromaticity.red_primary.x=0.00001*mng_get_long(&p[8]);
            image->chromaticity.red_primary.y=0.00001*mng_get_long(&p[12]);
            image->chromaticity.green_primary.x=0.00001*mng_get_long(&p[16]);
            image->chromaticity.green_primary.y=0.00001*mng_get_long(&p[20]);
            image->chromaticity.blue_primary.x=0.00001*mng_get_long(&p[24]);
            image->chromaticity.blue_primary.y=0.00001*mng_get_long(&p[28]);
          }

        chunk=(unsigned char *) RelinquishMagickMemory(chunk);
        continue;
      }

    if (memcmp(type,mng_sRGB,4) == 0)
      {
        if (length == 1)
          {
            image->rendering_intent=
              Magick_RenderingIntent_from_PNG_RenderingIntent(p[0]);
            image->gamma=1.000f/2.200f;
            image->chromaticity.red_primary.x=0.6400f;
            image->chromaticity.red_primary.y=0.3300f;
            image->chromaticity.green_primary.x=0.3000f;
            image->chromaticity.green_primary.y=0.6000f;
            image->chromaticity.blue_primary.x=0.1500f;
            image->chromaticity.blue_primary.y=0.0600f;
            image->chromaticity.white_point.x=0.3127f;
            image->chromaticity.white_point.y=0.3290f;
          }

        chunk=(unsigned char *) RelinquishMagickMemory(chunk);
        continue;
      }

    if (memcmp(type,mng_oFFs,4) == 0)
      {
        if (length > 8)
          {
            image->page.x=(ssize_t) mng_get_long(p);
            image->page.y=(ssize_t) mng_get_long(&p[4]);

            if ((int) p[8] != 0)
              {
                image->page.x/=10000;
                image->page.y/=10000;
              }
          }

        if (length != 0)
          chunk=(unsigned char *) RelinquishMagickMemory(chunk);

        continue;
      }

    if (memcmp(type,mng_pHYs,4) == 0)
      {
        if (length > 8)
          {
            image->x_resolution=(double) mng_get_long(p);
            image->y_resolution=(double) mng_get_long(&p[4]);
            if ((int) p[8] == PNG_RESOLUTION_METER)
              {
                image->units=PixelsPerCentimeterResolution;
                image->x_resolution=image->x_resolution/100.0f;
                image->y_resolution=image->y_resolution/100.0f;
              }
          }

        chunk=(unsigned char *) RelinquishMagickMemory(chunk);
        continue;
      }

#if 0
    if (memcmp(type,mng_iCCP,4) == 0)
      {
        /* To do: */
        if (length != 0)
          chunk=(unsigned char *) RelinquishMagickMemory(chunk);

        continue;
      }
#endif

    if (length != 0)
      chunk=(unsigned char *) RelinquishMagickMemory(chunk);

    if (memcmp(type,mng_IEND,4))
      continue;

    break;
  }


  /* IEND found */

  /*
    Finish up reading image data:

       o read main image from color_blob.

       o close color_blob.

       o if (color_type has alpha)
            if alpha_encoding is PNG
               read secondary image from alpha_blob via ReadPNG
            if alpha_encoding is JPEG
               read secondary image from alpha_blob via ReadJPEG

       o close alpha_blob.

       o copy intensity of secondary image into
         opacity samples of main image.

       o destroy the secondary image.
  */

  if (color_image_info == (ImageInfo *) NULL)
    {
      assert(color_image == (Image *) NULL);
      assert(alpha_image == (Image *) NULL);
      return(DestroyImageList(image));
    }

  if (color_image == (Image *) NULL)
    {
      assert(alpha_image == (Image *) NULL);
      return(DestroyImageList(image));
    }

  (void) SeekBlob(color_image,0,SEEK_SET);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    Reading jng_image from color_blob.");

  assert(color_image_info != (ImageInfo *) NULL);
  (void) FormatLocaleString(color_image_info->filename,MaxTextExtent,"%s",
    color_image->filename);

  color_image_info->ping=MagickFalse;   /* To do: avoid this */
  jng_image=ReadImage(color_image_info,exception);

  (void) RelinquishUniqueFileResource(color_image->filename);
  unique_filenames--;
  color_image=DestroyImage(color_image);
  color_image_info=DestroyImageInfo(color_image_info);

  if (jng_image == (Image *) NULL)
    return(DestroyImageList(image));

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "    Copying jng_image pixels to main image.");
  image->columns=jng_width;
  image->rows=jng_height;
  length=image->columns*sizeof(PixelPacket);

  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }

  for (y=0; y < (ssize_t) image->rows; y++)
  {
    s=GetVirtualPixels(jng_image,0,y,image->columns,1,&image->exception);
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    (void) CopyMagickMemory(q,s,length);

    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }

  jng_image=DestroyImage(jng_image);

  if (image_info->ping == MagickFalse)
    {
     if (jng_color_type >= 12)
       {
         if (jng_alpha_compression_method == 0)
           {
             png_byte
               data[5];
             (void) WriteBlobMSBULong(alpha_image,0x00000000L);
             PNGType(data,mng_IEND);
             LogPNGChunk(logging,mng_IEND,0L);
             (void) WriteBlob(alpha_image,4,data);
             (void) WriteBlobMSBULong(alpha_image,crc32(0,data,4));
           }

         (void) SeekBlob(alpha_image,0,SEEK_SET);

         if (logging != MagickFalse)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "    Reading opacity from alpha_blob.");

         (void) FormatLocaleString(alpha_image_info->filename,MaxTextExtent,
           "%s",alpha_image->filename);

         jng_image=ReadImage(alpha_image_info,exception);

         if (jng_image != (Image *) NULL)
           for (y=0; y < (ssize_t) image->rows; y++)
           {
             s=GetVirtualPixels(jng_image,0,y,image->columns,1,
                &image->exception);
             q=GetAuthenticPixels(image,0,y,image->columns,1,exception);

             if (image->matte != MagickFalse)
               for (x=(ssize_t) image->columns; x != 0; x--,q++,s++)
                  SetPixelOpacity(q,QuantumRange-
                      GetPixelRed(s));

             else
               for (x=(ssize_t) image->columns; x != 0; x--,q++,s++)
               {
                  SetPixelAlpha(q,GetPixelRed(s));
                  if (GetPixelOpacity(q) != OpaqueOpacity)
                    image->matte=MagickTrue;
               }

             if (SyncAuthenticPixels(image,exception) == MagickFalse)
               break;
           }
         (void) RelinquishUniqueFileResource(alpha_image->filename);
         unique_filenames--;
         alpha_image=DestroyImage(alpha_image);
         alpha_image_info=DestroyImageInfo(alpha_image_info);
         if (jng_image != (Image *) NULL)
           jng_image=DestroyImage(jng_image);
       }
    }

  /* Read the JNG image.  */

  if (mng_info->mng_type == 0)
    {
      mng_info->mng_width=jng_width;
      mng_info->mng_height=jng_height;
    }

  if (image->page.width == 0 && image->page.height == 0)
    {
      image->page.width=jng_width;
      image->page.height=jng_height;
    }

  if (image->page.x == 0 && image->page.y == 0)
    {
      image->page.x=mng_info->x_off[mng_info->object_id];
      image->page.y=mng_info->y_off[mng_info->object_id];
    }

  else
    {
      image->page.y=mng_info->y_off[mng_info->object_id];
    }

  mng_info->image_found++;
  status=SetImageProgress(image,LoadImagesTag,2*TellBlob(image),
    2*GetBlobSize(image));

  if (status == MagickFalse)
    return(DestroyImageList(image));

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  exit ReadOneJNGImage(); unique_filenames=%d",unique_filenames);

  return(image);
}
