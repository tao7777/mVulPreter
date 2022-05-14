static void usage(char *progname) {
static const char *exec_name = NULL;

static void usage() {
   printf("Usage:\n");
   printf("%s <input_yuv> <width>x<height> <target_width>x<target_height> ",
         exec_name);
   printf("<output_yuv> [<frames>]\n");
 }
