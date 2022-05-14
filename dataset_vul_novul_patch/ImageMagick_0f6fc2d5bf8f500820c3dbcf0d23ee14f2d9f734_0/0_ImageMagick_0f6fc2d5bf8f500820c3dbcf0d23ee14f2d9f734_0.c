MagickExport MemoryInfo *RelinquishVirtualMemory(MemoryInfo *memory_info)
{
  assert(memory_info != (MemoryInfo *) NULL);
  assert(memory_info->signature == MagickSignature);
  if (memory_info->blob != (void *) NULL)
    switch (memory_info->type)
    {
      case AlignedVirtualMemory:
      {
        memory_info->blob=RelinquishAlignedMemory(memory_info->blob);
        RelinquishMagickResource(MemoryResource,memory_info->length);
        break;
      }
      case MapVirtualMemory:
      {
        (void) UnmapBlob(memory_info->blob,memory_info->length);
         memory_info->blob=NULL;
         RelinquishMagickResource(MapResource,memory_info->length);
         if (*memory_info->filename != '\0')
          {
            (void) RelinquishUniqueFileResource(memory_info->filename);
            RelinquishMagickResource(DiskResource,memory_info->length);
          }
         break;
       }
       case UnalignedVirtualMemory:
      default:
      {
        memory_info->blob=RelinquishMagickMemory(memory_info->blob);
        break;
      }
    }
  memory_info->signature=(~MagickSignature);
  memory_info=(MemoryInfo *) RelinquishAlignedMemory(memory_info);
  return(memory_info);
}
