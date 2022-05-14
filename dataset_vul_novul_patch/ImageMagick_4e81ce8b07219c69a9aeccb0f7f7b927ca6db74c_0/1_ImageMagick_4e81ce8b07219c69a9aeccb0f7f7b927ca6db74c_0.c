static MagickBooleanType OpenPixelCache(Image *image,const MapMode mode,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info,
    source_info;

  char
    format[MaxTextExtent],
    message[MaxTextExtent];

  const char
    *type;

  MagickSizeType
    length,
    number_pixels;

  MagickStatusType
    status;

  size_t
    columns,
    packet_size;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->cache != (Cache) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowBinaryException(CacheError,"NoPixelsDefinedInCache",image->filename);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickSignature);
  if ((AcquireMagickResource(WidthResource,image->columns) == MagickFalse) ||
      (AcquireMagickResource(HeightResource,image->rows) == MagickFalse))
    ThrowBinaryException(ResourceLimitError,"PixelCacheAllocationFailed",
      image->filename);
  source_info=(*cache_info);
  source_info.file=(-1);
  (void) FormatLocaleString(cache_info->filename,MaxTextExtent,"%s[%.20g]",
    image->filename,(double) GetImageIndexInList(image));
  cache_info->mode=mode;
  cache_info->rows=image->rows;
  cache_info->columns=image->columns;
  cache_info->channels=image->channels;
  cache_info->active_index_channel=((image->storage_class == PseudoClass) ||
    (image->colorspace == CMYKColorspace)) ? MagickTrue : MagickFalse;
  number_pixels=(MagickSizeType) cache_info->columns*cache_info->rows;
  packet_size=sizeof(PixelPacket);
  if (cache_info->active_index_channel != MagickFalse)
    packet_size+=sizeof(IndexPacket);
  length=number_pixels*packet_size;
  columns=(size_t) (length/cache_info->rows/packet_size);
  if ((cache_info->columns != columns) || ((ssize_t) cache_info->columns < 0) ||
      ((ssize_t) cache_info->rows < 0))
    ThrowBinaryException(ResourceLimitError,"PixelCacheAllocationFailed",
      image->filename);
  cache_info->length=length;
  if (image->ping != MagickFalse)
    {
      cache_info->storage_class=image->storage_class;
      cache_info->colorspace=image->colorspace;
      cache_info->type=PingCache;
      return(MagickTrue);
    }
  status=AcquireMagickResource(AreaResource,cache_info->length);
  length=number_pixels*(sizeof(PixelPacket)+sizeof(IndexPacket));
  if ((status != MagickFalse) && (length == (MagickSizeType) ((size_t) length)))
    {
      status=AcquireMagickResource(MemoryResource,cache_info->length);
       if (((cache_info->type == UndefinedCache) && (status != MagickFalse)) ||
           (cache_info->type == MemoryCache))
         {
          AllocatePixelCachePixels(cache_info);
           if (cache_info->pixels == (PixelPacket *) NULL)
             cache_info->pixels=source_info.pixels;
           else
            {
              /*
                Create memory pixel cache.
              */
              cache_info->colorspace=image->colorspace;
              cache_info->type=MemoryCache;
              cache_info->indexes=(IndexPacket *) NULL;
              if (cache_info->active_index_channel != MagickFalse)
                cache_info->indexes=(IndexPacket *) (cache_info->pixels+
                  number_pixels);
              if ((source_info.storage_class != UndefinedClass) &&
                  (mode != ReadMode))
                {
                  status&=ClonePixelCacheRepository(cache_info,&source_info,
                    exception);
                  RelinquishPixelCachePixels(&source_info);
                }
              if (image->debug != MagickFalse)
                {
                  (void) FormatMagickSize(cache_info->length,MagickTrue,format);
                  type=CommandOptionToMnemonic(MagickCacheOptions,(ssize_t)
                    cache_info->type);
                  (void) FormatLocaleString(message,MaxTextExtent,
                    "open %s (%s %s, %.20gx%.20g %s)",cache_info->filename,
                    cache_info->mapped != MagickFalse ? "Anonymous" : "Heap",
                    type,(double) cache_info->columns,(double) cache_info->rows,
                    format);
                  (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%s",
                    message);
                }
              cache_info->storage_class=image->storage_class;
              return(MagickTrue);
            }
        }
      RelinquishMagickResource(MemoryResource,cache_info->length);
    }
  /*
    Create pixel cache on disk.
  */
  status=AcquireMagickResource(DiskResource,cache_info->length);
  if ((status == MagickFalse) || (cache_info->type == DistributedCache))
    {
      DistributeCacheInfo
        *server_info;

      if (cache_info->type == DistributedCache)
        RelinquishMagickResource(DiskResource,cache_info->length);
      server_info=AcquireDistributeCacheInfo(exception);
      if (server_info != (DistributeCacheInfo *) NULL)
        {
          status=OpenDistributePixelCache(server_info,image);
          if (status == MagickFalse)
            {
              ThrowFileException(exception,CacheError,"UnableToOpenPixelCache",
                GetDistributeCacheHostname(server_info));
              server_info=DestroyDistributeCacheInfo(server_info);
            }
          else
            {
              /*
                Create a distributed pixel cache.
              */
              cache_info->type=DistributedCache;
              cache_info->storage_class=image->storage_class;
              cache_info->colorspace=image->colorspace;
              cache_info->server_info=server_info;
              (void) FormatLocaleString(cache_info->cache_filename,
                MaxTextExtent,"%s:%d",GetDistributeCacheHostname(
                (DistributeCacheInfo *) cache_info->server_info),
                GetDistributeCachePort((DistributeCacheInfo *)
                cache_info->server_info));
              if ((source_info.storage_class != UndefinedClass) &&
                  (mode != ReadMode))
                {
                  status=ClonePixelCacheRepository(cache_info,&source_info,
                    exception);
                  RelinquishPixelCachePixels(&source_info);
                }
              if (image->debug != MagickFalse)
                {
                  (void) FormatMagickSize(cache_info->length,MagickFalse,
                    format);
                  type=CommandOptionToMnemonic(MagickCacheOptions,(ssize_t)
                    cache_info->type);
                  (void) FormatLocaleString(message,MaxTextExtent,
                    "open %s (%s[%d], %s, %.20gx%.20g %s)",cache_info->filename,
                    cache_info->cache_filename,GetDistributeCacheFile(
                    (DistributeCacheInfo *) cache_info->server_info),type,
                    (double) cache_info->columns,(double) cache_info->rows,
                    format);
                  (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%s",
                    message);
                }
              return(MagickTrue);
            }
        }
      RelinquishMagickResource(DiskResource,cache_info->length);
      (void) ThrowMagickException(exception,GetMagickModule(),CacheError,
        "CacheResourcesExhausted","`%s'",image->filename);
      return(MagickFalse);
    }
  if ((source_info.storage_class != UndefinedClass) && (mode != ReadMode))
    {
      (void) ClosePixelCacheOnDisk(cache_info);
      *cache_info->cache_filename='\0';
    }
  if (OpenPixelCacheOnDisk(cache_info,mode) == MagickFalse)
    {
      RelinquishMagickResource(DiskResource,cache_info->length);
      ThrowFileException(exception,CacheError,"UnableToOpenPixelCache",
        image->filename);
      return(MagickFalse);
    }
  status=SetPixelCacheExtent(image,(MagickSizeType) cache_info->offset+
    cache_info->length);
  if (status == MagickFalse)
    {
      ThrowFileException(exception,CacheError,"UnableToExtendCache",
        image->filename);
      return(MagickFalse);
    }
  cache_info->storage_class=image->storage_class;
  cache_info->colorspace=image->colorspace;
  length=number_pixels*(sizeof(PixelPacket)+sizeof(IndexPacket));
  if (length != (MagickSizeType) ((size_t) length))
    cache_info->type=DiskCache;
  else
    {
      status=AcquireMagickResource(MapResource,cache_info->length);
      if ((status == MagickFalse) && (cache_info->type != MapCache) &&
          (cache_info->type != MemoryCache))
        cache_info->type=DiskCache;
      else
        {
          cache_info->pixels=(PixelPacket *) MapBlob(cache_info->file,mode,
            cache_info->offset,(size_t) cache_info->length);
          if (cache_info->pixels == (PixelPacket *) NULL)
            {
              cache_info->pixels=source_info.pixels;
              cache_info->type=DiskCache;
            }
          else
            {
              /*
                Create file-backed memory-mapped pixel cache.
              */
              (void) ClosePixelCacheOnDisk(cache_info);
              cache_info->type=MapCache;
              cache_info->mapped=MagickTrue;
              cache_info->indexes=(IndexPacket *) NULL;
              if (cache_info->active_index_channel != MagickFalse)
                cache_info->indexes=(IndexPacket *) (cache_info->pixels+
                  number_pixels);
              if ((source_info.storage_class != UndefinedClass) &&
                  (mode != ReadMode))
                {
                  status=ClonePixelCacheRepository(cache_info,&source_info,
                    exception);
                  RelinquishPixelCachePixels(&source_info);
                }
              if (image->debug != MagickFalse)
                {
                  (void) FormatMagickSize(cache_info->length,MagickTrue,format);
                  type=CommandOptionToMnemonic(MagickCacheOptions,(ssize_t)
                    cache_info->type);
                  (void) FormatLocaleString(message,MaxTextExtent,
                    "open %s (%s[%d], %s, %.20gx%.20g %s)",
                    cache_info->filename,cache_info->cache_filename,
                    cache_info->file,type,(double) cache_info->columns,(double)
                    cache_info->rows,format);
                  (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%s",
                    message);
                }
              return(MagickTrue);
            }
        }
      RelinquishMagickResource(MapResource,cache_info->length);
    }
  if ((source_info.storage_class != UndefinedClass) && (mode != ReadMode))
    {
      status=ClonePixelCacheRepository(cache_info,&source_info,exception);
      RelinquishPixelCachePixels(&source_info);
    }
  if (image->debug != MagickFalse)
    {
      (void) FormatMagickSize(cache_info->length,MagickFalse,format);
      type=CommandOptionToMnemonic(MagickCacheOptions,(ssize_t)
        cache_info->type);
      (void) FormatLocaleString(message,MaxTextExtent,
        "open %s (%s[%d], %s, %.20gx%.20g %s)",cache_info->filename,
        cache_info->cache_filename,cache_info->file,type,(double)
        cache_info->columns,(double) cache_info->rows,format);
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%s",message);
    }
  return(MagickTrue);
}
