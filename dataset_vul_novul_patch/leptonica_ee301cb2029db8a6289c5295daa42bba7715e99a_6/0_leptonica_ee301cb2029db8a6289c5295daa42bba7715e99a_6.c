 selReadStream(FILE  *fp)
 {
 char    *selname;
char     linebuf[L_BUFSIZE];
 l_int32  sy, sx, cy, cx, i, j, version, ignore;
 SEL     *sel;
 
    PROCNAME("selReadStream");

    if (!fp)
        return (SEL *)ERROR_PTR("stream not defined", procName, NULL);

    if (fscanf(fp, "  Sel Version %d\n", &version) != 1)
        return (SEL *)ERROR_PTR("not a sel file", procName, NULL);
     if (version != SEL_VERSION_NUMBER)
         return (SEL *)ERROR_PTR("invalid sel version", procName, NULL);
 
    if (fgets(linebuf, L_BUFSIZE, fp) == NULL)
         return (SEL *)ERROR_PTR("error reading into linebuf", procName, NULL);
     selname = stringNew(linebuf);
    sscanf(linebuf, "  ------  %200s  ------", selname);
 
     if (fscanf(fp, "  sy = %d, sx = %d, cy = %d, cx = %d\n",
             &sy, &sx, &cy, &cx) != 4) {
        LEPT_FREE(selname);
        return (SEL *)ERROR_PTR("dimensions not read", procName, NULL);
    }

    if ((sel = selCreate(sy, sx, selname)) == NULL) {
        LEPT_FREE(selname);
        return (SEL *)ERROR_PTR("sel not made", procName, NULL);
    }
    selSetOrigin(sel, cy, cx);

    for (i = 0; i < sy; i++) {
        ignore = fscanf(fp, "    ");
        for (j = 0; j < sx; j++)
            ignore = fscanf(fp, "%1d", &sel->data[i][j]);
        ignore = fscanf(fp, "\n");
    }
    ignore = fscanf(fp, "\n");

    LEPT_FREE(selname);
    return sel;
}
