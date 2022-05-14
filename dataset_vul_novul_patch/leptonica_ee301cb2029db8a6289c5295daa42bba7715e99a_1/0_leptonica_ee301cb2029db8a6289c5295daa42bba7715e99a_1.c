gplotCreate(const char  *rootname,
            l_int32      outformat,
            const char  *title,
            const char  *xlabel,
             const char  *ylabel)
 {
 char    *newroot;
char     buf[L_BUFSIZE];
 l_int32  badchar;
 GPLOT   *gplot;
 
    PROCNAME("gplotCreate");

    if (!rootname)
        return (GPLOT *)ERROR_PTR("rootname not defined", procName, NULL);
    if (outformat != GPLOT_PNG && outformat != GPLOT_PS &&
        outformat != GPLOT_EPS && outformat != GPLOT_LATEX)
        return (GPLOT *)ERROR_PTR("outformat invalid", procName, NULL);
    stringCheckForChars(rootname, "`;&|><\"?*", &badchar);
    if (badchar)  /* danger of command injection */
        return (GPLOT *)ERROR_PTR("invalid rootname", procName, NULL);

    if ((gplot = (GPLOT *)LEPT_CALLOC(1, sizeof(GPLOT))) == NULL)
        return (GPLOT *)ERROR_PTR("gplot not made", procName, NULL);
    gplot->cmddata = sarrayCreate(0);
    gplot->datanames = sarrayCreate(0);
    gplot->plotdata = sarrayCreate(0);
    gplot->plottitles = sarrayCreate(0);
    gplot->plotstyles = numaCreate(0);

        /* Save title, labels, rootname, outformat, cmdname, outname */
     newroot = genPathname(rootname, NULL);
     gplot->rootname = newroot;
     gplot->outformat = outformat;
    snprintf(buf, L_BUFSIZE, "%s.cmd", rootname);
     gplot->cmdname = stringNew(buf);
     if (outformat == GPLOT_PNG)
        snprintf(buf, L_BUFSIZE, "%s.png", newroot);
     else if (outformat == GPLOT_PS)
        snprintf(buf, L_BUFSIZE, "%s.ps", newroot);
     else if (outformat == GPLOT_EPS)
        snprintf(buf, L_BUFSIZE, "%s.eps", newroot);
     else if (outformat == GPLOT_LATEX)
        snprintf(buf, L_BUFSIZE, "%s.tex", newroot);
     gplot->outname = stringNew(buf);
     if (title) gplot->title = stringNew(title);
     if (xlabel) gplot->xlabel = stringNew(xlabel);
    if (ylabel) gplot->ylabel = stringNew(ylabel);

    return gplot;
}
