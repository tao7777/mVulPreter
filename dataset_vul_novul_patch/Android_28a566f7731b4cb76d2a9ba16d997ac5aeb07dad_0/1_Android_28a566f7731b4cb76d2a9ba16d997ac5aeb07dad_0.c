static int sysMapFD(int fd, MemMapping* pMap)
{
 off_t start;
 size_t length;
 void* memPtr;

    assert(pMap != NULL);

 if (getFileStartAndLength(fd, &start, &length) < 0)
 return -1;

    memPtr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, start);
 if (memPtr == MAP_FAILED) {
        LOGW("mmap(%d, R, PRIVATE, %d, %d) failed: %s\n", (int) length,
            fd, (int) start, strerror(errno));
 return -1;
 }

    pMap->addr = memPtr;

     pMap->length = length;
     pMap->range_count = 1;
     pMap->ranges = malloc(sizeof(MappedRange));
     pMap->ranges[0].addr = memPtr;
     pMap->ranges[0].length = length;
 
 return 0;
}
