image_transform_png_set_strip_16_mod(PNG_CONST image_transform *this,
image_transform_png_set_strip_16_mod(const image_transform *this,
     image_pixel *that, png_const_structp pp,
    const transform_display *display)
 {
    if (that->bit_depth == 16)
    {
      that->sample_depth = that->bit_depth = 8;
 if (that->red_sBIT > 8) that->red_sBIT = 8;
 if (that->green_sBIT > 8) that->green_sBIT = 8;
 if (that->blue_sBIT > 8) that->blue_sBIT = 8;
 if (that->alpha_sBIT > 8) that->alpha_sBIT = 8;

 /* Prior to 1.5.4 png_set_strip_16 would use an 'accurate' method if this
       * configuration option is set.  From 1.5.4 the flag is never set and the
       * 'scale' API (above) must be used.
       */
#     ifdef PNG_READ_ACCURATE_SCALE_SUPPORTED
#        if PNG_LIBPNG_VER >= 10504
#           error PNG_READ_ACCURATE_SCALE should not be set
#        endif

 /* The strip 16 algorithm drops the low 8 bits rather than calculating
          * 1/257, so we need to adjust the permitted errors appropriately:
          * Notice that this is only relevant prior to the addition of the

           * png_set_scale_16 API in 1.5.4 (but 1.5.4+ always defines the above!)
           */
          {
            const double d = (255-128.5)/65535;
             that->rede += d;
             that->greene += d;
             that->bluee += d;
            that->alphae += d;
 }
#     endif
 }

 this->next->mod(this->next, that, pp, display);
}
