png_set_tIME(png_structp png_ptr, png_infop info_ptr, png_timep mod_time)
{
   png_debug1(1, "in %s storage function", "tIME");

   if (png_ptr == NULL || info_ptr == NULL ||
        (png_ptr->mode & PNG_WROTE_tIME))
       return;
 
    png_memcpy(&(info_ptr->mod_time), mod_time, png_sizeof(png_time));
    info_ptr->valid |= PNG_INFO_tIME;
 }
