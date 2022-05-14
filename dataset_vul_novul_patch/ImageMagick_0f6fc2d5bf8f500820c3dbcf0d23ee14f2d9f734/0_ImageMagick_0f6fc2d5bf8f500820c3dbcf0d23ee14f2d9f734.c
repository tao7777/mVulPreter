MagickExport MemoryInfo *AcquireVirtualMemory(const size_t count,
  const size_t quantum)
{
  MemoryInfo
    *memory_info;

  size_t
    length;

  length=count*quantum;
  if ((count == 0) || (quantum != (length/count)))
    {
      errno=ENOMEM;
      return((MemoryInfo *) NULL);
    }
  memory_info=(MemoryInfo *) MagickAssumeAligned(AcquireAlignedMemory(1,
    sizeof(*memory_info)));
  if (memory_info == (MemoryInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(memory_info,0,sizeof(*memory_info));
  memory_info->length=length;
  memory_info->signature=MagickSignature;
  if (AcquireMagickResource(MemoryResource,length) != MagickFalse)
    {
      memory_info->blob=AcquireAlignedMemory(1,length);
      if (memory_info->blob != NULL)
        memory_info->type=AlignedVirtualMemory;
      else
        RelinquishMagickResource(MemoryResource,length);
    }
  if ((memory_info->blob == NULL) &&
      (AcquireMagickResource(MapResource,length) != MagickFalse))
    {
      /*
        Heap memory failed, try anonymous memory mapping.
      */
      memory_info->blob=MapBlob(-1,IOMode,0,length);
      if (memory_info->blob != NULL)
        memory_info->type=MapVirtualMemory;
       else
         RelinquishMagickResource(MapResource,length);
     }
  if ((memory_info->blob == NULL) &&
      (AcquireMagickResource(DiskResource,length) != MagickFalse))
     {
       int
         file;

      /*
         Anonymous memory mapping failed, try file-backed memory mapping.
       */
       file=AcquireUniqueFileResource(memory_info->filename);
      if (file == -1)
        RelinquishMagickResource(DiskResource,length);
      else
         {
          if ((lseek(file,length-1,SEEK_SET) < 0) || (write(file,"",1) != 1))
            RelinquishMagickResource(DiskResource,length);
          else
             {
              if (AcquireMagickResource(MapResource,length) == MagickFalse)
                RelinquishMagickResource(DiskResource,length);
              else
                 {
                  memory_info->blob=MapBlob(file,IOMode,0,length);
                  if (memory_info->blob != NULL)
                    memory_info->type=MapVirtualMemory;
                  else
                    {
                      RelinquishMagickResource(MapResource,length);
                      RelinquishMagickResource(DiskResource,length);
                    }
                 }
             }
           (void) close(file);
        }
    }
  if (memory_info->blob == NULL)
    {
      memory_info->blob=AcquireMagickMemory(length);
      if (memory_info->blob != NULL)
        memory_info->type=UnalignedVirtualMemory;
    }
  if (memory_info->blob == NULL)
    memory_info=RelinquishVirtualMemory(memory_info);
  return(memory_info);
}
