static char *escape_pathname(const char *inp)
{
    const unsigned char *s;
    char *escaped, *d;

    if (!inp) {
        return NULL;
     }
     escaped = malloc (4 * strlen(inp) + 1);
     if (!escaped) {
		perror("malloc");
		return NULL;
     }
     for (d = escaped, s = (const unsigned char *)inp; *s; s++) {
         if (needs_escape (*s)) {
            snprintf (d, 5, "\\x%02x", *s);
            d += strlen (d);
        } else {
            *d++ = *s;
        }
    }
    *d++ = '\0';
    return escaped;
}
