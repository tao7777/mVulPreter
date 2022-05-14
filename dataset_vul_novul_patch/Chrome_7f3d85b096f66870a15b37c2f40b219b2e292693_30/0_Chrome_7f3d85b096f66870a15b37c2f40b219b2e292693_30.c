png_text_compress(png_structp png_ptr,
        png_charp text, png_size_t text_len, int compression,
        compression_state *comp)
{
   int ret;

   comp->num_output_ptr = 0;
   comp->max_output_ptr = 0;
   comp->output_ptr = NULL;
   comp->input = NULL;
   comp->input_len = 0;

   /* We may just want to pass the text right through */
   if (compression == PNG_TEXT_COMPRESSION_NONE)
   {
       comp->input = text;
       comp->input_len = text_len;
       return((int)text_len);
   }

   if (compression >= PNG_TEXT_COMPRESSION_LAST)
   {
#if defined(PNG_STDIO_SUPPORTED) && !defined(_WIN32_WCE)
      char msg[50];
      png_snprintf(msg, 50, "Unknown compression type %d", compression);
      png_warning(png_ptr, msg);
#else
      png_warning(png_ptr, "Unknown compression type");
#endif
   }

   /* We can't write the chunk until we find out how much data we have,
    * which means we need to run the compressor first and save the
    * output.  This shouldn't be a problem, as the vast majority of
    * comments should be reasonable, but we will set up an array of
    * malloc'd pointers to be sure.
    *
    * If we knew the application was well behaved, we could simplify this
    * greatly by assuming we can always malloc an output buffer large
    * enough to hold the compressed text ((1001 * text_len / 1000) + 12)
    * and malloc this directly.  The only time this would be a bad idea is
    * if we can't malloc more than 64K and we have 64K of random input
    * data, or if the input string is incredibly large (although this
    * wouldn't cause a failure, just a slowdown due to swapping).
    */

   /* Set up the compression buffers */
   png_ptr->zstream.avail_in = (uInt)text_len;
   png_ptr->zstream.next_in = (Bytef *)text;
   png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
   png_ptr->zstream.next_out = (Bytef *)png_ptr->zbuf;

   /* This is the same compression loop as in png_write_row() */
   do
   {
      /* Compress the data */
      ret = deflate(&png_ptr->zstream, Z_NO_FLUSH);
      if (ret != Z_OK)
      {
         /* Error */
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }
      /* Check to see if we need more room */
      if (!(png_ptr->zstream.avail_out))
      {
         /* Make sure the output array has room */
         if (comp->num_output_ptr >= comp->max_output_ptr)
         {
            int old_max;

            old_max = comp->max_output_ptr;
            comp->max_output_ptr = comp->num_output_ptr + 4;
            if (comp->output_ptr != NULL)
            {
               png_charpp old_ptr;

                old_ptr = comp->output_ptr;
                comp->output_ptr = (png_charpp)png_malloc(png_ptr,
                   (png_uint_32)
                  (comp->max_output_ptr * png_sizeof(png_charp)));
                png_memcpy(comp->output_ptr, old_ptr, old_max
                   * png_sizeof(png_charp));
                png_free(png_ptr, old_ptr);
            }
            else
               comp->output_ptr = (png_charpp)png_malloc(png_ptr,
                  (png_uint_32)
                  (comp->max_output_ptr * png_sizeof(png_charp)));
         }

         /* Save the data */
         comp->output_ptr[comp->num_output_ptr] =
            (png_charp)png_malloc(png_ptr,
            (png_uint_32)png_ptr->zbuf_size);
         png_memcpy(comp->output_ptr[comp->num_output_ptr], png_ptr->zbuf,
            png_ptr->zbuf_size);
         comp->num_output_ptr++;

         /* and reset the buffer */
         png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
         png_ptr->zstream.next_out = png_ptr->zbuf;
      }
   /* Continue until we don't have any more to compress */
   } while (png_ptr->zstream.avail_in);

   /* Finish the compression */
   do
   {
      /* Tell zlib we are finished */
      ret = deflate(&png_ptr->zstream, Z_FINISH);

      if (ret == Z_OK)
      {
         /* Check to see if we need more room */
         if (!(png_ptr->zstream.avail_out))
         {
            /* Check to make sure our output array has room */
            if (comp->num_output_ptr >= comp->max_output_ptr)
            {
               int old_max;

               old_max = comp->max_output_ptr;
               comp->max_output_ptr = comp->num_output_ptr + 4;
               if (comp->output_ptr != NULL)
               {
                  png_charpp old_ptr;

                  old_ptr = comp->output_ptr;
                  /* This could be optimized to realloc() */
                  comp->output_ptr = (png_charpp)png_malloc(png_ptr,
                     (png_uint_32)(comp->max_output_ptr *
                     png_sizeof(png_charp)));
                  png_memcpy(comp->output_ptr, old_ptr,
                     old_max * png_sizeof(png_charp));
                  png_free(png_ptr, old_ptr);
               }
               else
                  comp->output_ptr = (png_charpp)png_malloc(png_ptr,
                     (png_uint_32)(comp->max_output_ptr *
                     png_sizeof(png_charp)));
            }

            /* Save the data */
            comp->output_ptr[comp->num_output_ptr] =
               (png_charp)png_malloc(png_ptr,
               (png_uint_32)png_ptr->zbuf_size);
            png_memcpy(comp->output_ptr[comp->num_output_ptr], png_ptr->zbuf,
               png_ptr->zbuf_size);
            comp->num_output_ptr++;

            /* and reset the buffer pointers */
            png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
            png_ptr->zstream.next_out = png_ptr->zbuf;
         }
      }
      else if (ret != Z_STREAM_END)
      {
         /* We got an error */
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }
   } while (ret != Z_STREAM_END);

   /* Text length is number of buffers plus last buffer */
   text_len = png_ptr->zbuf_size * comp->num_output_ptr;
   if (png_ptr->zstream.avail_out < png_ptr->zbuf_size)
      text_len += png_ptr->zbuf_size - (png_size_t)png_ptr->zstream.avail_out;

   return((int)text_len);
}
