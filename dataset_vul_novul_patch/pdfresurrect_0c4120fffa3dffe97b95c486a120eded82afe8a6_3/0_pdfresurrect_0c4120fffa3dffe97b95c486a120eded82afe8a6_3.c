static char *get_object(
    FILE         *fp,
    int           obj_id,
    const xref_t *xref,
    size_t       *size,
    int          *is_stream)
{
    static const int    blk_sz = 256;
    int                 i, total_sz, read_sz, n_blks, search, stream;
    size_t              obj_sz;
    char               *c, *data;
    long                start;
    const xref_entry_t *entry;

    if (size)
      *size = 0;

    if (is_stream)
      *is_stream = 0;

    start = ftell(fp);

    /* Find object */
    entry = NULL;
    for (i=0; i<xref->n_entries; i++)
      if (xref->entries[i].obj_id == obj_id)
      {
          entry = &xref->entries[i];
          break;
      }

    if (!entry)
      return NULL;

    /* Jump to object start */
    fseek(fp, entry->offset, SEEK_SET);

    /* Initial allocate */
     obj_sz = 0;    /* Bytes in object */
     total_sz = 0;  /* Bytes read in   */
     n_blks = 1;
    data = safe_calloc(blk_sz * n_blks);
 
     /* Suck in data */
     stream = 0;
    while ((read_sz = fread(data+total_sz, 1, blk_sz-1, fp)) && !ferror(fp))
    {
        total_sz += read_sz;

        *(data + total_sz) = '\0';

        if (total_sz + blk_sz >= (blk_sz * n_blks))
          data = realloc(data, blk_sz * (++n_blks));

        search = total_sz - read_sz;
        if (search < 0)
          search = 0;

        if ((c = strstr(data + search, "endobj")))
        {
            *(c + strlen("endobj") + 1) = '\0';
            obj_sz = (void *)strstr(data + search, "endobj") - (void *)data;
            obj_sz += strlen("endobj") + 1;
            break;
        }
        else if (strstr(data, "stream"))
          stream = 1;
    }

    clearerr(fp);
    fseek(fp, start, SEEK_SET);

    if (size)
      *size = obj_sz;
            
    if (is_stream)
      *is_stream = stream;

    return data;
}
