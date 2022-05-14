selaGetCombName(SELA    *sela,
                l_int32  size,
                 l_int32  direction)
 {
 char    *selname;
char     combname[L_BUFSIZE];
 l_int32  i, nsels, sx, sy, found;
 SEL     *sel;
 
    PROCNAME("selaGetCombName");

    if (!sela)
        return (char *)ERROR_PTR("sela not defined", procName, NULL);
    if (direction != L_HORIZ && direction != L_VERT)
        return (char *)ERROR_PTR("invalid direction", procName, NULL);
 
         /* Derive the comb name we're looking for */
     if (direction == L_HORIZ)
        snprintf(combname, L_BUFSIZE, "sel_comb_%dh", size);
     else  /* direction == L_VERT */
        snprintf(combname, L_BUFSIZE, "sel_comb_%dv", size);
 
     found = FALSE;
     nsels = selaGetCount(sela);
    for (i = 0; i < nsels; i++) {
        sel = selaGetSel(sela, i);
        selGetParameters(sel, &sy, &sx, NULL, NULL);
        if (sy != 1 && sx != 1)  /* 2-D; not a comb */
            continue;
        selname = selGetName(sel);
        if (!strcmp(selname, combname)) {
            found = TRUE;
            break;
        }
    }

    if (found)
        return stringNew(selname);
    else
        return (char *)ERROR_PTR("sel not found", procName, NULL);
}
