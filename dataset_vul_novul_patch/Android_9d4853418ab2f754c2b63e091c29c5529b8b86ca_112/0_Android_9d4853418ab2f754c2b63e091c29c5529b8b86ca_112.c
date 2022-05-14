perform_formatting_test(png_store *volatile ps)
perform_formatting_test(png_store *ps)
 {
 #ifdef PNG_TIME_RFC1123_SUPPORTED
    /* The handle into the formatting code is the RFC1123 support; this test does
    * nothing if that is compiled out.
    */
   context(ps, fault);

 Try
 {
      png_const_charp correct = "29 Aug 2079 13:53:60 +0000";
      png_const_charp result;
#     if PNG_LIBPNG_VER >= 10600
 char timestring[29];
#     endif
      png_structp pp;
      png_time pt;

      pp = set_store_for_write(ps, NULL, "libpng formatting test");

 if (pp == NULL)
 Throw ps;


 /* Arbitrary settings: */
      pt.year = 2079;
      pt.month = 8;
      pt.day = 29;
      pt.hour = 13;
      pt.minute = 53;
      pt.second = 60; /* a leap second */

#     if PNG_LIBPNG_VER < 10600
         result = png_convert_to_rfc1123(pp, &pt);
#     else
 if (png_convert_to_rfc1123_buffer(timestring, &pt))
            result = timestring;

 else
            result = NULL;
#     endif

 if (result == NULL)
         png_error(pp, "png_convert_to_rfc1123 failed");

 if (strcmp(result, correct) != 0)
 {
 size_t pos = 0;
 char msg[128];

         pos = safecat(msg, sizeof msg, pos, "png_convert_to_rfc1123(");
         pos = safecat(msg, sizeof msg, pos, correct);
         pos = safecat(msg, sizeof msg, pos, ") returned: '");
         pos = safecat(msg, sizeof msg, pos, result);
         pos = safecat(msg, sizeof msg, pos, "'");

         png_error(pp, msg);
 }

      store_write_reset(ps);
 }

 Catch(fault)
 {
      store_write_reset(fault);
 }
#else
   UNUSED(ps)
#endif
}
