int sysMapFile(const char* fn, MemMapping* pMap)
{
    memset(pMap, 0, sizeof(*pMap));

 if (fn && fn[0] == '@') {
 FILE* mapf = fopen(fn+1, "r");
 if (mapf == NULL) {
            LOGV("Unable to open '%s': %s\n", fn+1, strerror(errno));
 return -1;
 }

 
         if (sysMapBlockFile(mapf, pMap) != 0) {
             LOGW("Map of '%s' failed\n", fn);
             return -1;
         }
 
        fclose(mapf);
 } else {
 int fd = open(fn, O_RDONLY, 0);
 if (fd < 0) {
            LOGE("Unable to open '%s': %s\n", fn, strerror(errno));
 return -1;
 }

 if (sysMapFD(fd, pMap) != 0) {
            LOGE("Map of '%s' failed\n", fn);
            close(fd);
 return -1;
 }

        close(fd);
 }
 return 0;
}
