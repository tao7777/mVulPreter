 expand_string_integer(uschar *string, BOOL isplus)
 {
return expanded_string_integer(expand_string(string), isplus);
}


/*************************************************
 *         Interpret string as an integer        *
 *************************************************/

/* Convert a string (that has already been expanded) into an integer.

This function is used inside the expansion code.

Arguments:
  s       the string to be expanded
  isplus  TRUE if a non-negative number is expected

Returns:  the integer value, or
          -1 if string is NULL (which implies an expansion error)
          -2 for an integer interpretation error
          expand_string_message is set NULL for an OK integer
*/

static int_eximarith_t
expanded_string_integer(uschar *s, BOOL isplus)
{
 int_eximarith_t value;
 uschar *msg = US"invalid integer \"%s\"";
 uschar *endptr;
/* If expansion failed, expand_string_message will be set. */

if (s == NULL) return -1;

/* On an overflow, strtol() returns LONG_MAX or LONG_MIN, and sets errno
to ERANGE. When there isn't an overflow, errno is not changed, at least on some
systems, so we set it zero ourselves. */

errno = 0;
expand_string_message = NULL;               /* Indicates no error */

/* Before Exim 4.64, strings consisting entirely of whitespace compared
equal to 0.  Unfortunately, people actually relied upon that, so preserve
the behaviour explicitly.  Stripping leading whitespace is a harmless
noop change since strtol skips it anyway (provided that there is a number
to find at all). */
if (isspace(*s))
  {
  while (isspace(*s)) ++s;
  if (*s == '\0')
    {
      DEBUG(D_expand)
       debug_printf("treating blank string as number 0\n");
      return 0;
    }
  }

value = strtoll(CS s, CSS &endptr, 10);

if (endptr == s)
  {
  msg = US"integer expected but \"%s\" found";
  }
else if (value < 0 && isplus)
  {
  msg = US"non-negative integer expected but \"%s\" found";
  }
else
  {
  switch (tolower(*endptr))
    {
    default:
      break;
    case 'k':
      if (value > EXIM_ARITH_MAX/1024 || value < EXIM_ARITH_MIN/1024) errno = ERANGE;
      else value *= 1024;
      endptr++;
      break;
    case 'm':
      if (value > EXIM_ARITH_MAX/(1024*1024) || value < EXIM_ARITH_MIN/(1024*1024)) errno = ERANGE;
      else value *= 1024*1024;
      endptr++;
      break;
    case 'g':
      if (value > EXIM_ARITH_MAX/(1024*1024*1024) || value < EXIM_ARITH_MIN/(1024*1024*1024)) errno = ERANGE;
      else value *= 1024*1024*1024;
      endptr++;
      break;
    }
  if (errno == ERANGE)
    msg = US"absolute value of integer \"%s\" is too large (overflow)";
  else
    {
    while (isspace(*endptr)) endptr++;
    if (*endptr == 0) return value;
    }
  }

expand_string_message = string_sprintf(CS msg, s);
return -2;
}
