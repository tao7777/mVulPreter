png_set_text_2(png_structp png_ptr, png_infop info_ptr, png_textp text_ptr,
               int num_text)
{
   int i;

   png_debug1(1, "in %s storage function", ((png_ptr == NULL ||
      png_ptr->chunk_name[0] == '\0') ?
      "text" : (png_const_charp)png_ptr->chunk_name));

   if (png_ptr == NULL || info_ptr == NULL || num_text == 0)
      return(0);

   /* Make sure we have enough space in the "text" array in info_struct
    * to hold all of the incoming text_ptr objects.
    */
   if (info_ptr->num_text + num_text > info_ptr->max_text)
   {
      int old_max_text = info_ptr->max_text;
      int old_num_text = info_ptr->num_text;

      if (info_ptr->text != NULL)
      {
         png_textp old_text;

         info_ptr->max_text = info_ptr->num_text + num_text + 8;
         old_text = info_ptr->text;

         info_ptr->text = (png_textp)png_malloc_warn(png_ptr,
            (png_uint_32)(info_ptr->max_text * png_sizeof(png_text)));
         if (info_ptr->text == NULL)
         {
            /* Restore to previous condition */
            info_ptr->max_text = old_max_text;
            info_ptr->text = old_text;
            return(1);
         }
         png_memcpy(info_ptr->text, old_text, (png_size_t)(old_max_text *
            png_sizeof(png_text)));
         png_free(png_ptr, old_text);
      }
      else
      {
         info_ptr->max_text = num_text + 8;
         info_ptr->num_text = 0;
         info_ptr->text = (png_textp)png_malloc_warn(png_ptr,
            (png_uint_32)(info_ptr->max_text * png_sizeof(png_text)));
         if (info_ptr->text == NULL)
         {
            /* Restore to previous condition */
            info_ptr->num_text = old_num_text;
            info_ptr->max_text = old_max_text;
            return(1);
         }
#ifdef PNG_FREE_ME_SUPPORTED
         info_ptr->free_me |= PNG_FREE_TEXT;
#endif
      }
      png_debug1(3, "allocated %d entries for info_ptr->text",
         info_ptr->max_text);
   }

   for (i = 0; i < num_text; i++)
   {
      png_size_t text_length, key_len;
      png_size_t lang_len, lang_key_len;
      png_textp textp = &(info_ptr->text[info_ptr->num_text]);

      if (text_ptr[i].key == NULL)
          continue;

      key_len = png_strlen(text_ptr[i].key);

      if (text_ptr[i].compression <= 0)
      {
         lang_len = 0;
         lang_key_len = 0;
      }

      else
#ifdef PNG_iTXt_SUPPORTED
      {
         /* Set iTXt data */

         if (text_ptr[i].lang != NULL)
            lang_len = png_strlen(text_ptr[i].lang);
         else
            lang_len = 0;
         if (text_ptr[i].lang_key != NULL)
            lang_key_len = png_strlen(text_ptr[i].lang_key);
         else
            lang_key_len = 0;
      }
#else /* PNG_iTXt_SUPPORTED */
      {
         png_warning(png_ptr, "iTXt chunk not supported.");
         continue;
      }
#endif

      if (text_ptr[i].text == NULL || text_ptr[i].text[0] == '\0')
      {
         text_length = 0;
#ifdef PNG_iTXt_SUPPORTED
         if (text_ptr[i].compression > 0)
            textp->compression = PNG_ITXT_COMPRESSION_NONE;
         else
#endif
            textp->compression = PNG_TEXT_COMPRESSION_NONE;
      }

      else
      {
         text_length = png_strlen(text_ptr[i].text);
         textp->compression = text_ptr[i].compression;
      }

      textp->key = (png_charp)png_malloc_warn(png_ptr,
         (png_uint_32)
          (key_len + text_length + lang_len + lang_key_len + 4));
       if (textp->key == NULL)
          return(1);
      png_debug2(2, "Allocated %lu bytes at %p in png_set_text",
                  (png_uint_32)
                  (key_len + lang_len + lang_key_len + text_length + 4),
                 textp->key);
 
       png_memcpy(textp->key, text_ptr[i].key,(png_size_t)(key_len));
       *(textp->key + key_len) = '\0';
#ifdef PNG_iTXt_SUPPORTED
      if (text_ptr[i].compression > 0)
      {
         textp->lang = textp->key + key_len + 1;
         png_memcpy(textp->lang, text_ptr[i].lang, lang_len);
         *(textp->lang + lang_len) = '\0';
         textp->lang_key = textp->lang + lang_len + 1;
         png_memcpy(textp->lang_key, text_ptr[i].lang_key, lang_key_len);
         *(textp->lang_key + lang_key_len) = '\0';
         textp->text = textp->lang_key + lang_key_len + 1;
      }
      else
#endif
      {
#ifdef PNG_iTXt_SUPPORTED
         textp->lang=NULL;
         textp->lang_key=NULL;
#endif
         textp->text = textp->key + key_len + 1;
      }
      if (text_length)
         png_memcpy(textp->text, text_ptr[i].text,
            (png_size_t)(text_length));
      *(textp->text + text_length) = '\0';

#ifdef PNG_iTXt_SUPPORTED
      if (textp->compression > 0)
      {
         textp->text_length = 0;
         textp->itxt_length = text_length;
      }
      else
#endif

      {
         textp->text_length = text_length;
#ifdef PNG_iTXt_SUPPORTED
         textp->itxt_length = 0;
#endif
      }
      info_ptr->num_text++;
      png_debug1(3, "transferred text chunk %d", info_ptr->num_text);
   }
   return(0);
}
