int sock_recv_all(int sock_fd, uint8_t* buf, int len)
{
 int r = len;

     int ret = -1;
     while(r)
     {
        do ret = recv(sock_fd, buf, r, MSG_WAITALL);
         while(ret < 0 && errno == EINTR);
         if(ret <= 0)
         {
            BTIF_TRACE_ERROR("sock fd:%d recv errno:%d, ret:%d", sock_fd, errno, ret);
 return -1;
 }
        buf += ret;
        r -= ret;
 }
 return len;
}
