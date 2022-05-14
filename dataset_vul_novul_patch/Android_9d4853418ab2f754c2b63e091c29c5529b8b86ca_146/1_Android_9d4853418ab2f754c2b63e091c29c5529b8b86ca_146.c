 transform_display_init(transform_display *dp, png_modifier *pm, png_uint_32 id,
    PNG_CONST image_transform *transform_list)
 {
    memset(dp, 0, sizeof *dp);
 
    /* Standard fields */
   standard_display_init(&dp->this, &pm->this, id, 0/*do_interlace*/,
       pm->use_update_info);
 
    /* Parameter fields */
    dp->pm = pm;
    dp->transform_list = transform_list;
 
    /* Local variable fields */
    dp->output_colour_type = 255; /* invalid */
    dp->output_bit_depth = 255;  /* invalid */
 }
