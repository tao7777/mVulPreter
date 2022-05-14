static Image *ReadSVGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  int
    status,
    unique_file;

  ssize_t
    n;

  SVGInfo
    *svg_info;

  unsigned char
    message[MaxTextExtent];

  xmlSAXHandler
    sax_modules;

  xmlSAXHandlerPtr
    sax_handler;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if ((image->x_resolution < MagickEpsilon) ||
      (image->y_resolution < MagickEpsilon))
    {
      GeometryInfo
        geometry_info;

      int
        flags;

      flags=ParseGeometry(SVGDensityGeometry,&geometry_info);
      image->x_resolution=geometry_info.rho;
      image->y_resolution=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        image->y_resolution=image->x_resolution;
    }
  if (LocaleCompare(image_info->magick,"MSVG") != 0)
    {
      const DelegateInfo
        *delegate_info;

      delegate_info=GetDelegateInfo("svg:decode",(char *) NULL,exception);
      if (delegate_info != (const DelegateInfo *) NULL)
        {
          char
            background[MaxTextExtent],
            command[MaxTextExtent],
            density[MaxTextExtent],
            input_filename[MaxTextExtent],
            opacity[MaxTextExtent],
            output_filename[MaxTextExtent],
            unique[MaxTextExtent];

          int
            status;

          struct stat
            attributes;

          /*
            Our best hope for compliance to the SVG standard.
          */
          status=AcquireUniqueSymbolicLink(image->filename,input_filename);
          (void) AcquireUniqueFilename(output_filename);
          (void) AcquireUniqueFilename(unique);
          (void) FormatLocaleString(density,MaxTextExtent,"%.20g,%.20g",
            image->x_resolution,image->y_resolution);
          (void) FormatLocaleString(background,MaxTextExtent,
            "rgb(%.20g%%,%.20g%%,%.20g%%)",
            100.0*QuantumScale*image->background_color.red,
            100.0*QuantumScale*image->background_color.green,
            100.0*QuantumScale*image->background_color.blue);
          (void) FormatLocaleString(opacity,MaxTextExtent,"%.20g",QuantumScale*
            (QuantumRange-image->background_color.opacity));
          (void) FormatLocaleString(command,MaxTextExtent,GetDelegateCommands(
            delegate_info),input_filename,output_filename,density,background,
            opacity,unique);
          status=ExternalDelegateCommand(MagickFalse,image_info->verbose,
            command,(char *) NULL,exception);
          (void) RelinquishUniqueFileResource(unique);
          (void) RelinquishUniqueFileResource(input_filename);
          if ((status == 0) && (stat(output_filename,&attributes) == 0) &&
              (attributes.st_size != 0))
            {
              ImageInfo
                *read_info;

              read_info=CloneImageInfo(image_info);
              (void) CopyMagickString(read_info->filename,output_filename,
                MaxTextExtent);
              image=ReadImage(read_info,exception);
              read_info=DestroyImageInfo(read_info);
              (void) RelinquishUniqueFileResource(output_filename);
              if (image != (Image *) NULL)
                return(image);
            }
          (void) RelinquishUniqueFileResource(output_filename);
        }
      {
#if defined(MAGICKCORE_RSVG_DELEGATE)
#if defined(MAGICKCORE_CAIRO_DELEGATE)
        cairo_surface_t
          *cairo_surface;

        cairo_t
          *cairo_image;

        MemoryInfo
          *pixel_info;

        register unsigned char
          *p;

        RsvgDimensionData
          dimension_info;

        unsigned char
          *pixels;

#else
        GdkPixbuf
          *pixel_buffer;

        register const guchar
          *p;
#endif

        GError
          *error;

        ssize_t
          y;

        PixelPacket
          fill_color;

        register ssize_t
          x;

        register PixelPacket
          *q;

        RsvgHandle
          *svg_handle;

        svg_handle=rsvg_handle_new();
        if (svg_handle == (RsvgHandle *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        rsvg_handle_set_base_uri(svg_handle,image_info->filename);
        if ((image->x_resolution != 90.0) && (image->y_resolution != 90.0))
          rsvg_handle_set_dpi_x_y(svg_handle,image->x_resolution,
            image->y_resolution);
        while ((n=ReadBlob(image,MaxTextExtent,message)) != 0)
        {
          error=(GError *) NULL;
          (void) rsvg_handle_write(svg_handle,message,n,&error);
          if (error != (GError *) NULL)
            g_error_free(error);
        }
        error=(GError *) NULL;
        rsvg_handle_close(svg_handle,&error);
        if (error != (GError *) NULL)
          g_error_free(error);
#if defined(MAGICKCORE_CAIRO_DELEGATE)
        rsvg_handle_get_dimensions(svg_handle,&dimension_info);
        image->columns=image->x_resolution*dimension_info.width/90.0;
        image->rows=image->y_resolution*dimension_info.height/90.0;
        pixel_info=(MemoryInfo *) NULL;
#else
        pixel_buffer=rsvg_handle_get_pixbuf(svg_handle);
        rsvg_handle_free(svg_handle);
         image->columns=gdk_pixbuf_get_width(pixel_buffer);
         image->rows=gdk_pixbuf_get_height(pixel_buffer);
 #endif
        status=SetImageExtent(image,image->columns,image->rows);
        if (status == MagickFalse)
          {
            InheritException(exception,&image->exception);
            return(DestroyImageList(image));
          }
         image->matte=MagickTrue;
         SetImageProperty(image,"svg:base-uri",
           rsvg_handle_get_base_uri(svg_handle));
        if ((image->columns == 0) || (image->rows == 0))
          {
#if !defined(MAGICKCORE_CAIRO_DELEGATE)
            g_object_unref(G_OBJECT(pixel_buffer));
#endif
            g_object_unref(svg_handle);
            ThrowReaderException(MissingDelegateError,
              "NoDecodeDelegateForThisImageFormat");
          }
        if (image_info->ping == MagickFalse)
          {
#if defined(MAGICKCORE_CAIRO_DELEGATE)
            size_t
              stride;

            stride=4*image->columns;
#if defined(MAGICKCORE_PANGOCAIRO_DELEGATE)
            stride=(size_t) cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,
              (int) image->columns);
#endif
            pixel_info=AcquireVirtualMemory(stride,image->rows*sizeof(*pixels));
            if (pixel_info == (MemoryInfo *) NULL)
              {
                g_object_unref(svg_handle);
                ThrowReaderException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
#endif
            (void) SetImageBackgroundColor(image);
#if defined(MAGICKCORE_CAIRO_DELEGATE)
            cairo_surface=cairo_image_surface_create_for_data(pixels,
              CAIRO_FORMAT_ARGB32,(int) image->columns,(int) image->rows, (int)
              stride);
            if (cairo_surface == (cairo_surface_t *) NULL)
              {
                pixel_info=RelinquishVirtualMemory(pixel_info);
                g_object_unref(svg_handle);
                ThrowReaderException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }
            cairo_image=cairo_create(cairo_surface);
            cairo_set_operator(cairo_image,CAIRO_OPERATOR_CLEAR);
            cairo_paint(cairo_image);
            cairo_set_operator(cairo_image,CAIRO_OPERATOR_OVER);
            cairo_scale(cairo_image,image->x_resolution/90.0,
              image->y_resolution/90.0);
            rsvg_handle_render_cairo(svg_handle,cairo_image);
            cairo_destroy(cairo_image);
            cairo_surface_destroy(cairo_surface);
            g_object_unref(svg_handle);
            p=pixels;
#else
            p=gdk_pixbuf_get_pixels(pixel_buffer);
#endif
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
#if defined(MAGICKCORE_CAIRO_DELEGATE)
                fill_color.blue=ScaleCharToQuantum(*p++);
                fill_color.green=ScaleCharToQuantum(*p++);
                fill_color.red=ScaleCharToQuantum(*p++);
#else
                fill_color.red=ScaleCharToQuantum(*p++);
                fill_color.green=ScaleCharToQuantum(*p++);
                fill_color.blue=ScaleCharToQuantum(*p++);
#endif
                fill_color.opacity=QuantumRange-ScaleCharToQuantum(*p++);
#if defined(MAGICKCORE_CAIRO_DELEGATE)
                {
                  double
                    gamma;

                  gamma=1.0-QuantumScale*fill_color.opacity;
                  gamma=PerceptibleReciprocal(gamma);
                  fill_color.blue*=gamma;
                  fill_color.green*=gamma;
                  fill_color.red*=gamma;
                }
#endif
                MagickCompositeOver(&fill_color,fill_color.opacity,q,
                  (MagickRealType) q->opacity,q);
                q++;
              }
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
          }
#if defined(MAGICKCORE_CAIRO_DELEGATE)
        if (pixel_info != (MemoryInfo *) NULL)
          pixel_info=RelinquishVirtualMemory(pixel_info);
#else
        g_object_unref(G_OBJECT(pixel_buffer));
#endif
        (void) CloseBlob(image);
        return(GetFirstImageInList(image));
#endif
      }
    }
  /*
    Open draw file.
  */
  file=(FILE *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  if (unique_file != -1)
    file=fdopen(unique_file,"w");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      (void) CopyMagickString(image->filename,filename,MaxTextExtent);
      ThrowFileException(exception,FileOpenError,"UnableToCreateTemporaryFile",
        image->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Parse SVG file.
  */
  if (image == (Image *) NULL)
    return((Image *) NULL);
  svg_info=AcquireSVGInfo();
  if (svg_info == (SVGInfo *) NULL)
    {
      (void) fclose(file);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  svg_info->file=file;
  svg_info->exception=exception;
  svg_info->image=image;
  svg_info->image_info=image_info;
  svg_info->bounds.width=image->columns;
  svg_info->bounds.height=image->rows;
  if (image_info->size != (char *) NULL)
    (void) CloneString(&svg_info->size,image_info->size);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"begin SAX");
  (void) xmlSubstituteEntitiesDefault(1);
  (void) ResetMagickMemory(&sax_modules,0,sizeof(sax_modules));
  sax_modules.internalSubset=SVGInternalSubset;
  sax_modules.isStandalone=SVGIsStandalone;
  sax_modules.hasInternalSubset=SVGHasInternalSubset;
  sax_modules.hasExternalSubset=SVGHasExternalSubset;
  sax_modules.resolveEntity=SVGResolveEntity;
  sax_modules.getEntity=SVGGetEntity;
  sax_modules.entityDecl=SVGEntityDeclaration;
  sax_modules.notationDecl=SVGNotationDeclaration;
  sax_modules.attributeDecl=SVGAttributeDeclaration;
  sax_modules.elementDecl=SVGElementDeclaration;
  sax_modules.unparsedEntityDecl=SVGUnparsedEntityDeclaration;
  sax_modules.setDocumentLocator=SVGSetDocumentLocator;
  sax_modules.startDocument=SVGStartDocument;
  sax_modules.endDocument=SVGEndDocument;
  sax_modules.startElement=SVGStartElement;
  sax_modules.endElement=SVGEndElement;
  sax_modules.reference=SVGReference;
  sax_modules.characters=SVGCharacters;
  sax_modules.ignorableWhitespace=SVGIgnorableWhitespace;
  sax_modules.processingInstruction=SVGProcessingInstructions;
  sax_modules.comment=SVGComment;
  sax_modules.warning=SVGWarning;
  sax_modules.error=SVGError;
  sax_modules.fatalError=SVGError;
  sax_modules.getParameterEntity=SVGGetParameterEntity;
  sax_modules.cdataBlock=SVGCDataBlock;
  sax_modules.externalSubset=SVGExternalSubset;
  sax_handler=(&sax_modules);
  n=ReadBlob(image,MaxTextExtent,message);
  if (n > 0)
    {
      svg_info->parser=xmlCreatePushParserCtxt(sax_handler,svg_info,(char *)
        message,n,image->filename);
      while ((n=ReadBlob(image,MaxTextExtent,message)) != 0)
      {
        status=xmlParseChunk(svg_info->parser,(char *) message,(int) n,0);
        if (status != 0)
          break;
      }
    }
  (void) xmlParseChunk(svg_info->parser,(char *) message,0,1);
  xmlFreeParserCtxt(svg_info->parser);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"end SAX");
  (void) fclose(file);
  (void) CloseBlob(image);
  image->columns=svg_info->width;
  image->rows=svg_info->height;
  if (exception->severity >= ErrorException)
    {
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  if (image_info->ping == MagickFalse)
    {
      ImageInfo
        *read_info;

      /*
        Draw image.
      */
      image=DestroyImage(image);
      image=(Image *) NULL;
      read_info=CloneImageInfo(image_info);
      SetImageInfoBlob(read_info,(void *) NULL,0);
      if (read_info->density != (char *) NULL)
        read_info->density=DestroyString(read_info->density);
      (void) FormatLocaleString(read_info->filename,MaxTextExtent,"mvg:%s",
        filename);
      image=ReadImage(read_info,exception);
      read_info=DestroyImageInfo(read_info);
      if (image != (Image *) NULL)
        (void) CopyMagickString(image->filename,image_info->filename,
          MaxTextExtent);
    }
  /*
    Relinquish resources.
  */
  if (image != (Image *) NULL)
    {
      if (svg_info->title != (char *) NULL)
        (void) SetImageProperty(image,"svg:title",svg_info->title);
      if (svg_info->comment != (char *) NULL)
        (void) SetImageProperty(image,"svg:comment",svg_info->comment);
    }
  svg_info=DestroySVGInfo(svg_info);
  (void) RelinquishUniqueFileResource(filename);
  return(GetFirstImageInList(image));
}
