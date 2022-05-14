compare_two_images(Image *a, Image *b, int via_linear,
   png_const_colorp background)
{
 ptrdiff_t stridea = a->stride;
 ptrdiff_t strideb = b->stride;
   png_const_bytep rowa = a->buffer+16;
   png_const_bytep rowb = b->buffer+16;
 const png_uint_32 width = a->image.width;
 const png_uint_32 height = a->image.height;
 const png_uint_32 formata = a->image.format;
 const png_uint_32 formatb = b->image.format;
 const unsigned int a_sample = PNG_IMAGE_SAMPLE_SIZE(formata);
 const unsigned int b_sample = PNG_IMAGE_SAMPLE_SIZE(formatb);
 int alpha_added, alpha_removed;
 int bchannels;
 int btoa[4];
   png_uint_32 y;
 Transform tr;

 /* This should never happen: */
 if (width != b->image.width || height != b->image.height)
 return logerror(a, a->file_name, ": width x height changed: ",
         b->file_name);

 /* Set up the background and the transform */
   transform_from_formats(&tr, a, b, background, via_linear);

 /* Find the first row and inter-row space. */
 if (!(formata & PNG_FORMAT_FLAG_COLORMAP) &&
 (formata & PNG_FORMAT_FLAG_LINEAR))
      stridea *= 2;

 if (!(formatb & PNG_FORMAT_FLAG_COLORMAP) &&
 (formatb & PNG_FORMAT_FLAG_LINEAR))
      strideb *= 2;

 if (stridea < 0) rowa += (height-1) * (-stridea);
 if (strideb < 0) rowb += (height-1) * (-strideb);

 /* First shortcut the two colormap case by comparing the image data; if it
    * matches then we expect the colormaps to match, although this is not
    * absolutely necessary for an image match.  If the colormaps fail to match
    * then there is a problem in libpng.
    */
 if (formata & formatb & PNG_FORMAT_FLAG_COLORMAP)
 {
 /* Only check colormap entries that actually exist; */
      png_const_bytep ppa, ppb;
 int match;
      png_byte in_use[256], amax = 0, bmax = 0;

      memset(in_use, 0, sizeof in_use);

      ppa = rowa;
      ppb = rowb;

 /* Do this the slow way to accumulate the 'in_use' flags, don't break out
       * of the loop until the end; this validates the color-mapped data to
       * ensure all pixels are valid color-map indexes.
       */
 for (y=0, match=1; y<height && match; ++y, ppa += stridea, ppb += strideb)
 {
         png_uint_32 x;

 for (x=0; x<width; ++x)
 {
            png_byte bval = ppb[x];
            png_byte aval = ppa[x];

 if (bval > bmax)
               bmax = bval;

 if (bval != aval)
               match = 0;

            in_use[aval] = 1;
 if (aval > amax)
               amax = aval;
 }
 }

 /* If the buffers match then the colormaps must too. */
 if (match)
 {
 /* Do the color-maps match, entry by entry?  Only check the 'in_use'
          * entries.  An error here should be logged as a color-map error.
          */
         png_const_bytep a_cmap = (png_const_bytep)a->colormap;
         png_const_bytep b_cmap = (png_const_bytep)b->colormap;
 int result = 1; /* match by default */

 /* This is used in logpixel to get the error message correct. */
         tr.is_palette = 1;

 for (y=0; y<256; ++y, a_cmap += a_sample, b_cmap += b_sample)
 if (in_use[y])
 {
 /* The colormap entries should be valid, but because libpng doesn't
             * do any checking at present the original image may contain invalid
             * pixel values.  These cause an error here (at present) unless
             * accumulating errors in which case the program just ignores them.
             */
 if (y >= a->image.colormap_entries)
 {
 if ((a->opts & ACCUMULATE) == 0)
 {
 char pindex[9];
                  sprintf(pindex, "%lu[%lu]", (unsigned long)y,
 (unsigned long)a->image.colormap_entries);
                  logerror(a, a->file_name, ": bad pixel index: ", pindex);
 }
               result = 0;
 }

 
             else if (y >= b->image.colormap_entries)
             {
               if ((b->opts & ACCUMULATE) == 0)
                   {
                   char pindex[9];
                   sprintf(pindex, "%lu[%lu]", (unsigned long)y,
 (unsigned long)b->image.colormap_entries);
                  logerror(b, b->file_name, ": bad pixel index: ", pindex);
 }
               result = 0;
 }

 /* All the mismatches are logged here; there can only be 256! */
 else if (!cmppixel(&tr, a_cmap, b_cmap, 0, y))
               result = 0;
 }

 /* If reqested copy the error values back from the Transform. */
 if (a->opts & ACCUMULATE)
 {
            tr.error_ptr[0] = tr.error[0];
            tr.error_ptr[1] = tr.error[1];
            tr.error_ptr[2] = tr.error[2];
            tr.error_ptr[3] = tr.error[3];
            result = 1; /* force a continue */
 }

 return result;
 }

 /* else the image buffers don't match pixel-wise so compare sample values
       * instead, but first validate that the pixel indexes are in range (but
       * only if not accumulating, when the error is ignored.)
       */
 else if ((a->opts & ACCUMULATE) == 0)
 {
 /* Check the original image first,
          * TODO: deal with input images with bad pixel values?
          */
 if (amax >= a->image.colormap_entries)
 {
 char pindex[9];
            sprintf(pindex, "%d[%lu]", amax,
 (unsigned long)a->image.colormap_entries);
 return logerror(a, a->file_name, ": bad pixel index: ", pindex);
 }

 else if (bmax >= b->image.colormap_entries)
 {
 char pindex[9];
            sprintf(pindex, "%d[%lu]", bmax,
 (unsigned long)b->image.colormap_entries);
 return logerror(b, b->file_name, ": bad pixel index: ", pindex);
 }
 }
 }

 /* We can directly compare pixel values without the need to use the read
    * or transform support (i.e. a memory compare) if:
    *
    * 1) The bit depth has not changed.
    * 2) RGB to grayscale has not been done (the reverse is ok; we just compare
    *    the three RGB values to the original grayscale.)
    * 3) An alpha channel has not been removed from an 8-bit format, or the
    *    8-bit alpha value of the pixel was 255 (opaque).
    *
    * If an alpha channel has been *added* then it must have the relevant opaque
    * value (255 or 65535).
    *
    * The fist two the tests (in the order given above) (using the boolean
    * equivalence !a && !b == !(a || b))
    */
 if (!(((formata ^ formatb) & PNG_FORMAT_FLAG_LINEAR) |
 (formata & (formatb ^ PNG_FORMAT_FLAG_COLOR) & PNG_FORMAT_FLAG_COLOR)))
 {
 /* Was an alpha channel changed? */
 const png_uint_32 alpha_changed = (formata ^ formatb) &
         PNG_FORMAT_FLAG_ALPHA;

 /* Was an alpha channel removed?  (The third test.)  If so the direct
       * comparison is only possible if the input alpha is opaque.
       */
      alpha_removed = (formata & alpha_changed) != 0;

 /* Was an alpha channel added? */
      alpha_added = (formatb & alpha_changed) != 0;

 /* The channels may have been moved between input and output, this finds
       * out how, recording the result in the btoa array, which says where in
       * 'a' to find each channel of 'b'.  If alpha was added then btoa[alpha]
       * ends up as 4 (and is not used.)
       */
 {
 int i;
         png_byte aloc[4];
         png_byte bloc[4];

 /* The following are used only if the formats match, except that
          * 'bchannels' is a flag for matching formats.  btoa[x] says, for each
          * channel in b, where to find the corresponding value in a, for the
          * bchannels.  achannels may be different for a gray to rgb transform
          * (a will be 1 or 2, b will be 3 or 4 channels.)
          */
 (void)component_loc(aloc, formata);
         bchannels = component_loc(bloc, formatb);

 /* Hence the btoa array. */
 for (i=0; i<4; ++i) if (bloc[i] < 4)
            btoa[bloc[i]] = aloc[i]; /* may be '4' for alpha */

 if (alpha_added)
            alpha_added = bloc[0]; /* location of alpha channel in image b */

 else
            alpha_added = 4; /* Won't match an image b channel */

 if (alpha_removed)
            alpha_removed = aloc[0]; /* location of alpha channel in image a */

 else
            alpha_removed = 4;
 }
 }

 else
 {
 /* Direct compare is not possible, cancel out all the corresponding local
       * variables.
       */
      bchannels = 0;
      alpha_removed = alpha_added = 4;
      btoa[3] = btoa[2] = btoa[1] = btoa[0] = 4; /* 4 == not present */
 }

 for (y=0; y<height; ++y, rowa += stridea, rowb += strideb)
 {
      png_const_bytep ppa, ppb;
      png_uint_32 x;

 for (x=0, ppa=rowa, ppb=rowb; x<width; ++x)
 {
         png_const_bytep psa, psb;

 if (formata & PNG_FORMAT_FLAG_COLORMAP)
            psa = (png_const_bytep)a->colormap + a_sample * *ppa++;
 else
            psa = ppa, ppa += a_sample;

 if (formatb & PNG_FORMAT_FLAG_COLORMAP)
            psb = (png_const_bytep)b->colormap + b_sample * *ppb++;
 else
            psb = ppb, ppb += b_sample;

 /* Do the fast test if possible. */
 if (bchannels)
 {
 /* Check each 'b' channel against either the corresponding 'a'
             * channel or the opaque alpha value, as appropriate.  If
             * alpha_removed value is set (not 4) then also do this only if the
             * 'a' alpha channel (alpha_removed) is opaque; only relevant for
             * the 8-bit case.
             */
 if (formatb & PNG_FORMAT_FLAG_LINEAR) /* 16-bit checks */
 {
               png_const_uint_16p pua = aligncastconst(png_const_uint_16p, psa);
               png_const_uint_16p pub = aligncastconst(png_const_uint_16p, psb);

 switch (bchannels)
 {
 case 4:
 if (pua[btoa[3]] != pub[3]) break;
 case 3:
 if (pua[btoa[2]] != pub[2]) break;
 case 2:
 if (pua[btoa[1]] != pub[1]) break;
 case 1:
 if (pua[btoa[0]] != pub[0]) break;
 if (alpha_added != 4 && pub[alpha_added] != 65535) break;
 continue; /* x loop */
 default:
 break; /* impossible */
 }
 }

 else if (alpha_removed == 4 || psa[alpha_removed] == 255)
 {
 switch (bchannels)
 {
 case 4:
 if (psa[btoa[3]] != psb[3]) break;
 case 3:
 if (psa[btoa[2]] != psb[2]) break;
 case 2:
 if (psa[btoa[1]] != psb[1]) break;
 case 1:
 if (psa[btoa[0]] != psb[0]) break;
 if (alpha_added != 4 && psb[alpha_added] != 255) break;
 continue; /* x loop */
 default:
 break; /* impossible */
 }
 }
 }

 /* If we get to here the fast match failed; do the slow match for this
          * pixel.
          */
 if (!cmppixel(&tr, psa, psb, x, y) && (a->opts & KEEP_GOING) == 0)
 return 0; /* error case */
 }
 }

 /* If reqested copy the error values back from the Transform. */
 if (a->opts & ACCUMULATE)
 {
      tr.error_ptr[0] = tr.error[0];
      tr.error_ptr[1] = tr.error[1];
      tr.error_ptr[2] = tr.error[2];
      tr.error_ptr[3] = tr.error[3];
 }

 return 1;
}
