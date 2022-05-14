 int vt_reset_keyboard(int fd) {
        int kb, r;
 
         /* If we can't read the default, then default to unicode. It's 2017 after all. */
         kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
 
        r = vt_verify_kbmode(fd);
        if (r == -EBUSY) {
                log_debug_errno(r, "Keyboard is not in XLATE or UNICODE mode, not resetting: %m");
                return 0;
        } else if (r < 0)
                return r;

         if (ioctl(fd, KDSKBMODE, kb) < 0)
                 return -errno;
 
        return 0;
}
