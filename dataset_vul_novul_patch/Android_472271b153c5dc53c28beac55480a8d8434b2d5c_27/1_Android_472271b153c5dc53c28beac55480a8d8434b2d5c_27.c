void close_uinput (void)

 {
     BTIF_TRACE_DEBUG("%s", __FUNCTION__);
     if (uinput_fd > 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
 
         close(uinput_fd);
         uinput_fd = -1;
 }
}
