static Image *ReadCAPTIONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    *caption,
    geometry[MaxTextExtent],
    *property,
    *text;

  const char
    *gravity,
    *option;

  DrawInfo
    *draw_info;

  Image
    *image;

  MagickBooleanType
    split,
    status;

  register ssize_t
    i;

  size_t
    height,
    width;

  TypeMetric
    metrics;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  (void) ResetImagePage(image,"0x0+0+0");
  /*
    Format caption.
  */
  option=GetImageOption(image_info,"filename");
  if (option == (const char *) NULL)
    property=InterpretImageProperties(image_info,image,image_info->filename);
  else
    if (LocaleNCompare(option,"caption:",8) == 0)
      property=InterpretImageProperties(image_info,image,option+8);
    else
      property=InterpretImageProperties(image_info,image,option);
  (void) SetImageProperty(image,"caption",property);
  property=DestroyString(property);
  caption=ConstantString(GetImageProperty(image,"caption"));
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  (void) CloneString(&draw_info->text,caption);
  gravity=GetImageOption(image_info,"gravity");
  if (gravity != (char *) NULL)
    draw_info->gravity=(GravityType) ParseCommandOption(MagickGravityOptions,
      MagickFalse,gravity);
  split=MagickFalse;
  status=MagickTrue;
  if (image->columns == 0)
    {
      text=AcquireString(caption);
      i=FormatMagickCaption(image,draw_info,split,&metrics,&text);
      (void) CloneString(&draw_info->text,text);
      text=DestroyString(text);
      (void) FormatLocaleString(geometry,MaxTextExtent,"%+g%+g",
        -metrics.bounds.x1,metrics.ascent);
      if (draw_info->gravity == UndefinedGravity)
        (void) CloneString(&draw_info->geometry,geometry);
      status=GetMultilineTypeMetrics(image,draw_info,&metrics);
      width=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
      image->columns=width;
    }
  if (image->rows == 0)
    {
      split=MagickTrue;
      text=AcquireString(caption);
      i=FormatMagickCaption(image,draw_info,split,&metrics,&text);
      (void) CloneString(&draw_info->text,text);
      text=DestroyString(text);
      (void) FormatLocaleString(geometry,MaxTextExtent,"%+g%+g",
        -metrics.bounds.x1,metrics.ascent);
      if (draw_info->gravity == UndefinedGravity)
        (void) CloneString(&draw_info->geometry,geometry);
      status=GetMultilineTypeMetrics(image,draw_info,&metrics);
      image->rows=(size_t) ((i+1)*(metrics.ascent-metrics.descent+
        draw_info->interline_spacing+draw_info->stroke_width)+0.5);
    }
  if (status != MagickFalse)
    status=SetImageExtent(image,image->columns,image->rows);
  if (status == MagickFalse)
    { 
      draw_info=DestroyDrawInfo(draw_info);
      InheritException(exception,&image->exception);
      return(DestroyImageList(image));
    }
  if (SetImageBackgroundColor(image) == MagickFalse)
    {
      draw_info=DestroyDrawInfo(draw_info);
      InheritException(exception,&image->exception);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if ((fabs(image_info->pointsize) < MagickEpsilon) && (strlen(caption) > 0))
    {
      double
        high,
        low;

      /*
        Auto fit text into bounding box.
      */
      for ( ; ; draw_info->pointsize*=2.0)
      {
        text=AcquireString(caption);
        i=FormatMagickCaption(image,draw_info,split,&metrics,&text);
        (void) CloneString(&draw_info->text,text);
        text=DestroyString(text);
        (void) FormatLocaleString(geometry,MaxTextExtent,"%+g%+g",
          -metrics.bounds.x1,metrics.ascent);
        if (draw_info->gravity == UndefinedGravity)
          (void) CloneString(&draw_info->geometry,geometry);
        status=GetMultilineTypeMetrics(image,draw_info,&metrics);
        (void) status;
        width=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
        height=(size_t) floor(metrics.height+draw_info->stroke_width+0.5);
        if ((image->columns != 0) && (image->rows != 0))
          {
            if ((width >= image->columns) && (height >= image->rows))
              break;
          }
        else
          if (((image->columns != 0) && (width >= image->columns)) ||
              ((image->rows != 0) && (height >= image->rows)))
            break;
      }
      high=draw_info->pointsize;
      for (low=1.0; (high-low) > 0.5; )
      {
        draw_info->pointsize=(low+high)/2.0;
        text=AcquireString(caption);
        i=FormatMagickCaption(image,draw_info,split,&metrics,&text);
        (void) CloneString(&draw_info->text,text);
        text=DestroyString(text);
        (void) FormatLocaleString(geometry,MaxTextExtent,"%+g%+g",
          -metrics.bounds.x1,metrics.ascent);
        if (draw_info->gravity == UndefinedGravity)
          (void) CloneString(&draw_info->geometry,geometry);
        (void) GetMultilineTypeMetrics(image,draw_info,&metrics);
        width=(size_t) floor(metrics.width+draw_info->stroke_width+0.5);
        height=(size_t) floor(metrics.height+draw_info->stroke_width+0.5);
        if ((image->columns != 0) && (image->rows != 0))
          {
            if ((width < image->columns) && (height < image->rows))
              low=draw_info->pointsize+0.5;
            else
              high=draw_info->pointsize-0.5;
          }
        else
          if (((image->columns != 0) && (width < image->columns)) ||
              ((image->rows != 0) && (height < image->rows)))
            low=draw_info->pointsize+0.5;
          else
            high=draw_info->pointsize-0.5;
      }
      draw_info->pointsize=floor((low+high)/2.0-0.5);
    }
  /*
    Draw caption.
  */
  i=FormatMagickCaption(image,draw_info,split,&metrics,&caption);
  (void) CloneString(&draw_info->text,caption);
  (void) FormatLocaleString(geometry,MaxTextExtent,"%+g%+g",MagickMax(
     draw_info->direction == RightToLeftDirection ? image->columns-
     metrics.bounds.x2 : -metrics.bounds.x1,0.0),draw_info->gravity ==
     UndefinedGravity ? metrics.ascent : 0.0);
  draw_info->geometry=AcquireString(geometry);
   status=AnnotateImage(image,draw_info);
   if (image_info->pointsize == 0.0)
     { 
      char
        pointsize[MaxTextExtent];
      
      (void) FormatLocaleString(pointsize,MaxTextExtent,"%.20g",
        draw_info->pointsize);
      (void) SetImageProperty(image,"caption:pointsize",pointsize);
    }
  draw_info=DestroyDrawInfo(draw_info);
  caption=DestroyString(caption);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  return(GetFirstImageInList(image));
}
