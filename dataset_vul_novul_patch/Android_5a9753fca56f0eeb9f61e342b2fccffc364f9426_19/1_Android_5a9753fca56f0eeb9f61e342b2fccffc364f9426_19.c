void usage_exit() {
   fprintf(stderr, "Usage: %s <codec> <width> <height> <infile> <outfile>\n",
           exec_name);
   exit(EXIT_FAILURE);
 }
