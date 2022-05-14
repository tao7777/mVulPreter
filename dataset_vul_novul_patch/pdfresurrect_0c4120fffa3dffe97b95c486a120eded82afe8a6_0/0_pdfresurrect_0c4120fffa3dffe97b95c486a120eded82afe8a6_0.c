static void write_version(
    FILE       *fp,
    const char *fname,
    const char *dirname,
    xref_t     *xref)
{
    long  start;
    char *c, *new_fname, data;
    FILE *new_fp;
    
    start = ftell(fp);

     /* Create file */
     if ((c = strstr(fname, ".pdf")))
       *c = '\0';
    new_fname = safe_calloc(strlen(fname) + strlen(dirname) + 16);
     snprintf(new_fname, strlen(fname) + strlen(dirname) + 16,
              "%s/%s-version-%d.pdf", dirname, fname, xref->version);
 
    if (!(new_fp = fopen(new_fname, "w")))
    {
        ERR("Could not create file '%s'\n", new_fname);
        fseek(fp, start, SEEK_SET);
        free(new_fname);
        return;
    }
    
    /* Copy original PDF */
    fseek(fp, 0, SEEK_SET);
    while (fread(&data, 1, 1, fp))
      fwrite(&data, 1, 1, new_fp);

    /* Emit an older startxref, refering to an older version. */
    fprintf(new_fp, "\r\nstartxref\r\n%ld\r\n%%%%EOF", xref->start);

    /* Clean */
    fclose(new_fp);
    free(new_fname);
    fseek(fp, start, SEEK_SET);
}
