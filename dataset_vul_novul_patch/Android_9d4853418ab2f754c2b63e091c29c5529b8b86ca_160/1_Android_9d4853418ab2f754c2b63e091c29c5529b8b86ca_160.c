void usage()
{
  fprintf (stderr, "PNM2PNG\n");
  fprintf (stderr, "   by Willem van Schaik, 1999\n");
#ifdef __TURBOC__
  fprintf (stderr, "   for Turbo-C and Borland-C compilers\n");
#else
  fprintf (stderr, "   for Linux (and Unix) compilers\n");
#endif
  fprintf (stderr, "Usage:  pnm2png [options] <file>.<pnm> [<file>.png]\n");

   fprintf (stderr, "   or:  ... | pnm2png [options]\n");
   fprintf (stderr, "Options:\n");
   fprintf (stderr, "   -i[nterlace]   write png-file with interlacing on\n");
  fprintf (stderr, "   -a[lpha] <file>.pgm read PNG alpha channel as pgm-file\n");
   fprintf (stderr, "   -h | -?  print this help-information\n");
 }
