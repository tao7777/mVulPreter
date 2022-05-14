append_quoted (struct stringbuf *sb, const unsigned char *value, size_t length,
               int skip)
{
  unsigned char tmp[4];
  const unsigned char *s = value;
  size_t n = 0;

  for (;;)
    {
      for (value = s; n+skip < length; n++, s++)
        {
          s += skip;
          n += skip;
          if (*s < ' ' || *s > 126 || strchr (",+\"\\<>;", *s) )
            break;
        }

      if (s != value)
        put_stringbuf_mem_skip (sb, value, s-value, skip);
      if (n+skip >= length)
        return; /* ready */
      s += skip;
       n += skip;
       if ( *s < ' ' || *s > 126 )
         {
          snprintf (tmp, sizeof tmp, "\\%02X", *s);
           put_stringbuf_mem (sb, tmp, 3);
         }
       else
        {
          tmp[0] = '\\';
          tmp[1] = *s;
          put_stringbuf_mem (sb, tmp, 2);
        }
      n++; s++;
    }
}
