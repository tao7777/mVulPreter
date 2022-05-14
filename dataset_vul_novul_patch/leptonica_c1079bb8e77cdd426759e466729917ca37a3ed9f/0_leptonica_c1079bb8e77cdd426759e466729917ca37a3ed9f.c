pixHtmlViewer(const char  *dirin,
              const char  *dirout,
              const char  *rootname,
              l_int32      thumbwidth,
              l_int32      viewwidth)
{
char      *fname, *fullname, *outname;
char      *mainname, *linkname, *linknameshort;
char      *viewfile, *thumbfile;
char      *shtml, *slink;
char       charbuf[512];
char       htmlstring[] = "<html>";
char       framestring[] = "</frameset></html>";
l_int32    i, nfiles, index, w, d, nimages, ret;
l_float32  factor;
PIX       *pix, *pixthumb, *pixview;
SARRAY    *safiles, *sathumbs, *saviews, *sahtml, *salink;

    PROCNAME("pixHtmlViewer");

    if (!dirin)
        return ERROR_INT("dirin not defined", procName, 1);
    if (!dirout)
        return ERROR_INT("dirout not defined", procName, 1);
    if (!rootname)
        return ERROR_INT("rootname not defined", procName, 1);

    if (thumbwidth == 0)
        thumbwidth = DEFAULT_THUMB_WIDTH;
    if (thumbwidth < MIN_THUMB_WIDTH) {
        L_WARNING("thumbwidth too small; using min value\n", procName);
        thumbwidth = MIN_THUMB_WIDTH;
    }
    if (viewwidth == 0)
        viewwidth = DEFAULT_VIEW_WIDTH;
    if (viewwidth < MIN_VIEW_WIDTH) {
        L_WARNING("viewwidth too small; using min value\n", procName);
        viewwidth = MIN_VIEW_WIDTH;
    }

        /* Make the output directory if it doesn't already exist */
#ifndef _WIN32
    snprintf(charbuf, sizeof(charbuf), "mkdir -p %s", dirout);
    ret = system(charbuf);
#else
    ret = CreateDirectory(dirout, NULL) ? 0 : 1;
#endif  /* !_WIN32 */
    if (ret) {
        L_ERROR("output directory %s not made\n", procName, dirout);
        return 1;
    }

        /* Capture the filenames in the input directory */
    if ((safiles = getFilenamesInDirectory(dirin)) == NULL)
         return ERROR_INT("safiles not made", procName, 1);
 
         /* Generate output text file names */
    snprintf(charbuf, sizeof(charbuf), "%s/%s.html", dirout, rootname);
     mainname = stringNew(charbuf);
    snprintf(charbuf, sizeof(charbuf), "%s/%s-links.html", dirout, rootname);
     linkname = stringNew(charbuf);
     linknameshort = stringJoin(rootname, "-links.html");
 
        /* Generate the thumbs and views */
    sathumbs = sarrayCreate(0);
    saviews = sarrayCreate(0);
    nfiles = sarrayGetCount(safiles);
    index = 0;
    for (i = 0; i < nfiles; i++) {
        fname = sarrayGetString(safiles, i, L_NOCOPY);
        fullname = genPathname(dirin, fname);
        fprintf(stderr, "name: %s\n", fullname);
        if ((pix = pixRead(fullname)) == NULL) {
            fprintf(stderr, "file %s not a readable image\n", fullname);
            lept_free(fullname);
            continue;
        }
        lept_free(fullname);

            /* Make and store the thumbnail images */
         pixGetDimensions(pix, &w, NULL, &d);
         factor = (l_float32)thumbwidth / (l_float32)w;
         pixthumb = pixScale(pix, factor, factor);
        snprintf(charbuf, sizeof(charbuf), "%s_thumb_%03d", rootname, index);
         sarrayAddString(sathumbs, charbuf, L_COPY);
         outname = genPathname(dirout, charbuf);
         WriteFormattedPix(outname, pixthumb);
        lept_free(outname);
        pixDestroy(&pixthumb);

            /* Make and store the view images */
        factor = (l_float32)viewwidth / (l_float32)w;
        if (factor >= 1.0)
            pixview = pixClone(pix);   /* no upscaling */
        else
            pixview = pixScale(pix, factor, factor);
        snprintf(charbuf, sizeof(charbuf), "%s_view_%03d", rootname, index);
        sarrayAddString(saviews, charbuf, L_COPY);
        outname = genPathname(dirout, charbuf);
        WriteFormattedPix(outname, pixview);
        lept_free(outname);
        pixDestroy(&pixview);
        pixDestroy(&pix);
        index++;
    }

        /* Generate the main html file */
    sahtml = sarrayCreate(0);
    sarrayAddString(sahtml, htmlstring, L_COPY);
    sprintf(charbuf, "<frameset cols=\"%d, *\">", thumbwidth + 30);
    sarrayAddString(sahtml, charbuf, L_COPY);
    sprintf(charbuf, "<frame name=\"thumbs\" src=\"%s\">", linknameshort);
    sarrayAddString(sahtml, charbuf, L_COPY);
    sprintf(charbuf, "<frame name=\"views\" src=\"%s\">",
            sarrayGetString(saviews, 0, L_NOCOPY));
    sarrayAddString(sahtml, charbuf, L_COPY);
    sarrayAddString(sahtml, framestring, L_COPY);
    shtml = sarrayToString(sahtml, 1);
    l_binaryWrite(mainname, "w", shtml, strlen(shtml));
    fprintf(stderr, "******************************************\n"
                    "Writing html file: %s\n"
                    "******************************************\n", mainname);
    lept_free(shtml);
    lept_free(mainname);

        /* Generate the link html file */
    nimages = sarrayGetCount(saviews);
    fprintf(stderr, "num. images = %d\n", nimages);
    salink = sarrayCreate(0);
    for (i = 0; i < nimages; i++) {
        viewfile = sarrayGetString(saviews, i, L_NOCOPY);
        thumbfile = sarrayGetString(sathumbs, i, L_NOCOPY);
        sprintf(charbuf, "<a href=\"%s\" TARGET=views><img src=\"%s\"></a>",
            viewfile, thumbfile);
        sarrayAddString(salink, charbuf, L_COPY);
    }
    slink = sarrayToString(salink, 1);
    l_binaryWrite(linkname, "w", slink, strlen(slink));
    lept_free(slink);
    lept_free(linkname);
    lept_free(linknameshort);
    sarrayDestroy(&safiles);
    sarrayDestroy(&sathumbs);
    sarrayDestroy(&saviews);
    sarrayDestroy(&sahtml);
    sarrayDestroy(&salink);
    return 0;
}
