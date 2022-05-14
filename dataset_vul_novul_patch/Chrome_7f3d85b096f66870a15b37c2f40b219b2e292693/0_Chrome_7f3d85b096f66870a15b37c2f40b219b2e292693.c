png_convert_to_rfc1123(png_structp png_ptr, png_timep ptime)
{
   static PNG_CONST char short_months[12][4] =
        {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
 
    if (png_ptr == NULL)
       return (NULL);

    if (png_ptr->time_buffer == NULL)
    {
       png_ptr->time_buffer = (png_charp)png_malloc(png_ptr, (png_uint_32)(29*
         png_sizeof(char)));
   }

#ifdef _WIN32_WCE
    {
       wchar_t time_buf[29];
       wsprintf(time_buf, TEXT("%d %S %d %02d:%02d:%02d +0000"),
          ptime->day % 32, short_months[(ptime->month - 1U) % 12],
         ptime->year, ptime->hour % 24, ptime->minute % 60,
           ptime->second % 61);
       WideCharToMultiByte(CP_ACP, 0, time_buf, -1, png_ptr->time_buffer,
          29, NULL, NULL);
   }
#else
#ifdef USE_FAR_KEYWORD
    {
       char near_time_buf[29];
       png_snprintf6(near_time_buf, 29, "%d %s %d %02d:%02d:%02d +0000",
          ptime->day % 32, short_months[(ptime->month - 1U) % 12],
           ptime->year, ptime->hour % 24, ptime->minute % 60,
           ptime->second % 61);
       png_memcpy(png_ptr->time_buffer, near_time_buf,
           29*png_sizeof(char));
    }
 #else
    png_snprintf6(png_ptr->time_buffer, 29, "%d %s %d %02d:%02d:%02d +0000",
       ptime->day % 32, short_months[(ptime->month - 1U) % 12],
        ptime->year, ptime->hour % 24, ptime->minute % 60,
        ptime->second % 61);
 #endif
#endif /* _WIN32_WCE */
   return ((png_charp)png_ptr->time_buffer);
}
