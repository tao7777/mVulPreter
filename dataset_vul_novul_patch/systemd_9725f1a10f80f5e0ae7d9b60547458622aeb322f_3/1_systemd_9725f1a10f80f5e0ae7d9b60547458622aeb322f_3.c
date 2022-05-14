static int verify_source_vc(char **ret_path, const char *src_vc) {
        _cleanup_close_ int fd = -1;
        char *path;
        int r;

        fd = open_terminal(src_vc, O_RDWR|O_CLOEXEC|O_NOCTTY);
        if (fd < 0)
                return log_error_errno(fd, "Failed to open %s: %m", src_vc);

        r = verify_vc_device(fd);
        if (r < 0)
                return log_error_errno(r, "Device %s is not a virtual console: %m", src_vc);

        r = verify_vc_allocation_byfd(fd);
         if (r < 0)
                 return log_error_errno(r, "Virtual console %s is not allocated: %m", src_vc);
 
        r = verify_vc_kbmode(fd);
         if (r < 0)
                 return log_error_errno(r, "Virtual console %s is not in K_XLATE or K_UNICODE: %m", src_vc);
 
        path = strdup(src_vc);
        if (!path)
                return log_oom();

        *ret_path = path;
        return TAKE_FD(fd);
}
