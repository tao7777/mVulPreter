static void print_value(int output, int num, const char *devname,
			const char *value, const char *name, size_t valsz)
{
	if (output & OUTPUT_VALUE_ONLY) {
		fputs(value, stdout);
		fputc('\n', stdout);

	} else if (output & OUTPUT_UDEV_LIST) {
		print_udev_format(name, value);

	} else if (output & OUTPUT_EXPORT_LIST) {
		if (num == 1 && devname)
 			printf("DEVNAME=%s\n", devname);
 		fputs(name, stdout);
 		fputs("=", stdout);
		safe_print(value, valsz, NULL);
 		fputs("\n", stdout);
 
 	} else {
		if (num == 1 && devname)
			printf("%s:", devname);
 		fputs(" ", stdout);
 		fputs(name, stdout);
 		fputs("=\"", stdout);
		safe_print(value, valsz, "\"");
 		fputs("\"", stdout);
 	}
 }
