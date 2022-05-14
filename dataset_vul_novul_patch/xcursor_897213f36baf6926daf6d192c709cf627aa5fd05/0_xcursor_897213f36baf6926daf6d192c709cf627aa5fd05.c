_XcursorThemeInherits (const char *full)
{
    char    line[8192];
    char    *result = NULL;
    FILE    *f;

    if (!full)
        return NULL;

    f = fopen (full, "r");
    if (f)
    {
	while (fgets (line, sizeof (line), f))
	{
	    if (!strncmp (line, "Inherits", 8))
	    {
		char    *l = line + 8;
		char    *r;
		while (*l == ' ') l++;
 		if (*l != '=') continue;
 		l++;
 		while (*l == ' ') l++;
		result = malloc (strlen (l) + 1);
 		if (result)
 		{
 		    r = result;
		    while (*l)
		    {
			while (XcursorSep(*l) || XcursorWhite (*l)) l++;
			if (!*l)
			    break;
			if (r != result)
			    *r++ = ':';
			while (*l && !XcursorWhite(*l) &&
			       !XcursorSep(*l))
			    *r++ = *l++;
		    }
		    *r++ = '\0';
		}
		break;
	    }
	}
	fclose (f);
    }
    return result;
}
