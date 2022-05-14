store_current_palette(png_store *ps, int *npalette)
{
 /* This is an internal error (the call has been made outside a read

     * operation.)
     */
    if (ps->current == NULL)
       store_log(ps, ps->pread, "no current stream for palette", 1);
 
    /* The result may be null if there is no palette. */
    *npalette = ps->current->npalette;
 return ps->current->palette;
}
