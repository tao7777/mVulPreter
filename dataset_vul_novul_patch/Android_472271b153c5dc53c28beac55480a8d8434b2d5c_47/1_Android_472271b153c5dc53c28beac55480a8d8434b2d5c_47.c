int sock_send_fd(int sock_fd, const uint8_t* buf, int len, int send_fd)
{
 ssize_t ret;
 struct msghdr msg;
 unsigned char *buffer = (unsigned char *)buf;
    memset(&msg, 0, sizeof(msg));

 struct cmsghdr *cmsg;
 char msgbuf[CMSG_SPACE(1)];
    asrt(send_fd != -1);
 if(sock_fd == -1 || send_fd == -1)
 return -1;
    msg.msg_control = msgbuf;
    msg.msg_controllen = sizeof msgbuf;
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof send_fd);
    memcpy(CMSG_DATA(cmsg), &send_fd, sizeof send_fd);

 int ret_len = len;
 while (len > 0) {
 struct iovec iv;
        memset(&iv, 0, sizeof(iv));

        iv.iov_base = buffer;
        iv.iov_len = len;

        msg.msg_iov = &iv;

         msg.msg_iovlen = 1;
 
         do {
            ret = sendmsg(sock_fd, &msg, MSG_NOSIGNAL);
         } while (ret < 0 && errno == EINTR);
 
         if (ret < 0) {
            BTIF_TRACE_ERROR("fd:%d, send_fd:%d, sendmsg ret:%d, errno:%d, %s",
                              sock_fd, send_fd, (int)ret, errno, strerror(errno));
            ret_len = -1;
 break;
 }

        buffer += ret;
        len -= ret;

        memset(&msg, 0, sizeof(msg));
 }
    BTIF_TRACE_DEBUG("close fd:%d after sent", send_fd);
    close(send_fd);
 return ret_len;
}
