transform_enable(PNG_CONST char *name)
image_transform_png_set_invert_alpha_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->colour_type & 4)
      that->alpha_inverted = 1;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_invert_alpha_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(bit_depth)

   this->next = *that;
   *that = this;

   /* Only has an effect on pixels with alpha: */
   return (colour_type & 4) != 0;
}

IT(invert_alpha);
#undef PT
#define PT ITSTRUCT(invert_alpha)

#endif /* PNG_READ_INVERT_ALPHA_SUPPORTED */

/* png_set_bgr */
#ifdef PNG_READ_BGR_SUPPORTED
/* Swap R,G,B channels to order B,G,R.
 *
 *  png_set_bgr(png_structrp png_ptr)
 *
 * This only has an effect on RGB and RGBA pixels.
 */
static void
image_transform_png_set_bgr_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_bgr(pp);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_bgr_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->colour_type == PNG_COLOR_TYPE_RGB ||
       that->colour_type == PNG_COLOR_TYPE_RGBA)
       that->swap_rgb = 1;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_bgr_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(bit_depth)

   this->next = *that;
   *that = this;

   return colour_type == PNG_COLOR_TYPE_RGB ||
       colour_type == PNG_COLOR_TYPE_RGBA;
}

IT(bgr);
#undef PT
#define PT ITSTRUCT(bgr)

#endif /* PNG_READ_BGR_SUPPORTED */

/* png_set_swap_alpha */
#ifdef PNG_READ_SWAP_ALPHA_SUPPORTED
/* Put the alpha channel first.
 *
 *  png_set_swap_alpha(png_structrp png_ptr)
 *
 * This only has an effect on GA and RGBA pixels.
 */
static void
image_transform_png_set_swap_alpha_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_swap_alpha(pp);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_swap_alpha_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->colour_type == PNG_COLOR_TYPE_GA ||
       that->colour_type == PNG_COLOR_TYPE_RGBA)
      that->alpha_first = 1;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_swap_alpha_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(bit_depth)

   this->next = *that;
   *that = this;

   return colour_type == PNG_COLOR_TYPE_GA ||
       colour_type == PNG_COLOR_TYPE_RGBA;
}

IT(swap_alpha);
#undef PT
#define PT ITSTRUCT(swap_alpha)

#endif /* PNG_READ_SWAP_ALPHA_SUPPORTED */

/* png_set_swap */
#ifdef PNG_READ_SWAP_SUPPORTED
/* Byte swap 16-bit components.
 *
 *  png_set_swap(png_structrp png_ptr)
 */
static void
image_transform_png_set_swap_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_swap(pp);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_swap_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->bit_depth == 16)
      that->swap16 = 1;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_swap_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(colour_type)

   this->next = *that;
   *that = this;

   return bit_depth == 16;
}

IT(swap);
#undef PT
#define PT ITSTRUCT(swap)

#endif /* PNG_READ_SWAP_SUPPORTED */

#ifdef PNG_READ_FILLER_SUPPORTED
/* Add a filler byte to 8-bit Gray or 24-bit RGB images.
 *
 *  png_set_filler, (png_structp png_ptr, png_uint_32 filler, int flags));
 *
 * Flags:
 *
 *  PNG_FILLER_BEFORE
 *  PNG_FILLER_AFTER
 */
#define data ITDATA(filler)
static struct
{
   png_uint_32 filler;
   int         flags;
} data;

static void
image_transform_png_set_filler_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   /* Need a random choice for 'before' and 'after' as well as for the
    * filler.  The 'filler' value has all 32 bits set, but only bit_depth
    * will be used.  At this point we don't know bit_depth.
    */
   RANDOMIZE(data.filler);
   data.flags = random_choice();

   png_set_filler(pp, data.filler, data.flags);

   /* The standard display handling stuff also needs to know that
    * there is a filler, so set that here.
    */
   that->this.filler = 1;

   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_filler_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->bit_depth >= 8 &&
       (that->colour_type == PNG_COLOR_TYPE_RGB ||
        that->colour_type == PNG_COLOR_TYPE_GRAY))
   {
      const unsigned int max = (1U << that->bit_depth)-1;
      that->alpha = data.filler & max;
      that->alphaf = ((double)that->alpha) / max;
      that->alphae = 0;

      /* The filler has been stored in the alpha channel, we must record
       * that this has been done for the checking later on, the color
       * type is faked to have an alpha channel, but libpng won't report
       * this; the app has to know the extra channel is there and this
       * was recording in standard_display::filler above.
       */
      that->colour_type |= 4; /* alpha added */
      that->alpha_first = data.flags == PNG_FILLER_BEFORE;
   }

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_filler_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   this->next = *that;
   *that = this;

   return bit_depth >= 8 && (colour_type == PNG_COLOR_TYPE_RGB ||
           colour_type == PNG_COLOR_TYPE_GRAY);
}

