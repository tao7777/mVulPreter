opj_image_t* pgxtoimage(const char *filename, opj_cparameters_t *parameters)
{
    FILE *f = NULL;
    int w, h, prec;
    int i, numcomps, max;
    OPJ_COLOR_SPACE color_space;
    opj_image_cmptparm_t cmptparm;  /* maximum of 1 component  */
    opj_image_t * image = NULL;
    int adjustS, ushift, dshift, force8;

    char endian1, endian2, sign;
    char signtmp[32];

    char temp[32];
    int bigendian;
    opj_image_comp_t *comp = NULL;

    numcomps = 1;
    color_space = OPJ_CLRSPC_GRAY;

    memset(&cmptparm, 0, sizeof(opj_image_cmptparm_t));

    max = 0;

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open %s for reading !\n", filename);
        return NULL;
     }
 
     fseek(f, 0, SEEK_SET);
    if (fscanf(f, "PG%31[ \t]%c%c%31[ \t+-]%d%31[ \t]%d%31[ \t]%d", temp, &endian1,
                &endian2, signtmp, &prec, temp, &w, temp, &h) != 9) {
         fclose(f);
         fprintf(stderr,
                "ERROR: Failed to read the right number of element from the fscanf() function!\n");
        return NULL;
    }

    i = 0;
    sign = '+';
    while (signtmp[i] != '\0') {
        if (signtmp[i] == '-') {
            sign = '-';
        }
        i++;
    }

    fgetc(f);
    if (endian1 == 'M' && endian2 == 'L') {
        bigendian = 1;
    } else if (endian2 == 'M' && endian1 == 'L') {
        bigendian = 0;
    } else {
        fclose(f);
        fprintf(stderr, "Bad pgx header, please check input file\n");
        return NULL;
    }

    /* initialize image component */

    cmptparm.x0 = (OPJ_UINT32)parameters->image_offset_x0;
    cmptparm.y0 = (OPJ_UINT32)parameters->image_offset_y0;
    cmptparm.w = !cmptparm.x0 ? (OPJ_UINT32)((w - 1) * parameters->subsampling_dx +
                 1) : cmptparm.x0 + (OPJ_UINT32)(w - 1) * (OPJ_UINT32)parameters->subsampling_dx
                 + 1;
    cmptparm.h = !cmptparm.y0 ? (OPJ_UINT32)((h - 1) * parameters->subsampling_dy +
                 1) : cmptparm.y0 + (OPJ_UINT32)(h - 1) * (OPJ_UINT32)parameters->subsampling_dy
                 + 1;

    if (sign == '-') {
        cmptparm.sgnd = 1;
    } else {
        cmptparm.sgnd = 0;
    }
    if (prec < 8) {
        force8 = 1;
        ushift = 8 - prec;
        dshift = prec - ushift;
        if (cmptparm.sgnd) {
            adjustS = (1 << (prec - 1));
        } else {
            adjustS = 0;
        }
        cmptparm.sgnd = 0;
        prec = 8;
    } else {
        ushift = dshift = force8 = adjustS = 0;
    }

    cmptparm.prec = (OPJ_UINT32)prec;
    cmptparm.bpp = (OPJ_UINT32)prec;
    cmptparm.dx = (OPJ_UINT32)parameters->subsampling_dx;
    cmptparm.dy = (OPJ_UINT32)parameters->subsampling_dy;

    /* create the image */
    image = opj_image_create((OPJ_UINT32)numcomps, &cmptparm, color_space);
    if (!image) {
        fclose(f);
        return NULL;
    }
    /* set image offset and reference grid */
    image->x0 = cmptparm.x0;
    image->y0 = cmptparm.x0;
    image->x1 = cmptparm.w;
    image->y1 = cmptparm.h;

    /* set image data */

    comp = &image->comps[0];

    for (i = 0; i < w * h; i++) {
        int v;
        if (force8) {
            v = readuchar(f) + adjustS;
            v = (v << ushift) + (v >> dshift);
            comp->data[i] = (unsigned char)v;

            if (v > max) {
                max = v;
            }

            continue;
        }
        if (comp->prec == 8) {
            if (!comp->sgnd) {
                v = readuchar(f);
            } else {
                v = (char) readuchar(f);
            }
        } else if (comp->prec <= 16) {
            if (!comp->sgnd) {
                v = readushort(f, bigendian);
            } else {
                v = (short) readushort(f, bigendian);
            }
        } else {
            if (!comp->sgnd) {
                v = (int)readuint(f, bigendian);
            } else {
                v = (int) readuint(f, bigendian);
            }
        }
        if (v > max) {
            max = v;
        }
        comp->data[i] = v;
    }
    fclose(f);
    comp->bpp = (OPJ_UINT32)int_floorlog2(max) + 1;

    return image;
}
