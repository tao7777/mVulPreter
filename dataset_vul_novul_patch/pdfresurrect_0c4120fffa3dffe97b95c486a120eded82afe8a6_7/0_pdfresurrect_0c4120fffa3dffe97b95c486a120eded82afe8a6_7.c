int pdf_load_xrefs(FILE *fp, pdf_t *pdf)
{
    int  i, ver, is_linear;
    long pos, pos_count;
    char x, *c, buf[256];
    
    c = NULL;

    /* Count number of xrefs */
    pdf->n_xrefs = 0;
    fseek(fp, 0, SEEK_SET);
    while (get_next_eof(fp) >= 0)
      ++pdf->n_xrefs;

    if (!pdf->n_xrefs)
      return 0;
 
     /* Load in the start/end positions */
     fseek(fp, 0, SEEK_SET);
    pdf->xrefs = safe_calloc(sizeof(xref_t) * pdf->n_xrefs);
     ver = 1;
     for (i=0; i<pdf->n_xrefs; i++)
     {
        /* Seek to %%EOF */
        if ((pos = get_next_eof(fp)) < 0)
          break;

        /* Set and increment the version */
        pdf->xrefs[i].version = ver++;

        /* Rewind until we find end of "startxref" */
        pos_count = 0;
        while (SAFE_F(fp, ((x = fgetc(fp)) != 'f')))
          fseek(fp, pos - (++pos_count), SEEK_SET);
        
        /* Suck in end of "startxref" to start of %%EOF */
        if (pos_count >= sizeof(buf)) {
          ERR("Failed to locate the startxref token. "
              "This might be a corrupt PDF.\n");
          return -1;
        }
        memset(buf, 0, sizeof(buf));
        SAFE_E(fread(buf, 1, pos_count, fp), pos_count,
               "Failed to read startxref.\n");
        c = buf;
        while (*c == ' ' || *c == '\n' || *c == '\r')
          ++c;
    
        /* xref start position */
        pdf->xrefs[i].start = atol(c);

        /* If xref is 0 handle linear xref table */
        if (pdf->xrefs[i].start == 0)
          get_xref_linear_skipped(fp, &pdf->xrefs[i]);

        /* Non-linear, normal operation, so just find the end of the xref */
        else
        {
            /* xref end position */
            pos = ftell(fp);
            fseek(fp, pdf->xrefs[i].start, SEEK_SET);
            pdf->xrefs[i].end = get_next_eof(fp);

            /* Look for next EOF and xref data */
            fseek(fp, pos, SEEK_SET);
        }

        /* Check validity */
        if (!is_valid_xref(fp, pdf, &pdf->xrefs[i]))
        {
            is_linear = pdf->xrefs[i].is_linear;
            memset(&pdf->xrefs[i], 0, sizeof(xref_t));
            pdf->xrefs[i].is_linear = is_linear;
            rewind(fp);
            get_next_eof(fp);
            continue;
        }

        /*  Load the entries from the xref */
        load_xref_entries(fp, &pdf->xrefs[i]);
    }

    /* Now we have all xref tables, if this is linearized, we need
     * to make adjustments so that things spit out properly
     */
    if (pdf->xrefs[0].is_linear)
      resolve_linearized_pdf(pdf);

    /* Ok now we have all xref data.  Go through those versions of the 
     * PDF and try to obtain creator information
     */
    load_creator(fp, pdf);

    return pdf->n_xrefs;
}
