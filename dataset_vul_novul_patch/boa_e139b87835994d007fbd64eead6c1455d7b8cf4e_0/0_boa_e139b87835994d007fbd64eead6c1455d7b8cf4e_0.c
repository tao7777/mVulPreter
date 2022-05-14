char *strstr(char *s1, char *s2)
{                               /* from libiberty */
    char *p;
    int len = strlen(s2);
 
     if (*s2 == '\0')            /* everything matches empty string */
         return s1;
    for (p = s1; (p = strchr(p, *s2)) != NULL; p++) {
         if (strncmp(p, s2, len) == 0)
             return (p);
     }
    return NULL;
}
