int main(int argc, char *argv[]) {
  struct mschm_decompressor *chmd;
  struct mschmd_header *chm;
  struct mschmd_file *file, **f;
  unsigned int numf, i;

  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  user_umask = umask(0); umask(user_umask);

  MSPACK_SYS_SELFTEST(i);
  if (i) return 0;

  if ((chmd = mspack_create_chm_decompressor(NULL))) {
    for (argv++; *argv; argv++) {
      printf("%s\n", *argv);
      if ((chm = chmd->open(chmd, *argv))) {

	/* build an ordered list of files for maximum extraction speed */
	for (numf=0, file=chm->files; file; file = file->next) numf++;
	if ((f = (struct mschmd_file **) calloc(numf, sizeof(struct mschmd_file *)))) {
	  for (i=0, file=chm->files; file; file = file->next) f[i++] = file;
 	  qsort(f, numf, sizeof(struct mschmd_file *), &sortfunc);
 
 	  for (i = 0; i < numf; i++) {
	    char *outname = create_output_name(f[i]->filename);
 	    printf("Extracting %s\n", outname);
 	    ensure_filepath(outname);
 	    if (chmd->extract(chmd, f[i], outname)) {
	      printf("%s: extract error on \"%s\": %s\n",
		     *argv, f[i]->filename, ERROR(chmd));
	    }
	    free(outname);
	  }
	  free(f);
	}
	chmd->close(chmd, chm);
      }
      else {
	printf("%s: can't open -- %s\n", *argv, ERROR(chmd));
      }
    }
    mspack_destroy_chm_decompressor(chmd);
  }
  return 0;
}
