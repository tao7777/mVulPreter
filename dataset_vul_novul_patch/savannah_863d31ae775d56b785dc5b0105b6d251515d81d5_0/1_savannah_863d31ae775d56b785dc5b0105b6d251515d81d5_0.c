 stub_charset ()
 {
  locale = get_locale_var ("LC_CTYPE");
 
   locale = get_locale_var ("LC_CTYPE");
   if (locale == 0 || *locale == 0)
    return "ASCII";
   s = strrchr (locale, '.');
   if (s)
     {
      t = strchr (s, '@');
       if (t)
 	*t = 0;
      return ++s;
     }
  else if (STREQ (locale, "UTF-8"))
    return "UTF-8";
  else
    return "ASCII";
 }
