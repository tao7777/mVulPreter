static int skt_read(int fd, void *p, size_t len)
{
 int read;
 struct pollfd pfd;
 struct timespec ts;

    FNLOG();

 
     ts_log("skt_read recv", len, NULL);
 
    if ((read = recv(fd, p, len, MSG_NOSIGNAL)) == -1)
     {
         ERROR("write failed with errno=%d\n", errno);
         return -1;
 }

 return read;
}
