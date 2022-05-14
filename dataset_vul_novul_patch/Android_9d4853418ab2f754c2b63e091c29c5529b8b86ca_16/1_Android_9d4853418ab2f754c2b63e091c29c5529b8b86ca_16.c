insert_hIST(png_structp png_ptr, png_infop info_ptr, int nparams, png_charpp params)
 {
    int i;
    png_uint_16 freq[256];

 /* libpng takes the count from the PLTE count; we don't check it here but we
    * do set the array to 0 for unspecified entries.
    */
   memset(freq, 0, sizeof freq);
 for (i=0; i<nparams; ++i)
 {
 char *endptr = NULL;
 unsigned long int l = strtoul(params[i], &endptr, 0/*base*/);

 if (params[i][0] && *endptr == 0 && l <= 65535)
         freq[i] = (png_uint_16)l;

 else
 {
         fprintf(stderr, "hIST[%d]: %s: invalid frequency\n", i, params[i]);
         exit(1);
 }
 }


    png_set_hIST(png_ptr, info_ptr, freq);
 }
