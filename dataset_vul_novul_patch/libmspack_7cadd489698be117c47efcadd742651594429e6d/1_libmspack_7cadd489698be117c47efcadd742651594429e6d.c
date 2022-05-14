static char *create_output_name(unsigned char *fname, unsigned char *dir,
			 int lower, int isunix, int utf8)
{
  unsigned char *p, *name, c, *fe, sep, slash;
  unsigned int x;
  sep   = (isunix) ? '/'  : '\\'; /* the path-seperator */
  slash = (isunix) ? '\\' : '/';  /* the other slash */
  /* length of filename */
  x = strlen((char *) fname);
  /* UTF8 worst case scenario: tolower() expands all chars from 1 to 3 bytes */
  if (utf8) x *= 3;
  /* length of output directory */
  if (dir) x += strlen((char *) dir);
  if (!(name = (unsigned char *) malloc(x + 2))) {
    fprintf(stderr, "out of memory!\n");
    return NULL;
  }
  /* start with blank name */
  *name = '\0';
  /* add output directory if needed */
  if (dir) {
    strcpy((char *) name, (char *) dir);
    strcat((char *) name, "/");
  }
  /* remove leading slashes */
  while (*fname == sep) fname++;
  /* copy from fi->filename to new name, converting MS-DOS slashes to UNIX
   * slashes as we go. Also lowercases characters if needed.
   */
  p = &name[strlen((char *)name)];
  fe = &fname[strlen((char *)fname)];
  if (utf8) {
    /* UTF8 translates two-byte unicode characters into 1, 2 or 3 bytes.
     * %000000000xxxxxxx -> %0xxxxxxx
     * %00000xxxxxyyyyyy -> %110xxxxx %10yyyyyy
     * %xxxxyyyyyyzzzzzz -> %1110xxxx %10yyyyyy %10zzzzzz
     *
     * Therefore, the inverse is as follows:
     * First char:
     *  0x00 - 0x7F = one byte char
     *  0x80 - 0xBF = invalid
     *  0xC0 - 0xDF = 2 byte char (next char only 0x80-0xBF is valid)
     *  0xE0 - 0xEF = 3 byte char (next 2 chars only 0x80-0xBF is valid)
     *  0xF0 - 0xFF = invalid
     */
    do {
      if (fname >= fe) {
	free(name);
	return NULL;
      }
      /* get next UTF8 char */
      if ((c = *fname++) < 0x80) x = c;
      else {
	if ((c >= 0xC0) && (c < 0xE0)) {
	  x = (c & 0x1F) << 6;
	  x |= *fname++ & 0x3F;
	}
	else if ((c >= 0xE0) && (c < 0xF0)) {
	  x = (c & 0xF) << 12;
	  x |= (*fname++ & 0x3F) << 6;
	  x |= *fname++ & 0x3F;
	}
	else x = '?';
      }
      /* whatever is the path seperator -> '/'
       * whatever is the other slash    -> '\\'
       * otherwise, if lower is set, the lowercase version */
      if      (x == sep)   x = '/';
      else if (x == slash) x = '\\';
      else if (lower)      x = (unsigned int) tolower((int) x);
      /* integer back to UTF8 */
      if (x < 0x80) {
	*p++ = (unsigned char) x;
      }
      else if (x < 0x800) {
	*p++ = 0xC0 | (x >> 6);   
	*p++ = 0x80 | (x & 0x3F);
      }
      else {
	*p++ = 0xE0 | (x >> 12);
	*p++ = 0x80 | ((x >> 6) & 0x3F);
	*p++ = 0x80 | (x & 0x3F);
      }
    } while (x);
  }
  else {
    /* regular non-utf8 version */
    do {
      c = *fname++;
      if      (c == sep)   c = '/';
      else if (c == slash) c = '\\';
      else if (lower)      c = (unsigned char) tolower((int) c);
    } while ((*p++ = c));
  }
  return (char *) name;
 }
