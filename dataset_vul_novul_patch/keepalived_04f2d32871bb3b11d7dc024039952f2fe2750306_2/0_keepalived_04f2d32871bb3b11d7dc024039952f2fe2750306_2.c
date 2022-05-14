read_file(gchar* filepath)
{
	FILE * f;
 	size_t length;
 	gchar *ret = NULL;
 
	f = fopen(filepath, "r");
 	if (f) {
 		fseek(f, 0, SEEK_END);
 		length = (size_t)ftell(f);
		fseek(f, 0, SEEK_SET);

		/* We can't use MALLOC since it isn't thread safe */
		ret = MALLOC(length + 1);
		if (ret) {
			if (fread(ret, length, 1, f) != 1) {
				log_message(LOG_INFO, "Failed to read all of %s", filepath);
			}
			ret[length] = '\0';
		}
		else
			log_message(LOG_INFO, "Unable to read Dbus file %s", filepath);

		fclose(f);
	}
	return ret;
}
