MagickExport MemoryInfo *AcquireVirtualMemory(const size_t count,
  const size_t quantum)
{
  MemoryInfo
    *memory_info;

   size_t
     extent;
 
  if (HeapOverflowSanityCheck(count,quantum) != MagickFalse)
     return((MemoryInfo *) NULL);
   memory_info=(MemoryInfo *) MagickAssumeAligned(AcquireAlignedMemory(1,
     sizeof(*memory_info)));
  if (memory_info == (MemoryInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(memory_info,0,sizeof(*memory_info));
  extent=count*quantum;
  memory_info->length=extent;
  memory_info->signature=MagickSignature;
  if (AcquireMagickResource(MemoryResource,extent) != MagickFalse)
    {
      memory_info->blob=AcquireAlignedMemory(1,extent);
      if (memory_info->blob != NULL)
        {
          memory_info->type=AlignedVirtualMemory;
          return(memory_info);
        }
    }
  RelinquishMagickResource(MemoryResource,extent);
  if (AcquireMagickResource(MapResource,extent) != MagickFalse)
    {
      /*
        Heap memory failed, try anonymous memory mapping.
      */
      memory_info->blob=MapBlob(-1,IOMode,0,extent);
      if (memory_info->blob != NULL)
        {
          memory_info->type=MapVirtualMemory;
          return(memory_info);
        }
      if (AcquireMagickResource(DiskResource,extent) != MagickFalse)
        {
          int
            file;

          /*
            Anonymous memory mapping failed, try file-backed memory mapping.
            If the MapResource request failed, there is no point in trying
            file-backed memory mapping.
          */
          file=AcquireUniqueFileResource(memory_info->filename);
          if (file != -1)
            {
              MagickOffsetType
                offset;

              offset=(MagickOffsetType) lseek(file,extent-1,SEEK_SET);
              if ((offset == (MagickOffsetType) (extent-1)) &&
                  (write(file,"",1) == 1))
                {
                  memory_info->blob=MapBlob(file,IOMode,0,extent);
                  if (memory_info->blob != NULL)
                    {
                      (void) close(file);
                      memory_info->type=MapVirtualMemory;
                      return(memory_info);
                    }
                }
              /*
                File-backed memory mapping failed, delete the temporary file.
              */
              (void) close(file);
              (void) RelinquishUniqueFileResource(memory_info->filename);
              *memory_info->filename='\0';
            }
        }
      RelinquishMagickResource(DiskResource,extent);
    }
  RelinquishMagickResource(MapResource,extent);
  if (memory_info->blob == NULL)
    {
      memory_info->blob=AcquireMagickMemory(extent);
      if (memory_info->blob != NULL)
        memory_info->type=UnalignedVirtualMemory;
    }
  if (memory_info->blob == NULL)
    memory_info=RelinquishVirtualMemory(memory_info);
  return(memory_info);
}
