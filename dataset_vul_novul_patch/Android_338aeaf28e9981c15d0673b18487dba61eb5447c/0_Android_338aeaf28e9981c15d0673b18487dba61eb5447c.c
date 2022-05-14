char* dexOptGenerateCacheFileName(const char* fileName, const char* subFileName)
{
 char nameBuf[512];
 char absoluteFile[sizeof(nameBuf)];
 const size_t kBufLen = sizeof(nameBuf) - 1;
 const char* dataRoot;
 char* cp;

 /*
     * Get the absolute path of the Jar or DEX file.
     */
    absoluteFile[0] = '\0';
 if (fileName[0] != '/') {
 /*
         * Generate the absolute path.  This doesn't do everything it
         * should, e.g. if filename is "./out/whatever" it doesn't crunch
         * the leading "./" out, but it'll do.
         */
 if (getcwd(absoluteFile, kBufLen) == NULL) {

             ALOGE("Can't get CWD while opening jar file");
             return NULL;
         }
        strncat(absoluteFile, "/", kBufLen - strlen(absoluteFile));
     }
    strncat(absoluteFile, fileName, kBufLen - strlen(absoluteFile));
 
     /*
      * Append the name of the Jar file entry, if any.  This is not currently
     * required, but will be if we start putting more than one DEX file

      * in a Jar.
      */
     if (subFileName != NULL) {
        strncat(absoluteFile, "/", kBufLen - strlen(absoluteFile));
        strncat(absoluteFile, subFileName, kBufLen - strlen(absoluteFile));
     }
 
     /* Turn the path into a flat filename by replacing
     * any slashes after the first one with '@' characters.
     */
    cp = absoluteFile + 1;
 while (*cp != '\0') {
 if (*cp == '/') {
 *cp = '@';
 }
        cp++;
 }

 /* Build the name of the cache directory.
     */
    dataRoot = getenv("ANDROID_DATA");
 if (dataRoot == NULL)
        dataRoot = "/data";
    snprintf(nameBuf, kBufLen, "%s/%s", dataRoot, kCacheDirectoryName);
 if (strcmp(dataRoot, "/data") != 0) {
 int result = dexOptMkdir(nameBuf, 0700);
 if (result != 0 && errno != EEXIST) {
            ALOGE("Failed to create dalvik-cache directory %s: %s", nameBuf, strerror(errno));
 return NULL;
 }
 }
    snprintf(nameBuf, kBufLen, "%s/%s/%s", dataRoot, kCacheDirectoryName, kInstructionSet);
 if (strcmp(dataRoot, "/data") != 0) {
 int result = dexOptMkdir(nameBuf, 0700);
 if (result != 0 && errno != EEXIST) {
            ALOGE("Failed to create dalvik-cache directory %s: %s", nameBuf, strerror(errno));
 return NULL;
 }
 }

 
     /* Tack on the file name for the actual cache file path.
      */
    strncat(nameBuf, absoluteFile, kBufLen - strlen(nameBuf));
 
     ALOGV("Cache file for '%s' '%s' is '%s'", fileName, subFileName, nameBuf);
     return strdup(nameBuf);
}
