int imagetobmp(opj_image_t * image, const char *outfile) {
    int w, h;
    int i, pad;
    FILE *fdest = NULL;
    int adjustR, adjustG, adjustB;

    if (image->comps[0].prec < 8) {
        fprintf(stderr, "Unsupported number of components: %d\n", image->comps[0].prec);
        return 1;
    }
    if (image->numcomps >= 3 && image->comps[0].dx == image->comps[1].dx
            && image->comps[1].dx == image->comps[2].dx
            && image->comps[0].dy == image->comps[1].dy
            && image->comps[1].dy == image->comps[2].dy
            && image->comps[0].prec == image->comps[1].prec
            && image->comps[1].prec == image->comps[2].prec) {

        /* -->> -->> -->> -->>
        24 bits color
        <<-- <<-- <<-- <<-- */

        fdest = fopen(outfile, "wb");
        if (!fdest) {
            fprintf(stderr, "ERROR -> failed to open %s for writing\n", outfile);
            return 1;
        }

        w = (int)image->comps[0].w;
        h = (int)image->comps[0].h;

        fprintf(fdest, "BM");

        /* FILE HEADER */
        /* ------------- */
        fprintf(fdest, "%c%c%c%c",
                (OPJ_UINT8) (h * w * 3 + 3 * h * (w % 2) + 54) & 0xff,
                (OPJ_UINT8) ((h * w * 3 + 3 * h * (w % 2) + 54)	>> 8) & 0xff,
                (OPJ_UINT8) ((h * w * 3 + 3 * h * (w % 2) + 54)	>> 16) & 0xff,
                (OPJ_UINT8) ((h * w * 3 + 3 * h * (w % 2) + 54)	>> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (0) & 0xff, ((0) >> 8) & 0xff, ((0) >> 16) & 0xff, ((0) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (54) & 0xff, ((54) >> 8) & 0xff,((54) >> 16) & 0xff, ((54) >> 24) & 0xff);

        /* INFO HEADER   */
        /* ------------- */
        fprintf(fdest, "%c%c%c%c", (40) & 0xff, ((40) >> 8) & 0xff,	((40) >> 16) & 0xff, ((40) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) ((w) & 0xff),
                (OPJ_UINT8) ((w) >> 8) & 0xff,
                (OPJ_UINT8) ((w) >> 16) & 0xff,
                (OPJ_UINT8) ((w) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) ((h) & 0xff),
                (OPJ_UINT8) ((h) >> 8) & 0xff,
                (OPJ_UINT8) ((h) >> 16) & 0xff,
                (OPJ_UINT8) ((h) >> 24) & 0xff);
        fprintf(fdest, "%c%c", (1) & 0xff, ((1) >> 8) & 0xff);
        fprintf(fdest, "%c%c", (24) & 0xff, ((24) >> 8) & 0xff);
        fprintf(fdest, "%c%c%c%c", (0) & 0xff, ((0) >> 8) & 0xff, ((0) >> 16) & 0xff, ((0) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) (3 * h * w + 3 * h * (w % 2)) & 0xff,
                (OPJ_UINT8) ((h * w * 3 + 3 * h * (w % 2)) >> 8) & 0xff,
                (OPJ_UINT8) ((h * w * 3 + 3 * h * (w % 2)) >> 16) & 0xff,
                (OPJ_UINT8) ((h * w * 3 + 3 * h * (w % 2)) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (7834) & 0xff, ((7834) >> 8) & 0xff, ((7834) >> 16) & 0xff, ((7834) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (7834) & 0xff, ((7834) >> 8) & 0xff,	((7834) >> 16) & 0xff, ((7834) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (0) & 0xff, ((0) >> 8) & 0xff, ((0) >> 16) & 0xff, ((0) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (0) & 0xff, ((0) >> 8) & 0xff, ((0) >> 16) & 0xff, ((0) >> 24) & 0xff);

        if (image->comps[0].prec > 8) {
            adjustR = (int)image->comps[0].prec - 8;
            printf("BMP CONVERSION: Truncating component 0 from %d bits to 8 bits\n", image->comps[0].prec);
        }
        else
            adjustR = 0;
        if (image->comps[1].prec > 8) {
            adjustG = (int)image->comps[1].prec - 8;
            printf("BMP CONVERSION: Truncating component 1 from %d bits to 8 bits\n", image->comps[1].prec);
        }
        else
            adjustG = 0;
        if (image->comps[2].prec > 8) {
            adjustB = (int)image->comps[2].prec - 8;
            printf("BMP CONVERSION: Truncating component 2 from %d bits to 8 bits\n", image->comps[2].prec);
        }
        else
            adjustB = 0;

        for (i = 0; i < w * h; i++) {
            OPJ_UINT8 rc, gc, bc;
            int r, g, b;

            r = image->comps[0].data[w * h - ((i) / (w) + 1) * w + (i) % (w)];
            r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
            r = ((r >> adjustR)+((r >> (adjustR-1))%2));
            if(r > 255) r = 255; else if(r < 0) r = 0;
            rc = (OPJ_UINT8)r;

            g = image->comps[1].data[w * h - ((i) / (w) + 1) * w + (i) % (w)];
            g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);
            g = ((g >> adjustG)+((g >> (adjustG-1))%2));
            if(g > 255) g = 255; else if(g < 0) g = 0;
            gc = (OPJ_UINT8)g;

            b = image->comps[2].data[w * h - ((i) / (w) + 1) * w + (i) % (w)];
            b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);
            b = ((b >> adjustB)+((b >> (adjustB-1))%2));
            if(b > 255) b = 255; else if(b < 0) b = 0;
            bc = (OPJ_UINT8)b;

            fprintf(fdest, "%c%c%c", bc, gc, rc);

            if ((i + 1) % w == 0) {
                for (pad = ((3 * w) % 4) ? (4 - (3 * w) % 4) : 0; pad > 0; pad--)	/* ADD */
                    fprintf(fdest, "%c", 0);
            }
        }
        fclose(fdest);
    } else {			/* Gray-scale */

        /* -->> -->> -->> -->>
        8 bits non code (Gray scale)
        <<-- <<-- <<-- <<-- */

        fdest = fopen(outfile, "wb");
        if (!fdest) {
            fprintf(stderr, "ERROR -> failed to open %s for writing\n", outfile);
            return 1;
        }
        w = (int)image->comps[0].w;
        h = (int)image->comps[0].h;

        fprintf(fdest, "BM");

        /* FILE HEADER */
        /* ------------- */
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) (h * w + 54 + 1024 + h * (w % 2)) & 0xff,
                (OPJ_UINT8) ((h * w + 54 + 1024 + h * (w % 2)) >> 8) & 0xff,
                (OPJ_UINT8) ((h * w + 54 + 1024 + h * (w % 2)) >> 16) & 0xff,
                (OPJ_UINT8) ((h * w + 54 + 1024 + w * (w % 2)) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (0) & 0xff, ((0) >> 8) & 0xff, ((0) >> 16) & 0xff, ((0) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (54 + 1024) & 0xff, ((54 + 1024) >> 8) & 0xff,
                ((54 + 1024) >> 16) & 0xff,
                ((54 + 1024) >> 24) & 0xff);

        /* INFO HEADER */
        /* ------------- */
        fprintf(fdest, "%c%c%c%c", (40) & 0xff, ((40) >> 8) & 0xff,	((40) >> 16) & 0xff, ((40) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) ((w) & 0xff),
                (OPJ_UINT8) ((w) >> 8) & 0xff,
                (OPJ_UINT8) ((w) >> 16) & 0xff,
                (OPJ_UINT8) ((w) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) ((h) & 0xff),
                (OPJ_UINT8) ((h) >> 8) & 0xff,
                (OPJ_UINT8) ((h) >> 16) & 0xff,
                (OPJ_UINT8) ((h) >> 24) & 0xff);
        fprintf(fdest, "%c%c", (1) & 0xff, ((1) >> 8) & 0xff);
        fprintf(fdest, "%c%c", (8) & 0xff, ((8) >> 8) & 0xff);
        fprintf(fdest, "%c%c%c%c", (0) & 0xff, ((0) >> 8) & 0xff, ((0) >> 16) & 0xff, ((0) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (OPJ_UINT8) (h * w + h * (w % 2)) & 0xff,
                (OPJ_UINT8) ((h * w + h * (w % 2)) >> 8) &	0xff,
                (OPJ_UINT8) ((h * w + h * (w % 2)) >> 16) &	0xff,
                (OPJ_UINT8) ((h * w + h * (w % 2)) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (7834) & 0xff, ((7834) >> 8) & 0xff,	((7834) >> 16) & 0xff, ((7834) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (7834) & 0xff, ((7834) >> 8) & 0xff,	((7834) >> 16) & 0xff, ((7834) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (256) & 0xff, ((256) >> 8) & 0xff, ((256) >> 16) & 0xff, ((256) >> 24) & 0xff);
        fprintf(fdest, "%c%c%c%c", (256) & 0xff, ((256) >> 8) & 0xff, ((256) >> 16) & 0xff, ((256) >> 24) & 0xff);

        if (image->comps[0].prec > 8) {
            adjustR = (int)image->comps[0].prec - 8;
            printf("BMP CONVERSION: Truncating component 0 from %d bits to 8 bits\n", image->comps[0].prec);
        }else
            adjustR = 0;

        for (i = 0; i < 256; i++) {
            fprintf(fdest, "%c%c%c%c", i, i, i, 0);
        }

        for (i = 0; i < w * h; i++) {
            int r;

            r = image->comps[0].data[w * h - ((i) / (w) + 1) * w + (i) % (w)];
            r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
            r = ((r >> adjustR)+((r >> (adjustR-1))%2));
            if(r > 255) r = 255; else if(r < 0) r = 0;

             fprintf(fdest, "%c", (OPJ_UINT8)r);
 
             if ((i + 1) % w == 0) {
                for ((pad = w % 4) ? (4 - w % 4) : 0; pad > 0; pad--)	/* ADD */
                     fprintf(fdest, "%c", 0);
             }
         }
        fclose(fdest);
    }

    return 0;
}
