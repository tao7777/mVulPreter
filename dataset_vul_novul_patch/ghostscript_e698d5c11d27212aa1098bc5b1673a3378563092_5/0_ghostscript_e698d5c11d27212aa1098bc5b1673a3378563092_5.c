jbig2_image_new(Jbig2Ctx *ctx, int width, int height)
jbig2_image_new(Jbig2Ctx *ctx, uint32_t width, uint32_t height)
 {
     Jbig2Image *image;
    uint32_t stride;
     int64_t check;
 
     image = jbig2_new(ctx, Jbig2Image, 1);
    if (image == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "could not allocate image structure in jbig2_image_new");
        return NULL;
    }

    stride = ((width - 1) >> 3) + 1;    /* generate a byte-aligned stride */
    /* check for integer multiplication overflow */
    check = ((int64_t) stride) * ((int64_t) height);
    if (check != (int)check) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "integer multiplication overflow from stride(%d)*height(%d)", stride, height);
        jbig2_free(ctx->allocator, image);
        return NULL;
    }
    /* Add 1 to accept runs that exceed image width and clamped to width+1 */
    image->data = jbig2_new(ctx, uint8_t, (int)check + 1);
    if (image->data == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "could not allocate image data buffer! [stride(%d)*height(%d) bytes]", stride, height);
        jbig2_free(ctx->allocator, image);
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->stride = stride;
    image->refcount = 1;

    return image;
}
