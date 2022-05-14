 int vt_reset_keyboard(int fd) {
        int kb;
 
         /* If we can't read the default, then default to unicode. It's 2017 after all. */
         kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
 
         if (ioctl(fd, KDSKBMODE, kb) < 0)
                 return -errno;
 
        return 0;
}
