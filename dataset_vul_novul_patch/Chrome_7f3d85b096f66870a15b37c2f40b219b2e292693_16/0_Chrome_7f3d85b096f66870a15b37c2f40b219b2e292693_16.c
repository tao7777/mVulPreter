png_handle_iCCP(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
/* Note: this does not properly handle chunks that are > 64K under DOS */
{
   png_byte compression_type;
   png_bytep pC;
   png_charp profile;
   png_uint_32 skip = 0;
   png_uint_32 profile_size, profile_length;
   png_size_t slength, prefix_length, data_length;

   png_debug(1, "in png_handle_iCCP");

   if (!(png_ptr->mode & PNG_HAVE_IHDR))
      png_error(png_ptr, "Missing IHDR before iCCP");
   else if (png_ptr->mode & PNG_HAVE_IDAT)
   {
      png_warning(png_ptr, "Invalid iCCP after IDAT");
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (png_ptr->mode & PNG_HAVE_PLTE)
      /* Should be an error, but we can cope with it */
      png_warning(png_ptr, "Out of place iCCP chunk");

   if (info_ptr != NULL && (info_ptr->valid & PNG_INFO_iCCP))
   {
      png_warning(png_ptr, "Duplicate iCCP chunk");
      png_crc_finish(png_ptr, length);
      return;
   }

#ifdef PNG_MAX_MALLOC_64K
   if (length > (png_uint_32)65535L)
   {
      png_warning(png_ptr, "iCCP chunk too large to fit in memory");
      skip = length - (png_uint_32)65535L;
      length = (png_uint_32)65535L;
   }
#endif

   png_free(png_ptr, png_ptr->chunkdata);
   png_ptr->chunkdata = (png_charp)png_malloc(png_ptr, length + 1);
   slength = (png_size_t)length;
   png_crc_read(png_ptr, (png_bytep)png_ptr->chunkdata, slength);

   if (png_crc_finish(png_ptr, skip))
   {
      png_free(png_ptr, png_ptr->chunkdata);
      png_ptr->chunkdata = NULL;
      return;
   }

   png_ptr->chunkdata[slength] = 0x00;

   for (profile = png_ptr->chunkdata; *profile; profile++)
      /* Empty loop to find end of name */ ;

   ++profile;

   /* There should be at least one zero (the compression type byte)
    * following the separator, and we should be on it
    */
   if ( profile >= png_ptr->chunkdata + slength - 1)
   {
      png_free(png_ptr, png_ptr->chunkdata);
      png_ptr->chunkdata = NULL;
      png_warning(png_ptr, "Malformed iCCP chunk");
      return;
   }

   /* Compression_type should always be zero */
   compression_type = *profile++;
   if (compression_type)
   {
      png_warning(png_ptr, "Ignoring nonzero compression type in iCCP chunk");
      compression_type = 0x00;  /* Reset it to zero (libpng-1.0.6 through 1.0.8
                                 wrote nonzero) */
   }

   prefix_length = profile - png_ptr->chunkdata;
   png_decompress_chunk(png_ptr, compression_type,
     slength, prefix_length, &data_length);

   profile_length = data_length - prefix_length;

   if ( prefix_length > data_length || profile_length < 4)
   {
      png_free(png_ptr, png_ptr->chunkdata);
      png_ptr->chunkdata = NULL;
      png_warning(png_ptr, "Profile size field missing from iCCP chunk");
      return;
   }
 
    /* Check the profile_size recorded in the first 32 bits of the ICC profile */
    pC = (png_bytep)(png_ptr->chunkdata + prefix_length);
   profile_size = ((png_uint_32) (*(pC    )<<24)) |
                  ((png_uint_32) (*(pC + 1)<<16)) |
                  ((png_uint_32) (*(pC + 2)<< 8)) |
                  ((png_uint_32) (*(pC + 3)    ));
 
    if (profile_size < profile_length)
       profile_length = profile_size;

   if (profile_size > profile_length)
   {
      png_free(png_ptr, png_ptr->chunkdata);
      png_ptr->chunkdata = NULL;
      png_warning(png_ptr, "Ignoring truncated iCCP profile.");
      return;
   }

   png_set_iCCP(png_ptr, info_ptr, png_ptr->chunkdata,
     compression_type, png_ptr->chunkdata + prefix_length, profile_length);
   png_free(png_ptr, png_ptr->chunkdata);
   png_ptr->chunkdata = NULL;
}
