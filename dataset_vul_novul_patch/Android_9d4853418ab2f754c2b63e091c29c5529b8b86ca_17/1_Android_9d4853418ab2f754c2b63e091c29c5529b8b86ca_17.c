 load_fake(png_charp param, png_bytepp profile)
 {
    char *endptr = NULL;
   unsigned long long int size = strtoull(param, &endptr, 0/*base*/);
 
    /* The 'fake' format is <number>*[string] */
    if (endptr != NULL && *endptr == '*')
 {
 size_t len = strlen(++endptr);
 size_t result = (size_t)size;

 if (len == 0) len = 1; /* capture the terminating '\0' */

 /* Now repeat that string to fill 'size' bytes. */
 if (result == size && (*profile = malloc(result)) != NULL)
 {
         png_bytep out = *profile;

 if (len == 1)
            memset(out, *endptr, result);

 else
 {
 while (size >= len)
 {
               memcpy(out, endptr, len);
               out += len;
               size -= len;
 }
            memcpy(out, endptr, size);
 }

 return result;
 }

 else
 {
         fprintf(stderr, "%s: size exceeds system limits\n", param);
         exit(1);
 }
 }

 return 0;
}
