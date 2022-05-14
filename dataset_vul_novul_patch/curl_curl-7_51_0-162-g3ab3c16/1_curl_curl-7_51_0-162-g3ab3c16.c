static int dprintf_formatf(
  void *data, /* untouched by format(), just sent to the stream() function in
                 the second argument */
  /* function pointer called for each output character */
  int (*stream)(int, FILE *),
  const char *format,    /* %-formatted string */
  va_list ap_save) /* list of parameters */
{
  /* Base-36 digits for numbers.  */
  const char *digits = lower_digits;

  /* Pointer into the format string.  */
  char *f;

  /* Number of characters written.  */
  int done = 0;

  long param; /* current parameter to read */
  long param_num=0; /* parameter counter */

  va_stack_t vto[MAX_PARAMETERS];
  char *endpos[MAX_PARAMETERS];
  char **end;

  char work[BUFFSIZE];

  va_stack_t *p;

  /* 'workend' points to the final buffer byte position, but with an extra
     byte as margin to avoid the (false?) warning Coverity gives us
     otherwise */
  char *workend = &work[sizeof(work) - 2];

  /* Do the actual %-code parsing */
  if(dprintf_Pass1(format, vto, endpos, ap_save))
    return -1;

  end = &endpos[0]; /* the initial end-position from the list dprintf_Pass1()
                       created for us */

  f = (char *)format;
  while(*f != '\0') {
    /* Format spec modifiers.  */
    int is_alt;

    /* Width of a field.  */
    long width;

    /* Precision of a field.  */
    long prec;

    /* Decimal integer is negative.  */
    int is_neg;

    /* Base of a number to be written.  */
    long base;

    /* Integral values to be written.  */
    mp_uintmax_t num;

    /* Used to convert negative in positive.  */
    mp_intmax_t signed_num;

    char *w;

    if(*f != '%') {
      /* This isn't a format spec, so write everything out until the next one
         OR end of string is reached.  */
      do {
        OUTCHAR(*f);
      } while(*++f && ('%' != *f));
      continue;
    }

    ++f;

    /* Check for "%%".  Note that although the ANSI standard lists
       '%' as a conversion specifier, it says "The complete format
       specification shall be `%%'," so we can avoid all the width
       and precision processing.  */
    if(*f == '%') {
      ++f;
      OUTCHAR('%');
      continue;
    }

    /* If this is a positional parameter, the position must follow immediately
       after the %, thus create a %<num>$ sequence */
    param=dprintf_DollarString(f, &f);

    if(!param)
      param = param_num;
    else
      --param;

    param_num++; /* increase this always to allow "%2$s %1$s %s" and then the
                    third %s will pick the 3rd argument */

    p = &vto[param];

    /* pick up the specified width */
    if(p->flags & FLAGS_WIDTHPARAM) {
      width = (long)vto[p->width].data.num.as_signed;
      param_num++; /* since the width is extracted from a parameter, we
                      must skip that to get to the next one properly */
      if(width < 0) {
        /* "A negative field width is taken as a '-' flag followed by a
           positive field width." */
        width = -width;
        p->flags |= FLAGS_LEFT;
        p->flags &= ~FLAGS_PAD_NIL;
      }
    }
    else
      width = p->width;

    /* pick up the specified precision */
    if(p->flags & FLAGS_PRECPARAM) {
      prec = (long)vto[p->precision].data.num.as_signed;
      param_num++; /* since the precision is extracted from a parameter, we
                      must skip that to get to the next one properly */
      if(prec < 0)
        /* "A negative precision is taken as if the precision were
           omitted." */
        prec = -1;
    }
    else if(p->flags & FLAGS_PREC)
      prec = p->precision;
    else
      prec = -1;

    is_alt = (p->flags & FLAGS_ALT) ? 1 : 0;

    switch(p->type) {
    case FORMAT_INT:
      num = p->data.num.as_unsigned;
      if(p->flags & FLAGS_CHAR) {
        /* Character.  */
        if(!(p->flags & FLAGS_LEFT))
          while(--width > 0)
            OUTCHAR(' ');
        OUTCHAR((char) num);
        if(p->flags & FLAGS_LEFT)
          while(--width > 0)
            OUTCHAR(' ');
        break;
      }
      if(p->flags & FLAGS_OCTAL) {
        /* Octal unsigned integer.  */
        base = 8;
        goto unsigned_number;
      }
      else if(p->flags & FLAGS_HEX) {
        /* Hexadecimal unsigned integer.  */

        digits = (p->flags & FLAGS_UPPER)? upper_digits : lower_digits;
        base = 16;
        goto unsigned_number;
      }
      else if(p->flags & FLAGS_UNSIGNED) {
        /* Decimal unsigned integer.  */
        base = 10;
        goto unsigned_number;
      }

      /* Decimal integer.  */
      base = 10;

      is_neg = (p->data.num.as_signed < (mp_intmax_t)0) ? 1 : 0;
      if(is_neg) {
        /* signed_num might fail to hold absolute negative minimum by 1 */
        signed_num = p->data.num.as_signed + (mp_intmax_t)1;
        signed_num = -signed_num;
        num = (mp_uintmax_t)signed_num;
        num += (mp_uintmax_t)1;
      }

      goto number;

      unsigned_number:
      /* Unsigned number of base BASE.  */
      is_neg = 0;

      number:
      /* Number of base BASE.  */

      /* Supply a default precision if none was given.  */
      if(prec == -1)
        prec = 1;

      /* Put the number in WORK.  */
      w = workend;
      while(num > 0) {
        *w-- = digits[num % base];
        num /= base;
      }
      width -= (long)(workend - w);
      prec -= (long)(workend - w);

      if(is_alt && base == 8 && prec <= 0) {
        *w-- = '0';
        --width;
      }

      if(prec > 0) {
        width -= prec;
        while(prec-- > 0)
          *w-- = '0';
      }

      if(is_alt && base == 16)
        width -= 2;

      if(is_neg || (p->flags & FLAGS_SHOWSIGN) || (p->flags & FLAGS_SPACE))
        --width;

      if(!(p->flags & FLAGS_LEFT) && !(p->flags & FLAGS_PAD_NIL))
        while(width-- > 0)
          OUTCHAR(' ');

      if(is_neg)
        OUTCHAR('-');
      else if(p->flags & FLAGS_SHOWSIGN)
        OUTCHAR('+');
      else if(p->flags & FLAGS_SPACE)
        OUTCHAR(' ');

      if(is_alt && base == 16) {
        OUTCHAR('0');
        if(p->flags & FLAGS_UPPER)
          OUTCHAR('X');
        else
          OUTCHAR('x');
      }

      if(!(p->flags & FLAGS_LEFT) && (p->flags & FLAGS_PAD_NIL))
        while(width-- > 0)
          OUTCHAR('0');

      /* Write the number.  */
      while(++w <= workend) {
        OUTCHAR(*w);
      }

      if(p->flags & FLAGS_LEFT)
        while(width-- > 0)
          OUTCHAR(' ');
      break;

    case FORMAT_STRING:
            /* String.  */
      {
        static const char null[] = "(nil)";
        const char *str;
        size_t len;

        str = (char *) p->data.str;
        if(str == NULL) {
          /* Write null[] if there's space.  */
          if(prec == -1 || prec >= (long) sizeof(null) - 1) {
            str = null;
            len = sizeof(null) - 1;
            /* Disable quotes around (nil) */
            p->flags &= (~FLAGS_ALT);
          }
          else {
            str = "";
            len = 0;
          }
        }
        else if(prec != -1)
          len = (size_t)prec;
        else
          len = strlen(str);

        width -= (len > LONG_MAX) ? LONG_MAX : (long)len;

        if(p->flags & FLAGS_ALT)
          OUTCHAR('"');

        if(!(p->flags&FLAGS_LEFT))
          while(width-- > 0)
            OUTCHAR(' ');

        while((len-- > 0) && *str)
          OUTCHAR(*str++);
        if(p->flags&FLAGS_LEFT)
          while(width-- > 0)
            OUTCHAR(' ');

        if(p->flags & FLAGS_ALT)
          OUTCHAR('"');
      }
      break;

    case FORMAT_PTR:
      /* Generic pointer.  */
      {
        void *ptr;
        ptr = (void *) p->data.ptr;
        if(ptr != NULL) {
          /* If the pointer is not NULL, write it as a %#x spec.  */
          base = 16;
          digits = (p->flags & FLAGS_UPPER)? upper_digits : lower_digits;
          is_alt = 1;
          num = (size_t) ptr;
          is_neg = 0;
          goto number;
        }
        else {
          /* Write "(nil)" for a nil pointer.  */
          static const char strnil[] = "(nil)";
          const char *point;

          width -= (long)(sizeof(strnil) - 1);
          if(p->flags & FLAGS_LEFT)
            while(width-- > 0)
              OUTCHAR(' ');
          for(point = strnil; *point != '\0'; ++point)
            OUTCHAR(*point);
          if(! (p->flags & FLAGS_LEFT))
            while(width-- > 0)
              OUTCHAR(' ');
        }
      }
      break;

    case FORMAT_DOUBLE:
      {
        char formatbuf[32]="%";
        char *fptr = &formatbuf[1];
        size_t left = sizeof(formatbuf)-strlen(formatbuf);
        int len;

        width = -1;
        if(p->flags & FLAGS_WIDTH)
          width = p->width;
        else if(p->flags & FLAGS_WIDTHPARAM)
          width = (long)vto[p->width].data.num.as_signed;

        prec = -1;
        if(p->flags & FLAGS_PREC)
          prec = p->precision;
        else if(p->flags & FLAGS_PRECPARAM)
          prec = (long)vto[p->precision].data.num.as_signed;

        if(p->flags & FLAGS_LEFT)
          *fptr++ = '-';
        if(p->flags & FLAGS_SHOWSIGN)
          *fptr++ = '+';
        if(p->flags & FLAGS_SPACE)
          *fptr++ = ' ';
        if(p->flags & FLAGS_ALT)
          *fptr++ = '#';

         *fptr = 0;
 
         if(width >= 0) {
           /* RECURSIVE USAGE */
           len = curl_msnprintf(fptr, left, "%ld", width);
           fptr += len;
           left -= len;
         }
         if(prec >= 0) {
           /* RECURSIVE USAGE */
           len = curl_msnprintf(fptr, left, ".%ld", prec);
           fptr += len;
        }
        if(p->flags & FLAGS_LONG)
          *fptr++ = 'l';

        if(p->flags & FLAGS_FLOATE)
          *fptr++ = (char)((p->flags & FLAGS_UPPER) ? 'E':'e');
        else if(p->flags & FLAGS_FLOATG)
          *fptr++ = (char)((p->flags & FLAGS_UPPER) ? 'G' : 'g');
        else
          *fptr++ = 'f';

        *fptr = 0; /* and a final zero termination */

         /* NOTE NOTE NOTE!! Not all sprintf implementations return number of
            output characters */
         (sprintf)(work, formatbuf, p->data.dnum);
         for(fptr=work; *fptr; fptr++)
           OUTCHAR(*fptr);
       }
      break;

    case FORMAT_INTPTR:
      /* Answer the count of characters written.  */
#ifdef HAVE_LONG_LONG_TYPE
      if(p->flags & FLAGS_LONGLONG)
        *(LONG_LONG_TYPE *) p->data.ptr = (LONG_LONG_TYPE)done;
      else
#endif
        if(p->flags & FLAGS_LONG)
          *(long *) p->data.ptr = (long)done;
      else if(!(p->flags & FLAGS_SHORT))
        *(int *) p->data.ptr = (int)done;
      else
        *(short *) p->data.ptr = (short)done;
      break;

    default:
      break;
    }
    f = *end++; /* goto end of %-code */

  }
  return done;
}
