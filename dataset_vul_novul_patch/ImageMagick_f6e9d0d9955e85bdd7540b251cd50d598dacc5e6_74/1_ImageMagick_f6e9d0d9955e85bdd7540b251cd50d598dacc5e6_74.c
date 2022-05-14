static Image *ReadWMFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  double
    bounding_height,
    bounding_width,
    image_height,
    image_height_inch,
    image_width,
    image_width_inch,
    resolution_y,
    resolution_x,
    units_per_inch;

  float
    wmf_width,
    wmf_height;

   Image
     *image;
 
   unsigned long
     wmf_options_flags = 0;
 
  wmf_error_t
    wmf_error;

  wmf_magick_t
    *ddata = 0;

  wmfAPI
    *API = 0;

  wmfAPI_Options
    wmf_api_options;

  wmfD_Rect
    bbox;

  image=AcquireImage(image_info);
  if (OpenBlob(image_info,image,ReadBinaryBlobMode,exception) == MagickFalse)
    {
      if (image->debug != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  OpenBlob failed");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "leave ReadWMFImage()");
        }
      image=DestroyImageList(image);
      return((Image *) NULL);
    }

  /*
   * Create WMF API
   *
   */

  /* Register callbacks */
  wmf_options_flags |= WMF_OPT_FUNCTION;
  (void) ResetMagickMemory(&wmf_api_options, 0, sizeof(wmf_api_options));
  wmf_api_options.function = ipa_functions;

  /* Ignore non-fatal errors */
  wmf_options_flags |= WMF_OPT_IGNORE_NONFATAL;

  wmf_error = wmf_api_create(&API, wmf_options_flags, &wmf_api_options);
  if (wmf_error != wmf_E_None)
    {
      if (API)
        wmf_api_destroy(API);
      if (image->debug != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  wmf_api_create failed");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "leave ReadWMFImage()");
        }
      ThrowReaderException(DelegateError,"UnableToInitializeWMFLibrary");
    }

  /* Register progress monitor */
  wmf_status_function(API,image,magick_progress_callback);

  ddata=WMF_MAGICK_GetData(API);
  ddata->image=image;
  ddata->image_info=image_info;
  ddata->draw_info=CloneDrawInfo(image_info,(const DrawInfo *) NULL);
  ddata->draw_info->font=(char *)
    RelinquishMagickMemory(ddata->draw_info->font);
  ddata->draw_info->text=(char *)
    RelinquishMagickMemory(ddata->draw_info->text);

#if defined(MAGICKCORE_WMFLITE_DELEGATE)
  /* Must initialize font subystem for WMFlite interface */
  lite_font_init (API,&wmf_api_options); /* similar to wmf_ipa_font_init in src/font.c */
  /* wmf_arg_fontdirs (API,options); */ /* similar to wmf_arg_fontdirs in src/wmf.c */

#endif

  /*
   * Open BLOB input via libwmf API
   *
   */
  wmf_error = wmf_bbuf_input(API,ipa_blob_read,ipa_blob_seek,
    ipa_blob_tell,(void*)image);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      if (image->debug != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  wmf_bbuf_input failed");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "leave ReadWMFImage()");
        }
      ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
        image->filename);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }

  /*
   * Scan WMF file
   *
   */
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Scanning WMF to obtain bounding box");
  wmf_error=wmf_scan(API, 0, &bbox);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      if (image->debug != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  wmf_scan failed with wmf_error %d", wmf_error);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "leave ReadWMFImage()");
        }
      ThrowReaderException(DelegateError,"FailedToScanFile");
    }

  /*
   * Compute dimensions and scale factors
   *
   */

  ddata->bbox=bbox;

  /* User specified resolution */
  resolution_y=DefaultResolution;
  if (image->y_resolution != 0.0)
    {
      resolution_y = image->y_resolution;
      if (image->units == PixelsPerCentimeterResolution)
        resolution_y *= CENTIMETERS_PER_INCH;
    }
  resolution_x=DefaultResolution;
  if (image->x_resolution != 0.0)
    {
      resolution_x = image->x_resolution;
      if (image->units == PixelsPerCentimeterResolution)
        resolution_x *= CENTIMETERS_PER_INCH;
    }

  /* Obtain output size expressed in metafile units */
  wmf_error=wmf_size(API,&wmf_width,&wmf_height);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      if (image->debug != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  wmf_size failed with wmf_error %d", wmf_error);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "leave ReadWMFImage()");
        }
      ThrowReaderException(DelegateError,"FailedToComputeOutputSize");
    }

  /* Obtain (or guess) metafile units */
  if ((API)->File->placeable)
    units_per_inch=(API)->File->pmh->Inch;
  else if ( (wmf_width*wmf_height) < 1024*1024)
    units_per_inch=POINTS_PER_INCH;  /* MM_TEXT */
  else
    units_per_inch=TWIPS_PER_INCH;  /* MM_TWIPS */

  /* Calculate image width and height based on specified DPI
     resolution */
  image_width_inch  = (double) wmf_width / units_per_inch;
  image_height_inch = (double) wmf_height / units_per_inch;
  image_width       = image_width_inch * resolution_x;
  image_height      = image_height_inch * resolution_y;

  /* Compute bounding box scale factors and origin translations
   *
   * This all just a hack since libwmf does not currently seem to
   * provide the mapping between LOGICAL coordinates and DEVICE
   * coordinates. This mapping is necessary in order to know
   * where to place the logical bounding box within the image.
   *
   */

  bounding_width  = bbox.BR.x - bbox.TL.x;
  bounding_height = bbox.BR.y - bbox.TL.y;

  ddata->scale_x = image_width/bounding_width;
  ddata->translate_x = 0-bbox.TL.x;
  ddata->rotate = 0;

  /* Heuristic: guess that if the vertical coordinates mostly span
     negative values, then the image must be inverted. */
  if ( fabs(bbox.BR.y) > fabs(bbox.TL.y) )
    {
      /* Normal (Origin at top left of image) */
      ddata->scale_y = (image_height/bounding_height);
      ddata->translate_y = 0-bbox.TL.y;
    }
  else
    {
      /* Inverted (Origin at bottom left of image) */
      ddata->scale_y = (-image_height/bounding_height);
      ddata->translate_y = 0-bbox.BR.y;
    }

  if (image->debug != MagickFalse)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
         "  Placeable metafile:          %s",
         (API)->File->placeable ? "Yes" : "No");

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Size in metafile units:      %gx%g",wmf_width,wmf_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Metafile units/inch:         %g",units_per_inch);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Size in inches:              %gx%g",
        image_width_inch,image_height_inch);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Bounding Box:                %g,%g %g,%g",
        bbox.TL.x, bbox.TL.y, bbox.BR.x, bbox.BR.y);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Bounding width x height:     %gx%g",bounding_width,
        bounding_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Output resolution:           %gx%g",resolution_x,resolution_y);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Image size:                  %gx%g",image_width,image_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Bounding box scale factor:   %g,%g",ddata->scale_x,
        ddata->scale_y);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Translation:                 %g,%g",
        ddata->translate_x, ddata->translate_y);
    }

