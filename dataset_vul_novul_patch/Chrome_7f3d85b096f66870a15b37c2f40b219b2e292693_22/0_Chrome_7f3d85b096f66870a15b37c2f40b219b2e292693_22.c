png_set_tIME(png_structp png_ptr, png_infop info_ptr, png_timep mod_time)
{
   png_debug1(1, "in %s storage function", "tIME");

   if (png_ptr == NULL || info_ptr == NULL ||
        (png_ptr->mode & PNG_WROTE_tIME))
       return;
 
   if (mod_time->month == 0   || mod_time->month > 12  ||
       mod_time->day   == 0   || mod_time->day   > 31  ||
       mod_time->hour  > 23   || mod_time->minute > 59 ||
       mod_time->second > 60)
   {
      png_warning(png_ptr, "Ignoring invalid time value");
      return;
   }

    png_memcpy(&(info_ptr->mod_time), mod_time, png_sizeof(png_time));
    info_ptr->valid |= PNG_INFO_tIME;
 }
