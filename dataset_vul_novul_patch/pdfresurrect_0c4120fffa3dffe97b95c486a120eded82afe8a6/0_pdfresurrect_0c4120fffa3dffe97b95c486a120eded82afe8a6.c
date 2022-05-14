 int main(int argc, char **argv)
 {
     int         i, n_valid, do_write, do_scrub;
    char       *c, *dname, *name;
    DIR        *dir;
    FILE       *fp;
    pdf_t      *pdf;
    pdf_flag_t  flags;

    if (argc < 2)
      usage();

    /* Args */
    do_write = do_scrub = flags = 0;
    name = NULL;
    for (i=1; i<argc; i++)
    {
        if (strncmp(argv[i], "-w", 2) == 0)
          do_write = 1;
        else if (strncmp(argv[i], "-i", 2) == 0)
          flags |= PDF_FLAG_DISP_CREATOR;
        else if (strncmp(argv[i], "-q", 2) == 0)
          flags |= PDF_FLAG_QUIET;
        else if (strncmp(argv[i], "-s", 2) == 0)
          do_scrub = 1;
        else if (argv[i][0] != '-')
          name = argv[i];
        else if (argv[i][0] == '-')
          usage();
    }

    if (!name)
      usage();

    if (!(fp = fopen(name, "r")))
    {
        ERR("Could not open file '%s'\n", argv[1]);
        return -1;
    }
    else if (!pdf_is_pdf(fp))
    {
        ERR("'%s' specified is not a valid PDF\n", name);
        fclose(fp);
        return -1;
    }

    /* Load PDF */
    if (!(pdf = init_pdf(fp, name)))
    {
        fclose(fp);
        return -1;
    }

    /* Count valid xrefs */
    for (i=0, n_valid=0; i<pdf->n_xrefs; i++)
      if (pdf->xrefs[i].version)
        ++n_valid;

    /* Bail if we only have 1 valid */
    if (n_valid < 2)
    {
        if (!(flags & (PDF_FLAG_QUIET | PDF_FLAG_DISP_CREATOR)))
          printf("%s: There is only one version of this PDF\n", pdf->name);

        if (do_write)
        {
            fclose(fp);
            pdf_delete(pdf);
            return 0;
        }
    }

    dname = NULL;
    if (do_write)
    {
        /* Create directory to place the various versions in */
        if ((c = strrchr(name, '/')))
          name = c + 1;

         if ((c = strrchr(name, '.')))
           *c = '\0';
 
        dname = safe_calloc(strlen(name) + 16);
         sprintf(dname, "%s-versions", name);
         if (!(dir = opendir(dname)))
           mkdir(dname, S_IRWXU);
        else
        {
            ERR("This directory already exists, PDF version extraction will "
                "not occur.\n");
            fclose(fp);
            closedir(dir);
            free(dname);
            pdf_delete(pdf);
            return -1;
        }
    
        /* Write the pdf as a pervious version */
        for (i=0; i<pdf->n_xrefs; i++)
          if (pdf->xrefs[i].version)
            write_version(fp, name, dname, &pdf->xrefs[i]);
    }

    /* Generate a per-object summary */
    pdf_summarize(fp, pdf, dname, flags);

    /* Have we been summoned to scrub history from this PDF */
    if (do_scrub)
      scrub_document(fp, pdf);

    /* Display extra information */
    if (flags & PDF_FLAG_DISP_CREATOR)
      display_creator(fp, pdf);

    fclose(fp);
    free(dname);
    pdf_delete(pdf);

    return 0;
}
