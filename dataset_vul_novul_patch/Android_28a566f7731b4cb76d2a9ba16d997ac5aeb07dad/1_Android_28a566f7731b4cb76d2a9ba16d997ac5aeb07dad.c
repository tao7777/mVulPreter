static int sysMapBlockFile(FILE* mapf, MemMapping* pMap)
{

     char block_dev[PATH_MAX+1];
     size_t size;
     unsigned int blksize;
    unsigned int blocks;
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
    blocks = ((size-1) / blksize) + 1;
 
     pMap->range_count = range_count;
    pMap->ranges = malloc(range_count * sizeof(MappedRange));
    memset(pMap->ranges, 0, range_count * sizeof(MappedRange));
 
     unsigned char* reserve;
     reserve = mmap64(NULL, blocks * blksize, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
     if (reserve == MAP_FAILED) {
         LOGW("failed to reserve address space: %s\n", strerror(errno));
         return -1;
     }
 
    pMap->ranges[range_count-1].addr = reserve;
    pMap->ranges[range_count-1].length = blocks * blksize;
     int fd = open(block_dev, O_RDONLY);
     if (fd < 0) {
         LOGW("failed to open block device %s: %s\n", block_dev, strerror(errno));
         return -1;
     }
 
     unsigned char* next = reserve;
     for (i = 0; i < range_count; ++i) {
        int start, end;
        if (fscanf(mapf, "%d %d\n", &start, &end) != 2) {
             LOGW("failed to parse range %d in block map\n", i);
            return -1;
         }
 
        void* addr = mmap64(next, (end-start)*blksize, PROT_READ, MAP_PRIVATE | MAP_FIXED, fd, ((off64_t)start)*blksize);
         if (addr == MAP_FAILED) {
             LOGW("failed to map block %d: %s\n", i, strerror(errno));
            return -1;
         }
         pMap->ranges[i].addr = addr;
        pMap->ranges[i].length = (end-start)*blksize;
 
        next += pMap->ranges[i].length;
     }
 
     pMap->addr = reserve;
     pMap->length = size;
 
    LOGI("mmapped %d ranges\n", range_count);

 return 0;
}
