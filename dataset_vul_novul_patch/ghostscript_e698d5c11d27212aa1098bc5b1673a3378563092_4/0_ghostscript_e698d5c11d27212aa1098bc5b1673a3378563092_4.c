 jbig2_image_compose_unopt(Jbig2Ctx *ctx, Jbig2Image *dst, Jbig2Image *src, int x, int y, Jbig2ComposeOp op)
 {
    uint32_t i, j;
    uint32_t sw = src->width;
    uint32_t sh = src->height;
    uint32_t sx = 0;
    uint32_t sy = 0;
 
     /* clip to the dst image boundaries */
     if (x < 0) {
        sx += -x;
        sw -= -x;
        x = 0;
    }
    if (y < 0) {
        sy += -y;
        sh -= -y;
        y = 0;
    }
    if (x + sw >= dst->width)
        sw = dst->width - x;
    if (y + sh >= dst->height)
        sh = dst->height - y;

    switch (op) {
    case JBIG2_COMPOSE_OR:
        for (j = 0; j < sh; j++) {
            for (i = 0; i < sw; i++) {
                jbig2_image_set_pixel(dst, i + x, j + y, jbig2_image_get_pixel(src, i + sx, j + sy) | jbig2_image_get_pixel(dst, i + x, j + y));
            }
        }
        break;
    case JBIG2_COMPOSE_AND:
        for (j = 0; j < sh; j++) {
            for (i = 0; i < sw; i++) {
                jbig2_image_set_pixel(dst, i + x, j + y, jbig2_image_get_pixel(src, i + sx, j + sy) & jbig2_image_get_pixel(dst, i + x, j + y));
            }
        }
        break;
    case JBIG2_COMPOSE_XOR:
        for (j = 0; j < sh; j++) {
            for (i = 0; i < sw; i++) {
                jbig2_image_set_pixel(dst, i + x, j + y, jbig2_image_get_pixel(src, i + sx, j + sy) ^ jbig2_image_get_pixel(dst, i + x, j + y));
            }
        }
        break;
    case JBIG2_COMPOSE_XNOR:
        for (j = 0; j < sh; j++) {
            for (i = 0; i < sw; i++) {
                jbig2_image_set_pixel(dst, i + x, j + y, (jbig2_image_get_pixel(src, i + sx, j + sy) == jbig2_image_get_pixel(dst, i + x, j + y)));
            }
        }
        break;
    case JBIG2_COMPOSE_REPLACE:
        for (j = 0; j < sh; j++) {
            for (i = 0; i < sw; i++) {
                jbig2_image_set_pixel(dst, i + x, j + y, jbig2_image_get_pixel(src, i + sx, j + sy));
            }
        }
        break;
    }

    return 0;
}
