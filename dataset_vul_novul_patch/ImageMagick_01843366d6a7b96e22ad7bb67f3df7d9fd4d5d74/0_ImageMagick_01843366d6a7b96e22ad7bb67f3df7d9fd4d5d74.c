MagickExport Image *CloneImage(const Image *image,const size_t columns,
  const size_t rows,const MagickBooleanType detach,ExceptionInfo *exception)
{
  double
    scale;

  Image
    *clone_image;

  size_t
    length;

  /*
    Clone the image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns == 0) || (image->rows == 0))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        "NegativeOrZeroImageSize","`%s'",image->filename);
      return((Image *) NULL);
    }
  clone_image=(Image *) AcquireMagickMemory(sizeof(*clone_image));
  if (clone_image == (Image *) NULL)
    ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(clone_image,0,sizeof(*clone_image));
  clone_image->signature=MagickSignature;
  clone_image->storage_class=image->storage_class;
  clone_image->channels=image->channels;
  clone_image->colorspace=image->colorspace;
  clone_image->matte=image->matte;
  clone_image->columns=image->columns;
  clone_image->rows=image->rows;
  clone_image->dither=image->dither;
  if (image->colormap != (PixelPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      clone_image->colors=image->colors;
      length=(size_t) image->colors;
      clone_image->colormap=(PixelPacket *) AcquireQuantumMemory(length,
         sizeof(*clone_image->colormap));
       if (clone_image->colormap == (PixelPacket *) NULL)
         {
          image=(Image *) RelinquishMagickMemory(image);
           ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
         }
       (void) CopyMagickMemory(clone_image->colormap,image->colormap,length*
        sizeof(*clone_image->colormap));
    }
  (void) CloneImageProfiles(clone_image,image);
  (void) CloneImageProperties(clone_image,image);
  (void) CloneImageArtifacts(clone_image,image);
  GetTimerInfo(&clone_image->timer);
  InitializeExceptionInfo(&clone_image->exception);
  InheritException(&clone_image->exception,&image->exception);
  if (image->ascii85 != (void *) NULL)
    Ascii85Initialize(clone_image);
  clone_image->magick_columns=image->magick_columns;
  clone_image->magick_rows=image->magick_rows;
  clone_image->type=image->type;
  (void) CopyMagickString(clone_image->magick_filename,image->magick_filename,
    MaxTextExtent);
  (void) CopyMagickString(clone_image->magick,image->magick,MaxTextExtent);
  (void) CopyMagickString(clone_image->filename,image->filename,MaxTextExtent);
  clone_image->progress_monitor=image->progress_monitor;
  clone_image->client_data=image->client_data;
  clone_image->reference_count=1;
  clone_image->next=image->next;
  clone_image->previous=image->previous;
  clone_image->list=NewImageList();
  clone_image->clip_mask=NewImageList();
  clone_image->mask=NewImageList();
  if (detach == MagickFalse)
    clone_image->blob=ReferenceBlob(image->blob);
  else
    {
      clone_image->next=NewImageList();
      clone_image->previous=NewImageList();
      clone_image->blob=CloneBlobInfo((BlobInfo *) NULL);
    }
  clone_image->ping=image->ping;
  clone_image->debug=IsEventLogging();
  clone_image->semaphore=AllocateSemaphoreInfo();
  if ((columns == 0) || (rows == 0))
    {
      if (image->montage != (char *) NULL)
        (void) CloneString(&clone_image->montage,image->montage);
      if (image->directory != (char *) NULL)
        (void) CloneString(&clone_image->directory,image->directory);
      if (image->clip_mask != (Image *) NULL)
        clone_image->clip_mask=CloneImage(image->clip_mask,0,0,MagickTrue,
          exception);
      if (image->mask != (Image *) NULL)
        clone_image->mask=CloneImage(image->mask,0,0,MagickTrue,exception);
      clone_image->cache=ReferencePixelCache(image->cache);
      return(clone_image);
    }
  if ((columns == image->columns) && (rows == image->rows))
    {
      if (image->clip_mask != (Image *) NULL)
        clone_image->clip_mask=CloneImage(image->clip_mask,0,0,MagickTrue,
          exception);
      if (image->mask != (Image *) NULL)
        clone_image->mask=CloneImage(image->mask,0,0,MagickTrue,exception);
    }
  scale=1.0;
  if (image->columns != 0)
    scale=(double) columns/(double) image->columns;
  clone_image->page.width=(size_t) floor(scale*image->page.width+0.5);
  clone_image->page.x=(ssize_t) ceil(scale*image->page.x-0.5);
  clone_image->tile_offset.x=(ssize_t) ceil(scale*image->tile_offset.x-0.5);
  scale=1.0;
  if (image->rows != 0)
    scale=(double) rows/(double) image->rows;
  clone_image->page.height=(size_t) floor(scale*image->page.height+0.5);
  clone_image->page.y=(ssize_t) ceil(scale*image->page.y-0.5);
  clone_image->tile_offset.y=(ssize_t) ceil(scale*image->tile_offset.y-0.5);
  clone_image->cache=ClonePixelCache(image->cache);
  if (SetImageExtent(clone_image,columns,rows) == MagickFalse)
    {
      InheritException(exception,&clone_image->exception);
      clone_image=DestroyImage(clone_image);
    }
  return(clone_image);
}
