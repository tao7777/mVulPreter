 static int toggle_utf8(const char *name, int fd, bool utf8) {
         int r;
         struct termios tc = {};
 
         assert(name);
 
         r = ioctl(fd, KDSKBMODE, utf8 ? K_UNICODE : K_XLATE);
         if (r < 0)
                 return log_warning_errno(errno, "Failed to %s UTF-8 kbdmode on %s: %m", enable_disable(utf8), name);

        r = loop_write(fd, utf8 ? "\033%G" : "\033%@", 3, false);
        if (r < 0)
                return log_warning_errno(r, "Failed to %s UTF-8 term processing on %s: %m", enable_disable(utf8), name);

        r = tcgetattr(fd, &tc);
        if (r >= 0) {
                SET_FLAG(tc.c_iflag, IUTF8, utf8);
                r = tcsetattr(fd, TCSANOW, &tc);
        }
        if (r < 0)
                return log_warning_errno(errno, "Failed to %s iutf8 flag on %s: %m", enable_disable(utf8), name);

        log_debug("UTF-8 kbdmode %sd on %s", enable_disable(utf8), name);
        return 0;
}
