void pdf_summarize(
    FILE        *fp,
    const pdf_t *pdf,
    const char  *name,
    pdf_flag_t   flags)
{
    int   i, j, page, n_versions, n_entries;
    FILE *dst, *out;
    char *dst_name, *c;

    dst = NULL;
    dst_name = NULL;
 
     if (name)
     {
        dst_name = safe_calloc(strlen(name) * 2 + 16);
         sprintf(dst_name, "%s/%s", name, name);
 
         if ((c = strrchr(dst_name, '.')) && (strncmp(c, ".pdf", 4) == 0))
          *c = '\0';

        strcat(dst_name, ".summary");
        if (!(dst = fopen(dst_name, "w")))
        {
            ERR("Could not open file '%s' for writing\n", dst_name);
            return;
        }
    }
    
    /* Send output to file or stdout */
    out = (dst) ? dst : stdout;

    /* Count versions */
    n_versions = pdf->n_xrefs;
    if (n_versions && pdf->xrefs[0].is_linear)
      --n_versions;

    /* Ignore bad xref entry */
    for (i=1; i<pdf->n_xrefs; ++i)
      if (pdf->xrefs[i].end == 0)
        --n_versions;

    /* If we have no valid versions but linear, count that */
    if (!pdf->n_xrefs || (!n_versions && pdf->xrefs[0].is_linear))
      n_versions = 1;

    /* Compare each object (if we dont have xref streams) */
    n_entries = 0;
    for (i=0; !(const int)pdf->has_xref_streams && i<pdf->n_xrefs; i++)
    {
        if (flags & PDF_FLAG_QUIET)
          continue;

        for (j=0; j<pdf->xrefs[i].n_entries; j++)
        {
            ++n_entries;
            fprintf(out,
                    "%s: --%c-- Version %d -- Object %d (%s)",
                    pdf->name,
                    pdf_get_object_status(pdf, i, j),
                    pdf->xrefs[i].version,
                    pdf->xrefs[i].entries[j].obj_id,
                    get_type(fp, pdf->xrefs[i].entries[j].obj_id,
                             &pdf->xrefs[i]));

            /* TODO
            page = get_page(pdf->xrefs[i].entries[j].obj_id, &pdf->xrefs[i]);
            */

            if (0 /*page*/)
              fprintf(out, " Page(%d)\n", page);
            else
              fprintf(out, "\n");
        }
    }

    /* Trailing summary */
    if (!(flags & PDF_FLAG_QUIET))
    {
        /* Let the user know that we cannot we print a per-object summary.
         * If we have a 1.5 PDF using streams for xref, we have not objects
         * to display, so let the user know whats up.
         */
        if (pdf->has_xref_streams || !n_entries)
           fprintf(out,
               "%s: This PDF contains potential cross reference streams.\n"
               "%s: An object summary is not available.\n",
               pdf->name,
               pdf->name);

        fprintf(out,
                "---------- %s ----------\n"
                "Versions: %d\n", 
                pdf->name,
                n_versions);

        /* Count entries for summary */
        if (!pdf->has_xref_streams)
          for (i=0; i<pdf->n_xrefs; i++)
          {
              if (pdf->xrefs[i].is_linear)
                continue;

              n_entries = pdf->xrefs[i].n_entries;

              /* If we are a linearized PDF, all versions are made from those
               * objects too.  So count em'
               */
              if (pdf->xrefs[0].is_linear)
                n_entries += pdf->xrefs[0].n_entries; 

              if (pdf->xrefs[i].version && n_entries)
                fprintf(out,
                        "Version %d -- %d objects\n",
                        pdf->xrefs[i].version, 
                        n_entries);
           }
    }
    else /* Quiet output */
      fprintf(out, "%s: %d\n", pdf->name, n_versions);

    if (dst)
    {
        fclose(dst);
        free(dst_name);
    }
}
