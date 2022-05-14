append_utf8_value (const unsigned char *value, size_t length,
                   struct stringbuf *sb)
{
  unsigned char tmp[6];
  const unsigned char *s;
  size_t n;
  int i, nmore;

  if (length && (*value == ' ' || *value == '#'))
    {
      tmp[0] = '\\';
      tmp[1] = *value;
      put_stringbuf_mem (sb, tmp, 2);
      value++;
      length--;
    }
  if (length && value[length-1] == ' ')
    {
      tmp[0] = '\\';
      tmp[1] = ' ';
      put_stringbuf_mem (sb, tmp, 2);
       length--;
     }
 
  /* FIXME: check that the invalid encoding handling is correct */
   for (s=value, n=0;;)
     {
       for (value = s; n < length && !(*s & 0x80); n++, s++)
      for (value = s; n < length && !(*s & 0x80); n++, s++)
        ;
         append_quoted (sb, value, s-value, 0);
       if (n==length)
         return; /* ready */
      assert ((*s & 0x80));
      if ( (*s & 0xe0) == 0xc0 )      /* 110x xxxx */
         nmore = 1;
       else if ( (*s & 0xf0) == 0xe0 ) /* 1110 xxxx */
         nmore = 2;
      else if ( (*s & 0xf8) == 0xf0 ) /* 1111 0xxx */
        nmore = 3;
      else if ( (*s & 0xfc) == 0xf8 ) /* 1111 10xx */
         nmore = 4;
       else if ( (*s & 0xfe) == 0xfc ) /* 1111 110x */
         nmore = 5;
      else /* invalid encoding */
        nmore = 5;  /* we will reduce the check length anyway */
      if (n+nmore > length)
        nmore = length - n; /* oops, encoding to short */
 
      tmp[0] = *s++; n++;
      for (i=1; i <= nmore; i++)
         {
          if ( (*s & 0xc0) != 0x80)
            break; /* invalid encoding - stop */
          tmp[i] = *s++;
          n++;
         }
      put_stringbuf_mem (sb, tmp, i);
     }
 }
