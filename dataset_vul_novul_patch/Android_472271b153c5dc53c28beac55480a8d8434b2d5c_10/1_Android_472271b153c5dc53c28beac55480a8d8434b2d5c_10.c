 static int uhid_write(int fd, const struct uhid_event *ev)
 {
    ssize_t ret = write(fd, ev, sizeof(*ev));
     if (ret < 0){
         int rtn = -errno;
         APPL_TRACE_ERROR("%s: Cannot write to uhid:%s",
                         __FUNCTION__, strerror(errno));
 return rtn;
 } else if (ret != (ssize_t)sizeof(*ev)) {
        APPL_TRACE_ERROR("%s: Wrong size written to uhid: %zd != %zu",
                         __FUNCTION__, ret, sizeof(*ev));
 return -EFAULT;
 }

 return 0;
}
