jbig2_page_add_result(Jbig2Ctx *ctx, Jbig2Page *page, Jbig2Image *image, int x, int y, Jbig2ComposeOp op)
{
    /* ensure image exists first */
    if (page->image == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, -1, "page info possibly missing, no image defined");
        return 0;
    }
 
     /* grow the page to accomodate a new stripe if necessary */
     if (page->striped) {
        uint32_t new_height = y + image->height + page->end_row;
 
         if (page->image->height < new_height) {
             jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, -1, "growing page buffer to %d rows " "to accomodate new stripe", new_height);
            jbig2_image_resize(ctx, page->image, page->image->width, new_height);
        }
    }

    jbig2_image_compose(ctx, page->image, image, x, y + page->end_row, op);

    return 0;
}
