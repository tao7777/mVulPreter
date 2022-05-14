static pngquant_error rwpng_read_image24_libpng(FILE *infile, png24_image *mainprog_ptr, int verbose)
{
    png_structp  png_ptr = NULL;
    png_infop    info_ptr = NULL;
    png_size_t   rowbytes;
    int          color_type, bit_depth;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, mainprog_ptr,
      rwpng_error_handler, verbose ? rwpng_warning_stderr_handler : rwpng_warning_silent_handler);
    if (!png_ptr) {
        return PNG_OUT_OF_MEMORY_ERROR;   /* out of memory */
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return PNG_OUT_OF_MEMORY_ERROR;   /* out of memory */
    }

    /* setjmp() must be called in every function that calls a non-trivial
     * libpng function */

    if (setjmp(mainprog_ptr->jmpbuf)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return LIBPNG_FATAL_ERROR;   /* fatal libpng error (via longjmp()) */
    }

#if defined(PNG_SKIP_sRGB_CHECK_PROFILE) && defined(PNG_SET_OPTION_SUPPORTED)
    png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);
#endif

#if PNG_LIBPNG_VER >= 10500 && defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
    /* copy standard chunks too */
    png_set_keep_unknown_chunks(png_ptr, PNG_HANDLE_CHUNK_IF_SAFE, (png_const_bytep)"pHYs\0iTXt\0tEXt\0zTXt", 4);
#endif
    png_set_read_user_chunk_fn(png_ptr, &mainprog_ptr->chunks, read_chunk_callback);

    struct rwpng_read_data read_data = {infile, 0};
    png_set_read_fn(png_ptr, &read_data, user_read_data);

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */

    /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */

     png_get_IHDR(png_ptr, info_ptr, &mainprog_ptr->width, &mainprog_ptr->height,
                  &bit_depth, &color_type, NULL, NULL, NULL);
 
     /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
      * transparency chunks to full alpha channel; strip 16-bit-per-sample
      * images to 8 bits per sample; and convert grayscale to RGB[A] */

    /* GRR TO DO:  preserve all safe-to-copy ancillary PNG chunks */

    if (!(color_type & PNG_COLOR_MASK_ALPHA)) {
#ifdef PNG_READ_FILLER_SUPPORTED
        png_set_expand(png_ptr);
        png_set_filler(png_ptr, 65535L, PNG_FILLER_AFTER);
#else
        fprintf(stderr, "pngquant readpng:  image is neither RGBA nor GA\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        mainprog_ptr->retval = WRONG_INPUT_COLOR_TYPE;
        return mainprog_ptr->retval;
#endif
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }

    if (!(color_type & PNG_COLOR_MASK_COLOR)) {
        png_set_gray_to_rgb(png_ptr);
    }

    /* get source gamma for gamma correction, or use sRGB default */
    double gamma = 0.45455;
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sRGB)) {
        mainprog_ptr->input_color = RWPNG_SRGB;
        mainprog_ptr->output_color = RWPNG_SRGB;
    } else {
        png_get_gAMA(png_ptr, info_ptr, &gamma);
        if (gamma > 0 && gamma <= 1.0) {
            mainprog_ptr->input_color = RWPNG_GAMA_ONLY;
            mainprog_ptr->output_color = RWPNG_GAMA_ONLY;
        } else {
            fprintf(stderr, "pngquant readpng:  ignored out-of-range gamma %f\n", gamma);
            mainprog_ptr->input_color = RWPNG_NONE;
            mainprog_ptr->output_color = RWPNG_NONE;
            gamma = 0.45455;
        }
    }
    mainprog_ptr->gamma = gamma;

    png_set_interlace_handling(png_ptr);

    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */

    png_read_update_info(png_ptr, info_ptr);
 
     rowbytes = png_get_rowbytes(png_ptr, info_ptr);
 
    // For overflow safety reject images that won't fit in 32-bit
    if (rowbytes > INT_MAX/mainprog_ptr->height) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return PNG_OUT_OF_MEMORY_ERROR;
    }

     if ((mainprog_ptr->rgba_data = malloc(rowbytes * mainprog_ptr->height)) == NULL) {
         fprintf(stderr, "pngquant readpng:  unable to allocate image data\n");
         png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return PNG_OUT_OF_MEMORY_ERROR;
    }

    png_bytepp row_pointers = rwpng_create_row_pointers(info_ptr, png_ptr, mainprog_ptr->rgba_data, mainprog_ptr->height, 0);

    /* now we can go ahead and just read the whole image */

    png_read_image(png_ptr, row_pointers);

    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

    png_read_end(png_ptr, NULL);

