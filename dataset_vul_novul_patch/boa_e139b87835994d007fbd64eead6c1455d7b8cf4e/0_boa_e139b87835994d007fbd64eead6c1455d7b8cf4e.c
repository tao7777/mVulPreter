scandir(const char *dir, struct dirent ***namelist,
        int (*select) (const struct dirent *),
        int (*compar) (const struct dirent **, const struct dirent **))
{
    DIR *d = opendir(dir);
    struct dirent *current;
    struct dirent **names;
    int count = 0;
    int pos = 0;
    int result = -1;

    if (NULL == d)
        return -1;
 
     while (NULL != readdir(d))
         count++;
	
	closedir(d);
	
     names = malloc(sizeof (struct dirent *) * count);
	if (!names) 
		return -1;
 
     d = opendir(dir);
    if (NULL == d) {
		free(names);
         return -1;
    }
 
     while (NULL != (current = readdir(d))) {
         if (NULL == select || select(current)) {
             struct dirent *copyentry = malloc(current->d_reclen);
			/* FIXME: OOM, silently skip it?*/
			if (!copyentry)
				continue;
			
             memcpy(copyentry, current, current->d_reclen);
 
             names[pos] = copyentry;
            pos++;
        }
    }
    result = closedir(d);

    if (pos != count)
        names = realloc(names, sizeof (struct dirent *) * pos);

    *namelist = names;

    return pos;
}
