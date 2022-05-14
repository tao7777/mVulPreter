jbig2_decode_gray_scale_image(Jbig2Ctx *ctx, Jbig2Segment *segment,
                              const byte *data, const size_t size,
                              bool GSMMR, uint32_t GSW, uint32_t GSH,
                              uint32_t GSBPP, bool GSUSESKIP, Jbig2Image *GSKIP, int GSTEMPLATE, Jbig2ArithCx *GB_stats)
 {
     uint8_t **GSVALS = NULL;
     size_t consumed_bytes = 0;
    uint32_t i, j, stride, x, y;
    int code;
     Jbig2Image **GSPLANES;
     Jbig2GenericRegionParams rparams;
     Jbig2WordStream *ws = NULL;
    Jbig2ArithState *as = NULL;

    /* allocate GSPLANES */
    GSPLANES = jbig2_new(ctx, Jbig2Image *, GSBPP);
    if (GSPLANES == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate %d bytes for GSPLANES", GSBPP);
        return NULL;
    }

    for (i = 0; i < GSBPP; ++i) {
        GSPLANES[i] = jbig2_image_new(ctx, GSW, GSH);
         if (GSPLANES[i] == NULL) {
             jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate %dx%d image for GSPLANES", GSW, GSH);
             /* free already allocated */
            for (j = i; j > 0;)
                jbig2_image_release(ctx, GSPLANES[--j]);
             jbig2_free(ctx->allocator, GSPLANES);
             return NULL;
         }
        }
    }
