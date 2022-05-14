opj_image_t* tgatoimage(const char *filename, opj_cparameters_t *parameters)
{
    FILE *f;
    opj_image_t *image;
    unsigned int image_width, image_height, pixel_bit_depth;
    unsigned int x, y;
    int flip_image = 0;
    opj_image_cmptparm_t cmptparm[4];   /* maximum 4 components */
    int numcomps;
    OPJ_COLOR_SPACE color_space;
    OPJ_BOOL mono ;
    OPJ_BOOL save_alpha;
    int subsampling_dx, subsampling_dy;
    int i;

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open %s for reading !!\n", filename);
        return 0;
    }

    if (!tga_readheader(f, &pixel_bit_depth, &image_width, &image_height,
                        &flip_image)) {
        fclose(f);
        return NULL;
    }

    /* We currently only support 24 & 32 bit tga's ... */
    if (!((pixel_bit_depth == 24) || (pixel_bit_depth == 32))) {
        fclose(f);
        return NULL;
    }

    /* initialize image components */
    memset(&cmptparm[0], 0, 4 * sizeof(opj_image_cmptparm_t));

    mono = (pixel_bit_depth == 8) ||
           (pixel_bit_depth == 16);  /* Mono with & without alpha. */
    save_alpha = (pixel_bit_depth == 16) ||
                 (pixel_bit_depth == 32); /* Mono with alpha, or RGB with alpha */

    if (mono) {
        color_space = OPJ_CLRSPC_GRAY;
        numcomps = save_alpha ? 2 : 1;
    } else {
        numcomps = save_alpha ? 4 : 3;
         color_space = OPJ_CLRSPC_SRGB;
     }
 
    /* If the declared file size is > 10 MB, check that the file is big */
    /* enough to avoid excessive memory allocations */
    if (image_height != 0 && image_width > 10000000 / image_height / numcomps) {
        char ch;
        OPJ_UINT64 expected_file_size =
            (OPJ_UINT64)image_width * image_height * numcomps;
        long curpos = ftell(f);
        if (expected_file_size > (OPJ_UINT64)INT_MAX) {
            expected_file_size = (OPJ_UINT64)INT_MAX;
        }
        fseek(f, (long)expected_file_size - 1, SEEK_SET);
        if (fread(&ch, 1, 1, f) != 1) {
            fclose(f);
            return NULL;
        }
        fseek(f, curpos, SEEK_SET);
    }

     subsampling_dx = parameters->subsampling_dx;
     subsampling_dy = parameters->subsampling_dy;
 
    for (i = 0; i < numcomps; i++) {
        cmptparm[i].prec = 8;
        cmptparm[i].bpp = 8;
        cmptparm[i].sgnd = 0;
        cmptparm[i].dx = (OPJ_UINT32)subsampling_dx;
        cmptparm[i].dy = (OPJ_UINT32)subsampling_dy;
        cmptparm[i].w = image_width;
        cmptparm[i].h = image_height;
    }

    /* create the image */
    image = opj_image_create((OPJ_UINT32)numcomps, &cmptparm[0], color_space);

    if (!image) {
        fclose(f);
        return NULL;
    }


    /* set image offset and reference grid */
    image->x0 = (OPJ_UINT32)parameters->image_offset_x0;
    image->y0 = (OPJ_UINT32)parameters->image_offset_y0;
    image->x1 = !image->x0 ? (OPJ_UINT32)(image_width - 1)  *
                (OPJ_UINT32)subsampling_dx + 1 : image->x0 + (OPJ_UINT32)(image_width - 1)  *
                (OPJ_UINT32)subsampling_dx + 1;
    image->y1 = !image->y0 ? (OPJ_UINT32)(image_height - 1) *
                (OPJ_UINT32)subsampling_dy + 1 : image->y0 + (OPJ_UINT32)(image_height - 1) *
                (OPJ_UINT32)subsampling_dy + 1;

    /* set image data */
    for (y = 0; y < image_height; y++) {
        int index;

        if (flip_image) {
            index = (int)((image_height - y - 1) * image_width);
        } else {
            index = (int)(y * image_width);
        }

        if (numcomps == 3) {
            for (x = 0; x < image_width; x++) {
                unsigned char r, g, b;

                if (!fread(&b, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (!fread(&g, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (!fread(&r, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }

                image->comps[0].data[index] = r;
                image->comps[1].data[index] = g;
                image->comps[2].data[index] = b;
                index++;
            }
        } else if (numcomps == 4) {
            for (x = 0; x < image_width; x++) {
                unsigned char r, g, b, a;
                if (!fread(&b, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (!fread(&g, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (!fread(&r, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }
                if (!fread(&a, 1, 1, f)) {
                    fprintf(stderr,
                            "\nError: fread return a number of element different from the expected.\n");
                    opj_image_destroy(image);
                    fclose(f);
                    return NULL;
                }

                image->comps[0].data[index] = r;
                image->comps[1].data[index] = g;
                image->comps[2].data[index] = b;
                image->comps[3].data[index] = a;
                index++;
            }
        } else {
            fprintf(stderr, "Currently unsupported bit depth : %s\n", filename);
        }
    }
    fclose(f);
    return image;
}
