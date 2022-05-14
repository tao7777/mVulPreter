emit_string(const char *str, FILE *out)
 /* Print a string with spaces replaced by '_' and non-printing characters by
    * an octal escape.
    */
{
 for (; *str; ++str)
 if (isgraph(UCHAR_MAX & *str))
         putc(*str, out);

 
       else if (isspace(UCHAR_MAX & *str))
          putc('_', out);

       else
          fprintf(out, "\\%.3o", *str);
 }
