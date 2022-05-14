transform_test(png_modifier *pmIn, PNG_CONST png_uint_32 idIn,
transform_test(png_modifier *pmIn, const png_uint_32 idIn,
    const image_transform* transform_listIn, const char * const name)
 {
    transform_display d;
    context(&pmIn->this, fault);

   transform_display_init(&d, pmIn, idIn, transform_listIn);

 Try
 {
 size_t pos = 0;
      png_structp pp;
      png_infop pi;
 char full_name[256];

 /* Make sure the encoding fields are correct and enter the required
       * modifications.
       */
      transform_set_encoding(&d);

 /* Add any modifications required by the transform list. */
      d.transform_list->ini(d.transform_list, &d);

 /* Add the color space information, if any, to the name. */
      pos = safecat(full_name, sizeof full_name, pos, name);
      pos = safecat_current_encoding(full_name, sizeof full_name, pos, d.pm);

 /* Get a png_struct for reading the image. */
      pp = set_modifier_for_read(d.pm, &pi, d.this.id, full_name);
      standard_palette_init(&d.this);

#     if 0
 /* Logging (debugging only) */
 {
 char buffer[256];

 (void)store_message(&d.pm->this, pp, buffer, sizeof buffer, 0,
 "running test");

            fprintf(stderr, "%s\n", buffer);
 }
#     endif

 /* Introduce the correct read function. */
 if (d.pm->this.progressive)
 {
 /* Share the row function with the standard implementation. */
         png_set_progressive_read_fn(pp, &d, transform_info, progressive_row,
            transform_end);

 /* Now feed data into the reader until we reach the end: */
         modifier_progressive_read(d.pm, pp, pi);
 }
 else
 {
 /* modifier_read expects a png_modifier* */
         png_set_read_fn(pp, d.pm, modifier_read);

 /* Check the header values: */
         png_read_info(pp, pi);

 /* Process the 'info' requirements. Only one image is generated */
         transform_info_imp(&d, pp, pi);

         sequential_row(&d.this, pp, pi, -1, 0);

 if (!d.this.speed)
            transform_image_validate(&d, pp, pi);
 else
            d.this.ps->validated = 1;
 }

      modifier_reset(d.pm);
 }

 Catch(fault)
 {
      modifier_reset(voidcast(png_modifier*,(void*)fault));
 }
}
