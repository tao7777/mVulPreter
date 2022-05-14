 gplotRead(const char  *filename)
 {
char     buf[L_BUFSIZE];
 char    *rootname, *title, *xlabel, *ylabel, *ignores;
 l_int32  outformat, ret, version, ignore;
 FILE    *fp;
GPLOT   *gplot;

    PROCNAME("gplotRead");

    if (!filename)
        return (GPLOT *)ERROR_PTR("filename not defined", procName, NULL);

    if ((fp = fopenReadStream(filename)) == NULL)
        return (GPLOT *)ERROR_PTR("stream not opened", procName, NULL);

    ret = fscanf(fp, "Gplot Version %d\n", &version);
    if (ret != 1) {
        fclose(fp);
        return (GPLOT *)ERROR_PTR("not a gplot file", procName, NULL);
    }
    if (version != GPLOT_VERSION_NUMBER) {
        fclose(fp);
         return (GPLOT *)ERROR_PTR("invalid gplot version", procName, NULL);
     }
 
    ignore = fscanf(fp, "Rootname: %511s\n", buf);  /* L_BUFSIZE - 1 */
     rootname = stringNew(buf);
     ignore = fscanf(fp, "Output format: %d\n", &outformat);
    ignores = fgets(buf, L_BUFSIZE, fp);   /* Title: ... */
     title = stringNew(buf + 7);
     title[strlen(title) - 1] = '\0';
    ignores = fgets(buf, L_BUFSIZE, fp);   /* X axis label: ... */
     xlabel = stringNew(buf + 14);
     xlabel[strlen(xlabel) - 1] = '\0';
    ignores = fgets(buf, L_BUFSIZE, fp);   /* Y axis label: ... */
     ylabel = stringNew(buf + 14);
     ylabel[strlen(ylabel) - 1] = '\0';
 
    gplot = gplotCreate(rootname, outformat, title, xlabel, ylabel);
    LEPT_FREE(rootname);
    LEPT_FREE(title);
    LEPT_FREE(xlabel);
    LEPT_FREE(ylabel);
    if (!gplot) {
        fclose(fp);
        return (GPLOT *)ERROR_PTR("gplot not made", procName, NULL);
    }
    sarrayDestroy(&gplot->cmddata);
    sarrayDestroy(&gplot->datanames);
    sarrayDestroy(&gplot->plotdata);
     sarrayDestroy(&gplot->plottitles);
     numaDestroy(&gplot->plotstyles);
 
    ignore = fscanf(fp, "Commandfile name: %511s\n", buf);  /* L_BUFSIZE - 1 */
     stringReplace(&gplot->cmdname, buf);
     ignore = fscanf(fp, "\nCommandfile data:");
     gplot->cmddata = sarrayReadStream(fp);
    ignore = fscanf(fp, "\nDatafile names:");
    gplot->datanames = sarrayReadStream(fp);
    ignore = fscanf(fp, "\nPlot data:");
    gplot->plotdata = sarrayReadStream(fp);
    ignore = fscanf(fp, "\nPlot titles:");
    gplot->plottitles = sarrayReadStream(fp);
    ignore = fscanf(fp, "\nPlot styles:");
     gplot->plotstyles = numaReadStream(fp);
 
     ignore = fscanf(fp, "Number of plots: %d\n", &gplot->nplots);
    ignore = fscanf(fp, "Output file name: %511s\n", buf);
     stringReplace(&gplot->outname, buf);
     ignore = fscanf(fp, "Axis scaling: %d\n", &gplot->scaling);
 
    fclose(fp);
    return gplot;
}
