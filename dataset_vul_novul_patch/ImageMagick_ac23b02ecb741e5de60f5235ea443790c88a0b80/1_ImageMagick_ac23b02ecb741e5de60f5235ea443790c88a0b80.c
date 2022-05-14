static Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
{
  const char
    *option;

  Image
    *image;

  int
    jp2_status;

  MagickBooleanType
    status;

  opj_codec_t
    *jp2_codec;

  opj_codestream_index_t
    *codestream_index = (opj_codestream_index_t *) NULL;

  opj_dparameters_t
    parameters;

  opj_image_t
    *jp2_image;

  opj_stream_t
    *jp2_stream;

  register ssize_t
    i;

  ssize_t
    y;

  unsigned char
    sans[4];

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
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Initialize JP2 codec.
  */
  if (ReadBlob(image,4,sans) != 4)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  (void) SeekBlob(image,SEEK_SET,0);
  if (LocaleCompare(image_info->magick,"JPT") == 0)
    jp2_codec=opj_create_decompress(OPJ_CODEC_JPT);
  else
    if (IsJ2K(sans,4) != MagickFalse)
      jp2_codec=opj_create_decompress(OPJ_CODEC_J2K);
    else
      jp2_codec=opj_create_decompress(OPJ_CODEC_JP2);
  opj_set_warning_handler(jp2_codec,JP2WarningHandler,exception);
  opj_set_error_handler(jp2_codec,JP2ErrorHandler,exception);
  opj_set_default_decoder_parameters(&parameters);
  option=GetImageOption(image_info,"jp2:reduce-factor");
  if (option != (const char *) NULL)
    parameters.cp_reduce=StringToInteger(option);
  option=GetImageOption(image_info,"jp2:quality-layers");
  if (option == (const char *) NULL)
    option=GetImageOption(image_info,"jp2:layer-number");
  if (option != (const char *) NULL)
    parameters.cp_layer=StringToInteger(option);
  if (opj_setup_decoder(jp2_codec,&parameters) == 0)
    {
      opj_destroy_codec(jp2_codec);
      ThrowReaderException(DelegateError,"UnableToManageJP2Stream");
    }
  jp2_stream=opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE,OPJ_TRUE);
  opj_stream_set_read_function(jp2_stream,JP2ReadHandler);
  opj_stream_set_write_function(jp2_stream,JP2WriteHandler);
  opj_stream_set_seek_function(jp2_stream,JP2SeekHandler);
  opj_stream_set_skip_function(jp2_stream,JP2SkipHandler);
  opj_stream_set_user_data(jp2_stream,image,NULL);
  opj_stream_set_user_data_length(jp2_stream,GetBlobSize(image));
  if (opj_read_header(jp2_stream,jp2_codec,&jp2_image) == 0)
    {
      opj_stream_destroy(jp2_stream);
      opj_destroy_codec(jp2_codec);
      ThrowReaderException(DelegateError,"UnableToDecodeImageFile");
    }
  jp2_status=1;
  if ((image->columns != 0) && (image->rows != 0))
    {
      /*
        Extract an area from the image.
      */
      jp2_status=opj_set_decode_area(jp2_codec,jp2_image,
        (OPJ_INT32) image->extract_info.x,(OPJ_INT32) image->extract_info.y,
        (OPJ_INT32) image->extract_info.x+(ssize_t) image->columns,
        (OPJ_INT32) image->extract_info.y+(ssize_t) image->rows);
      if (jp2_status == 0)
        {
          opj_stream_destroy(jp2_stream);
          opj_destroy_codec(jp2_codec);
          opj_image_destroy(jp2_image);
          ThrowReaderException(DelegateError,"UnableToDecodeImageFile");
        }
    }
  if ((image_info->number_scenes != 0) && (image_info->scene != 0))
    jp2_status=opj_get_decoded_tile(jp2_codec,jp2_stream,jp2_image,
      (unsigned int) image_info->scene-1);
  else
    if (image->ping == MagickFalse)
      {
        jp2_status=opj_decode(jp2_codec,jp2_stream,jp2_image);
        if (jp2_status != 0)
          jp2_status=opj_end_decompress(jp2_codec,jp2_stream);
      }
  if (jp2_status == 0)
    {
      opj_stream_destroy(jp2_stream);
      opj_destroy_codec(jp2_codec);
      opj_image_destroy(jp2_image);
      ThrowReaderException(DelegateError,"UnableToDecodeImageFile");
    }
  opj_stream_destroy(jp2_stream);
  for (i=0; i < (ssize_t) jp2_image->numcomps; i++)
  {
    if ((jp2_image->comps[0].dx == 0) || (jp2_image->comps[0].dy == 0) ||
         (jp2_image->comps[0].dx != jp2_image->comps[i].dx) ||
         (jp2_image->comps[0].dy != jp2_image->comps[i].dy) ||
         (jp2_image->comps[0].prec != jp2_image->comps[i].prec) ||
        (jp2_image->comps[0].sgnd != jp2_image->comps[i].sgnd))
       {
         opj_destroy_codec(jp2_codec);
         opj_image_destroy(jp2_image);
        ThrowReaderException(CoderError,"IrregularChannelGeometryNotSupported")
      }
  }
  /*
    Convert JP2 image.
  */
  image->columns=(size_t) jp2_image->comps[0].w;
  image->rows=(size_t) jp2_image->comps[0].h;
  image->depth=jp2_image->comps[0].prec;
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
  image->compression=JPEG2000Compression;
  if (jp2_image->color_space == 2)
    {
      SetImageColorspace(image,GRAYColorspace);
      if (jp2_image->numcomps > 1)
        image->matte=MagickTrue;
    }
  else
    if (jp2_image->color_space == 3)
      SetImageColorspace(image,Rec601YCbCrColorspace);
  if (jp2_image->numcomps > 3)
    image->matte=MagickTrue;
  if (jp2_image->icc_profile_buf != (unsigned char *) NULL)
    {
      StringInfo
        *profile;

      profile=BlobToStringInfo(jp2_image->icc_profile_buf,
        jp2_image->icc_profile_len);
      if (profile != (StringInfo *) NULL)
        SetImageProfile(image,"icc",profile);
    }
  if (image->ping != MagickFalse)
    {
      opj_destroy_codec(jp2_codec);
      opj_image_destroy(jp2_image);
      opj_destroy_cstr_index(&codestream_index);
      return(GetFirstImageInList(image));
    }
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      register ssize_t
        i;

      for (i=0; i < (ssize_t) jp2_image->numcomps; i++)
      {
        double
          pixel,
          scale;

        scale=QuantumRange/(double) ((1UL << jp2_image->comps[i].prec)-1);
        pixel=scale*(jp2_image->comps[i].data[y/jp2_image->comps[i].dy*
          image->columns/jp2_image->comps[i].dx+x/jp2_image->comps[i].dx]+
          (jp2_image->comps[i].sgnd ? 1UL << (jp2_image->comps[i].prec-1) : 0));
        switch (i)
        {
           case 0:
           {
             q->red=ClampToQuantum(pixel);
             q->green=q->red;
             q->blue=q->red;
             q->opacity=OpaqueOpacity;
             break;
           }
           case 1:
           {
             if (jp2_image->numcomps == 2)
               {
                 q->opacity=ClampToQuantum(QuantumRange-pixel);
                 break;
               }
             q->green=ClampToQuantum(pixel);
             break;
           }
           case 2:
           {
             q->blue=ClampToQuantum(pixel);
             break;
           }
           case 3:
           {
             q->opacity=ClampToQuantum(QuantumRange-pixel);
             break;
           }
        }
      }
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  /*
    Free resources.
  */
  opj_destroy_codec(jp2_codec);
  opj_image_destroy(jp2_image);
  opj_destroy_cstr_index(&codestream_index);
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}
