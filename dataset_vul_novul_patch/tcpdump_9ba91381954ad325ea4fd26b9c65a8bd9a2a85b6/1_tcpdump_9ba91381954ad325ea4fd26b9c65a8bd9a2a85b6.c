 get_next_file(FILE *VFile, char *ptr)
 {
 	char *ret;
 
 	ret = fgets(ptr, PATH_MAX, VFile);
 	if (!ret)
 		return NULL;
 
	if (ptr[strlen(ptr) - 1] == '\n')
		ptr[strlen(ptr) - 1] = '\0';
 
 	return ret;
 }
