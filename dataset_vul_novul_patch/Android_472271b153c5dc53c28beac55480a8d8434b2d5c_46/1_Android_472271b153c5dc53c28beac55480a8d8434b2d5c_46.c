int sock_send_all(int sock_fd, const uint8_t* buf, int len)
{
 int s = len;

     int ret;
     while(s)
     {
        do ret = send(sock_fd, buf, s, 0);
         while(ret < 0 && errno == EINTR);
         if(ret <= 0)
         {
            BTIF_TRACE_ERROR("sock fd:%d send errno:%d, ret:%d", sock_fd, errno, ret);
 return -1;
 }
        buf += ret;
        s -= ret;
 }
 return len;
}