#if USE_LCMS
#if PNG_LIBPNG_VER < 10500
    png_charp ProfileData;
#else
    png_bytep ProfileData;
#endif
    png_uint_32 ProfileLen;

    cmsHPROFILE hInProfile = NULL;

    /* color_type is read from the image before conversion to RGBA */
    int COLOR_PNG = color_type & PNG_COLOR_MASK_COLOR;

    /* embedded ICC profile */
    if (png_get_iCCP(png_ptr, info_ptr, &(png_charp){0}, &(int){0}, &ProfileData, &ProfileLen)) {

        hInProfile = cmsOpenProfileFromMem(ProfileData, ProfileLen);
        cmsColorSpaceSignature colorspace = cmsGetColorSpace(hInProfile);

        /* only RGB (and GRAY) valid for PNGs */
        if (colorspace == cmsSigRgbData && COLOR_PNG) {
            mainprog_ptr->input_color = RWPNG_ICCP;
            mainprog_ptr->output_color = RWPNG_SRGB;
        } else {
            if (colorspace == cmsSigGrayData && !COLOR_PNG) {
                mainprog_ptr->input_color = RWPNG_ICCP_WARN_GRAY;
                mainprog_ptr->output_color = RWPNG_SRGB;
            }
            cmsCloseProfile(hInProfile);
            hInProfile = NULL;
        }
    }

    /* build RGB profile from cHRM and gAMA */
    if (hInProfile == NULL && COLOR_PNG &&
        !png_get_valid(png_ptr, info_ptr, PNG_INFO_sRGB) &&
        png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA) &&
        png_get_valid(png_ptr, info_ptr, PNG_INFO_cHRM)) {

        cmsCIExyY WhitePoint;
        cmsCIExyYTRIPLE Primaries;

        png_get_cHRM(png_ptr, info_ptr, &WhitePoint.x, &WhitePoint.y,
                     &Primaries.Red.x, &Primaries.Red.y,
                     &Primaries.Green.x, &Primaries.Green.y,
                     &Primaries.Blue.x, &Primaries.Blue.y);

        WhitePoint.Y = Primaries.Red.Y = Primaries.Green.Y = Primaries.Blue.Y = 1.0;

        cmsToneCurve *GammaTable[3];
        GammaTable[0] = GammaTable[1] = GammaTable[2] = cmsBuildGamma(NULL, 1/gamma);

        hInProfile = cmsCreateRGBProfile(&WhitePoint, &Primaries, GammaTable);

        cmsFreeToneCurve(GammaTable[0]);

        mainprog_ptr->input_color = RWPNG_GAMA_CHRM;
        mainprog_ptr->output_color = RWPNG_SRGB;
    }

    /* transform image to sRGB colorspace */
    if (hInProfile != NULL) {

        cmsHPROFILE hOutProfile = cmsCreate_sRGBProfile();
        cmsHTRANSFORM hTransform = cmsCreateTransform(hInProfile, TYPE_RGBA_8,
                                                      hOutProfile, TYPE_RGBA_8,
                                                      INTENT_PERCEPTUAL,
                                                      omp_get_max_threads() > 1 ? cmsFLAGS_NOCACHE : 0);

        #pragma omp parallel for \
            if (mainprog_ptr->height*mainprog_ptr->width > 8000) \
            schedule(static)
        for (unsigned int i = 0; i < mainprog_ptr->height; i++) {
            /* It is safe to use the same block for input and output,
               when both are of the same TYPE. */
            cmsDoTransform(hTransform, row_pointers[i],
                                       row_pointers[i],
                                       mainprog_ptr->width);
        }

        cmsDeleteTransform(hTransform);
        cmsCloseProfile(hOutProfile);
        cmsCloseProfile(hInProfile);

        mainprog_ptr->gamma = 0.45455;
    }
#endif

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    mainprog_ptr->file_size = read_data.bytes_read;
    mainprog_ptr->row_pointers = (unsigned char **)row_pointers;

    return SUCCESS;
}
