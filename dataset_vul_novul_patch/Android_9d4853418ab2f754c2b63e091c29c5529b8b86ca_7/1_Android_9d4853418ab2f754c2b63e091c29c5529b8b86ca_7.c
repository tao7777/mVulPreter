int main(int argc, char **argv)
{
#ifdef sgi
 char tmpline[80];
#endif
 char *p;
 int rc, alen, flen;
 int error = 0;
 int have_bg = FALSE;
 double LUT_exponent; /* just the lookup table */
 double CRT_exponent = 2.2; /* just the monitor */
 double default_display_exponent; /* whole display system */
 XEvent e;
 KeySym k;


    displayname = (char *)NULL;
    filename = (char *)NULL;


 /* First set the default value for our display-system exponent, i.e.,
     * the product of the CRT exponent and the exponent corresponding to
     * the frame-buffer's lookup table (LUT), if any.  This is not an
     * exhaustive list of LUT values (e.g., OpenStep has a lot of weird
     * ones), but it should cover 99% of the current possibilities. */

#if defined(NeXT)
    LUT_exponent = 1.0 / 2.2;
 /*
    if (some_next_function_that_returns_gamma(&next_gamma))
        LUT_exponent = 1.0 / next_gamma;
     */
#elif defined(sgi)
    LUT_exponent = 1.0 / 1.7;
 /* there doesn't seem to be any documented function to get the
     * "gamma" value, so we do it the hard way */
    infile = fopen("/etc/config/system.glGammaVal", "r");
 if (infile) {
 double sgi_gamma;

        fgets(tmpline, 80, infile);
        fclose(infile);
        sgi_gamma = atof(tmpline);
 if (sgi_gamma > 0.0)
            LUT_exponent = 1.0 / sgi_gamma;
 }
#elif defined(Macintosh)
    LUT_exponent = 1.8 / 2.61;
 /*
    if (some_mac_function_that_returns_gamma(&mac_gamma))
        LUT_exponent = mac_gamma / 2.61;
     */
#else
    LUT_exponent = 1.0; /* assume no LUT:  most PCs */
#endif

 /* the defaults above give 1.0, 1.3, 1.5 and 2.2, respectively: */
    default_display_exponent = LUT_exponent * CRT_exponent;


 /* If the user has set the SCREEN_GAMMA environment variable as suggested
     * (somewhat imprecisely) in the libpng documentation, use that; otherwise
     * use the default value we just calculated.  Either way, the user may
     * override this via a command-line option. */

 if ((p = getenv("SCREEN_GAMMA")) != NULL)
        display_exponent = atof(p);
 else
        display_exponent = default_display_exponent;


 /* Now parse the command line for options and the PNG filename. */

 while (*++argv && !error) {
 if (!strncmp(*argv, "-display", 2)) {
 if (!*++argv)
 ++error;
 else
                displayname = *argv;
 } else if (!strncmp(*argv, "-gamma", 2)) {
 if (!*++argv)
 ++error;
 else {
                display_exponent = atof(*argv);
 if (display_exponent <= 0.0)
 ++error;
 }
 } else if (!strncmp(*argv, "-bgcolor", 2)) {
 if (!*++argv)
 ++error;
 else {
                bgstr = *argv;
 if (strlen(bgstr) != 7 || bgstr[0] != '#')
 ++error;
 else
                    have_bg = TRUE;
 }
 } else {
 if (**argv != '-') {
                filename = *argv;
 if (argv[1]) /* shouldn't be any more args after filename */
 ++error;
 } else
 ++error; /* not expecting any other options */
 }
 }

 if (!filename)
 ++error;


 /* print usage screen if any errors up to this point */

 if (error) {
        fprintf(stderr, "\n%s %s:  %s\n", PROGNAME, VERSION, appname);
        readpng_version_info();
        fprintf(stderr, "\n"

           "Usage:  %s [-display xdpy] [-gamma exp] [-bgcolor bg] file.png\n"
           "    xdpy\tname of the target X display (e.g., ``hostname:0'')\n"
           "    exp \ttransfer-function exponent (``gamma'') of the display\n"
          "\t\t  system in floating-point format (e.g., ``%.1f''); equal\n"
           "\t\t  to the product of the lookup-table exponent (varies)\n"
           "\t\t  and the CRT exponent (usually 2.2); must be positive\n"
           "    bg  \tdesired background color in 7-character hex RGB format\n"
           "\t\t  (e.g., ``#ff7700'' for orange:  same as HTML colors);\n"
           "\t\t  used with transparent images\n"
           "\nPress Q, Esc or mouse button 1 (within image window, after image\n"
          "is displayed) to quit.\n"
          "\n", PROGNAME, default_display_exponent);
         exit(1);
     }
 

 if (!(infile = fopen(filename, "rb"))) {
        fprintf(stderr, PROGNAME ":  can't open PNG file [%s]\n", filename);
 ++error;
 } else {
 if ((rc = readpng_init(infile, &image_width, &image_height)) != 0) {
 switch (rc) {
 case 1:
                    fprintf(stderr, PROGNAME
 ":  [%s] is not a PNG file: incorrect signature\n",
                      filename);
 break;
 case 2:
                    fprintf(stderr, PROGNAME
 ":  [%s] has bad IHDR (libpng longjmp)\n", filename);
 break;
 case 4:
                    fprintf(stderr, PROGNAME ":  insufficient memory\n");
 break;
 default:
                    fprintf(stderr, PROGNAME
 ":  unknown readpng_init() error\n");
 break;
 }
 ++error;
 } else {
            display = XOpenDisplay(displayname);
 if (!display) {
                readpng_cleanup(TRUE);
                fprintf(stderr, PROGNAME ":  can't open X display [%s]\n",
                  displayname? displayname : "default");
 ++error;
 }
 }
 if (error)
            fclose(infile);
 }


 if (error) {
        fprintf(stderr, PROGNAME ":  aborting.\n");
        exit(2);
 }


 /* set the title-bar string, but make sure buffer doesn't overflow */

    alen = strlen(appname);
    flen = strlen(filename);
 if (alen + flen + 3 > 1023)
        sprintf(titlebar, "%s:  ...%s", appname, filename+(alen+flen+6-1023));
 else
        sprintf(titlebar, "%s:  %s", appname, filename);


 /* if the user didn't specify a background color on the command line,
     * check for one in the PNG file--if not, the initialized values of 0
     * (black) will be used */

 if (have_bg) {
 unsigned r, g, b; /* this approach quiets compiler warnings */

        sscanf(bgstr+1, "%2x%2x%2x", &r, &g, &b);
        bg_red   = (uch)r;
        bg_green = (uch)g;
        bg_blue  = (uch)b;
 } else if (readpng_get_bgcolor(&bg_red, &bg_green, &bg_blue) > 1) {
        readpng_cleanup(TRUE);
        fprintf(stderr, PROGNAME
 ":  libpng error while checking for background color\n");
        exit(2);
 }


 /* do the basic X initialization stuff, make the window and fill it
     * with the background color */

 if (rpng_x_create_window())
        exit(2);


 /* decode the image, all at once */

 Trace((stderr, "calling readpng_get_image()\n"))
    image_data = readpng_get_image(display_exponent, &image_channels,
 &image_rowbytes);
 Trace((stderr, "done with readpng_get_image()\n"))


 /* done with PNG file, so clean up to minimize memory usage (but do NOT
     * nuke image_data!) */

    readpng_cleanup(FALSE);
    fclose(infile);

 if (!image_data) {
        fprintf(stderr, PROGNAME ":  unable to decode PNG image\n");
        exit(3);
 }


 /* display image (composite with background if requested) */

 Trace((stderr, "calling rpng_x_display_image()\n"))
 if (rpng_x_display_image()) {
        free(image_data);
        exit(4);
 }
 Trace((stderr, "done with rpng_x_display_image()\n"))


 /* wait for the user to tell us when to quit */

    printf(
 "Done.  Press Q, Esc or mouse button 1 (within image window) to quit.\n");
    fflush(stdout);

 do
 XNextEvent(display, &e);
 while (!(e.type == ButtonPress && e.xbutton.button == Button1) &&
 !(e.type == KeyPress && /*  v--- or 1 for shifted keys */
 ((k = XLookupKeysym(&e.xkey, 0)) == XK_q || k == XK_Escape) ));


 /* OK, we're done:  clean up all image and X resources and go away */

 
     rpng_x_cleanup();
 
     return 0;
 }
