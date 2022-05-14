void pdf_load_pages_kids(FILE *fp, pdf_t *pdf)
{
    int     i, id, dummy;
    char   *buf, *c;
    long    start, sz;

    start = ftell(fp);

    /* Load all kids for all xref tables (versions) */
    for (i=0; i<pdf->n_xrefs; i++)
    {
        if (pdf->xrefs[i].version && (pdf->xrefs[i].end != 0))
        {
            fseek(fp, pdf->xrefs[i].start, SEEK_SET);
            while (SAFE_F(fp, (fgetc(fp) != 't')))
                ; /* Iterate to trailer */
 
             /* Get root catalog */
             sz = pdf->xrefs[i].end - ftell(fp);
            buf = safe_calloc(sz + 1);
             SAFE_E(fread(buf, 1, sz, fp), sz, "Failed to load /Root.\n");
             buf[sz] = '\0';
             if (!(c = strstr(buf, "/Root")))
            {
                free(buf);
                continue;
            }

            /* Jump to catalog (root) */
            id = atoi(c + strlen("/Root") + 1);
            free(buf);
            buf = get_object(fp, id, &pdf->xrefs[i], NULL, &dummy);
            if (!buf || !(c = strstr(buf, "/Pages")))
            {
                free(buf);
                continue;
            }

            /* Start at the first Pages obj and get kids */
            id = atoi(c + strlen("/Pages") + 1);
            load_kids(fp, id, &pdf->xrefs[i]);
            free(buf); 
        }
    }
            
    fseek(fp, start, SEEK_SET);
}
