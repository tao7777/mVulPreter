static int sysMapBlockFile(FILE* mapf, MemMapping* pMap)
{

     char block_dev[PATH_MAX+1];
     size_t size;
     unsigned int blksize;
    size_t blocks;
     unsigned int range_count;
     unsigned int i;
 
 if (fgets(block_dev, sizeof(block_dev), mapf) == NULL) {
        LOGW("failed to read block device from header\n");
 return -1;
 }
 for (i = 0; i < sizeof(block_dev); ++i) {
 if (block_dev[i] == '\n') {
            block_dev[i] = 0;
 break;
 }
 }

 if (fscanf(mapf, "%zu %u\n%u\n", &size, &blksize, &range_count) != 3) {

         LOGW("failed to parse block map header\n");
         return -1;
     }
    if (blksize != 0) {
        blocks = ((size-1) / blksize) + 1;
    }
    if (size == 0 || blksize == 0 || blocks > SIZE_MAX / blksize || range_count == 0) {
        LOGE("invalid data in block map file: size %zu, blksize %u, range_count %u\n",
             size, blksize, range_count);
        return -1;
    }
 
     pMap->range_count = range_count;
    pMap->ranges = calloc(range_count, sizeof(MappedRange));
    if (pMap->ranges == NULL) {
        LOGE("calloc(%u, %zu) failed: %s\n", range_count, sizeof(MappedRange), strerror(errno));
        return -1;
    }
 
     unsigned char* reserve;
     reserve = mmap64(NULL, blocks * blksize, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
     if (reserve == MAP_FAILED) {
         LOGW("failed to reserve address space: %s\n", strerror(errno));
        free(pMap->ranges);
         return -1;
     }
 
     int fd = open(block_dev, O_RDONLY);
     if (fd < 0) {
         LOGW("failed to open block device %s: %s\n", block_dev, strerror(errno));
        munmap(reserve, blocks * blksize);
        free(pMap->ranges);
         return -1;
     }
 
     unsigned char* next = reserve;
    size_t remaining_size = blocks * blksize;
    bool success = true;
     for (i = 0; i < range_count; ++i) {
        size_t start, end;
        if (fscanf(mapf, "%zu %zu\n", &start, &end) != 2) {
             LOGW("failed to parse range %d in block map\n", i);
            success = false;
            break;
        }
        size_t length = (end - start) * blksize;
        if (end <= start || (end - start) > SIZE_MAX / blksize || length > remaining_size) {
          LOGE("unexpected range in block map: %zu %zu\n", start, end);
          success = false;
          break;
         }
 
        void* addr = mmap64(next, length, PROT_READ, MAP_PRIVATE | MAP_FIXED, fd, ((off64_t)start)*blksize);
         if (addr == MAP_FAILED) {
             LOGW("failed to map block %d: %s\n", i, strerror(errno));
            success = false;
            break;
         }
         pMap->ranges[i].addr = addr;
        pMap->ranges[i].length = length;
 
        next += length;
        remaining_size -= length;
    }
    if (success && remaining_size != 0) {
      LOGE("ranges in block map are invalid: remaining_size = %zu\n", remaining_size);
      success = false;
    }
    if (!success) {
      close(fd);
      munmap(reserve, blocks * blksize);
      free(pMap->ranges);
      return -1;
     }
 
    close(fd);
     pMap->addr = reserve;
     pMap->length = size;
 
    LOGI("mmapped %d ranges\n", range_count);

 return 0;
}
