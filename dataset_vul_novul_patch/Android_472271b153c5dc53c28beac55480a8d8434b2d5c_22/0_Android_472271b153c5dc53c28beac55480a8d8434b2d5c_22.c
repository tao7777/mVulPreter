int btpan_tap_open()
{
 struct ifreq ifr;
 int fd, err;
 const char *clonedev = "/dev/tun";

 
     /* open the clone device */
 
    if ((fd = TEMP_FAILURE_RETRY(open(clonedev, O_RDWR))) < 0)
     {
         BTIF_TRACE_DEBUG("could not open %s, err:%d", clonedev, errno);
         return fd;
 }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;


     strncpy(ifr.ifr_name, TAP_IF_NAME, IFNAMSIZ);
 
     /* try to create the device */
    if ((err = TEMP_FAILURE_RETRY(ioctl(fd, TUNSETIFF, (void *) &ifr))) < 0)
     {
         BTIF_TRACE_DEBUG("ioctl error:%d, errno:%s", err, strerror(errno));
         close(fd);
 return err;

     }
     if (tap_if_up(TAP_IF_NAME, controller_get_interface()->get_address()) == 0)
     {
        int flags = TEMP_FAILURE_RETRY(fcntl(fd, F_GETFL, 0));
        TEMP_FAILURE_RETRY(fcntl(fd, F_SETFL, flags | O_NONBLOCK));
         return fd;
     }
     BTIF_TRACE_ERROR("can not bring up tap interface:%s", TAP_IF_NAME);
    close(fd);
 return INVALID_FD;
}
