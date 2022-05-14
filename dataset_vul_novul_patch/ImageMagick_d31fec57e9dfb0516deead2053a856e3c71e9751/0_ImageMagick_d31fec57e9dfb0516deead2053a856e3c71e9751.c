static Image *ReadXCFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    magick[14];

  Image
    *image;

  int
    foundPropEnd = 0;

  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  register ssize_t
    i;

  size_t
    image_type,
    length;

  ssize_t
    count;

  XCFDocInfo
    doc_info;

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
  count=ReadBlob(image,14,(unsigned char *) magick);
  if ((count != 14) ||
      (LocaleNCompare((char *) magick,"gimp xcf",8) != 0))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  (void) ResetMagickMemory(&doc_info,0,sizeof(XCFDocInfo));
  doc_info.exception=exception;
  doc_info.width=ReadBlobMSBLong(image);
  doc_info.height=ReadBlobMSBLong(image);
  if ((doc_info.width > 262144) || (doc_info.height > 262144))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  doc_info.image_type=ReadBlobMSBLong(image);
  /*
    Initialize image attributes.
  */
  image->columns=doc_info.width;
  image->rows=doc_info.height;
  image_type=doc_info.image_type;
  doc_info.file_size=GetBlobSize(image);
  image->compression=NoCompression;
  image->depth=8;
  status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    {
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
  if (image_type == GIMP_RGB)
    ;
  else
    if (image_type == GIMP_GRAY)
      image->colorspace=GRAYColorspace;
    else
      if (image_type == GIMP_INDEXED)
        ThrowReaderException(CoderError,"ColormapTypeNotSupported");
  (void) SetImageOpacity(image,OpaqueOpacity); 
  (void) SetImageBackgroundColor(image);
  /*
    Read properties.
  */
  while ((foundPropEnd == MagickFalse) && (EOFBlob(image) == MagickFalse))
  {
    PropType prop_type = (PropType) ReadBlobMSBLong(image);
    size_t prop_size = ReadBlobMSBLong(image);

    switch (prop_type)
    {
      case PROP_END:
        foundPropEnd=1;
        break;
      case PROP_COLORMAP:
      {
        /* Cannot rely on prop_size here--the value is set incorrectly
           by some Gimp versions.
        */
        size_t num_colours = ReadBlobMSBLong(image);
        if (DiscardBlobBytes(image,3*num_colours) == MagickFalse)
          ThrowFileException(&image->exception,CorruptImageError,
            "UnexpectedEndOfFile",image->filename);
    /*
      if (info->file_version == 0)
      {
        gint i;

        g_message (_("XCF warning: version 0 of XCF file format\n"
           "did not save indexed colormaps correctly.\n"
           "Substituting grayscale map."));
        info->cp +=
          xcf_read_int32 (info->fp, (guint32*) &gimage->num_cols, 1);
        gimage->cmap = g_new (guchar, gimage->num_cols*3);
        xcf_seek_pos (info, info->cp + gimage->num_cols);
        for (i = 0; i<gimage->num_cols; i++)
          {
            gimage->cmap[i*3+0] = i;
            gimage->cmap[i*3+1] = i;
            gimage->cmap[i*3+2] = i;
          }
      }
      else
      {
        info->cp +=
          xcf_read_int32 (info->fp, (guint32*) &gimage->num_cols, 1);
        gimage->cmap = g_new (guchar, gimage->num_cols*3);
        info->cp +=
          xcf_read_int8 (info->fp,
                   (guint8*) gimage->cmap, gimage->num_cols*3);
      }
     */
        break;
      }
      case PROP_COMPRESSION:
      {
        doc_info.compression = ReadBlobByte(image);
        if ((doc_info.compression != COMPRESS_NONE) &&
            (doc_info.compression != COMPRESS_RLE) &&
            (doc_info.compression != COMPRESS_ZLIB) &&
            (doc_info.compression != COMPRESS_FRACTAL))
          ThrowReaderException(CorruptImageError,"UnrecognizedImageCompression");
      }
      break;

      case PROP_GUIDES:
      {
         /* just skip it - we don't care about guides */
        if (DiscardBlobBytes(image,prop_size) == MagickFalse)
          ThrowFileException(&image->exception,CorruptImageError,
            "UnexpectedEndOfFile",image->filename);
      }
      break;

    case PROP_RESOLUTION:
      {
        /* float xres = (float) */ (void) ReadBlobMSBLong(image);
        /* float yres = (float) */ (void) ReadBlobMSBLong(image);

        /*
        if (xres < GIMP_MIN_RESOLUTION || xres > GIMP_MAX_RESOLUTION ||
            yres < GIMP_MIN_RESOLUTION || yres > GIMP_MAX_RESOLUTION)
        {
        g_message ("Warning, resolution out of range in XCF file");
        xres = gimage->gimp->config->default_xresolution;
        yres = gimage->gimp->config->default_yresolution;
        }
        */


        /* BOGUS: we don't write these yet because we aren't
              reading them properly yet :(
              image->x_resolution = xres;
              image->y_resolution = yres;
        */
      }
      break;

    case PROP_TATTOO:
      {
        /* we need to read it, even if we ignore it */
        /*size_t  tattoo_state = */ (void) ReadBlobMSBLong(image);
      }
      break;

    case PROP_PARASITES:
      {
        /* BOGUS: we may need these for IPTC stuff */
        if (DiscardBlobBytes(image,prop_size) == MagickFalse)
          ThrowFileException(&image->exception,CorruptImageError,
            "UnexpectedEndOfFile",image->filename);
        /*
      gssize_t         base = info->cp;
      GimpParasite *p;

      while (info->cp - base < prop_size)
        {
          p = xcf_load_parasite (info);
          gimp_image_parasite_attach (gimage, p);
          gimp_parasite_free (p);
        }
      if (info->cp - base != prop_size)
        g_message ("Error detected while loading an image's parasites");
      */
          }
      break;

    case PROP_UNIT:
      {
        /* BOGUS: ignore for now... */
      /*size_t unit =  */ (void) ReadBlobMSBLong(image);
      }
      break;

    case PROP_PATHS:
      {
      /* BOGUS: just skip it for now */
        if (DiscardBlobBytes(image,prop_size) == MagickFalse)
          ThrowFileException(&image->exception,CorruptImageError,
            "UnexpectedEndOfFile",image->filename);

        /*
      PathList *paths = xcf_load_bzpaths (gimage, info);
      gimp_image_set_paths (gimage, paths);
      */
      }
      break;

    case PROP_USER_UNIT:
      {
        char  unit_string[1000];
        /*BOGUS: ignored for now */
        /*float  factor = (float) */ (void) ReadBlobMSBLong(image);
        /* size_t digits =  */ (void) ReadBlobMSBLong(image);
        for (i=0; i<5; i++)
         (void) ReadBlobStringWithLongSize(image, unit_string,
           sizeof(unit_string));
      }
     break;

      default:
      {
        int buf[16];
        ssize_t amount;

      /* read over it... */
      while ((prop_size > 0) && (EOFBlob(image) == MagickFalse))
      {
        amount=(ssize_t) MagickMin(16, prop_size);
        amount=(ssize_t) ReadBlob(image,(size_t) amount,(unsigned char *) &buf);
        if (!amount)
          ThrowReaderException(CorruptImageError,"CorruptImage");
        prop_size -= (size_t) MagickMin(16,(size_t) amount);
      }
    }
    break;
  }
  }
  if (foundPropEnd == MagickFalse)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");

  if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
    {
      ; /* do nothing, were just pinging! */
    }
  else
    {
      int
        current_layer = 0,
        foundAllLayers = MagickFalse,
        number_layers = 0;

      MagickOffsetType
        oldPos=TellBlob(image);

      XCFLayerInfo
        *layer_info;

      /*
        The read pointer.
      */
      do
      {
        ssize_t offset = ReadBlobMSBSignedLong(image);
        if (offset == 0)
          foundAllLayers=MagickTrue;
        else
          number_layers++;
        if (EOFBlob(image) != MagickFalse)
          {
            ThrowFileException(exception,CorruptImageError,
              "UnexpectedEndOfFile",image->filename);
            break;
          }
    } while (foundAllLayers == MagickFalse);
    doc_info.number_layers=number_layers;
    offset=SeekBlob(image,oldPos,SEEK_SET); /* restore the position! */
    if (offset < 0)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    /* allocate our array of layer info blocks */
    length=(size_t) number_layers;
    layer_info=(XCFLayerInfo *) AcquireQuantumMemory(length,
      sizeof(*layer_info));
    if (layer_info == (XCFLayerInfo *) NULL)
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    (void) ResetMagickMemory(layer_info,0,number_layers*sizeof(XCFLayerInfo));
    for ( ; ; )
    {
      MagickBooleanType
        layer_ok;

      MagickOffsetType
        offset,
        saved_pos;

      /* read in the offset of the next layer */
      offset=(MagickOffsetType) ReadBlobMSBLong(image);
      /* if the offset is 0 then we are at the end
      *  of the layer list.
      */
      if (offset == 0)
        break;
      /* save the current position as it is where the
      *  next layer offset is stored.
      */
      saved_pos=TellBlob(image);
      /* seek to the layer offset */
      if (SeekBlob(image,offset,SEEK_SET) != offset)
        ThrowReaderException(ResourceLimitError,"NotEnoughPixelData");
      /* read in the layer */
      layer_ok=ReadOneLayer(image_info,image,&doc_info,
        &layer_info[current_layer],current_layer);
      if (layer_ok == MagickFalse)
        {
          int j;

          for (j=0; j < current_layer; j++)
            layer_info[j].image=DestroyImage(layer_info[j].image);
          layer_info=(XCFLayerInfo *) RelinquishMagickMemory(layer_info);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      /* restore the saved position so we'll be ready to
      *  read the next offset.
      */
      offset=SeekBlob(image, saved_pos, SEEK_SET);
      current_layer++;
    }
#if 0
        {
        /* NOTE: XCF layers are REVERSED from composite order! */
        signed int  j;
        for (j=number_layers-1; j>=0; j--) {
          /* BOGUS: need to consider layer blending modes!! */

          if ( layer_info[j].visible ) { /* only visible ones, please! */
            CompositeImage(image, OverCompositeOp, layer_info[j].image,
                     layer_info[j].offset_x, layer_info[j].offset_y );
             layer_info[j].image =DestroyImage( layer_info[j].image );

            /* If we do this, we'll get REAL gray images! */
            if ( image_type == GIMP_GRAY ) {
              QuantizeInfo  qi;
              GetQuantizeInfo(&qi);
              qi.colorspace = GRAYColorspace;
              QuantizeImage( &qi, layer_info[j].image );
            }
          }
        }
      }
#else
      {
        /* NOTE: XCF layers are REVERSED from composite order! */
        ssize_t  j;

        /* now reverse the order of the layers as they are put
           into subimages
        */
        for (j=(long) number_layers-1; j >= 0; j--)
          AppendImageToList(&image,layer_info[j].image);
      }
#endif

    layer_info=(XCFLayerInfo *) RelinquishMagickMemory(layer_info);

#if 0  /* BOGUS: do we need the channels?? */
    while (MagickTrue)
    {
      /* read in the offset of the next channel */
      info->cp += xcf_read_int32 (info->fp, &offset, 1);

      /* if the offset is 0 then we are at the end
      *  of the channel list.
      */
      if (offset == 0)
        break;

      /* save the current position as it is where the
      *  next channel offset is stored.
      */
      saved_pos = info->cp;

      /* seek to the channel offset */
      xcf_seek_pos (info, offset);

      /* read in the layer */
      channel = xcf_load_channel (info, gimage);
      if (channel == 0)
        goto error;

      num_successful_elements++;

      /* add the channel to the image if its not the selection */
      if (channel != gimage->selection_mask)
        gimp_image_add_channel (gimage, channel, -1);

      /* restore the saved position so we'll be ready to
      *  read the next offset.
      */
      xcf_seek_pos (info, saved_pos);
    }
#endif
   }
 
   (void) CloseBlob(image);
  if (GetNextImageInList(image) != (Image *) NULL)
    DestroyImage(RemoveFirstImageFromList(&image));
   if (image_type == GIMP_GRAY)
     image->type=GrayscaleType;
   return(GetFirstImageInList(image));
}
