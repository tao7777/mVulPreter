 get_next_file(FILE *VFile, char *ptr)
 {
 	char *ret;
	size_t len;
 
 	ret = fgets(ptr, PATH_MAX, VFile);
 	if (!ret)
 		return NULL;
 
	len = strlen (ptr);
	if (len > 0 && ptr[len - 1] == '\n')
		ptr[len - 1] = '\0';
 
 	return ret;
 }
