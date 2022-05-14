int mutt_b64_decode(char *out, const char *in)
int mutt_b64_decode(char *out, const char *in, size_t olen)
 {
   int len = 0;
   unsigned char digit4;

  do
  {
    const unsigned char digit1 = in[0];
    if ((digit1 > 127) || (base64val(digit1) == BAD))
      return -1;
    const unsigned char digit2 = in[1];
    if ((digit2 > 127) || (base64val(digit2) == BAD))
      return -1;
    const unsigned char digit3 = in[2];
    if ((digit3 > 127) || ((digit3 != '=') && (base64val(digit3) == BAD)))
      return -1;
    digit4 = in[3];
    if ((digit4 > 127) || ((digit4 != '=') && (base64val(digit4) == BAD)))
      return -1;
     in += 4;
 
     /* digits are already sanity-checked */
    if (len == olen)
      return len;
     *out++ = (base64val(digit1) << 2) | (base64val(digit2) >> 4);
     len++;
     if (digit3 != '=')
     {
      if (len == olen)
        return len;
       *out++ = ((base64val(digit2) << 4) & 0xf0) | (base64val(digit3) >> 2);
       len++;
       if (digit4 != '=')
       {
        if (len == olen)
          return len;
         *out++ = ((base64val(digit3) << 6) & 0xc0) | base64val(digit4);
         len++;
       }
    }
  } while (*in && digit4 != '=');

  return len;
}
