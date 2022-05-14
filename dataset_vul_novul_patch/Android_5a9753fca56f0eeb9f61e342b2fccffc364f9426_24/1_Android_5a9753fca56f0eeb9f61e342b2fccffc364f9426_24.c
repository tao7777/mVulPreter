void usage_exit() {
   fprintf(stderr, "Usage: %s <width> <height> <infile> <outfile> <frame>\n",
           exec_name);
   exit(EXIT_FAILURE);
 }
