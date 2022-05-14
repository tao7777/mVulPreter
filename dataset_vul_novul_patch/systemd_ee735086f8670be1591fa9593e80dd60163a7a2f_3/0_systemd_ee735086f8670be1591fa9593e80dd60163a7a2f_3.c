static void setup_test_dir(char *tmp_dir, const char *files, ...) {
        va_list ap;

        assert_se(mkdtemp(tmp_dir) != NULL);

         va_start(ap, files);
         while (files != NULL) {
                 _cleanup_free_ char *path = strappend(tmp_dir, files);
                assert_se(touch_file(path, true, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID) == 0);
                 files = va_arg(ap, const char *);
         }
         va_end(ap);
}
