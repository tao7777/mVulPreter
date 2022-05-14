int main(void)
 {
 	FILE *f;
 	char *tmpname;
	f = xfmkstemp(&tmpname, NULL);
 	unlink(tmpname);
 	free(tmpname);
 	fclose(f);
	return EXIT_FAILURE;
}
