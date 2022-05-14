static int verify_vc_kbmode(int fd) {
        int curr_mode;
        /*
         * Make sure we only adjust consoles in K_XLATE or K_UNICODE mode.
         * Otherwise we would (likely) interfere with X11's processing of the
         * key events.
         *
         * http://lists.freedesktop.org/archives/systemd-devel/2013-February/008573.html
         */
        if (ioctl(fd, KDGKBMODE, &curr_mode) < 0)
                return -errno;
        return IN_SET(curr_mode, K_XLATE, K_UNICODE) ? 0 : -EBUSY;
}