#undef data
IT(filler);
#undef PT
#define PT ITSTRUCT(filler)

/* png_set_add_alpha, (png_structp png_ptr, png_uint_32 filler, int flags)); */
/* Add an alpha byte to 8-bit Gray or 24-bit RGB images. */
#define data ITDATA(add_alpha)
static struct
{
   png_uint_32 filler;
   int         flags;
} data;

static void
image_transform_png_set_add_alpha_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   /* Need a random choice for 'before' and 'after' as well as for the
    * filler.  The 'filler' value has all 32 bits set, but only bit_depth
    * will be used.  At this point we don't know bit_depth.
    */
   RANDOMIZE(data.filler);
   data.flags = random_choice();

   png_set_add_alpha(pp, data.filler, data.flags);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_add_alpha_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->bit_depth >= 8 &&
       (that->colour_type == PNG_COLOR_TYPE_RGB ||
        that->colour_type == PNG_COLOR_TYPE_GRAY))
   {
      const unsigned int max = (1U << that->bit_depth)-1;
      that->alpha = data.filler & max;
      that->alphaf = ((double)that->alpha) / max;
      that->alphae = 0;

      that->colour_type |= 4; /* alpha added */
      that->alpha_first = data.flags == PNG_FILLER_BEFORE;
   }

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_add_alpha_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   this->next = *that;
   *that = this;

   return bit_depth >= 8 && (colour_type == PNG_COLOR_TYPE_RGB ||
           colour_type == PNG_COLOR_TYPE_GRAY);
}

#undef data
IT(add_alpha);
#undef PT
#define PT ITSTRUCT(add_alpha)

#endif /* PNG_READ_FILLER_SUPPORTED */

/* png_set_packing */
#ifdef PNG_READ_PACK_SUPPORTED
/* Use 1 byte per pixel in 1, 2, or 4-bit depth files.
 *
 *  png_set_packing(png_structrp png_ptr)
 *
 * This should only affect grayscale and palette images with less than 8 bits
 * per pixel.
 */
static void
image_transform_png_set_packing_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_packing(pp);
   that->unpacked = 1;
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_packing_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   /* The general expand case depends on what the colour type is,
    * low bit-depth pixel values are unpacked into bytes without
    * scaling, so sample_depth is not changed.
    */
   if (that->bit_depth < 8) /* grayscale or palette */
      that->bit_depth = 8;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_packing_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(colour_type)

   this->next = *that;
   *that = this;

   /* Nothing should happen unless the bit depth is less than 8: */
   return bit_depth < 8;
}

IT(packing);
#undef PT
#define PT ITSTRUCT(packing)

#endif /* PNG_READ_PACK_SUPPORTED */

/* png_set_packswap */
#ifdef PNG_READ_PACKSWAP_SUPPORTED
/* Swap pixels packed into bytes; reverses the order on screen so that
 * the high order bits correspond to the rightmost pixels.
 *
 *  png_set_packswap(png_structrp png_ptr)
 */
static void
image_transform_png_set_packswap_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_packswap(pp);
   that->this.littleendian = 1;
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_packswap_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->bit_depth < 8)
      that->littleendian = 1;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_packswap_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(colour_type)

   this->next = *that;
   *that = this;

   return bit_depth < 8;
}

IT(packswap);
#undef PT
#define PT ITSTRUCT(packswap)

#endif /* PNG_READ_PACKSWAP_SUPPORTED */


