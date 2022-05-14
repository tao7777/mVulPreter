png_get_copyright(png_structp png_ptr)
{
   PNG_UNUSED(png_ptr)  /* Silence compiler warning about unused png_ptr */
#ifdef PNG_STRING_COPYRIGHT
      return PNG_STRING_COPYRIGHT
 #else
 #ifdef __STDC__
    return ((png_charp) PNG_STRING_NEWLINE \
     "libpng version 1.2.52 - November 20, 2014" PNG_STRING_NEWLINE \
     "Copyright (c) 1998-2014 Glenn Randers-Pehrson" PNG_STRING_NEWLINE \
      "Copyright (c) 1996-1997 Andreas Dilger" PNG_STRING_NEWLINE \
      "Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc." \
      PNG_STRING_NEWLINE);
 #else
      return ((png_charp) "libpng version 1.2.52 - November 20, 2014\
      Copyright (c) 1998-2014 Glenn Randers-Pehrson\
       Copyright (c) 1996-1997 Andreas Dilger\
       Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc.");
 #endif
#endif
}