#if 0
#if 0
  {
    typedef struct _wmfPlayer_t wmfPlayer_t;
    struct _wmfPlayer_t
    {
      wmfPen   default_pen;
      wmfBrush default_brush;
      wmfFont  default_font;

      wmfDC* dc; /* current dc */
    };

    wmfDC
      *dc;

#define WMF_ELICIT_DC(API) (((wmfPlayer_t*)((API)->player_data))->dc)

    dc = WMF_ELICIT_DC(API);

    printf("dc->Window.Ox     = %d\n", dc->Window.Ox);
    printf("dc->Window.Oy     = %d\n", dc->Window.Oy);
    printf("dc->Window.width  = %d\n", dc->Window.width);
    printf("dc->Window.height = %d\n", dc->Window.height);
    printf("dc->pixel_width   = %g\n", dc->pixel_width);
    printf("dc->pixel_height  = %g\n", dc->pixel_height);
#if defined(MAGICKCORE_WMFLITE_DELEGATE)  /* Only in libwmf 0.3 */
    printf("dc->Ox            = %.d\n", dc->Ox);
    printf("dc->Oy            = %.d\n", dc->Oy);
    printf("dc->width         = %.d\n", dc->width);
    printf("dc->height        = %.d\n", dc->height);
#endif

  }
#endif

#endif

  /*
   * Create canvas image
   *
   */
  image->rows=(unsigned long) ceil(image_height);
  image->columns=(unsigned long) ceil(image_width);

  if (image_info->ping != MagickFalse)
    {
      wmf_api_destroy(API);
      (void) CloseBlob(image);
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "leave ReadWMFImage()");
       return(GetFirstImageInList(image));
     }
   if (image->debug != MagickFalse)
     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Creating canvas image with size %lux%lu",(unsigned long) image->rows,
       (unsigned long) image->columns);

  /*
   * Set solid background color
   */
  {
    image->background_color = image_info->background_color;
    if (image->background_color.opacity != OpaqueOpacity)
      image->matte = MagickTrue;
    (void) SetImageBackgroundColor(image);
  }
  /*
   * Play file to generate Vector drawing commands
   *
   */

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Playing WMF to prepare vectors");

  wmf_error = wmf_play(API, 0, &bbox);
  if (wmf_error != wmf_E_None)
    {
      wmf_api_destroy(API);
      if (image->debug != MagickFalse)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "  Playing WMF failed with wmf_error %d", wmf_error);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "leave ReadWMFImage()");
        }
      ThrowReaderException(DelegateError,"FailedToRenderFile");
    }

  /*
   * Scribble on canvas image
   *
   */

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Rendering WMF vectors");
  DrawRender(ddata->draw_wand);

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"leave ReadWMFImage()");

  /* Cleanup allocated data */
  wmf_api_destroy(API);
  (void) CloseBlob(image);

  /* Return image */
  return image;
}
