MagickExport Image *ComplexImages(const Image *images,const ComplexOperator op,
  ExceptionInfo *exception)
{
#define ComplexImageTag  "Complex/Image"

  CacheView
    *Ai_view,
    *Ar_view,
    *Bi_view,
    *Br_view,
    *Ci_view,
    *Cr_view;

  const char
    *artifact;

  const Image
    *Ai_image,
    *Ar_image,
    *Bi_image,
    *Br_image;

  double
    snr;

  Image
    *Ci_image,
    *complex_images,
    *Cr_image,
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (images->next == (Image *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
        "ImageSequenceRequired","`%s'",images->filename);
      return((Image *) NULL);
    }
  image=CloneImage(images,0,0,MagickTrue,exception);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    {
      image=DestroyImageList(image);
      return(image);
    }
  image->depth=32UL;
  complex_images=NewImageList();
  AppendImageToList(&complex_images,image);
  image=CloneImage(images,0,0,MagickTrue,exception);
  if (image == (Image *) NULL)
    {
      complex_images=DestroyImageList(complex_images);
      return(complex_images);
    }
  AppendImageToList(&complex_images,image);
  /*
    Apply complex mathematics to image pixels.
  */
  artifact=GetImageArtifact(image,"complex:snr");
  snr=0.0;
  if (artifact != (const char *) NULL)
    snr=StringToDouble(artifact,(char **) NULL);
  Ar_image=images;
  Ai_image=images->next;
  Br_image=images;
  Bi_image=images->next;
  if ((images->next->next != (Image *) NULL) &&
      (images->next->next->next != (Image *) NULL))
    {
      Br_image=images->next->next;
      Bi_image=images->next->next->next;
    }
  Cr_image=complex_images;
  Ci_image=complex_images->next;
  Ar_view=AcquireVirtualCacheView(Ar_image,exception);
  Ai_view=AcquireVirtualCacheView(Ai_image,exception);
  Br_view=AcquireVirtualCacheView(Br_image,exception);
  Bi_view=AcquireVirtualCacheView(Bi_image,exception);
  Cr_view=AcquireAuthenticCacheView(Cr_image,exception);
  Ci_view=AcquireAuthenticCacheView(Ci_image,exception);
  status=MagickTrue;
   progress=0;
 #if defined(MAGICKCORE_OPENMP_SUPPORT)
   #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(Cr_image,complex_images,Cr_image->rows,1L)
 #endif
  for (y=0; y < (ssize_t) Cr_image->rows; y++)
   {
     register const PixelPacket
       *magick_restrict Ai,
      *magick_restrict Ar,
      *magick_restrict Bi,
      *magick_restrict Br;

    register PixelPacket
      *magick_restrict Ci,
      *magick_restrict Cr;

    register ssize_t
      x;
 
     if (status == MagickFalse)
       continue;
    Ar=GetCacheViewVirtualPixels(Ar_view,0,y,Cr_image->columns,1,exception);
    Ai=GetCacheViewVirtualPixels(Ai_view,0,y,Cr_image->columns,1,exception);
    Br=GetCacheViewVirtualPixels(Br_view,0,y,Cr_image->columns,1,exception);
    Bi=GetCacheViewVirtualPixels(Bi_view,0,y,Cr_image->columns,1,exception);
     Cr=QueueCacheViewAuthenticPixels(Cr_view,0,y,Cr_image->columns,1,exception);
     Ci=QueueCacheViewAuthenticPixels(Ci_view,0,y,Ci_image->columns,1,exception);
     if ((Ar == (const PixelPacket *) NULL) ||
        (Ai == (const PixelPacket *) NULL) ||
        (Br == (const PixelPacket *) NULL) ||
        (Bi == (const PixelPacket *) NULL) ||
        (Cr == (PixelPacket *) NULL) || (Ci == (PixelPacket *) NULL))
      {
         status=MagickFalse;
         continue;
       }
    for (x=0; x < (ssize_t) Cr_image->columns; x++)
     {
       switch (op)
       {
        case AddComplexOperator:
        {
          Cr->red=Ar->red+Br->red;
          Ci->red=Ai->red+Bi->red;
          Cr->green=Ar->green+Br->green;
          Ci->green=Ai->green+Bi->green;
          Cr->blue=Ar->blue+Br->blue;
          Ci->blue=Ai->blue+Bi->blue;
          if (images->matte != MagickFalse)
            {
              Cr->opacity=Ar->opacity+Br->opacity;
              Ci->opacity=Ai->opacity+Bi->opacity;
            }
          break;
        }
        case ConjugateComplexOperator:
        default:
        {
          Cr->red=Ar->red;
          Ci->red=(-Bi->red);
          Cr->green=Ar->green;
          Ci->green=(-Bi->green);
          Cr->blue=Ar->blue;
          Ci->blue=(-Bi->blue);
          if (images->matte != MagickFalse)
            {
              Cr->opacity=Ar->opacity;
              Ci->opacity=(-Bi->opacity);
            }
          break;
        }
        case DivideComplexOperator:
        {
          double
             gamma;
 
           gamma=PerceptibleReciprocal(Br->red*Br->red+Bi->red*Bi->red+snr);
          Cr->red=gamma*((double) Ar->red*Br->red+(double) Ai->red*Bi->red);
          Ci->red=gamma*((double) Ai->red*Br->red-(double) Ar->red*Bi->red);
          gamma=PerceptibleReciprocal((double) Br->green*Br->green+(double)
            Bi->green*Bi->green+snr);
          Cr->green=gamma*((double) Ar->green*Br->green+(double)
            Ai->green*Bi->green);
          Ci->green=gamma*((double) Ai->green*Br->green-(double)
            Ar->green*Bi->green);
          gamma=PerceptibleReciprocal((double) Br->blue*Br->blue+(double)
            Bi->blue*Bi->blue+snr);
          Cr->blue=gamma*((double) Ar->blue*Br->blue+(double)
            Ai->blue*Bi->blue);
          Ci->blue=gamma*((double) Ai->blue*Br->blue-(double)
             Ar->blue*Bi->blue);
           if (images->matte != MagickFalse)
             {
              gamma=PerceptibleReciprocal((double) Br->opacity*Br->opacity+
                (double) Bi->opacity*Bi->opacity+snr);
              Cr->opacity=gamma*((double) Ar->opacity*Br->opacity+(double)
                Ai->opacity*Bi->opacity);
              Ci->opacity=gamma*((double) Ai->opacity*Br->opacity-(double)
                Ar->opacity*Bi->opacity);
             }
           break;
         }
         case MagnitudePhaseComplexOperator:
         {
          Cr->red=sqrt((double) Ar->red*Ar->red+(double) Ai->red*Ai->red);
          Ci->red=atan2((double) Ai->red,(double) Ar->red)/(2.0*MagickPI)+0.5;
          Cr->green=sqrt((double) Ar->green*Ar->green+(double)
            Ai->green*Ai->green);
          Ci->green=atan2((double) Ai->green,(double) Ar->green)/
            (2.0*MagickPI)+0.5;
          Cr->blue=sqrt((double) Ar->blue*Ar->blue+(double) Ai->blue*Ai->blue);
           Ci->blue=atan2(Ai->blue,Ar->blue)/(2.0*MagickPI)+0.5;
           if (images->matte != MagickFalse)
             {
              Cr->opacity=sqrt((double) Ar->opacity*Ar->opacity+(double)
                Ai->opacity*Ai->opacity);
              Ci->opacity=atan2((double) Ai->opacity,(double) Ar->opacity)/
                (2.0*MagickPI)+0.5;
             }
           break;
         }
         case MultiplyComplexOperator:
         {
          Cr->red=QuantumScale*((double) Ar->red*Br->red-(double)
            Ai->red*Bi->red);
          Ci->red=QuantumScale*((double) Ai->red*Br->red+(double)
            Ar->red*Bi->red);
          Cr->green=QuantumScale*((double) Ar->green*Br->green-(double)
            Ai->green*Bi->green);
          Ci->green=QuantumScale*((double) Ai->green*Br->green+(double)
            Ar->green*Bi->green);
          Cr->blue=QuantumScale*((double) Ar->blue*Br->blue-(double)
            Ai->blue*Bi->blue);
          Ci->blue=QuantumScale*((double) Ai->blue*Br->blue+(double)
            Ar->blue*Bi->blue);
           if (images->matte != MagickFalse)
             {
              Cr->opacity=QuantumScale*((double) Ar->opacity*Br->opacity-
                (double) Ai->opacity*Bi->opacity);
              Ci->opacity=QuantumScale*((double) Ai->opacity*Br->opacity+
                (double) Ar->opacity*Bi->opacity);
             }
           break;
         }
        case RealImaginaryComplexOperator:
        {
          Cr->red=Ar->red*cos(2.0*MagickPI*(Ai->red-0.5));
          Ci->red=Ar->red*sin(2.0*MagickPI*(Ai->red-0.5));
          Cr->green=Ar->green*cos(2.0*MagickPI*(Ai->green-0.5));
          Ci->green=Ar->green*sin(2.0*MagickPI*(Ai->green-0.5));
          Cr->blue=Ar->blue*cos(2.0*MagickPI*(Ai->blue-0.5));
          Ci->blue=Ar->blue*sin(2.0*MagickPI*(Ai->blue-0.5));
          if (images->matte != MagickFalse)
            {
               Cr->opacity=Ar->opacity*cos(2.0*MagickPI*(Ai->opacity-0.5));
               Ci->opacity=Ar->opacity*sin(2.0*MagickPI*(Ai->opacity-0.5));
            }
          break;
        }
        case SubtractComplexOperator:
        {
          Cr->red=Ar->red-Br->red;
          Ci->red=Ai->red-Bi->red;
          Cr->green=Ar->green-Br->green;
           Ci->green=Ai->green-Bi->green;
           Cr->blue=Ar->blue-Br->blue;
           Ci->blue=Ai->blue-Bi->blue;
          if (Cr_image->matte != MagickFalse)
             {
               Cr->opacity=Ar->opacity-Br->opacity;
               Ci->opacity=Ai->opacity-Bi->opacity;
            }
          break;
        }
      }
      Ar++;
      Ai++;
      Br++;
      Bi++;
      Cr++;
      Ci++;
    }
    if (SyncCacheViewAuthenticPixels(Ci_view,exception) == MagickFalse)
      status=MagickFalse;
    if (SyncCacheViewAuthenticPixels(Cr_view,exception) == MagickFalse)
      status=MagickFalse;
    if (images->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(images,ComplexImageTag,progress,images->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  Cr_view=DestroyCacheView(Cr_view);
  Ci_view=DestroyCacheView(Ci_view);
  Br_view=DestroyCacheView(Br_view);
  Bi_view=DestroyCacheView(Bi_view);
  Ar_view=DestroyCacheView(Ar_view);
  Ai_view=DestroyCacheView(Ai_view);
  if (status == MagickFalse)
    complex_images=DestroyImageList(complex_images);
  return(complex_images);
}
