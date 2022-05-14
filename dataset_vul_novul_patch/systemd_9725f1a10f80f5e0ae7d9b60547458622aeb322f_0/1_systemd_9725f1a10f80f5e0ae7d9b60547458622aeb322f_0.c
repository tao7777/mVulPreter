static int find_source_vc(char **ret_path, unsigned *ret_idx) {
        _cleanup_free_ char *path = NULL;
        int r, err = 0;
        unsigned i;

        path = new(char, sizeof("/dev/tty63"));
        if (!path)
                return log_oom();

        for (i = 1; i <= 63; i++) {
                _cleanup_close_ int fd = -1;

                r = verify_vc_allocation(i);
                if (r < 0) {
                        if (!err)
                                err = -r;
                        continue;
                }

                sprintf(path, "/dev/tty%u", i);
                fd = open_terminal(path, O_RDWR|O_CLOEXEC|O_NOCTTY);
                if (fd < 0) {
                        if (!err)
                                 err = -fd;
                         continue;
                 }
                r = verify_vc_kbmode(fd);
                 if (r < 0) {
                         if (!err)
                                 err = -r;
                        continue;
                }

                /* all checks passed, return this one as a source console */
                *ret_idx = i;
                *ret_path = TAKE_PTR(path);
                return TAKE_FD(fd);
        }

        return log_error_errno(err, "No usable source console found: %m");
}
