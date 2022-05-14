 main(void)
 {
    fprintf(stderr,
       "pngfix needs libpng with a zlib >=1.2.4 (not 0x%x)\n",
      ZLIB_VERNUM);
    return 77;
 }