/* png_set_invert_mono */
#ifdef PNG_READ_INVERT_MONO_SUPPORTED
/* Invert the gray channel
 *
 *  png_set_invert_mono(png_structrp png_ptr)
 */
static void
image_transform_png_set_invert_mono_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_invert_mono(pp);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_invert_mono_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   if (that->colour_type & 4)
      that->mono_inverted = 1;

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_invert_mono_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(bit_depth)

   this->next = *that;
   *that = this;

   /* Only has an effect on pixels with no colour: */
   return (colour_type & 2) == 0;
}

IT(invert_mono);
#undef PT
#define PT ITSTRUCT(invert_mono)

#endif /* PNG_READ_INVERT_MONO_SUPPORTED */

#ifdef PNG_READ_SHIFT_SUPPORTED
/* png_set_shift(png_structp, png_const_color_8p true_bits)
 *
 * The output pixels will be shifted by the given true_bits
 * values.
 */
#define data ITDATA(shift)
static png_color_8 data;

static void
image_transform_png_set_shift_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   /* Get a random set of shifts.  The shifts need to do something
    * to test the transform, so they are limited to the bit depth
    * of the input image.  Notice that in the following the 'gray'
    * field is randomized independently.  This acts as a check that
    * libpng does use the correct field.
    */
   const unsigned int depth = that->this.bit_depth;

   data.red = (png_byte)/*SAFE*/(random_mod(depth)+1);
   data.green = (png_byte)/*SAFE*/(random_mod(depth)+1);
   data.blue = (png_byte)/*SAFE*/(random_mod(depth)+1);
   data.gray = (png_byte)/*SAFE*/(random_mod(depth)+1);
   data.alpha = (png_byte)/*SAFE*/(random_mod(depth)+1);

   png_set_shift(pp, &data);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_shift_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   /* Copy the correct values into the sBIT fields, libpng does not do
    * anything to palette data:
    */
   if (that->colour_type != PNG_COLOR_TYPE_PALETTE)
   {
       that->sig_bits = 1;

       /* The sBIT fields are reset to the values previously sent to
        * png_set_shift according to the colour type.
        * does.
        */
       if (that->colour_type & 2) /* RGB channels */
       {
          that->red_sBIT = data.red;
          that->green_sBIT = data.green;
          that->blue_sBIT = data.blue;
       }

       else /* One grey channel */
          that->red_sBIT = that->green_sBIT = that->blue_sBIT = data.gray;

       that->alpha_sBIT = data.alpha;
   }

   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_shift_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   UNUSED(bit_depth)

   this->next = *that;
   *that = this;

   return colour_type != PNG_COLOR_TYPE_PALETTE;
}

IT(shift);
#undef PT
#define PT ITSTRUCT(shift)

#endif /* PNG_READ_SHIFT_SUPPORTED */

#ifdef THIS_IS_THE_PROFORMA
static void
image_transform_png_set_@_set(const image_transform *this,
    transform_display *that, png_structp pp, png_infop pi)
{
   png_set_@(pp);
   this->next->set(this->next, that, pp, pi);
}

static void
image_transform_png_set_@_mod(const image_transform *this,
    image_pixel *that, png_const_structp pp,
    const transform_display *display)
{
   this->next->mod(this->next, that, pp, display);
}

static int
image_transform_png_set_@_add(image_transform *this,
    const image_transform **that, png_byte colour_type, png_byte bit_depth)
{
   this->next = *that;
   *that = this;

   return 1;
}

IT(@);
#endif


/* This may just be 'end' if all the transforms are disabled! */
static image_transform *const image_transform_first = &PT;

static void
transform_enable(const char *name)
 {
    /* Everything starts out enabled, so if we see an 'enable' disabled
     * everything else the first time round.
    */
 static int all_disabled = 0;
 int found_it = 0;
   image_transform *list = image_transform_first;

 while (list != &image_transform_end)
 {
 if (strcmp(list->name, name) == 0)
 {
 list->enable = 1;
         found_it = 1;
 }
 else if (!all_disabled)
 list->enable = 0;

 list = list->list;
 }

   all_disabled = 1;

 if (!found_it)
 {
      fprintf(stderr, "pngvalid: --transform-enable=%s: unknown transform\n",
         name);
      exit(99);
 }

 }
