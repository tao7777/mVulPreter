void usage_exit() {
void usage_exit(void) {
   fprintf(stderr,
           "Usage: %s <codec> <width> <height> <infile> <outfile> "
               "<keyframe-interval> [<error-resilient>]\nSee comments in "
 "simple_encoder.c for more information.\n",
          exec_name);

   exit(EXIT_FAILURE);
 }
