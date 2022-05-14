static char *create_output_name(unsigned char *fname, unsigned char *dir,
char *create_output_name(char *fname) {
    char *out, *p;
    if ((out = malloc(strlen(fname) + 1))) {
        /* remove leading slashes */
        while (*fname == '/' || *fname == '\\') fname++;
        /* if that removes all characters, just call it "x" */
        strcpy(out, (*fname) ? fname : "x");

        /* change "../" to "xx/" */
        for (p = out; *p; p++) {
            if (p[0] == '.' && p[1] == '.' && (p[2] == '/' || p[2] == '\\')) {
               p[0] = p[1] = 'x';
            }
        }
    }
    return out;
 }
