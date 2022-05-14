void usage_exit() {
void usage_exit(void) {
   fprintf(stderr, "Usage: %s <width> <height> <infile> <outfile> <frame>\n",
           exec_name);
   exit(EXIT_FAILURE);
 }
