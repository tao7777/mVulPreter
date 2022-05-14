void usage_exit() {
void usage_exit(void) {
   fprintf(stderr, "Usage: %s <infile> <outfile>\n", exec_name);
   exit(EXIT_FAILURE);
 }
